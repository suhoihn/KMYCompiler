# KMY Ownership Implementation Plan

This document turns `permissive-ownership-roadmap.md` into small milestones for improving the current compiler.

The current implementation objective is deliberately narrow:

> Add explicit, non-atomic reference-counted ownership with correct retain/release behavior and a documented strong-cycle leak.

This RC milestone comes before unique ownership and borrowing. It must not silently make ordinary `T` permanently reference-counted. Keep the internal ownership categories separate so later `T`, `shared T`, `&T`, `&mut T`, and `*T` can coexist.

## Milestone 0: Stabilize the current compiler

### Tasks

1. Classify existing tests as parser, semantics, VM, or x86 tests.
2. Add a semantic-only mode resembling `kmyc --check file.kmy`.
3. Make `--check` stop after parsing, symbol building, resolution, and the reference checker.
4. Store a stable expected diagnostic fragment beside every negative test.
5. Fix the known native array element-size failure before ownership changes obscure it.
6. Document intentional VM/x86 feature differences.
7. Never execute generated test programs unless execution was explicitly requested.

Suggested backend table:

| Feature | VM | x86 |
|---|---:|---:|
| Raw pointers | No | Yes |
| Modules | Limited | Yes |
| Arrays | Yes | Yes |
| Closures | Yes | Yes |

### Done when

- positive semantic tests pass;
- native compile-only tests pass except for documented failures;
- negative tests fail for their intended reason;
- semantic tests cannot accidentally launch an executable.

## Milestone RC-1: Explicit shared ownership (current priority)

The first automatic-memory implementation is explicit RC:

```kmy
var first: shared Foo = shared new Foo();
var second = first;       // retain: both handles remain valid
```

Strong cycles are a documented leak in this milestone. There is no cycle collector and no built-in region system.

### RC-1.1 Add ownership categories

Add an internal ownership enum containing at least:

```cpp
enum class OwnershipKind {
    Unique,
    Shared,
    Weak,
    BorrowedRead,
    BorrowedWrite,
    Raw
};
```

Only `Shared` needs complete behavior during this milestone. Reserving the other categories prevents architectural collapse into one generic pointer type.

### RC-1.2 Choose explicit syntax

Use or finalize syntax equivalent to:

```kmy
shared Foo
shared new Foo()
```

Ordinary `Foo` remains reserved for later unique/move semantics.

### RC-1.3 Add a shared allocation header

A shared allocation needs conceptually:

```text
[strong count][destruction metadata][object fields...]
```

Use a non-atomic count. Thread-safe shared ownership is a separate future feature.

### RC-1.4 Add runtime operations

Implement operations equivalent to:

```text
retain(null)  -> no operation
retain(value) -> strongCount++

release(null)  -> no operation
release(value) -> strongCount--
                  if strongCount == 0:
                      destroy owned fields
                      free allocation
```

Add debug-only counters for allocations, retains, releases, destructions, and live allocations.

### RC-1.5 Handle local bindings

```kmy
var value: shared Foo = shared new Foo(); // count = 1
```

Release `value` on every exit from its scope, including early `return`, `break`, and `continue` when applicable.

### RC-1.6 Handle shared copies

```kmy
var second = first;
```

Retain the allocation so both handles stay valid.

### RC-1.7 Handle overwrite safely

For:

```kmy
destination = source;
```

Perform the conceptual order:

```text
retain(source)
release(destination)
destination = source
```

Retaining first makes self-assignment and aliasing safe.

### RC-1.8 Handle arguments and returns

Define whether each compiler-generated transfer is a copy or move. A copied shared handle retains. A moved shared handle transfers the handle without changing the count. Returning a fresh local should normally be emitted as a move when possible.

### RC-1.9 Document cycle behavior

This intentionally leaks:

```kmy
var first: shared Node = shared new Node();
var second: shared Node = shared new Node();

first.next = second;
second.next = first;
```

