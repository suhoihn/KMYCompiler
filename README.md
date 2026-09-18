# KMY

KMY is a work-in-progress programming language implemented in C++. It has a
shared front end, a stack-based VM backend, and an experimental native x86-64
backend for Windows/MinGW.

## Pipeline

```text
.kmy source
  -> lexer / Pratt parser
  -> scope and type resolution
  -> closure analysis and slot allocation
  -> HIR -> SSA -> MIR
  -> x86-64 assembly -> GCC/MinGW executable
```

The native backend currently targets 64-bit Windows using the MinGW toolchain.
It supports integer arithmetic, functions, closures, mutable captured values,
conditionals, `while`, parser-lowered `for`, `break`, and `continue`. Arrays,
records/classes, and several type-system features are still incomplete in the
native backend.

Enum variants are represented as ordinal integer constants on x86, matching
the VM behavior; enum payloads and pattern matching are not implemented yet.

Rust-style native-width aliases are accepted: `i64` and `u64` currently map to
KMY's existing integer representation, `f64` maps to `double`, and `byte` maps
to the current integer representation. True width-specific and unsigned
arithmetic will require a later backend/layout update.

## Prerequisites

- A C++20-capable `g++`
- MinGW `gcc` for assembling/linking x86 output
- PowerShell 7+ for the automated x86 test runner

## Build and run

The batch files are historical helpers. The reproducible command used by the
test suite is in `tests/run-x86-tests.ps1`.

To run one program after building `kmyc.exe`:

```powershell
.\kmyc.exe .\Examples\SimpleASMTest.kmy -asm -o .\build\simpleasm.exe
.\build\simpleasm.exe
```

`-asm` writes an adjacent `.s` assembly file and invokes GCC unless
`--no-run` is supplied. `-o` selects the executable path.

## Source modules

KMY supports compile-time source modules in the native x86 pipeline. The
syntax is `import "path/to/file.kmy" as Alias;`,
followed by references such as `Alias::Type`. Imports will be compile-time
dependencies, not runtime values or C-style text inclusion. They will be
allowed only at the start of a file, before other statements. See
[`grammar.md`](grammar.md) for the rule.

There is no `export` keyword yet: module-level type declarations and functions
are visible to an importing module by default. Native x86 imports currently
support imported types, enum variants, and functions such as
`math::triple(7)`. Imported module variables are deliberately not supported
yet: `let x = 3;` in `math.kmy` cannot currently be read as `math::x`.
Supporting that requires a real module-global storage and initialization model,
which is separate from compiling callable functions. The stack-VM backend does
not yet compile imports. Consequently, an imported function must not capture a
top-level module variable until that storage/initialization model exists.

Import paths will resolve relative to the **file containing the import**,
regardless of where `kmyc` is launched. For example, an import of
`"lib/math.kmy"` in `src/main.kmy` will find `src/lib/math.kmy`. An import
inside that library will resolve relative to `src/lib/` in turn. Use `/` in
source paths for portability.

## Automated native tests

Run the x86 suite from the repository root:

```powershell
pwsh -File .\tests\run-x86-tests.ps1
```

The runner builds the compiler and runtime, then for each test it compiles with
`-asm`, runs the native executable, and compares its standard output with the
matching `.expected` file. Artifacts are placed under `build\x86-tests`.

Current native coverage includes:

- nested `if` / `else if` / `else`
- `while` and parser-lowered `for` loops, including nested branches and `%`
- nested closures, multiple independent closures, and mutation of captured
  locals and upvalues

The existing `Examples/SimpleASMTest.kmy` also exercises `continue`. More
complex `break`/`continue` paths that require loop-carried SSA values still
need work in the SSA renamer and are not yet part of the automated suite.

## Project layout

- `Core/` — AST, tokens, lexer, parser, symbols, and common data structures
- `Semantics/` — symbol scopes, types, resolution, method lowering, closures
- `CodegenIR/`, `SSA/`, `MachineIR/` — native-backend intermediate forms
- `X86Codegen/` — x86-64 assembly emitter
- `BytecodeVM/` — VM runtime and bytecode execution
- `Compiler/` — compiler command-line entry point and VM code generator
- `Runtime C Functions/` — native runtime functions called by generated code
- `Examples/` — exploratory language programs
- `tests/x86/` — native regression programs and expected output

## Status

This is an actively evolving compiler. Treat the test suite as the supported
native-backend contract; a successful compile of an untested language feature
does not yet guarantee correct native code.
