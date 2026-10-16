# KMY Memory Model and Ownership Plan

Last consolidated: 2026-10-07.

This is the single current memory-model document. It replaces the older
reference-checker notes, daily schedule, and permissive-ownership roadmap.
Examples describe the intended rules unless explicitly labelled implemented;
successful parsing or checking does not prove automatic memory management exists.

## 1. Goal and boundaries

Ordinary objects use unique ownership and moves. Multiple object owners use
explicit `shared T`. All callable values are shareable, including closures.
Destruction is intended to be deterministic, without a tracing GC or built-in
region/arena system. Manual allocation and raw pointers remain available.

Later borrowing should prevent dangling references without copying all of
Rust's exclusivity rules. General references are a separate future milestone,
not a requirement for finishing unique/shared ownership.

This model does not promise zero leaks: strong RC cycles can leak. Current KMY
also has unfinished destruction, allocated strings treated as Copy, and unchecked
array indexing. It is not currently a memory-safe or leak-free language.
A future safe subset must address bounds/invalidation as well as ownership.
Raw/manual operations remain outside its guarantees.

## 2. Settled value rules

| Value | Checker kind | Transfer to another owner | Original binding |
|---|---|---|---|
| Primitives, current ordinal enums | Copy | Copy bits | Available |
| Raw `T*` | Copy | Copy pointer, not pointee | Available |
| Ordinary class/object, array, structural value | Unique | Transfer ownership | Moved |
| Explicit `shared T` | Shared | Copy handle and retain | Available |
| Any function/closure value | Shared | Copy code/env; retain env | Available |
| `T?` | Same as inner T | Apply inner ownership rule | Depends on inner |
| String | Copy, temporarily | Copy pointer | Available |

Internal `CELL` is currently classified Copy by the checker. This is not a
decision that heap-allocated capture cells have no lifetime owner.
`ANY` is currently conservatively Unique; representation/conversion rules
still need validation.

`let` makes a binding immutable; `var` permits reassignment. Mutability and
ownership are different: a Unique `let` can still be moved.

Current shared spelling:

```kmy
let first: shared Foo = new shared Foo();
let second = first; // Shared copy; first remains usable.
```

Do not use the older proposal `shared new Foo()`.
No implicit unique-to-shared pointer cast is part of this plan. A future
promotion must transfer ownership and establish a valid RC allocation/header.

Arrays own their slots and backing storage. An array of shared elements does
not automatically have shared backing storage: container and element ownership
are separate. Currently `new shared Foo[n]` parses as an array of shared Foo;
shared-array allocation syntax is not finalized.

## 3. Type, availability, and runtime count are different

`getOwnershipKind(expr)` computes Copy/Unique/Shared from the semantic type.
Explicit object sharing uses `SharedType(innerType)`. Callable signatures stay
`FunctionType`; the checker classifies FUNCTION as Shared without wrapping every
function type in SharedType.

The checker stores only binding availability:

```cpp
enum class BindingAvailability {
    Uninitialized,
    Available,
    Moved,
    MaybeUnavailable
};

using AvailabilityMap =
    std::unordered_map<const VarSymbol*, BindingAvailability>;
```

Runtime strong counts live in allocation headers, never in this map.

Do not add callable-kind flow maps or separate Copy/owning function signature
types for the settled callable policy. Do not copy `nativeFnPtr` into aliases:
it identifies a builtin declaration, not arbitrary stored callable values.

## 4. Reads and ownership transfers

Reading an available binding does not itself move it. A consuming context
applies the ownership rule: declaration initializer, assignment RHS, by-value
argument, return value, constructor argument, or array-literal element.

```kmy
var x = new Foo();
let y = x;       // Unique transfer: x is Moved.
x = new Foo();   // Whole-variable overwrite: x is Available again.

let n = 42;
let m = n;       // Copy: n remains Available.
```

Evaluate values in language evaluation order. Visit/check each argument or array
element, then consume it before checking the next one. Thus repeated Unique
sources in `f(x, x)` or `[x, x]` are rejected.

Fresh `new`/call results have no existing source binding to invalidate.
However, failure to resolve a place is NOT proof of a fresh temporary:
computed object bases and forwarding expressions need explicit handling.

Calls obey resolved parameter types; ordinary by-value Unique parameters take
ownership, Copy parameters copy, and Shared parameters acquire shared ownership.
Calling the callee does not consume its callable value. Parameters start
Available in an isolated function context.

