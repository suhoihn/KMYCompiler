# KMY Module Runtime Design

## Current native import support

Imports are compile-time aliases:

```kmy
import "math.kmy" as math;
math::triple(7);
```

The native x86 pipeline currently combines every imported module's functions
into one output file. Qualified types, enum variants, and functions work.
Qualified module variables such as `math::x` do not yet work.

## Planned unified module-value lowering

Do not lower an imported function specially. Every module-level runtime value
will receive one program-wide global slot, whether it is a variable, constant,
or function closure.

```text
math::x       -> LOAD_GLOBAL slot(math::x)
math::triple  -> LOAD_GLOBAL slot(math::triple)
math::x = 4   -> STORE_GLOBAL slot(math::x), 4
```

A function declaration is initialized once by creating its closure and storing
it in its slot:

```text
closure = FUNC_LABEL triple, module_environment
STORE_GLOBAL slot(math::triple), closure
```

This replaces the temporary native special case that materializes an imported
function directly with `FUNC_LABEL` and a null environment. It also permits a
function to capture module-level state after that state has a persistent home.

## Module initialization

Before user `main` begins, generated startup code must run each module's
top-level initializers exactly once in dependency order:

```text
initialize math
initialize collections
initialize application
call application main
```

This gives top-level `let` values storage, initializes function closures, and
defines deterministic behavior for imports. Cyclic imports should remain a
compile-time error until KMY deliberately specifies partially initialized
modules.

## Removing the synthetic root function

Today the parser represents each source file as an implicit zero-argument
`FunctionExpr`. That was a convenient early implementation, but it makes
module globals look like locals of a hidden function.

The better end state is:

```text
Module
  imports
  top-level declarations
  top-level initializer statements
  global scope
```

Named functions remain `FunctionExpr` nodes. A module gets an explicit
compiler-generated initializer function only during code generation; it is not
an AST function and does not own the module's lexical scope. This cleanly
separates local variables from persistent module globals.

Suggested migration order:

1. Change `Module.program` into top-level declarations/statements plus its
   global scope.
2. Make semantic passes visit module contents directly rather than a root
   `FunctionExpr`.
3. Assign global slots to module-level `VarSymbol`s.
4. Generate one hidden native initializer per module and run them in dependency
   order before generated `main`.
5. Lower qualified and unqualified module values through `LOAD_GLOBAL` and
   `STORE_GLOBAL`.
6. Delete the imported-function `FUNC_LABEL` special case once globals work.
