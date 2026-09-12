# Capabilities Needed for a Self-Hosted Compiler

## Already usable

- loops and conditionals;
- recursion;
- closures and captured mutable state;
- classes, methods, and constructors;
- fixed-size heap arrays;
- string literals and pooled string constants;
- `strconcat`, `streq`, `strlen`, and `strByteAt`;
- `readFile` and `writeFile`;
- integer arithmetic and bit operations.

## High priority gaps

### Dynamic collections

A compiler cannot know the maximum number of tokens, AST nodes, symbols, or
diagnostics ahead of time. Fixed arrays are useful for tests but insufficient
for a real compiler.

Needed:

- resizable `ArrayList<T>` or equivalent;
- `push`, `pop`, `get`, `set`, and `length`;
- a way to allocate larger backing storage and copy elements;
- eventually generic collections.

### Maps and sets

Name resolution and string pooling need associative lookup.

Needed:

- `Map<string, Symbol>` or a temporary integer-keyed hash table;
- collision handling;
- deterministic iteration when emitting diagnostics or assembly.

### Better strings and bytes

The compiler needs efficient construction of diagnostics, identifiers, and
assembly output.

Needed:

- mutable/growable string builder;
- byte arrays;
- explicit UTF-8 versus byte semantics;
- efficient slicing or substring operations.

### Compiler errors

The compiler must report errors without crashing.

Needed:

- structured `Diagnostic` values;
- source spans with file, line, column, and length;
- an error/result convention;
- recovery for multiple parser errors where practical.

### Modules and command-line input

The first bootstrap can use one hardcoded source file, but a practical
compiler needs:

- command-line arguments;
- multiple source files;
- imports/modules;
- output-path selection.

### File/stream details

Whole-file I/O is enough for the first lexer. Later add:

- explicit file handles;
- `close`, `flush`, `seek`, and streaming reads;
- binary read/write;
- reliable error values.

### Testing and introspection

Add:

- string assertions;
- collection assertions;
- golden-file comparison;
- pass-level dump functions;
- deterministic compiler output.

## Important later gaps

- generics or a type-erased collection strategy;
- interfaces/traits for reusable compiler abstractions;
- enums/tagged unions for AST and token kinds;
- pattern matching;
- a principled ownership/GC strategy for compiler-created objects;
- a process/command API to invoke the assembler and linker;
- optimization and incremental compilation.

## What is not required for the first bootstrap

- a sophisticated optimizer;
- Unicode code-point iteration;
- multithreading;
- dynamic linking;
- a full standard library;
- true `i8`/`i16`/`u8`/`u32` machine-width semantics.