Returns transfer the result before cleanup and terminate the current path.
Both `return;` and `return value;` terminate. The returned owner must survive
cleanup of the callee's remaining owners.

Constructor arguments and array literals follow the same transfer rules.
Record literals are deferred for the x86 milestone.

## 5. Places, replacement, and partial moves

A Place describes storage; it is not a new AST node or a backend address:

```cpp
struct Place {
    VarSymbol* sym = nullptr;
    Type* type = nullptr; // Type at the final projected location.
    std::vector<Projection> projections;
};
```

Current projections are separate structs in a variant: Field (field index),
Index (optional index expression, default nullptr), and Dereference. Index
expressions do not prove equality across evaluations; assume potential overlap.

```text
x                  -> root x, []
math::x            -> the same resolved binding, []
object.items[i]    -> root object, [Field(items), Index(i)]
(*pointer).field   -> root pointer, [Dereference, Field(field)]
```

Enum constants are values, not places. Computed bases are not fully represented.

Replacing a slot is different from extracting its owner:

```kmy
arr[i] = new Foo(); // Replace one slot; arr stays Available.
let item = arr[i];  // Reject if element is Unique: unsupported partial move.
```

A projected write requires a live container and checks base/index evaluation.
It consumes the RHS, but does not mark the entire container Available or Moved.
Replacement must eventually destroy/release the previous slot value.

Plain whole-variable assignment need not read the old binding: it may restore
a moved/uninitialized mutable variable. Compound assignment reads the old value
and currently requires Copy operands. Unique self-move assignment is rejected.

Initially reject Unique moves from fields, elements, or dereferences instead of
silently copying an owning pointer. Copy/Shared projected loads remain reusable.
Raw-pointer writes are currently rejected by this checker; their eventual
unchecked/manual policy is separate from safe-place ownership.

## 6. Control-flow analysis

`OwnershipFunctionContext currCtx` holds the active function, availability map,
path reachability, and function-local loop stack. It is not a context per block.

`reachable` means: can the current path reach its next statement?
Return/break/continue set it false. Block traversal stops on that path.
Nested function analysis saves/resets/restores the enclosing context.

An if analyses both branches from the state AFTER condition evaluation.
Merge only pre-existing bindings and only paths reaching the next statement:

```text
Available + Available -> Available
Moved + Moved         -> Moved
Available + Moved     -> MaybeUnavailable
returned + Available  -> Available
returned + returned   -> unreachable
```

Other unequal availability states also become MaybeUnavailable. Reading it
fails because availability is not proven on every surviving path.

Loops have two merge sites:

```text
continue + normal body completion -> back edges
entry + previous head + back edges -> next head

break + condition-false paths      -> state after loop
```

Head is the availability estimate before evaluating the condition. Reanalyse
condition/body while nextHead differs from head. This is compiler fixed-point
analysis, not execution of the source loop. Fixed keys and widening ensure
convergence; an invariant guard bounds analysis passes.

Return feeds neither loop merge. Break/continue target the innermost loop.
Branch merging must preserve their collected snapshots even when the branch
cannot fall through. Body-local declarations do not escape the loop.

Logical `&&`, `||`, and `??` merge RHS-skipped and RHS-evaluated paths.
Consuming a Unique `??` result additionally accounts for either selected owner,
without evaluating the operands again.

The analysis is deliberately conservative: it does not track condition
correlations or prune constant paths. Even a literal-true loop currently retains
a possible condition-false exit. Short-circuit compound assignments need their
own path-aware handling.

No dominance frontier or SSA phi insertion is required for these checker merges.

## 7. All callable values are shareable

This is the settled rule, independent of capture count or type annotation:

```kmy
fun addOne(x: int): int { return x + 1; }
let a = addOne;
let b = a;
a(1);
b(2); // Both valid; addOne also remains callable.
```

A callable is conceptually a code pointer plus an environment pointer:

```text
plain function: { code, null }
closure:        { code, RC-managed environment }
```

The signature type does not distinguish these representations. At construction,
capture discovery/layout determines what environment is required.
At copy/cleanup, null-safe retain/release handles both representations:

```text
create closure a: env strong count = 1
copy b = a:      retain(env), count = 2
destroy a:      release(env), count = 1
destroy b:      release(env), count = 0 -> destroy env and owned contents
```

Retain/release the environment, never the code address. Copies share captured
state; they do not deep-copy captures. Sharing the environment does not
automatically make every captured object a shared object.

