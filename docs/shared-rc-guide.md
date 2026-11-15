# Shared RC: when to retain and release

This guide explains the current native/x86 Shared-object convention. It covers
uncaptured locals, parameters, and variable/new/call expressions. Shared globals,
managed fields/array elements, and captured owners are separate work.

The design roadmap remains in [the ownership plan](ownership-implementation-plan.md).
The executable examples are listed in [the RC test suite](../tests/rc/README.md).

## 1. The one rule to remember

**Every owner accounts for one reference count.**

- Creating a Shared object starts its count at **1**.
- Creating another owner of an existing object calls **retain**: count +1.
- An owner finishing calls **release**: count -1.
- Transferring an existing count to another owner needs **neither operation**.
- The final release reaches zero and frees the object.

The count belongs to the object, not to each variable separately. Two variables
can hold the same pointer and account for two counts on that one object.

For this milestone, objects have only unmanaged fields, such as integers. Freeing
an outer object does not yet establish cleanup for nested Shared fields.

## 2. Does an expression supply a count?

Getting a pointer and getting a count to keep are different things:

| Shared expression | Supplies a count for its receiver? | Meaning |
|---|---|---|
| Reading variable `x` | No | x keeps its existing ownership |
| `new shared X()` | Yes | Allocation supplies count 1 |
| `makeX()` returning `shared X` | Yes | The function supplies one count |

We call the first result **borrowed** and the others **owned**. These names only
describe RC accounting here. They do not mean that a borrow checker validates
lifetimes or prevents aliasing.

For the current subset, the compiler distinguishes these cases using `ExprKind`,
after checking that the type is Shared. Do not extend "anything except a variable
supplies a count" to arbitrary expressions: fields, indexing, conditionals, and
other forms need their own rules.

## 3. A local binding accepts one count

```kmy
let x = new shared X(); // Count 1, belonging to x.
let y = x;             // Retain: count 2, belonging to x and y.
```

Reading x does not give away x's count, so y needs another one. At scope exit,
release y and then x: 2 -> 1 -> 0, freeing the object.

Compare:

```kmy
let x = new shared X(); // Accept allocation's count; no retain.
let y = makeX();        // Accept returned count; no retain.
```

Both initializers already supply a count. Retaining them again would leave an
extra count after cleanup and leak the object.

## 4. Calls give each Shared parameter one count

```kmy
fun inspect(p: shared X) {
    // p owns one count for this invocation.
}

let x = new shared X(); // 1
inspect(x);            // Caller retains: 2. Callee exits and releases p: 1.
                       // Later, x's scope exit releases: 0 -> free.
```

**Retain at the call site, not again at function entry.** The caller knows whether
the argument supplies a count. By the time the callee begins, every Shared
parameter owns exactly one count, regardless of how its argument was written.

```kmy
inspect(new shared X()); // Allocation supplies 1; transfer it into p.
                        // Function exit releases p: 0 -> free.

inspect(makeX());        // Returned count transfers into p, without retain.
                        // Function exit releases p.
```

Passing the same object twice creates two parameter owners:

```kmy
inspectTwo(x, x); // Caller retains twice. Callee releases both parameters.
```

If x starts at 1, the count goes 1 -> 2 -> 3 at the caller, then 3 -> 2 -> 1
as the two parameters are cleaned up. x's original count remains.

## 5. Returns give the caller one count

```kmy
fun identity(p: shared X): shared X {
    return p;
}

let x = new shared X();
let y = identity(x);
```

| Event | Count | Owners accounted for |
|---|---:|---|
| Allocate x | 1 | x |
| Caller retains argument | 2 | x, p |
| `return p` retains the result | 3 | x, p, returned result |
| Function cleanup releases p | 2 | x, returned result |
| y accepts returned count | 2 | x, y |
| Scope releases y | 1 | x |
| Scope releases x | 0 | None; object freed |

**Returning a parameter follows the same rule as returning a local variable.**
Retain the returned variable before cleanup, because cleanup removes that
variable's existing count. Otherwise the caller receives no ownership of its own.

Fresh results already supply the count:

