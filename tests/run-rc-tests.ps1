#requires -Version 7.0
[CmdletBinding()]
param(
    [string]$CompilerPath = '',
    [string]$TestName = '',
    [switch]$Run,
    [switch]$IncludeKnownGaps,
    [switch]$KeepGoing
)

# Default is compile/link only. -Run explicitly opts into five-second native tests.
$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$buildDir = Join-Path $repoRoot ('build/rc-tests/' + [Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
Add-Type -TypeDefinition 'using System.Runtime.InteropServices; public static class KmyRcSuiteErrorMode { [DllImport("kernel32.dll")] public static extern uint SetErrorMode(uint mode); }' -ErrorAction SilentlyContinue

function Invoke-BoundedProcess([string]$File, [string[]]$ProcessArgs, [string]$Log, [int]$LimitMs) {
    $info = [System.Diagnostics.ProcessStartInfo]::new()
    $info.FileName = $File
    $info.WorkingDirectory = $repoRoot
    $info.UseShellExecute = $false
    $info.CreateNoWindow = $true
    $info.RedirectStandardOutput = $true
    $info.RedirectStandardError = $true
    foreach ($item in $ProcessArgs) { [void]$info.ArgumentList.Add($item) }
    $process = [System.Diagnostics.Process]::new()
    $process.StartInfo = $info
    $started = $false
    try {
        [void]$process.Start()
        $started = $true
        $stdout = $process.StandardOutput.ReadToEndAsync()
        $stderr = $process.StandardError.ReadToEndAsync()
        if (-not $process.WaitForExit($LimitMs)) {
            $process.Kill($true)
            $process.WaitForExit()
            throw "Timed out and terminated: $File"
        }
        $process.WaitForExit()
        $result = @{ Code = $process.ExitCode; Out = $stdout.Result; Err = $stderr.Result }
        Set-Content -LiteralPath ($Log + '.stdout.log') -Value $result.Out
        Set-Content -LiteralPath ($Log + '.stderr.log') -Value $result.Err
        return $result
    } finally {
        if ($started -and -not $process.HasExited) { $process.Kill($true); $process.WaitForExit() }
        $process.Dispose()
    }
}

$tests = @(
    @{ Name='single'; Alloc=1; Retain=0; Release=1; Trace=@(
        'alloc id=1 count=1', 'release id=1 count=0', 'free id=1',
        'summary alloc=1 retain=0 release=1 free=1 live=0') },
    @{ Name='copy'; Alloc=1; Retain=1; Release=2; Trace=@(
        'alloc id=1 count=1', 'retain id=1 count=2', 'release id=1 count=1',
        'release id=1 count=0', 'free id=1',
        'summary alloc=1 retain=1 release=2 free=1 live=0') },
    @{ Name='nested_alias'; Alloc=1; Retain=3; Release=4; Trace=@(
        'alloc id=1 count=1', 'retain id=1 count=2', 'retain id=1 count=3',
        'release id=1 count=2', 'release id=1 count=1', 'retain id=1 count=2',
        'release id=1 count=1', 'release id=1 count=0', 'free id=1',
        'summary alloc=1 retain=3 release=4 free=1 live=0') },
    @{ Name='reverse_order'; Alloc=2; Retain=0; Release=2; Trace=@(
        'alloc id=1 count=1', 'alloc id=2 count=1', 'release id=2 count=0', 'free id=2',
        'release id=1 count=0', 'free id=1',
        'summary alloc=2 retain=0 release=2 free=2 live=0') },
    @{ Name='shadow'; Alloc=2; Retain=2; Release=4; Trace=@(
        'alloc id=1 count=1', 'alloc id=2 count=1', 'retain id=2 count=2',
        'release id=2 count=1', 'release id=2 count=0', 'free id=2', 'retain id=1 count=2',
        'release id=1 count=1', 'release id=1 count=0', 'free id=1',
        'summary alloc=2 retain=2 release=4 free=2 live=0') },
    @{ Name='branches'; Alloc=1; Retain=2; Release=3; Trace=@(
        'alloc id=1 count=1', 'retain id=1 count=2', 'release id=1 count=1',
        'release id=1 count=0', 'free id=1', 'alloc id=2 count=1',
        'retain id=2 count=2', 'release id=2 count=1', 'release id=2 count=0', 'free id=2',
        'summary alloc=2 retain=2 release=4 free=2 live=0') },
    @{ Name='loop'; Alloc=1; Retain=1; Release=2; Trace=@(
        'alloc id=1 count=1', 'retain id=1 count=2', 'release id=1 count=1',
        'retain id=1 count=2', 'release id=1 count=1', 'retain id=1 count=2',
        'release id=1 count=1', 'release id=1 count=0', 'free id=1',
        'summary alloc=1 retain=3 release=4 free=1 live=0') }
)
# New control-flow cases check runtime traces rather than prescribing static
# call-site counts, which change with branch layout and unreachable-block pruning.
$tests += @(
    @{ Name='known_gap_return'; Trace=$tests[1].Trace },
    @{ Name='assignment'; Trace=@(
        'alloc id=1 count=1', 'alloc id=2 count=1', 'retain id=2 count=2',
        'release id=1 count=0', 'free id=1', 'release id=2 count=1',
        'release id=2 count=0', 'free id=2',
        'summary alloc=2 retain=1 release=3 free=2 live=0') },
    @{ Name='self_assignment'; Trace=$tests[1].Trace },
    @{ Name='alias_assignment'; Trace=@(
        'alloc id=1 count=1', 'retain id=1 count=2', 'retain id=1 count=3',
        'release id=1 count=2', 'release id=1 count=1', 'release id=1 count=0', 'free id=1',
        'summary alloc=1 retain=2 release=3 free=1 live=0') },
    @{ Name='integer_assignment'; Output='2'; Trace=$tests[0].Trace },
    @{ Name='return_nested'; Output='73'; Trace=@(
        'alloc id=1 count=1', 'alloc id=2 count=1', 'retain id=1 count=2',
        'release id=1 count=1', 'release id=2 count=0', 'free id=2',
        'release id=1 count=0', 'free id=1',
        'summary alloc=2 retain=1 release=3 free=2 live=0') },
    @{ Name='return_void'; Trace=$tests[1].Trace },
    @{ Name='return_paths'; Output="7`n9"; Trace=$tests[5].Trace },
    @{ Name='break_nested'; Trace=@(
        'alloc id=1 count=1', 'alloc id=2 count=1', 'retain id=1 count=2',
        'release id=1 count=1', 'release id=2 count=0', 'free id=2',
        'retain id=1 count=2', 'release id=1 count=1', 'release id=1 count=0', 'free id=1',
        'summary alloc=2 retain=2 release=4 free=2 live=0') },
    @{ Name='continue_nested'; Trace=@(
        'alloc id=1 count=1', 'alloc id=2 count=1', 'retain id=1 count=2',
        'release id=1 count=1', 'release id=2 count=0', 'free id=2',
        'alloc id=3 count=1', 'retain id=1 count=2', 'release id=1 count=1',
        'release id=3 count=0', 'free id=3', 'retain id=1 count=2',
        'release id=1 count=1', 'release id=1 count=0', 'free id=1',
        'summary alloc=3 retain=3 release=6 free=3 live=0') },
    @{ Name='nested_loop_jumps'; Trace=@(
        'alloc id=1 count=1', 'retain id=1 count=2', 'alloc id=2 count=1',
        'retain id=1 count=3', 'release id=1 count=2', 'release id=2 count=0', 'free id=2',
        'release id=1 count=1', 'retain id=1 count=2', 'alloc id=3 count=1',
        'retain id=1 count=3', 'release id=1 count=2', 'release id=3 count=0', 'free id=3',
        'release id=1 count=1', 'release id=1 count=0', 'free id=1',
        'summary alloc=3 retain=4 release=7 free=3 live=0') },
    @{ Name='known_gap_fresh_assignment'; Trace=@(
        'alloc id=1 count=1', 'alloc id=2 count=1', 'release id=1 count=0', 'free id=1',
        'release id=2 count=0', 'free id=2',
        'summary alloc=2 retain=0 release=2 free=2 live=0') },
    @{ Name='for_initializer'; Output='1'; Trace=@(
        'alloc id=1 count=1', 'retain id=1 count=2', 'release id=1 count=1',
        'retain id=1 count=2', 'release id=1 count=1', 'release id=1 count=0', 'free id=1',
        'summary alloc=1 retain=2 release=3 free=1 live=0') }
)
# Function-boundary regressions assert complete runtime event ordering.
$tests += @(
    @{ Name='param_local'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=1 release=2 free=1 live=0') },
    @{ Name='param_fresh'; Trace=@(
        'alloc id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=0 release=1 free=1 live=0') },
    @{ Name='param_same_twice'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'retain id=1 count=3',
        'release id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=2 release=3 free=1 live=0') },
    @{ Name='param_distinct'; Trace=@(
        'alloc id=1 count=1',
        'alloc id=2 count=1',
        'retain id=1 count=2',
        'retain id=2 count=2',
        'release id=1 count=1',
        'release id=2 count=1',
        'release id=2 count=0',
        'free id=2',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=2 retain=2 release=4 free=2 live=0') },
    @{ Name='param_early_return'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'release id=1 count=1',
        'retain id=1 count=2',
        'retain id=1 count=3',
        'release id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=3 release=4 free=1 live=0') },
    @{ Name='return_param'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'retain id=1 count=3',
        'release id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=2 release=3 free=1 live=0') },
    @{ Name='return_local_shared'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=1 release=2 free=1 live=0') },
    @{ Name='return_fresh_shared'; Trace=@(
        'alloc id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=0 release=1 free=1 live=0') },
    @{ Name='return_forward_shared'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'retain id=1 count=3',
        'retain id=1 count=4',
        'release id=1 count=3',
        'release id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=3 release=4 free=1 live=0') },
    @{ Name='nested_shared_argument'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'retain id=1 count=3',
        'release id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=2 release=3 free=1 live=0') },
    @{ Name='discard_fresh_shared'; Trace=@(
        'alloc id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=0 release=1 free=1 live=0') },
    @{ Name='discard_returned_param'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'retain id=1 count=3',
        'release id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=2 release=3 free=1 live=0') },
    @{ Name='discard_returned_fresh'; Trace=@(
        'alloc id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=0 release=1 free=1 live=0') },
    @{ Name='return_shared_branches'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'retain id=1 count=3',
        'release id=1 count=2',
        'retain id=1 count=3',
        'alloc id=2 count=1',
        'release id=1 count=2',
        'release id=2 count=0',
        'free id=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=2 retain=3 release=5 free=2 live=0') },
    @{ Name='return_shared_nested_cleanup'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'retain id=1 count=3',
        'release id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=2 release=3 free=1 live=0') },
    @{ Name='loop_shared_calls'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'release id=1 count=1',
        'retain id=1 count=2',
        'release id=1 count=1',
        'retain id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=3 release=4 free=1 live=0') },
    @{ Name='recursive_shared_return'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'retain id=1 count=3',
        'retain id=1 count=4',
        'retain id=1 count=5',
        'release id=1 count=4',
        'release id=1 count=3',
        'release id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=4 release=5 free=1 live=0') },
    @{ Name='shared_stack_arguments'; Output='6'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'retain id=1 count=3',
        'retain id=1 count=4',
        'retain id=1 count=5',
        'release id=1 count=4',
        'release id=1 count=3',
        'release id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=4 release=5 free=1 live=0') },
    @{ Name='shared_param_reassignment'; Trace=@(
        'alloc id=1 count=1',
        'alloc id=2 count=1',
        'retain id=1 count=2',
        'retain id=2 count=2',
        'retain id=2 count=3',
        'release id=1 count=1',
        'release id=2 count=2',
        'release id=2 count=1',
        'release id=2 count=0',
        'free id=2',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=2 retain=3 release=5 free=2 live=0') },
    @{ Name='assign_returned_shared'; Trace=@(
        'alloc id=1 count=1',
        'alloc id=2 count=1',
        'release id=1 count=0',
        'free id=1',
        'release id=2 count=0',
        'free id=2',
        'summary alloc=2 retain=0 release=2 free=2 live=0') }
)
$tests += @(
    @{ Name='global_shutdown'; Trace=@(
        'alloc id=1 count=1', 'release id=1 count=0', 'free id=1',
        'summary alloc=1 retain=0 release=1 free=1 live=0') },
    @{ Name='global_local_alias'; Trace=@(
        'alloc id=1 count=1', 'retain id=1 count=2', 'release id=1 count=1',
        'retain id=1 count=2', 'release id=1 count=1', 'release id=1 count=0', 'free id=1',
        'summary alloc=1 retain=2 release=3 free=1 live=0') },
    @{ Name='global_reverse_shutdown'; Trace=@(
        'alloc id=1 count=1', 'alloc id=2 count=1', 'release id=2 count=0', 'free id=2',
        'release id=1 count=0', 'free id=1',
        'summary alloc=2 retain=0 release=2 free=2 live=0') }
)
# Qualified reads/writes, dependency shutdown order, and invalid-target diagnostics.
$tests += @(
    @{ Name='module_shared_read'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=1 release=2 free=1 live=0') },
    @{ Name='module_shared_argument'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=1 release=2 free=1 live=0') },
    @{ Name='module_shared_return'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=1 release=2 free=1 live=0') },
    @{ Name='module_shared_library_return'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=1 release=2 free=1 live=0') },
    @{ Name='module_shared_global_alias'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=1 release=2 free=1 live=0') },
    @{ Name='module_shared_write_new'; Trace=@(
        'alloc id=1 count=1',
        'alloc id=2 count=1',
        'release id=1 count=0',
        'free id=1',
        'release id=2 count=0',
        'free id=2',
        'summary alloc=2 retain=0 release=2 free=2 live=0') },
    @{ Name='module_shared_write_local'; Trace=@(
        'alloc id=1 count=1',
        'alloc id=2 count=1',
        'retain id=2 count=2',
        'release id=1 count=0',
        'free id=1',
        'release id=2 count=1',
        'release id=2 count=0',
        'free id=2',
        'summary alloc=2 retain=1 release=3 free=2 live=0') },
    @{ Name='module_shared_self_assignment'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=1 release=2 free=1 live=0') },
    @{ Name='module_shared_write_call'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=1 release=2 free=1 live=0') },
    @{ Name='module_shared_local_assignment'; Trace=@(
        'alloc id=1 count=1',
        'alloc id=2 count=1',
        'retain id=1 count=2',
        'release id=2 count=0',
        'free id=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=2 retain=1 release=3 free=2 live=0') },
    @{ Name='module_shared_global_assignment'; Trace=@(
        'alloc id=1 count=1',
        'alloc id=2 count=1',
        'retain id=1 count=2',
        'release id=2 count=0',
        'free id=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=2 retain=1 release=3 free=2 live=0') },
    @{ Name='module_shared_qualified_assignment'; Trace=@(
        'alloc id=1 count=1',
        'alloc id=2 count=1',
        'retain id=1 count=2',
        'release id=2 count=0',
        'free id=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=2 retain=1 release=3 free=2 live=0') },
    @{ Name='module_shared_transitive_alias'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'retain id=1 count=3',
        'release id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=2 release=3 free=1 live=0') },
    @{ Name='module_numeric_assignment'; Output='7'; Trace=@(
        'alloc id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=0 release=1 free=1 live=0') },
    @{ Name='module_shared_old_alias'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'alloc id=2 count=1',
        'release id=1 count=1',
        'retain id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'release id=2 count=0',
        'free id=2',
        'summary alloc=2 retain=2 release=4 free=2 live=0') },
    @{ Name='module_shared_discard'; Trace=@(
        'alloc id=1 count=1',
        'retain id=1 count=2',
        'release id=1 count=1',
        'release id=1 count=0',
        'free id=1',
        'summary alloc=1 retain=1 release=2 free=1 live=0') },
    @{ Name='module_reject_immutable'; ErrorPattern='Assignment to a constant variable' },
    @{ Name='module_reject_function'; ErrorPattern='Assignment to a constant variable' },
    @{ Name='module_reject_enum'; ErrorPattern='Invalid assignment target' },
    @{ Name='module_reject_enum_unchecked'; ErrorPattern='Compile error:.*(assignment|lvalue|target)'; CheckOwnership=$false }
)
if ($IncludeKnownGaps) {
    # Real regression targets: failures are not swallowed or reported as passes.
    $tests += @(
        @{ Name='known_gap_for_continue'; Output='2'; Trace=@(
            'alloc id=1 count=1', 'release id=1 count=0', 'free id=1',
            'alloc id=2 count=1', 'release id=2 count=0', 'free id=2',
            'alloc id=3 count=1', 'release id=3 count=0', 'free id=3',
            'summary alloc=3 retain=0 release=3 free=3 live=0') }
    )
}
if ($TestName) {
    $tests = @($tests | Where-Object { $_.Name -eq $TestName })
    if ($tests.Count -eq 0) { throw "Unknown test name (known gaps require -IncludeKnownGaps): $TestName" }
}

$previousMode = [KmyRcSuiteErrorMode]::SetErrorMode(0x8003)
Push-Location $repoRoot
try {
    if ($CompilerPath) {
        $compiler = (Resolve-Path -LiteralPath $CompilerPath).Path
    } else {
        $compiler = Join-Path $buildDir 'kmyc-rc-tests.exe'
        $sources = @(
            'Compiler/main.cpp', 'Semantics/MethodLower.cpp', 'Semantics/SymbolScopeBuilder.cpp',
            'Semantics/Resolver.cpp', 'Semantics/ReferenceChecker.cpp', 'Semantics/ClosureAnalyser.cpp',
            'Semantics/TypeInterner.cpp', 'Semantics/DeclTypeResolver.cpp', 'Compiler/compiler.cpp',
            'BytecodeVM/vm.cpp', 'Utils/NativeFunctionImpl.cpp', 'Utils/utils.cpp', 'Utils/FrontendJson.cpp',
            'Utils/PrettyAst.cpp', 'Utils/SymbolPrinter.cpp', 'Utils/DefaultVisitor.cpp', 'Core/lexer.cpp',
            'Core/newParser.cpp', 'Core/Ast.cpp', 'Core/value.cpp', 'CodegenIR/StringPool.cpp',
            'CodegenIR/IRBuilder.cpp', 'SSA/SSABuilder.cpp', 'MachineIR/MIRBuilder.cpp', 'X86Codegen/x86Builder.cpp'
        )
        Write-Host 'Building current compiler...'
        $build = Invoke-BoundedProcess 'g++' (@('-std=c++20') + $sources + @('-o', $compiler)) (Join-Path $buildDir 'compiler-build') 180000
        if ($build.Code -ne 0) { throw "Compiler build failed: $($build.Err)" }
    }
    # Check that the test runtime can distinguish balanced RC from actual errors.
    $calibration = Join-Path $buildDir 'trace-selftest.exe'
    $link = Invoke-BoundedProcess 'gcc' @('-g', '-O0', 'tests/runtime/rc_trace_selftest.c', 'tests/runtime/rc_trace_runtime.c', '-o', $calibration) (Join-Path $buildDir 'trace-selftest.link') 30000
    if ($link.Code -ne 0) { throw "Trace self-test link failed: $($link.Err)" }
    if ($Run) {
        foreach ($probe in @(@{ Mode='balanced'; Code=0; Message='live=0' }, @{ Mode='leak'; Code=98; Message='managed allocations leaked' }, @{ Mode='double-release'; Code=97; Message='already freed payload' })) {
            $result = Invoke-BoundedProcess $calibration @($probe.Mode) (Join-Path $buildDir ('trace-' + $probe.Mode)) 5000
            if ($result.Code -ne $probe.Code -or -not $result.Err.Contains($probe.Message)) {
                throw "Trace calibration failed: $($probe.Mode)`n$($result.Err)"
            }
            Write-Host "PASS trace checker $($probe.Mode)"
        }
    }
    $failures = [System.Collections.Generic.List[string]]::new()
    foreach ($test in $tests) {
        try {
        $base = Join-Path $buildDir $test.Name
        $source = Join-Path $repoRoot ('tests/rc/' + $test.Name + '.kmy')
        $compileArgs = @($source, '-asm', '--no-run', '-d', '-o', $base)
        if ($test.CheckOwnership -ne $false) { $compileArgs += '--check-ownership' }
        $compile = Invoke-BoundedProcess $compiler $compileArgs ($base + '.compile') 15000
        if ($test.ErrorPattern) {
            # Require a normal diagnostic exit; compiler crashes do not count as rejection.
            if ($compile.Code -ne 1 -or $compile.Err -notmatch $test.ErrorPattern) {
                throw "Expected diagnostic rejection: $($test.Name), exit=$($compile.Code). See $base.compile.stderr.log"
            }
            Write-Host "PASS rejection $($test.Name)"
            continue
        }
        if ($compile.Code -ne 0) {
            $tail = ($compile.Err -split '\r?\n' | Select-Object -Last 3) -join "`n"
            throw "Compile failed: $($test.Name)`n$tail`nFull logs: $base.compile.stderr.log"
        }
        $assembly = Get-Content -LiteralPath ($base + '.s') -Raw
        foreach ($operation in @(@{ Symbol='alloc'; Count=$test.Alloc }, @{ Symbol='retain'; Count=$test.Retain }, @{ Symbol='release'; Count=$test.Release })) {
            if ($null -eq $operation.Count) { continue }
            $count = [regex]::Matches($assembly, ('(?m)^\s*call kmy_rc_' + $operation.Symbol + '\s*$')).Count
            if ($count -ne $operation.Count) { throw "$($test.Name): expected $($operation.Count) $($operation.Symbol) call sites, got $count. See $base.s" }
        }
        # Do not modify/overwrite checked-in runtime.o. Link the test-only runtime.
        $executable = $base + '.exe'
        $link = Invoke-BoundedProcess 'gcc' @('-g', '-O0', ($base + '.s'), 'tests/runtime/rc_trace_runtime.c', '-o', $executable) ($base + '.link') 30000
        if ($link.Code -ne 0) { throw "Link failed: $($test.Name)`n$($link.Err)" }
        if ($Run) {
            $result = Invoke-BoundedProcess $executable @() $base 5000
            $actual = ($result.Err -replace "`r", '').Trim()
            $expected = (($test.Trace | ForEach-Object { 'RC ' + $_ }) -join "`n").Trim()
            $expectedOutput = ([string]$test.Output -replace "`r", '').Trim()
            $actualOutput = ($result.Out -replace "`r", '').Trim()
            if ($result.Code -ne 0 -or $actual -ne $expected -or $actualOutput -ne $expectedOutput) {
                throw "$($test.Name): RC lifecycle/output mismatch (exit $($result.Code)).`nExpected trace:`n$expected`nActual trace:`n$actual`nExpected stdout: $expectedOutput`nActual stdout: $actualOutput"
            }
            Write-Host "PASS runtime $($test.Name): counts, order, actual frees, live=0"
        } else {
            Write-Host "PASS compile/link $($test.Name) (not executed)"
        }
        } catch {
            if (-not $KeepGoing) { throw }
            $failures.Add("$($test.Name): $($_.Exception.Message)")
            Write-Host "FAIL $($test.Name): $($_.Exception.Message)"
        }
    }
    if ($failures.Count -gt 0) { throw ($failures -join "`n") }
} finally {
    Pop-Location
    [void][KmyRcSuiteErrorMode]::SetErrorMode($previousMode)
    Write-Host "Artifacts: $buildDir"
}
