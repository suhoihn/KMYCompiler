# Native module stress showcase

`main.kmy` intentionally exercises exported globals, functions, closures,
enums, arrays, and control flow through one import alias. The focused native
test `tests/x86/modules/cross_class_main.kmy` constructs and calls
`library::Counter`, proving that imported aggregate methods, constructors, and
field initializers use their compilation-wide native labels.

Expected runnable output is checked by `module_showcase` in the native test
runner. Its imported library includes an enum-backed ArrayList-style class,
array indexing, methods, constructors, nested control flow, closure mutation,
and a raw allocated `int*` round trip. The importing module also covers `%`,
`if`, `while`, and mutable locals.

Run with native code generation:

```powershell
kmyc Examples/ModuleShowcase/main.kmy -asm -o build/module_showcase.exe
```

This is also a compatibility probe for cross-module aggregate construction.
