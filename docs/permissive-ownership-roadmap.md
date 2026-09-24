# KMY Permissive Ownership and Memory Roadmap

## Goal

KMY should feel closer to C++ or Kotlin than to Rust:

- ordinary application code should be pleasant and mostly automatically managed;
- destruction should be deterministic rather than controlled by a tracing garbage collector;
- borrowing should prevent dangling references and use-after-move errors;
- common aliasing and mutation patterns should not be rejected merely because Rust would reject them;
- low-level programs may deliberately use raw pointers, `malloc`, and `free`;
- programmers should not need lifetime syntax for ordinary code.

This is a deliberate hybrid of **explicit reference counting (RC)**, a **permissive borrow checker (BC)**, and **manual memory control (MC)**. RC is implemented first for explicitly shared objects. Unique ownership and borrowing remain the intended fast default model. Manual memory is an escape hatch, not the default way to write every program.

## Honest safety boundary

KMY can make its **safe subset** memory-safe without reproducing Rust's full borrow checker.

Safe code should prevent:

- use after free;
- double destruction;
- use after move;
- returning or storing a reference that outlives its owner;
- invalidating a live interior reference through collection relocation;
- leaking normally owned values on ordinary control-flow exits.

The language cannot guarantee those properties after a value is deliberately converted to a raw pointer or passed through unchecked manual-memory operations. `*T`, `&raw`, `malloc`, `free`, foreign calls, and future unsafe blocks form the explicit unsafe boundary.

Strong reference-counted cycles are also not automatically reclaimed. Until weak references exist, they are a documented source of leaks rather than memory corruption.

Permitting aliases also means that the first model does not automatically provide Rust's data-race guarantee. Concurrency must eventually add synchronization rules or require unsafe code for shared writable state.

## Core kinds of values

```kmy
Foo           // unique owning value; moves
shared Foo    // reference-counted shared owner
weak Foo      // future non-owning link to a shared object
&Foo          // non-owning, read-only reference
&mut Foo      // non-owning reference with write permission
*Foo          // unchecked raw pointer
```

### Owning values

An owning value is responsible for destroying its resource. Non-copy owners move by default:

```kmy
var first = new Foo();
var second = first;       // ownership moves to second
use(first);               // error: first was moved
```

The compiler automatically destroys `second` when its lifetime ends.

Primitive values such as `int`, `byte`, `bool`, and ordinary value-like enums are copied instead:

```kmy
var first = 10;
var second = first;
use(first);               // valid
```

### Shared owners

`shared T` provides explicit reference-counted ownership when multiple independent owners are genuinely required:

```kmy
var first: shared Foo = shared new Foo();
var second = first;       // retain: both remain valid
```

The first implementation uses non-atomic counts and is single-threaded. Copying a shared handle retains it, destroying a shared handle releases it, and reaching zero destroys the object. Strong cycles leak until `weak T` is implemented. Ordinary `T` must not silently acquire permanent shared/RC semantics.

### Planned self-hosted `Shared<T>`

The initial implementation treats shared ownership as a compiler/runtime intrinsic and represents it explicitly with `ALLOC_SHARED`, `RETAIN`, and `RELEASE` operations. This is a bootstrap design, not the intended permanent location of the RC algorithm.

Once KMY supports the necessary language facilities, `shared T` is planned to become syntax sugar for a standard-library `Shared<T>` implemented in KMY itself. Required facilities include:

- inline records/structs with predictable layout;
- generics and monomorphization;
- deterministic user-defined `drop` behavior;
- copy/clone and move protocols;
- operator or member-access forwarding where needed;
- function and method overloading;
- raw layout operations, pointer casts, and restricted unsafe pointer offsets;
- stable exported function symbols;
- compiler-generated type-specific destruction glue;
- atomic primitives if thread-safe shared ownership is later added.

At that stage, high-level ownership lowering should prefer generic `COPY_VALUE`, `MOVE_VALUE`, `DROP_VALUE`, and ordinary function calls. `Shared<T>`'s copy logic will retain, its move logic will transfer the handle without changing the count, and its drop logic will release. The current RC-specific operations may then be removed from HIR, retained only as lower-level optimization intrinsics, or produced by recognizing/inlining the KMY standard-library implementation.

The compiler still remains responsible for inserting generic copy, move, and drop actions on every control-flow path. Self-hosting the RC algorithm does not remove that compiler responsibility; it moves the implementation of shared ownership policy into KMY.

### Shared references

`&T` permits reading without taking ownership:

```kmy
fun inspect(value: &Foo) {
    print(value.id);
}

inspect(&foo);
```

### Mutable references

`&mut T` grants write permission, but it does **not initially promise unique access**:

```kmy
fun update(value: &mut Foo) {
    value.count++;
}
```

KMY may allow multiple references to the same object when doing so does not produce a lifetime or invalidation error. This is the major deliberate difference from Rust.

The compiler therefore must not silently treat ordinary `&mut T` as C `restrict` or backend `noalias`. A separate compiler-verified exclusive reference can be considered later if optimization genuinely needs one.

