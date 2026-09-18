[CmdletBinding()]
param(
    [switch]$KeepArtifacts
)

$ErrorActionPreference = 'Stop'

# Inherit the no-error-dialog mode in native child processes, so a crash
# reports an exit code instead of leaving a Windows error window open.
Add-Type -TypeDefinition 'using System.Runtime.InteropServices; public static class KmyTestErrorMode { [DllImport("kernel32.dll")] public static extern uint SetErrorMode(uint mode); }'

$repoRoot = Split-Path -Parent $PSScriptRoot
$buildRoot = Join-Path $repoRoot 'build\x86-tests'
$buildDir = Join-Path $buildRoot ([Guid]::NewGuid().ToString())
$compiler = Join-Path $buildDir 'kmyc-x86-tests.exe'
$runtimeSource = Join-Path $repoRoot 'Runtime C Functions\runtime.c'
$runtimeObject = Join-Path $repoRoot 'Runtime C Functions\runtime.o'

New-Item -ItemType Directory -Path $buildDir | Out-Null

$compilerSources = @(
    'Compiler/main.cpp',
    'Semantics/MethodLower.cpp',
    'Semantics/SymbolScopeBuilder.cpp',
    'Semantics/Resolver.cpp',
    'Semantics/ClosureAnalyser.cpp',
    'Semantics/TypeInterner.cpp',
    'Semantics/DeclTypeResolver.cpp',
    'Compiler/compiler.cpp',
    'BytecodeVM/vm.cpp',
    'Utils/NativeFunctionImpl.cpp',
    'Utils/utils.cpp',
    'Utils/FrontendJson.cpp',
    'Utils/PrettyAst.cpp',
    'Utils/SymbolPrinter.cpp',
    'Utils/DefaultVisitor.cpp',
    'Core/lexer.cpp',
    'Core/newParser.cpp',
    'Core/Ast.cpp',
    'Core/value.cpp',
    'CodegenIR/StringPool.cpp',
    'CodegenIR/IRBuilder.cpp',
    'SSA/SSABuilder.cpp',
    'MachineIR/MIRBuilder.cpp',
    'X86Codegen/x86Builder.cpp'
) | ForEach-Object { Join-Path $repoRoot $_ }