The actual backend may share a frame environment/cells among several closures.
Their ownership edges and destruction glue must reflect that layout, not assume
every closure has an independent capture payload.

## 8. Capture rules: decisions still required

Do not implement capture ownership merely from “a variable is outside this
function.” Resolve these remaining questions before changing closure creation.

| Case | Candidate rule discussed | Status |
|---|---|---|
| Immutable Copy capture | Copy its value into environment | Proposed |
| Immutable Shared capture | Retain its owner into environment | Proposed |
| Immutable Unique capture | Move into environment; invalidate outer source | Proposed |
| Mutable outer binding | Shared stable cell, preserving observable mutation | Open |
| Unique value read inside repeatable shared closure | Read without transferring its owner out | Required constraint |
| Unique value moved out of repeatable shared closure | Reject initially; call-once support deferred | Required constraint |
| Capturing a reference/raw pointer | Safe-reference escape rules later; raw use unchecked | Deferred |

Example of the proposed Unique-capture rule:

```kmy
let object = new Foo();
let f = fun() { object.someMethod(); };
use(object); // Would fail AFTER capture transfer is implemented.
```

Example of a mutable-capture requirement, not yet a specified ownership rule:

```kmy
var count = 0;
let f = fun() { count++; };
let g = f;
f();
g();
// Copies must observe the same captured count.
```

Before implementation, decide who owns the mutable cell, whether the outer
binding remains usable, how a Unique payload is replaced in that cell, and how
multiple closures capture it. Rejecting moves out is not enough: overwrites must
also destroy/release previous contents safely.

Repeated closure creation cannot move the same Unique capture twice. Creating
and checking a closure body are different events: checking the body must not
accidentally perform transfers in the enclosing availability map.

Capture identities are resolved VarSymbol pointers, not unseen names.
A capture-discovery pass can first store `FunctionExpr::captures`; storage layout
then builds upvalue indexes and environment offsets.

Existing `FunctionContext::upvalues` describes incoming captures.
Existing `envMap` describes values made accessible to children.
If inner uses outer's local through middle, discovery must propagate the
requirement through middle even if middle never directly reads that local.

## 9. Automatic lifetime operations (not yet integrated)

The checker answers whether source use is legal. Ownership lowering emits
allocation, transfer, retain, release, and destruction on the actual execution
paths; it must not emit RC calls once per fixed-point analysis pass.

Unique: destroy exactly once at the final owner, never at a moved source.
Shared: a copied handle gains a strong owner; a transferred fresh handle does
not gain an unnecessary owner. Ownership of parameters and returns must be
balanced consistently by caller/callee.

Shared overwrite must protect the replacement before releasing the old value:

```text
evaluate/save RHS
retain RHS if this is a shared copy
release old destination if initialized
store RHS
```

This permits self-assignment and aliases. Unique replacement must similarly
preserve RHS evaluation before destroying old initialized storage.

Insert appropriate cleanup on normal scope/function exit, return, break, and
continue. Only scopes crossed by the exit are cleaned. Destroy owned fields and
array elements recursively, then backing storage. For conditionally live owners,
use path-specific cleanup or runtime drop flags; never unconditionally free a
MaybeUnavailable binding.

RC currently has a hidden strongCount/destructor header before its payload.
Allocation initializes that header, with count 1; payload is NOT zeroed.
Construction must initialize owned slots before any destructor can read them.
Allocation failure behaviour and partial-construction cleanup are still open.

RC is non-atomic/single-threaded. Atomic RC is a future feature and would not
alone make shared mutable objects thread-safe.

`free` is for valid raw/manual allocations, not unique/shared managed values,
stack addresses, or interior pointers. Enforcing this boundary requires more
than the current raw pointer type can prove.

## 10. Cycles, strings, and future borrowing

Strong-only RC cycles leak, including object -> callback -> captured object.
There is no automatic cycle collector. Future weak handles/captures should not
keep payloads alive and must support a checked upgrade. Raw pointers are not
safe substitutes for weak handles.

Strings temporarily remain Copy because literal storage and allocated runtime
strings share one type. Owned strings need a distinct lifetime strategy before
automatic string cleanup is claimed complete.

Future references:

- `&T`: non-owning readable reference.
- `&mut T`: non-owning writable reference; exclusivity is not assumed.
- `&raw value`: proposed explicit raw-address expression; current `&value`
  produces a raw pointer of current type spelling `T*`.
