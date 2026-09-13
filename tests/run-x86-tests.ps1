[CmdletBinding()]
param(
    [switch]$KeepArtifacts
)

$ErrorActionPreference = 'Stop'

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

    $tests = @(
        @{ Name = 'control_flow'; Source = 'tests/x86/control_flow.kmy'; Expected = 'tests/x86/control_flow.expected' },
        @{ Name = 'closures'; Source = 'tests/x86/closures.kmy'; Expected = 'tests/x86/closures.expected' },
        @{ Name = 'aggregates'; Source = 'tests/x86/aggregates.kmy'; Expected = 'tests/x86/aggregates.expected' },
        @{ Name = 'arrays'; Source = 'tests/x86/arrays.kmy'; Expected = 'tests/x86/arrays.expected' },
        @{ Name = 'dynamic_array'; Source = 'Examples/DynamicArray.kmy'; Expected = 'tests/x86/dynamic_array.expected' },
        @{ Name = 'data_structures'; Source = 'Examples/DataStructures.kmy'; Expected = 'tests/x86/data_structures.expected' },
        @{ Name = 'kmy_lexer'; Source = 'Examples/KmyLexer.kmy'; Expected = 'tests/x86/kmy_lexer.expected' },
        @{ Name = 'arraylist'; Source = 'Examples/ArrayList.kmy'; Expected = 'tests/x86/arraylist.expected' },
        @{ Name = 'conways_life'; Source = 'Examples/ConwaysLife.kmy'; Expected = 'tests/x86/conways_life.expected' },
        @{ Name = 'string_ops'; Source = 'Examples/StringOps.kmy'; Expected = 'tests/x86/string_ops.expected' },
        @{ Name = 'file_io'; Source = 'Examples/FileIO.kmy'; Expected = 'tests/x86/file_io.expected' },
        @{ Name = 'enum_x86'; Source = 'Examples/EnumX86.kmy'; Expected = 'tests/x86/enum_x86.expected' }
    )

    foreach ($test in $tests) {
        $source = Join-Path $repoRoot $test.Source
        $expectedPath = Join-Path $repoRoot $test.Expected
        $executable = Join-Path $buildDir ($test.Name + '.exe')

        Write-Host "Compiling $($test.Name)..."
        & $compiler $source -asm -o $executable *> $null
        if ($LASTEXITCODE -ne 0) { throw "Compilation failed: $($test.Name)" }

        $actualRaw = (& $executable 2>&1 | Out-String)
        $actual = [string]($actualRaw -join '')
        $actual = ($actual -replace "`r`n", "`n" -replace "`r", "`n").Trim()
        if ($LASTEXITCODE -ne 0) { throw "Native program crashed: $($test.Name)" }

        $expectedRaw = Get-Content -LiteralPath $expectedPath -Raw
        $expected = [string]($expectedRaw -join '')
        $expected = ($expected -replace "`r`n", "`n" -replace "`r", "`n").Trim()
        if ($actual -ne $expected) {
            throw "Output mismatch: $($test.Name)`nExpected:`n$expected`nActual:`n$actual"
        }

        Write-Host "PASS $($test.Name)"
    }
}
finally {
    Pop-Location
    if (-not $KeepArtifacts -and (Test-Path -LiteralPath $buildDir)) {
        Remove-Item -LiteralPath $buildDir -Recurse -Force
    }
}

Write-Host 'All x86 tests passed.'