Push-Location $repoRoot
try {
    Write-Host 'Building native runtime...'
    & gcc -c $runtimeSource -o $runtimeObject
    if ($LASTEXITCODE -ne 0) { throw 'Failed to build the native runtime.' }

    Write-Host 'Building compiler...'
    & g++ -std=c++20 @compilerSources -o $compiler
    if ($LASTEXITCODE -ne 0) { throw 'Failed to build the compiler.' }

    $helpText = & $compiler --help | Out-String
    if (-not $helpText.Contains('2026 (c) KMY')) {
        throw 'Help output is missing the KMY copyright line.'
    }
    Write-Host 'PASS help banner'

    $debugSource = Join-Path $repoRoot 'tests/x86/debug_tokens.kmy'
    $previousErrorAction = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    try {
        $debugText = (& $compiler $debugSource -d -asm --no-run -o (Join-Path $buildDir 'debug_tokens') 2>&1 | Out-String)
    }
    finally {
        $ErrorActionPreference = $previousErrorAction
    }
    if ($LASTEXITCODE -ne 0 -or
        -not $debugText.Contains('NullCoalesce') -or
        -not $debugText.Contains('Nullable') -or
        -not $debugText.Contains('ForceUnwrap') -or
        -not $debugText.Contains('Xor') -or
        -not $debugText.Contains('Parsed AST') -or
        -not $debugText.Contains('[Resolver] visit Let') -or
        -not $debugText.Contains('[Resolver] detail:')) {
        throw "Debug output failed: $debugText"
    }
    Write-Host 'PASS debug tokens and AST trace'

    $vmSource = Join-Path $repoRoot 'tests/x86/vm_smoke.kmy'
    foreach ($debugVm in @($false, $true)) {
        $vmStartInfo = [System.Diagnostics.ProcessStartInfo]::new()
        $vmStartInfo.FileName = $compiler
        $vmStartInfo.Arguments = '"' + $vmSource + '"' + $(if ($debugVm) { ' -d' } else { '' })
        $vmStartInfo.UseShellExecute = $false
        $vmStartInfo.CreateNoWindow = $true
        $vmStartInfo.RedirectStandardOutput = $true
        $vmStartInfo.RedirectStandardError = $true
        $vmProcess = [System.Diagnostics.Process]::new()
        $vmProcess.StartInfo = $vmStartInfo
        $previousErrorMode = [KmyTestErrorMode]::SetErrorMode(2)
        try {
            [void]$vmProcess.Start()
            $vmOutputTask = $vmProcess.StandardOutput.ReadToEndAsync()
            $vmErrorTask = $vmProcess.StandardError.ReadToEndAsync()
            if (-not $vmProcess.WaitForExit(5000)) {
                $vmProcess.Kill()
                [void]$vmProcess.WaitForExit(2000)
                throw 'VM smoke test timed out and was terminated.'
            }
            $vmOutput = [regex]::Replace($vmOutputTask.Result, [char]27 + '\[[0-9;]*m', '').Trim()
            $vmError = $vmErrorTask.Result
            if ($vmProcess.ExitCode -ne 0) {
                throw "VM smoke test failed: $vmOutput`n$vmError"
            }
            if ($debugVm) {
                if (-not $vmOutput.Contains('3') -or -not $vmError.Contains('[VM] detail:')) {
                    throw 'VM debug view lost the program result or retained diagnostics.'
                }
            } elseif ($vmOutput -ne '3') {
                throw "Normal VM output contains debug chatter: $vmOutput"
            }
        }
        finally {
            $vmProcess.Dispose()
            [void][KmyTestErrorMode]::SetErrorMode($previousErrorMode)
        }
    }
    Write-Host 'PASS VM program output and retained debug details'

    $tests = @(
        @{ Name = 'control_flow'; Source = 'tests/x86/control_flow.kmy'; Expected = 'tests/x86/control_flow.expected' },
        @{ Name = 'closures'; Source = 'tests/x86/closures.kmy'; Expected = 'tests/x86/closures.expected' },
        @{ Name = 'aggregates'; Source = 'tests/x86/aggregates.kmy'; Expected = 'tests/x86/aggregates.expected' },
        @{ Name = 'arrays'; Source = 'tests/x86/arrays.kmy'; Expected = 'tests/x86/arrays.expected' },
        @{ Name = 'pointers'; Source = 'tests/x86/pointers.kmy'; Expected = 'tests/x86/pointers.expected' },
        @{ Name = 'pointers_nasty'; Source = 'tests/x86/pointers_nasty.kmy'; Expected = 'tests/x86/pointers_nasty.expected' },
        @{ Name = 'malloc'; Source = 'tests/x86/malloc.kmy'; Expected = 'tests/x86/malloc.expected' },
        @{ Name = 'malloc_shadow'; Source = 'tests/x86/malloc_shadow.kmy'; Expected = 'tests/x86/malloc_shadow.expected' },
        @{ Name = 'dynamic_array'; Source = 'Examples/DynamicArray.kmy'; Expected = 'tests/x86/dynamic_array.expected' },
        @{ Name = 'data_structures'; Source = 'Examples/DataStructures.kmy'; Expected = 'tests/x86/data_structures.expected' },
        @{ Name = 'linked_list'; Source = 'Examples/LinkedList.kmy'; Expected = 'tests/x86/linked_list.expected' },
        @{ Name = 'kmy_lexer'; Source = 'Examples/KmyLexer.kmy'; Expected = 'tests/x86/kmy_lexer.expected' },
        @{ Name = 'kmy_parser'; Source = 'Examples/KmyParser.kmy'; Expected = 'tests/x86/kmy_parser.expected' },
        @{ Name = 'arraylist'; Source = 'Examples/ArrayList.kmy'; Expected = 'tests/x86/arraylist.expected' },
        @{ Name = 'string_int_hash_map'; Source = 'Examples/StringIntHashMap.kmy'; Expected = 'tests/x86/string_int_hash_map.expected' },
        @{ Name = 'conways_life'; Source = 'Examples/ConwaysLife.kmy'; Expected = 'tests/x86/conways_life.expected' },
        @{ Name = 'string_ops'; Source = 'Examples/StringOps.kmy'; Expected = 'tests/x86/string_ops.expected' },
        @{ Name = 'file_io'; Source = 'Examples/FileIO.kmy'; Expected = 'tests/x86/file_io.expected' },
        @{ Name = 'enum_x86'; Source = 'Examples/EnumX86.kmy'; Expected = 'tests/x86/enum_x86.expected' },
        @{ Name = 'modules'; Source = 'tests/x86/modules/main.kmy'; Expected = 'tests/x86/modules.expected' },
        @{ Name = 'module_showcase'; Source = 'Examples/ModuleShowcase/main.kmy'; Expected = 'tests/x86/module_showcase.expected' }
    )

    foreach ($test in $tests) {
        $source = Join-Path $repoRoot $test.Source
        $expectedPath = Join-Path $repoRoot $test.Expected
        $executable = Join-Path $buildDir ($test.Name + '.exe')

        Write-Host "Compiling $($test.Name)..."
        & $compiler $source -asm -o $executable *> $null
        if ($LASTEXITCODE -ne 0) { throw "Compilation failed: $($test.Name)" }

        $startInfo = [System.Diagnostics.ProcessStartInfo]::new()
        $startInfo.FileName = $executable
        $startInfo.UseShellExecute = $false
        $startInfo.CreateNoWindow = $true
        $startInfo.RedirectStandardOutput = $true
        $startInfo.RedirectStandardError = $true
        $process = [System.Diagnostics.Process]::new()
        $process.StartInfo = $startInfo
        $previousErrorMode = [KmyTestErrorMode]::SetErrorMode(2)
        try {
            [void]$process.Start()
            $stdoutTask = $process.StandardOutput.ReadToEndAsync()
            $stderrTask = $process.StandardError.ReadToEndAsync()
            if (-not $process.WaitForExit(5000)) {
                $process.Kill()
                [void]$process.WaitForExit(2000)
                throw "Native program timed out and was terminated: $($test.Name)"
            }
            if ($process.ExitCode -ne 0) {
                throw "Native program crashed: $($test.Name)`n$($stderrTask.Result)"
            }
            $actual = ($stdoutTask.Result -replace "`r`n", "`n" -replace "`r", "`n").Trim()
        }
        finally {
            $process.Dispose()
            [void][KmyTestErrorMode]::SetErrorMode($previousErrorMode)
        }

        $expectedRaw = Get-Content -LiteralPath $expectedPath -Raw
        $expected = [string]($expectedRaw -join '')
        $expected = ($expected -replace "`r`n", "`n" -replace "`r", "`n").Trim()
        if ($actual -ne $expected) {
            throw "Output mismatch: $($test.Name)`nExpected:`n$expected`nActual:`n$actual"
        }

        if ($test.Name -eq 'kmy_parser') {
            $astPath = Join-Path $repoRoot 'build\kmy_parser_ast.txt'
            $ast = (Get-Content -LiteralPath $astPath -Raw).Trim()
            if ($ast -ne $expected) {
                throw "AST file mismatch: $astPath"
            }
        }

        Write-Host "PASS $($test.Name)"
    }

    $rejectedTests = @(
        @{ Name = 'malloc_wrong_arg'; Source = 'tests/x86/malloc_wrong_arg.kmy'; Diagnostic = 'Argument type mismatch.' },
        @{ Name = 'malloc_opaque_deref'; Source = 'tests/x86/malloc_opaque_deref.kmy'; Diagnostic = 'Cannot dereference any*' }
    )
    foreach ($test in $rejectedTests) {
        $source = Join-Path $repoRoot $test.Source
        # These cases intentionally write diagnostics to stderr. Keep PowerShell
        # from treating that expected output as a terminating script error.
        $previousErrorAction = $ErrorActionPreference
        $ErrorActionPreference = 'Continue'
        try {
            $result = (& $compiler $source -asm --no-run -o (Join-Path $buildDir $test.Name) 2>&1 | Out-String)
        }
        finally {
            $ErrorActionPreference = $previousErrorAction
        }
        if ($LASTEXITCODE -eq 0 -or -not $result.Contains($test.Diagnostic)) {
            throw "Expected compile error not found: $($test.Name)`n$result"
        }
        Write-Host "PASS $($test.Name) (rejected)"
    }
}
finally {
    Pop-Location
    if (-not $KeepArtifacts -and (Test-Path -LiteralPath $buildDir)) {
        $resolvedRepoRoot = (Resolve-Path -LiteralPath $repoRoot).Path
        $resolvedBuildRoot = (Resolve-Path -LiteralPath $buildRoot).Path
        $resolvedBuildDir = (Resolve-Path -LiteralPath $buildDir).Path
        if ((Split-Path -Parent $resolvedBuildRoot) -ne (Join-Path $resolvedRepoRoot 'build') -or
            (Split-Path -Parent $resolvedBuildDir) -ne $resolvedBuildRoot) {
            throw "Refusing to remove unexpected test directory: $resolvedBuildDir"
        }
        Remove-Item -LiteralPath $buildDir -Recurse -Force
    }
}

Write-Host 'All x86 tests passed.'
