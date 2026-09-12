# KMY Self-Hosting Compiler Plan

This directory describes how to eventually build a KMY compiler written in
KMY itself. The goal is not to copy the current C++ implementation line for
line, but to define a small, testable compiler that can produce KMY programs
without depending on the C++ compiler at runtime.

## Current starting point

KMY already has the beginnings of the required foundation:

- functions, recursion, closures, and captured mutable variables;
- conditionals, `while`, parser-lowered `for`, `break`, and `continue`;
- classes, constructors, methods, and mutable fields;
- fixed-size heap arrays and indexing;
- strings, string pooling, concatenation, equality, length, and byte access;
- whole-file text I/O through `readFile` and `writeFile`;
- a native x86-64 backend and an automated test runner.

The compiler should initially target a simple KMY source-to-source or textual
IR output before attempting to emit machine code directly.

## Recommended bootstrap target

The first self-hosted compiler should read a KMY source file and emit a simple
portable intermediate representation or assembly text file. It can then call
the existing `kmyc` tool to assemble that output. Once the self-hosted front
end is reliable, replace the external step with a KMY implementation of the
backend.

This separates language bootstrapping from machine-code generation.

## Compiler layers

```text
source text
  -> lexer
  -> parser
  -> AST
  -> name/scope resolver
  -> type checker
  -> closure analysis
  -> HIR
  -> SSA (later)
  -> MIR (later)
  -> assembly/text output
```

Each layer should have its own input/output tests and should not reach into
the implementation details of later layers.

## Stages

### Stage 0: Stabilize the host language

Before writing the compiler, add the missing library and language foundations
listed in `CAPABILITIES.md`.

### Stage 1: Lexer

Implement a KMY lexer in KMY that supports:

- identifiers and keywords;
- integer, floating-point, boolean, and string literals;
- escapes in strings;
- operators and punctuation;
- `//` comments;
- line/column error reporting.

The lexer should return a dynamic sequence of token records.

### Stage 2: Parser

Implement recursive descent or Pratt parsing for the current grammar:

- expressions and precedence;
- calls, indexing, and field access;
- declarations and assignments;
- blocks and control flow;
- functions and closures;
- classes, fields, constructors, and methods.

Start with a parser that produces a small AST sufficient for arithmetic,
functions, and control flow. Add aggregates and closures afterward.

### Stage 3: Name resolution

Implement lexical scopes and symbol tables:

- globals and locals;
- declaration-before-use rules;
- shadowing;
- function parameters;
- fields and methods;
- captured variables.

Use explicit symbol IDs instead of relying on raw string names in later passes.

### Stage 4: Type checking

Implement the current type rules first:

- `int`, `double`, `bool`, `string`, `void`;
- arrays and function types;
- class instances;
- assignment compatibility;
- call arity and argument checks;
- indexing checks;
- return-type checks.

Add `Result<T>`-style compiler errors before attempting advanced diagnostics.

### Stage 5: Initial code generation

Generate a simple textual representation from the AST. Do not begin with SSA.
The first target can be a structured three-address IR with explicit labels:

```text
v0 = const 1
v1 = add v0, v0
return v1
```

This makes debugging and self-hosted testing much easier.

### Stage 6: Self-hosted testing

The KMY compiler should compile small KMY programs and compare their output
against checked-in expected files. Include golden tests for every compiler
pass, not only end-to-end executable tests.

### Stage 7: Native backend

Once the front end and textual IR are stable, implement:

- HIR construction;
- MIR lowering;
- x86 instruction emission;
- string/constant data emission;
- runtime call emission;
- object/array layout.

The self-hosted backend can initially emit assembly and invoke GCC through a
native helper. Direct process execution can be added later.

### Stage 8: Bootstrap milestone

The first meaningful bootstrap is:

1. C++ `kmyc` compiles `kmyc.kmy`.
2. The resulting KMY compiler compiles a small test program.
3. The generated program produces the expected output.
4. The KMY compiler compiles itself again.

Only after this fixed-point test should the C++ compiler be considered
replaceable.

## Design rule

Keep compiler data structures explicit and boring. A token, AST node, type,
symbol, instruction, and diagnostic should each have a clear representation.
Avoid using high-level language features in the compiler before they are
covered by native tests.