The compiler does not need to detect arbitrary runtime cycles. Future `weak T` can express non-owning back-links.

### RC-1.10 Defer advanced RC integration

Do not add all of these at once. After locals, assignment, calls, and returns are stable, separately add:

1. shared object fields;
2. nullable shared handles;
3. arrays of shared handles;
4. closure captures;
5. module globals;
6. future weak handles.

### Milestone RC-1 is complete when

- acyclic shared objects are destroyed exactly once;
- shared copies remain usable independently;
- overwrite and self-assignment are correct;
- arguments and returns have balanced counts;
- early exits release all appropriate local handles;
- strong cycles are documented and tested as retained allocations;
- ordinary `T` has not been permanently redefined as shared RC.

## Milestone 1: Classify Copy and owning types

### Tasks

1. Add one authoritative `isCopyType(Type *type)` helper.
2. Initially classify `int`, `byte`, `bool`, `double`, plain enums, and raw pointers as Copy.
3. Initially classify classes, arrays, strings, collections, and closures with owned captures as owning.
4. Add `isOwningType(Type *type)` rather than repeating `TypeKind` checks.
5. Classify nullable types recursively: `int?` is Copy, while `Foo?` is owning.
6. Reserve a distinct classification for future safe references.

Example:

```kmy
var first = 10;
var second = first;
print(first);             // valid: int copies
```

### Done when

Every resolved value type is consistently categorized as Copy, owning, borrowed, raw, or non-value.

## Milestone 2: Straight-line move checking

Only handle local variables in straight-line code. Do not handle branches yet.

### 2.1 Add ownership state

```cpp
enum class OwnershipState {
    Uninitialized,
    Live,
    Moved
};
```

Store state by resolved symbol identity, not source name:

```cpp
std::unordered_map<Symbol *, OwnershipState> ownershipStates;
```

### 2.2 Mark initialized owners live

```kmy
var value = new Foo();    // value = Live
```

### 2.3 Classify expression use

The parent expression determines whether a value is read, moved, borrowed, or converted to a raw address:

```cpp
enum class ValueUse {
    Read,
    Move,
    BorrowRead,
    BorrowWrite,
    RawAddress
};
```

These consume an owning value:

```kmy
var second = value;
consume(value);
return value;
```

These do not inherently move it:

```kmy
value.field;
inspect(&value);
```

### 2.4 Record moves

```kmy
var second = first;
```

Produces:

```text
first = Moved
second = Live
```

Store the source location of the move for diagnostics.

### 2.5 Reject use after move

```kmy
var first = new Foo();
var second = first;
use(first);               // error
```

Suggested diagnostic:

```text
Cannot use 'first': its value was moved on line N.
```

### 2.6 Permit reinitialization

```kmy
var first = new Foo();
var second = first;
first = new Foo();        // first becomes Live again
use(first);               // valid
```

### 2.7 Preserve Copy behavior

```kmy
var first = 10;
var second = first;
use(first);               // valid
```

### Done when

Straight-line use-after-move checking works without changing HIR, MIR, or generated code.

## Milestone 3: Moves through calls

### Tasks

1. Passing an owner to an owning parameter moves it.
2. Passing a Copy value copies it.
3. Determine behavior from the resolved function symbol and parameter types, never the function name.
4. Give built-in and native functions real parameter type metadata.
5. Initially make ordinary method calls borrow their receiver rather than consume it.
6. Add explicit consuming-receiver syntax only later.

```kmy
fun consume(value: Foo) {}
fun consumeNumber(value: int) {}

consume(foo);             // moves foo
consumeNumber(number);    // copies number
```

### Done when

Function calls consistently transfer ownership according to their resolved parameter types.

## Milestone 4: Owned returns

### Tasks

1. Returning an owning local moves it into the caller.
2. Returning a Copy value copies it.
3. Record move state separately on every return path.
4. Do not add destruction yet; first verify semantic ownership transfer.