### Raw pointers

`*T` provides C-like access:

```kmy
var pointer: *Foo = &raw foo;
```

Raw pointers may support pointer arithmetic, explicit allocation, and explicit freeing. The safe checker does not claim that raw-pointer code is memory-safe.

## Initial reference restrictions

The first useful version should keep safe references deliberately simple. A reference may be used as a temporary argument or parameter, but it may not initially be:

- returned from a function;
- stored in an object or array;
- assigned to a global;
- captured by a closure;
- kept across an operation that can relocate its target.

For example:

```kmy
inspect(&foo);            // supported first
modify(&mut foo);         // supported first
```

The borrow ends when the call returns. This gives KMY safe, convenient parameter passing without requiring general lifetime inference immediately.

## Function conventions

Use the type to state the function's ownership requirement:

```kmy
fun consume(value: Foo) {}       // takes ownership
fun inspect(value: &Foo) {}      // temporarily reads
fun modify(value: &mut Foo) {}   // temporarily writes
fun dangerous(value: *Foo) {}    // unchecked raw access
```

Calls should initially remain explicit:

```kmy
consume(foo);
inspect(&foo);
modify(&mut foo);
dangerous(&raw foo);
```

Implicit borrowing can be added later as ergonomic sugar after the rules and diagnostics are stable.

## Automatic deterministic cleanup

Normal owners should behave like C++ values with compiler-generated cleanup:

```kmy
fun example() {
    var value = new Foo();
} // compiler destroys value here
```

The compiler must insert destruction on:

- normal block exit;
- `return`;
- `break`;
- `continue`;
- every applicable branch;
- future error-propagation exits.

A moved value is not destroyed by its old binding. Objects recursively destroy owned fields, while borrowed and raw fields do not own their targets. Owning arrays destroy their non-copy elements before releasing their backing memory.

This automatic cleanup is the normal memory-management path. `free` is for explicitly raw/manual allocations and unsafe interoperation.

## What the permissive borrow checker checks

The checker should track a small set of properties rather than duplicate every Rust rule:

1. Whether each owner is uninitialized, live, or moved.
2. Whether a reference's owner remains alive for every use.
3. Whether `&mut` was created from writable storage.
4. Whether a move or destruction would leave a live safe reference dangling.
5. Whether a collection operation can invalidate a live interior reference.
6. Where a returned reference originated once returned references are supported.

The checker does not initially reject two references merely because they alias:

```kmy
fun editBoth(first: &mut Foo, second: &mut Foo) {}

editBoth(&mut foo, &mut foo); // may be legal in permissive KMY
```

This freedom costs some optimization opportunities and requires separate concurrency rules, but it avoids much of Rust's restrictiveness.

## Cyclic structures

KMY will not add a built-in region/arena ownership construct. Unique ownership naturally handles trees and singly owned chains, but arbitrary cycles require a different representation.

Supported design choices are:

- indexes or generational handles into an owning collection;
- explicit `shared T` with future `weak T` back-links;
- raw/manual pointers when the programmer accepts unsafe lifetime management;
- redesigning the ownership graph so it is acyclic.

Strong-only RC cycles intentionally leak in the first RC version:

```kmy
var first: shared Node = shared new Node();
var second: shared Node = shared new Node();

first.next = second;
second.next = first;      // strong cycle: documented leak
```

The compiler is not required to detect arbitrary runtime cycles. Later, `weak T` can break cycles explicitly. Programs that do not want RC overhead should use indexes/handles or unique ownership.

## Future returned references

Returned references can be added without exposing Rust lifetime parameters in source code. The compiler records reference provenance in function metadata:

```kmy
fun identity(value: &Foo): &Foo {
    return value;
}
```

The compiler records that the return value originates from parameter 0. The caller must keep that argument alive while the result remains live.

This must be rejected:

```kmy
fun broken(): &Foo {
    var local = new Foo();
    return &local;              // error: local is destroyed on return
}
```

Start with references originating from exactly one parameter or a global. Reject ambiguous cases until provenance analysis becomes mature.

## Collections and invalidation

Element references introduce another lifetime problem:

```kmy
var element = &items[0];
items.append(value);            // may relocate the backing array
use(element);                   // could otherwise dangle
```

Collection methods should eventually declare whether they can structurally invalidate references. While an interior reference is live, the checker rejects operations such as `append`, `remove`, `reserve`, or `clear` when they may relocate or destroy the referenced element.

Simple element mutation may remain legal when it does not invalidate the address.

## Concurrency

Because ordinary `&mut` is not exclusive, KMY cannot infer data-race freedom from reference types alone. Before safe shared-memory concurrency is introduced, choose one of these policies:

1. require synchronization-backed types for cross-thread mutation;
2. allow only immutable shared references across threads;
3. mark unchecked shared mutation unsafe.

This decision is independent of single-threaded lifetime safety and should not delay the initial ownership implementation.

## Implementation roadmap

### Stage 1: specify the model