```kmy
fun makeX(): shared X {
    return new shared X(); // Initial count 1 goes to caller. No retain.
}

fun forward(): shared X {
    return makeX();        // Transfer returned count. No additional retain.
}
```

The explicit-return sequence is:

1. Evaluate the return expression and save its operand.
2. Retain it if it is a Shared variable read.
3. Release all active Shared locals, from inner scopes outward.
4. Release the function's Shared parameters.
5. Return the saved result.

An implicit return also releases Shared parameters. Every executed exit must
clean each owner exactly once; do not append another unconditional cleanup after
an explicit return's cleanup.

## 6. Nested calls transfer the returned count

```kmy
inspect(identity(x));
```

Starting with x's count of 1:

```text
retain x for identity's parameter       2
identity retains its returned parameter 3
identity releases its parameter         2
transfer returned count into inspect    2  (no retain)
inspect releases its parameter          1
later, x's scope releases x             0 -> free
```

The intermediate result needs no named variable. Retain/release can use an
`IRValue` operand as well as a `VarRef` operand.

## 7. An unused supplied count must be released

An expression statement evaluates an expression and discards its result:

```kmy
new shared X(); // Release the unused allocation immediately.
makeX();        // Release the unused returned count immediately.
x;             // No release: reading x supplied no new count.
```

For `identity(x);`, returning still supplies a count. The caller's expression
statement releases that returned count, leaving x's original ownership intact.

Do not wait for scope cleanup here: no variable was declared to track the unused
result. `ExprStmt` must emit its release.

## 8. Reassignment replaces one owner

```kmy
var x = new shared X(); // Object A: 1.
let y = new shared X(); // Object B: 1.
x = y;
```

The assignment must:

1. Save x's old pointer, A.
2. Evaluate y and retain B: B becomes 2.
3. Release saved A: A reaches 0 and is freed.
4. Bind x to B.

x and y now account for B's two counts. Their scope exits reduce B to zero.

For `x = new shared X()` or `x = makeX()`, skip step 2's retain: the RHS already
supplies one count. Still release the previous object.

For `x = x`, retain before release: 1 -> 2 -> 1. Releasing first could free the
object before the assignment acquires its replacement count.

### Why the old pointer needs an explicit HIR MOVE

`bindLocalDefinition()` can attach x's new definition to the earlier instruction
that produced the RHS. A later `RELEASE VarRef{x}` can therefore resolve to the
new object during SSA conversion. Preserve the old pointer before evaluating RHS:

```text
old = MOVE x
replacement = evaluate RHS
RETAIN replacement          // Only for a borrowed RHS.
RELEASE old
bind x = replacement
```

This HIR `MOVE` copies pointer bits. It does not retain, release, transfer an RC
count, or mark a source-language variable as moved.

The cleanup stack keeps x's symbol, not its old object. At scope exit,
`RELEASE VarRef{x}` reads x's current object. Reassignment does not add another
cleanup entry or replace x's symbol in that stack.

## 9. Quick reference

| Situation | Retain? | Release? |
|---|---|---|
| Keep/pass/return a Shared variable | Yes | Receiver cleans its count later |
| Keep/pass/return a Shared new/call result | No; transfer supplied count | Receiver cleans its count later |
| Shared local leaves its scope | No | Yes |
| Shared parameter's function exits | No | Yes |
| Overwrite a Shared owner | Acquire replacement as above | Release old object |
| Discard a Shared new/call result | No | Immediately |
| Discard a Shared variable read | No | No |
| Ordinary non-Shared value | No object RC retain | No object RC release |

The helpers `emitRetain(operand)` and `emitRelease(operand)` accept either HIR
operand form and ignore non-Shared types. **They do not decide ownership:** the
visitor still decides when the operation is required.

## 10. What the tests establish

As of 2026-11-15, the suite passed 40 native RC cases and three tracer self-checks.
It checks exact RC event order, intermediate counts, actual frees, and `live=0`.

These are finite tests of the supported subset, not a memory-safety guarantee.
Strong cycles can leak. Shared globals, escaping captures, nested managed fields,
array ownership, unchecked indexing, and raw-pointer safety need separate work.
Ordinary function descriptors and other non-RC allocations are not covered by
the RC payload leak check.