```kmy
fun create(): Foo {
    var result = new Foo();
    return result;         // moves result to the caller
}
```

Conditional owned returns must also be representable:

```kmy
fun choose(condition: bool, first: Foo, second: Foo): Foo {
    if condition {
        return first;
    }

    return second;
}
```

### Done when

Factories and conditional owner-returning functions pass move analysis.

## Milestone 5: Merge ownership through control flow

Add a conservative state:

```cpp
enum class OwnershipState {
    Uninitialized,
    Live,
    Moved,
    MaybeMoved
};
```

### Branch algorithm

1. Save the state entering the `if`.
2. Analyze the `then` branch.
3. Restore the incoming state.
4. Analyze the `else` branch.
5. Merge the two output states.

Use an explicit merge table:

```text
Live  + Live  = Live
Moved + Moved = Moved
Live  + Moved = MaybeMoved
Moved + Live  = MaybeMoved
```

Do not merge a branch that cannot reach the following statement:

```kmy
if condition {
    consume(value);
    return;
}

use(value);               // valid on every path reaching here
```

Initially treat loops conservatively:

```kmy
while condition {
    consume(value);
}

use(value);               // error: value may have moved
```

### Done when

Move checking works across `if`, loops, early returns, `break`, and `continue`.

## Milestone 6: Deterministic automatic destruction

Begin cleanup only after move analysis is stable.

### Tasks

1. Add a HIR `DROP value` operation.
2. Display `DROP` in HIR debugging before lowering it.
3. Calculate the still-live owners at every scope exit.
4. Insert cleanup for normal exit, `return`, `break`, `continue`, and function end.
5. Never drop a binding after its ownership moved elsewhere.
6. Add hidden runtime drop flags for values that may be either live or moved.
7. Lower destruction for classes first, then strings, arrays, owned fields, array elements, closures, and collections.

Conditional example:

```text
value_live = true

if condition:
    consume(value)
    value_live = false

if value_live:
    drop value
```

### Done when

Ordinary owned programs require no explicit `free`, including programs with early returns and conditional moves.

## Milestone 7: Add safe reference types

Keep safe references distinct from existing raw pointers:

```kmy
&Foo       // shared safe reference
&mut Foo   // writable safe reference; aliasing is permitted
*Foo       // unchecked raw pointer
```

### Tasks

1. Parse `&T` and `&mut T` as types.
2. Parse `&value`, `&mut value`, and `&raw value` as expressions.
3. Require the operand to be an addressable place.
4. Initially reject references to literals, arithmetic temporaries, and returned temporaries.
5. Require mutable storage for `&mut`.
6. Permit aliases deliberately.
7. Never lower ordinary `&mut` as C `restrict` or backend `noalias`.

```kmy
let fixed = new Foo();
modify(&mut fixed);        // error

var writable = new Foo();
modify(&mut writable);     // valid
```

### Done when

Safe references work as distinct semantic types and cannot be confused with raw pointers.

## Milestone 8: Call-scoped borrowing

Initially allow safe references only as temporary call arguments:

```kmy
inspect(&value);
modify(&mut value);
```

Temporarily reject keeping them in local variables:

```kmy
var reference = &value;
```

Check that the owner is live, the operand is a place, the reference type matches, mutable access comes from mutable storage, and the callee cannot retain the reference.

KMY deliberately permits aliasing:

```kmy
edit(&mut value, &mut value); // valid under KMY's policy
```

### Done when

Large values can be passed efficiently without requiring general lifetime inference.

## Milestone 9: Local references and last-use analysis

### Tasks

1. Allow `var reference = &value`.
2. Record provenance such as `reference -> Local(value)`.
3. Prevent moving or destroying an owner while a live safe reference may point into it.
4. End the borrow at the reference's final use rather than the end of its lexical block.
5. Begin with last-use analysis inside one basic block, then extend across the CFG.

