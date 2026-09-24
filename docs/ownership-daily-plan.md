# KMY Shared/Unique Ownership Milestone

## Daily plan: 1–2 hours per day

This milestone delivers a usable first ownership model:

- ordinary owning objects and arrays are unique and move by default;
- `shared T` uses non-atomic reference counting;
- whole-variable moves are checked;
- `Place` represents variables, fields, indexes, and dereferences;
- projected unique moves are recognized but initially rejected;
- straight-line assignments, calls, and returns obey ownership rules;
- `if` and basic loops merge availability conservatively;
- shared locals are retained and released correctly;
- unique locals are dropped correctly on the supported control-flow paths.

The milestone does **not** include `&`, `&mut`, partial moves, `weak T`, cyclic-RC
solutions, multithreading, or a complete Rust-style borrow checker. `Place` is
designed so those can be added later.

## Rules for every day

1. Change one ownership concept at a time.
2. Build the compiler after each change.
3. Add tiny compile-pass and compile-fail tests before adding the next concept.
4. Do not run generated executables casually. When runtime execution is required,
   use a hidden process, redirected output, and a short timeout.
5. If the day's tests do not pass, move the unfinished work to the next day rather
   than starting the following feature.

## Week 1: Semantic ownership and places

### Day 1 — Freeze the ownership rules

Time: 60–90 minutes.

- Review `docs/reference-checker-plan.md`.
- Freeze the three ownership kinds:
  - `Copy`: primitives, enums, and raw pointers;
  - `Unique`: ordinary objects, arrays, and closures;
  - `Shared`: `shared T`.
- Keep strings temporarily `Copy` and document why.
- Confirm that `BindingAvailability` remains separate from `OwnershipKind`.
- Write a small table for declaration, assignment, call, and return behavior.

Done when there are no unresolved semantic decisions needed for straight-line code.

### Day 2 — Add the minimal `Place` representation

Time: 60–120 minutes.

- Add `PlaceProjectionKind`: `Field`, `Index`, `Dereference`.
- Add `PlaceProjection` and `Place` in a semantic header.
- Keep a root `VarSymbol*`, final `Type*`, and projection vector.
- Do not put stack slots, registers, or byte offsets in semantic `Place`.
- Add a debug printer such as:

```text
x
x.field
x.[index]
x.*.field
```

Done when the compiler builds and the structures are unused but inspectable.

### Day 3 — Implement `resolvePlace()`

Time: 90–120 minutes.

- Resolve `Variable` to a root place.
- Resolve `Get` by recursively resolving its object and appending `Field`.
- Resolve `Index` by appending `Index`.
- Resolve unary dereference by appending `Dereference`.
- Return no place for literals, calls, `new`, and arithmetic.
- Add unit-style cases for each expression shape.

Do not implement ownership effects yet.

Done when every lvalue shape is classified centrally without duplicated casts in
the test/debug path.

### Day 4 — Introduce value-use intent

Time: 60–120 minutes.

Add:

```cpp
enum class ValueUse {
    Read,
    Write,
    Move
    // BorrowShared and BorrowMutable come later.
};
```

- Add `checkPlaceUse(const Place&, ValueUse)`.
- `Read` requires an available root when one exists.
- `Write` checks root/field mutability.
- `Move` allows a whole unique variable.
- `Move` rejects a projected unique place with a clear partial-move diagnostic.
- Copy/shared reads do not mark the root moved.

Done when `consumeValue()` can delegate place decisions to this API.

### Day 5 — Finish `let` and assignment

Time: 90–120 minutes.

- Preserve `Let` ordering:
  `Uninitialized -> check/consume RHS -> Available`.
- Refactor assignment to resolve its LHS once.
- Direct variable assignment makes the destination available.
- Field/index/dereference assignment writes a place but does not invent a symbol.
- A unique RHS variable becomes moved.
- A Copy RHS remains available.
- A Shared RHS remains available semantically; retain emission comes later.
- Reinitializing a moved mutable variable is allowed.

Tests:

- Copy assignment followed by source use passes.
- Unique assignment followed by source use fails.
- Reinitialization after move passes.
- Moving from `array[i]` or `object.field` fails clearly.

Done when straight-line declaration and assignment behavior is stable.

### Day 6 — Isolate function state and parameters

Time: 60–120 minutes.

- Save the enclosing function and binding-state map.
- Start a fresh map for every `FunctionExpr`.
- Mark every parameter available.
- Preserve recursive/hoisted function declarations.
- Restore the enclosing map after checking the function body.
- Ensure one function's locals never appear in another function's map.

Tests:

- All parameters are readable initially.
- Moving a unique parameter twice fails.
- Two functions may use same-spelled locals independently.
- Recursive functions remain legal.

### Day 7 — Calls and returns

Time: 90–120 minutes.

- For calls, inspect resolved function parameter types.
- Consume unique by-value arguments.
- Copy Copy arguments.
- Leave Shared arguments available; record that IR must retain.
- Treat implicit method receivers as borrowed/read-only for this milestone.
- Make return use the same value-use machinery.
- Returning a unique local moves it.
- Returning a fresh temporary moves no source binding.

Tests:

