# Records/classes in the parser and bytecode VM

This note describes the aggregate pipeline currently present in the repository. `record` and `class` use the same AST representation (`Aggregate`); the `AggregateKind` is retained, but the active bytecode representation is mostly a `Record` containing fields.

## 1. Parsing and desugaring

`Parser::parse_aggregate` handles `class Name { ... }` and `record Name { ... }`.

- `let field: T = expression;` becomes a `FieldMember`.
- `fun method(...) { ... }` becomes a `MethodMember`.
- `init(...) { ... }` becomes a `ConstructorMember`.
- While parsing fields, an assignment of the form `this.field = initializer` is also added to a synthetic statement list.
- After the closing brace, the parser creates `Aggregate::fieldInitFunc`, a synthetic function whose body is that statement list.

Thus field initialization is not emitted as a class-level declaration. It is represented as an ordinary function that receives the object as its first argument and assigns each initialized field.

One detail worth remembering: the parser moves the initializer into the synthetic assignment, then moves `letStmt->expr` into `FieldMember`, so `FieldMember::initialiser` is normally empty for initialized fields. The synthetic function is the effective source of field initialization in the VM path.

## 2. Semantic passes and implicit `this`

The compiler runs these relevant passes in order:

1. `SymbolScopeBuilder` creates the aggregate type/scope and field/method/constructor symbols.
2. For every method, constructor, and `fieldInitFunc`, it prepends a parameter named `$implicit_this_param` and marks it `implicitThis`.
3. `DeclTypeResolver` gives that parameter the aggregate's `InstanceType` and builds method/constructor signatures.
4. `Resolver` resolves field offsets, method metadata, constructor arguments, and `new`'s aggregate type.
5. `MethodLower` rewrites `obj.method(a)` into a normal function call with `obj` supplied as the implicit receiver argument.
6. `ClosureAnalyser` allocates the implicit parameter as local slot 0 (unless it is captured by a nested closure). `this` expressions resolve to that local or to an upvalue when nested.

The VM does not have a special `this` register. `this` is simply the first ordinary function parameter. A method body loads it with `LOAD_LOCAL 0`; a nested closure can load it through `LOAD_UPVALUE`.

## 3. Compiling aggregate members

`Compiler::visit(Aggregate)` emits function prototypes for:

- methods;
- the synthetic field-initializer function; and
- constructors.

The field-initializer prototype is recorded in `fieldInitFuncProtoIdx`. Constructor prototypes are recorded on their symbols. No aggregate object is emitted at the declaration site; the declaration only prepares prototypes and metadata.

When compiling a constructor, the compiler marks `isConstructor`. If the constructor body falls through, `visit(FunctionExpr)` emits:

```text
LOAD_LOCAL 0
RETURN_VALUE
```

So a constructor returns the same object passed as its implicit first argument.

## 4. What `new` emits

For `new Type(arg0, arg1, ...)`, `Compiler::visit(NewExpr)`:

1. Finds a constructor whose function signature has one extra parameter for implicit `this`.
2. Pushes a closure for the user constructor.
3. Allocates `fieldCount` uninitialized values with `PUSH_UNINITIALISED` and packages them with `MAKE_RECORD`.
4. Duplicates the record, then pushes the field-initializer closure.
5. Uses `SWAP` so the field initializer is called with the duplicate record as its one argument.
6. Calls the field initializer, then pops its void/garbage result.
7. Pushes the explicit constructor arguments.
8. Calls the user constructor with `argCount + 1` arguments: the initialized record first, followed by the user arguments.

The intended stack choreography is:

```text
[constructor] [empty record]
[constructor] [filled record]
[constructor] [filled record] [explicit args...]
```

The final constructor result is the record returned from slot 0.

## 5. Field and method access

The bytecode compiler uses the resolver-assigned numeric field offset:

- `obj.field` → evaluate `obj`, then `GET_PROPERTY offset`;
- `obj.field = value` → evaluate value and object, then `SET_PROPERTY offset`.

At runtime, `MAKE_RECORD` creates a `Record` containing a `std::vector<Value>`. `GET_PROPERTY` and `SET_PROPERTY` index that vector directly. `Record` values are held through `RecordPtr`; `Value::clone()` recursively clones nested records when value-copy semantics are requested.

For methods, `MethodLower` appends the receiver to the call's argument vector. Because normal call arguments are emitted in reverse order, the receiver ends up as argument/slot 0, matching the implicit parameter.

## 6. Important current limitations

The design has two partially overlapping runtime models:

- The active bytecode constructor path uses `Record` plus explicit field-init and constructor calls.
- `Opcode::MAKE_INSTANCE` and `Opcode::INIT_RECORD` exist, but their VM cases are currently empty. The current `new` compiler path does not use them; it uses `MAKE_RECORD` and ordinary `CALL` instead.
- The older interpreter has a `ClassObj` model with copied `fieldDefaults` and bound methods, but its `visit(NewExpr)` currently creates an object and contains a `TODO` for constructor invocation.

Therefore, the conceptual lifecycle is already present in the bytecode compiler, but nominal instance opcodes and the legacy interpreter's constructor execution are not complete implementations. Any new struct/class work should first decide whether to preserve the current “record + synthetic initializer + constructor” ABI or replace it with the dormant instance opcodes.

## Source map

- Parser/desugaring: `Core/newParser.cpp`, `parse_aggregate`
- AST aggregate shape: `Core/Ast.hpp`, `Aggregate`, `FieldMember`, `ConstructorMember`
- Implicit receiver insertion: `Semantics/SymbolScopeBuilder.cpp`
- Type/signature setup: `Semantics/DeclTypeResolver.cpp`
- Resolver and `new` typing: `Semantics/Resolver.cpp`
- Method lowering: `Semantics/MethodLower.cpp`
- Closure/slot resolution: `Semantics/ClosureAnalyser.cpp`
- VM bytecode emission: `Compiler/compiler.cpp`
- VM execution: `BytecodeVM/vm.cpp`
- Runtime value representation: `Core/value.hpp`