```kmy
var reference = &value;
use(reference);            // last use
consume(value);            // valid
```

### Done when

Local references work without unnecessarily locking their owners for an entire block.

## Milestone 10: Simple returned references

Support a return derived from exactly one input first:

```kmy
fun identity(value: &Foo): &Foo {
    return value;
}
```

Store hidden metadata:

```text
returnOrigins = { Parameter(0) }
```

Methods may return references derived from `this`:

```kmy
fun getName(): &String {
    return &this.name;
}
```

Reject references to locals:

```kmy
fun broken(): &Foo {
    var local = new Foo();
    return &local;          // error
}
```

### Done when

Getters and basic container accessors safely return references without source-level lifetime annotations.

## Milestone 11: Conditional returned references

Collect every possible origin from reachable returns:

```kmy
fun choose(condition: bool, first: &Foo, second: &Foo): &Foo {
    if condition {
        return first;
    }

    return second;
}
```

Store:

```text
returnOrigins = { Parameter(0), Parameter(1) }
```

At the call site, every possible source owner must remain alive until the returned reference's final use.

### Done when

Reference-returning functions support ordinary conditions and early returns.

## Milestone 12: Safe resizable collections

Give collection methods effect metadata:

```text
Vector.get     returns a reference derived from this
Vector.set     preserves element addresses
Vector.push    may relocate elements
Vector.reserve may relocate elements
Vector.remove  may invalidate elements
Vector.clear   invalidates elements
```

Reject invalidation while an interior reference remains live:

```kmy
var item = values.get(0);
values.push(value);        // error: push may relocate item
use(item);
```

Allow mutation after the final use:

```kmy
var item = values.get(0);
use(item);
values.push(value);        // valid
```

### Done when

Vectors and similar collections can safely return element references while supporting resizing.

## Later work

Only after Milestone 12 should the compiler attempt:

- references stored in fields;
- borrowed closure captures;
- complex multi-source provenance;
- `weak T` for explicitly breaking shared RC cycles;
- standard index and generational-handle collections for graph-shaped data;
- explicit unsafe blocks around manual memory;
- cross-thread ownership rules;
- an optional exclusive/no-alias reference for optimization.

KMY will not add a built-in region ownership system. Programs needing arbitrary graphs should use indexes/handles, explicit shared/weak ownership, redesign the graph, or deliberately use raw/manual pointers.

## Exact next coding session

Do only these tasks next:

1. add internal `OwnershipKind::Shared` without changing ordinary `T` semantics;
2. finalize the explicit `shared T` and shared-allocation syntax;
3. add a non-atomic strong count to shared allocations;
4. add null-safe runtime `retain` and `release` functions;
5. release one shared local on normal scope exit;
6. retain one shared local-copy assignment;
7. handle overwrite in retain-before-release order;
8. handle shared self-assignment;
9. add five RC lifecycle tests using debug counters;
10. stop and review before adding shared fields, arrays, closures, or weak references.

### Five initial RC tests

```kmy
// One owner: allocation is destroyed at scope exit.
var value: shared Foo = shared new Foo();
```

```kmy
// Copy: both handles remain valid and destruction happens once.
var first: shared Foo = shared new Foo();
var second = first;
print(first.value);
print(second.value);
```

```kmy
// Overwrite: old destination is released and source is retained.
var first: shared Foo = shared new Foo();
var second: shared Foo = shared new Foo();
second = first;
```

```kmy
// Self-assignment: the allocation survives.
var value: shared Foo = shared new Foo();
value = value;
print(value.value);
```

```kmy
// Strong cycle: intentionally remains live in the first RC version.
var first: shared Node = shared new Node();
var second: shared Node = shared new Node();
first.next = second;
second.next = first;
```

Stop after the local RC lifecycle milestone. Shared fields, arrays, closures, globals, weak references, unique moves, and borrowing are separate independently reviewable steps.
