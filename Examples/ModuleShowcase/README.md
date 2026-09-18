# Native module stress showcase

`main.kmy` intentionally exercises exported globals, functions, closures,
enums, arrays, and control flow through one import alias. `library.kmy` also
declares a class. `cross_module_class_not_yet.kmy` is a retained negative probe:
the parser and resolver now accept `new library::Counter(...)`, but native
codegen still lacks cross-module constructor/field-initializer lookup.

Expected runnable output is checked by `module_showcase` in the native test
runner. Its imported library includes an enum-backed ArrayList-style class,
array indexing, methods, constructors, nested control flow, closure mutation,
and a raw allocated `int*` round trip. The importing module also covers `%`,
`if`, `while`, and mutable locals.

Run with native code generation:

```powershell
kmyc Examples/ModuleShowcase/main.kmy -asm -o build/module_showcase.exe
```

This is also a compatibility probe: if a cross-module capability is unfinished,
the compiler error identifies the next native module boundary to implement.