- Calling a consumer twice with the same unique object fails.
- Passing integers repeatedly passes.
- Multiple method calls on one object pass.
- Returning `new Foo()` passes.
- Returning a local and then using it on a reachable path fails.

Stop for a Week 1 review. Do not begin control flow if any straight-line test is
ambiguous or depends on special-case function names.

## Week 2: Control flow and lifetime operations

### Day 8 — Merge plain `if` branches

Time: 90–120 minutes.

- Snapshot the incoming binding map.
- Analyze `then` and `else` from independent copies.
- Treat a missing `else` as the unchanged incoming map.
- Merge only symbols present before the branch.
- Equal states remain unchanged.
- Disagreeing states become `MaybeUnavailable`.
- Reject later use of `MaybeUnavailable`.

No dominance frontier, SSA construction, or phi insertion is needed.

### Day 9 — Track path exits and block reachability

Time: 90–120 minutes.

- Add flow outcomes: fallthrough, return, break, and continue.
- Stop checking statements after a terminating statement on that path.
- Exclude returning branches from a following `if` merge.
- Keep flow outcome separate from binding availability.

Tests:

- A returning branch that moves a value does not poison the surviving branch.
- Statements after unconditional return are unreachable.
- Both-returning branches make the continuation unreachable.

### Day 10 — Conservative loops

Time: 90–120 minutes.

- Include the zero-iteration path in loop exit state.
- Collect normal body exit and `break` states.
- Feed `continue` states toward the next iteration.
- Initially use conservative merging.
- Add a small fixed-point iteration only if the one-pass result is unsound.

Tests:

- Moving an outer owner in a possible loop makes it maybe unavailable afterward.
- Reusing a moved value on a later iteration fails.
- `break` contributes its binding state to loop exit.
- Loop locals do not escape.

Stop for a semantic milestone review. At this point invalid ownership programs
should be rejected, but generated lifetime code is not complete yet.

### Day 11 — Define lifetime actions in HIR

Time: 60–120 minutes.

- Keep existing RC-specific operations for bootstrap:
  `ALLOC_SHARED`, `RETAIN`, and `RELEASE`.
- Add or confirm a generic unique `DROP_VALUE` operation.
- Print all lifetime operations in HIR debugging.
- Do not lower them until expected HIR tests are written.
- Decide exactly which AST transfers create a new shared owner.

Done when small source examples have an unambiguous expected HIR listing.

### Day 12 — Emit shared retain/release

Time: 90–120 minutes.

- `new shared Foo()` emits shared allocation with count 1.
- Shared copies and by-value calls emit `RETAIN`.
- Shared overwrite releases the old destination.
- Retain the new value before releasing the old one for self-assignment safety.
- Normal shared-local scope exit emits `RELEASE`.
- Shared return transfers or retains according to the chosen calling convention;
  document one consistent rule.

Start with locals only. Reject/defer shared fields, arrays, globals, and closure
captures if their cleanup paths are not implemented.

### Day 13 — Emit unique drops

Time: 90–120 minutes.

- Drop unique locals still available at normal scope exit.
- Never drop moved bindings.
- Drop overwritten unique destinations before storing replacements.
- Add drops for supported early returns and block exits.
- Use object destructors before freeing allocations.
- Keep unique fields/elements deferred unless their destructor traversal is ready.

Tests should inspect HIR/MIR before executing generated programs.

### Day 14 — Integration and bounded runtime tests

Time: 90–120 minutes.

- Build the compiler and runtime with warnings enabled.
- Run compile-pass and compile-fail ownership suites.
- Inspect emitted HIR/MIR for balanced retains/releases/drops.
- Run only small native programs using a hidden process, redirected output, and a
  short timeout.
- Test:
  - one unique move and destruction;
  - one shared copy and final destruction;
  - shared self-assignment;
  - early return;
  - one `if` merge;
  - one bounded loop;
  - null shared retain/release behavior.
- Update the README with implemented behavior and explicit limitations.

## Milestone acceptance checklist

The milestone is complete only when all of these are true:

- Copy values remain usable after assignment, calls, and returns.
- Unique values cannot be used after a whole-variable move.
- Unique values can be reinitialized when the binding is mutable.
- Shared copies remain usable and have balanced retain/release behavior.
- Direct variable, field, index, and dereference destinations resolve as places.
- Unique partial moves from fields/indexes are rejected rather than silently copied.
- Function parameters and locals have isolated state maps.
- Branches and supported loops conservatively merge availability.
- Return, break, and continue paths do not leak required cleanup.
- A unique object is destroyed exactly once on every supported path.
- An acyclic shared object is destroyed exactly once after its last owner releases.
- No generated test process is allowed to run without a timeout.

## Contingency schedule

If a day exceeds two hours, split it. The most likely split points are:

- Day 5: variable assignment first, projected-place assignment second;
- Day 7: calls first, returns second;
- Day 10: plain `while` first, break/continue second;
- Day 12: retain/copy first, release/overwrite second;
- Day 13: normal scope exit first, early exits second.

A realistic completion window is therefore 14–19 working days. Correct ownership
semantics matter more than finishing in exactly two weeks.