- Do not silently change existing raw syntax while implementing ownership.

Borrowing starts with call-scoped references, then locals/last-use, returned
references with inferred origins, and collection invalidation. Conditional
returned references must account for every possible owner.
Do not return references to locals or let an owner die/move/relocate while a
safe reference remains usable.

Aliasable writable references must not imply `restrict`/LLVM noalias.
Safe aliasing still needs protection against replacement/invalidation; relaxed
exclusivity is not a shortcut around lifetime or value-validity checks.
References in fields/closures and concurrency remain later design work.

Eventually, generic `Shared<T>` may implement RC in KMY itself once generics,
layout operations, copy/drop protocols, and destruction glue exist. Compiler
insertion of ownership actions is still necessary. Current RC HIR/MIR ops are
bootstrap support, not a commitment to keeping the RC algorithm in C forever.

## 11. Actual implementation status

Implemented foundations:

- Parser/semantic SharedType representation and interning.
- Copy/Unique/Shared classification; every FUNCTION is Shared.
- Binding availability, whole moves, rooted writes, function/parameter state.
- Calls, returns, constructor arguments, and array-literal consumption.
- If, loop fixed points, break/continue, and binary short-circuit flow.
- Place for Variable, ScopeAccess, Field, Index, and Dereference shapes.
- Non-atomic RC runtime and ALLOC_SHARED/RETAIN/RELEASE HIR/MIR/x86 support.
- 29 hand-crafted resolved-AST checker cases passed in the latest bounded run.

Still incomplete:

- Capture discovery before checking and capture transfer/lifetime policy.
- Explicit globals, builtins/hoisting, receiver, and default-value entry rules.
- Unexpected missing map entries currently pass silently.
- Some computed/forwarded values, raw writes, and partial moves.
- Short-circuit compound assignments and precise constant-condition handling.
- Source-driven shared allocation/retain/release/destructor/cleanup integration.
- Array/field/closure-cell recursive destruction and allocation failure handling.
- Full source-to-x86 ownership regression tests; AST tests do not prove backend RC.

Current pass order:

```text
Resolver -> ReferenceChecker -> MethodLower -> ClosureAnalyser -> native IR
```

Intended separation, subject to the capture design review:

```text
Resolver -> capture discovery -> ReferenceChecker
         -> MethodLower -> closure storage/layout
         -> ownership-aware IR/cleanup -> native lowering
```

Do not simply move checking after MethodLower: it adds a hidden method receiver
argument that the current checker would consume like an ordinary argument.
Capture discovery belongs before final ownership checking; slot allocation does
not have to move with it.

## 12. Next baby steps and decision gates

1. Read this specification and settle capture modes, especially mutable cells.
   Output: one accepted/rejected example per capture rule. No compiler changes.
2. Specify globals, hoisted declarations, defaults, and receiver ownership.
   Candidate: read Unique globals but disallow ownership extraction initially;
   ordinary methods temporarily read/borrow the receiver. These are proposals,
   not implemented guarantees.
3. Plan the capture-discovery/layout split using the existing scope identities.
   Then collect/deduplicate captures and propagate transitive requirements.
4. Implement capture checking only, with positive/negative AST and source tests.
   Unexpected missing bindings must become invariant failures after registration
   policies cover legitimate uses.
5. Close unsupported transfer/control-flow cases before enabling cleanup.
6. Implement one shared local lifecycle: allocation, copy, overwrite, scope exit.
7. Implement callable environment and cell ownership/destruction using the agreed
   capture policy; test plain null-env functions and shared closures.
8. Add unique destruction and cleanup for early exits and conditionally live values.
9. Add recursive fields/array cleanup and source-to-x86 tests.
10. Only then begin borrowing, weak references, or ownership optimizations.

Each coding step changes one rule, includes tests, and stops for review.
No date-based promise: split a step if it does not fit a 1–2 hour session.
Do not revisit rejected callable-kind-map/annotation-dependent designs unless
the language policy is explicitly reopened.

Validation layers: resolved-AST unit tests -> real-source semantic tests ->
IR lifetime-action checks -> bounded native lifecycle tests.
Runtime tests need hidden processes, redirected output, short timeouts, and
confirmed termination. Do not run generated programs without authorization.

Completion requires acyclic Unique/Shared values to be destroyed exactly once,
balanced shared counts, correct cleanup on every supported exit, documented
cycles/unsafe boundaries, and tested diagnostics. Checking alone is not completion.
