# Bootstrap Strategy

## Bootstrap 1: KMY front end, existing backend

Write a KMY lexer, parser, resolver, and type checker. Have it emit a simple
textual IR or generated KMY source that the existing C++ `kmyc` can compile.

This proves that KMY is expressive enough to describe its own front end
without requiring the self-hosted compiler to solve native code generation
immediately.

## Bootstrap 2: KMY HIR generator

Move HIR construction into KMY. The existing C++ tool can consume the emitted
HIR dump while the new implementation is compared against it using golden
tests.

## Bootstrap 3: KMY native backend

Implement MIR and x86 assembly generation in KMY. Initially write `.s` files
and use a small native process helper to invoke GCC/MinGW.

## Fixed-point test

The project is self-hosting only when this cycle works:

```text
C++ kmyc -> kmyc.kmy -> KMY compiler -> kmyc.kmy -> same result
```

Compare generated textual IR or assembly after normalizing harmless labels
and temporary IDs. Exact binary equality is not required initially.

## Recommended first compiler subset

The first compiler written in KMY should support only:

- integer and string literals;
- token arrays and dynamic lists;
- functions and recursion;
- conditionals and loops;
- records/classes for token and AST nodes;
- file reading and writing;
- diagnostics.

Do not make the first version compile all of KMY. A small compiler subset can
compile the next larger version of itself incrementally.