1. Freeze the meanings of `T`, `shared T`, future `weak T`, `&T`, `&mut T`, and `*T`.
2. Keep unique, shared, borrowed, weak, and raw ownership kinds distinct internally.
3. Document that the first RC implementation is non-atomic and that strong cycles leak.
4. List which built-in types are copied and which are owned.
5. Define the raw/manual safety boundary.
6. Add small accepted and rejected language examples as specification tests.

### Stage 2: explicit RC foundation

1. Add `Shared` to the internal ownership kinds.
2. Choose and parse explicit `shared T` ownership syntax.
3. Add an allocation header containing a non-atomic strong count and destruction metadata.
4. Implement runtime retain and release operations.
5. Retain on shared-handle copies and release on overwrite and scope exit.
6. Retain a replacement before releasing the old value so self-assignment is safe.
7. Handle shared arguments, returns, fields, nullable values, arrays, and closure captures.
8. Add debug counters and tests for exact retain/release balance.
9. Document that strong cycles leak; do not attempt runtime cycle detection.

### Stage 3: finish type representation

1. Represent unique, shared-owner, shared-reference, mutable-reference, weak, and raw kinds explicitly.
2. Add `isCopyType`, `isOwnedType`, `isReferenceType`, and `isRawPointer` helpers.
3. Define reference equality and assignability.
4. Produce readable diagnostics containing the actual source types.

### Stage 4: complete reference syntax and semantic checking

1. Parse `&T`, `&mut T`, and `*T`.
2. Parse `&value`, `&mut value`, `&raw value`, and raw dereference.
3. Require address-taking operands to be lvalues.
4. Require `&mut` operands to be mutable.
5. Keep the existing pointer pipeline explicitly classified as raw.

### Stage 5: unique ownership-state checking

1. Track uninitialized, live, and moved local owners.
2. Reject straightforward use after move.
3. Merge ownership state across `if` and `else`.
4. Conservatively handle loops.
5. Track moves through function arguments, returns, fields, and module boundaries.
6. Improve diagnostics to identify where a value was moved.

### Stage 6: call-scoped borrowing

1. Check shared-reference arguments.
2. Check mutable-reference arguments.
3. Keep the owner alive through the call.
4. End temporary borrows when the call returns.
5. Reject reference escape through returns, fields, globals, arrays, and closures.
6. Permit safe aliasing rather than enforcing Rust-style exclusivity.

### Stage 7: automatic destruction

1. Add a HIR destruction operation.
2. Lower destruction into the native pipeline.
3. Insert cleanup at normal block exits.
4. Insert cleanup before `return`, `break`, and `continue`.
5. Do not destroy moved values at their former binding.
6. Generate recursive cleanup for owned fields and array elements.
7. Test nested scopes, branches, loops, early returns, methods, and modules.

### Stage 8: raw/manual memory boundary

1. Keep `malloc`, `free`, raw dereference, and pointer arithmetic available.
2. Clearly document that their misuse is outside safe-code guarantees.
3. Add `unsafe` blocks or equivalent annotations later.
4. Make safe-to-raw conversions visually explicit with `&raw`.

### Stage 9: advanced references

1. Add inferred provenance for simple returned references.
2. Permit references derived from one parameter or a global.
3. Reject references to locals and ambiguous origins.
4. Add collection invalidation metadata.
5. Permit stored references only after object lifetime relationships can be checked.
6. Add closure reference captures only after escape analysis exists.

### Stage 10: explicit weak ownership and graph guidance

1. Add `weak T` for non-owning links to shared allocations.
2. Ensure weak handles do not increase the strong count.
3. Define safe weak-to-shared upgrade behavior.
4. Add examples of strong forward links and weak back-links.
5. Provide standard generational handles for graphs that should avoid RC.
6. Keep raw/manual memory available for specialized graph layouts.

### Stage 11: optimization and concurrency

1. Define cross-thread ownership transfer.
2. Define synchronized shared access.
3. Consider a separate exclusive/no-alias reference only when useful.
4. Never lower ordinary aliasable `&mut` to `restrict` or `noalias`.

## Immediate next work

Implement explicit RC first:

1. add an internal `Shared` ownership kind without changing ordinary `T` into permanent RC;
2. settle the explicit `shared T` syntax;
3. implement non-atomic retain and release runtime functions;
4. release shared locals on every scope exit;
5. handle copy assignment, overwrite, self-assignment, arguments, and returns;
6. add tests that count allocations, retains, releases, and destructions;
7. document and test that strong cycles remain allocated;
8. stop and review before adding shared fields, arrays, and closures.

Do not implement cycle detection, built-in regions, atomic RC, general borrowing, or `restrict` during this first RC milestone.

## Final design statement

KMY is not intended to be Rust with different syntax. Explicit `shared T` initially provides convenient RC-managed sharing, while ordinary `T` remains reserved for deterministic unique ownership. The later borrow checker concentrates on lifetime validity, moves, and invalidation rather than universal exclusive aliasing. Strong shared cycles leak until broken with future weak links, while graph-heavy code may use indexes or generational handles. Programmers retain C-like raw and manual control when required, with an explicit loss of safety guarantees. KMY does not require a built-in region system.
