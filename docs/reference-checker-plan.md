# KMY Memory Model (written by human!)

- Copyable values (primitives)
  - They are copied everywhere.
- Object values
  - `Foo`: unique owner; moves like `std::unique_ptr<Foo>` in C++.
  - `shared Foo`: shared owner; copies retain like `std::shared_ptr<Foo>`.
  - `&Foo`: borrow-checked readable reference (planned).
  - `&mut Foo`: borrow-checked readable/writable reference (planned).
  - `&raw Foo`: unchecked raw pointer (planned; the current spelling is `Foo*`).

## Step 1: Only unique and shared pointers

- Declaration: `let x: Foo = new Foo();` makes `x` available.
- `let x: shared Foo = new shared Foo();` makes `x` available with a strong count of 1.
- Converting an existing unique `Foo` into `shared Foo` may eventually be allowed
  as a one-way promotion that invalidates the unique source. It is not a free
  pointer cast: with the current allocation layout it requires reboxing into an
  RC allocation, so do not implement implicit promotion yet.
- `let x = y;` moves when `y` is unique, and marks `y` moved.
- `let x = y;` copies when `y` is a primitive, and leaves `y` available.
- `let x = y;` retains when `y` is shared, and leaves `y` available.
- Function parameters start available in a fresh function-local state map.
- Function-call arguments are moved, copied, or retained according to their
  resolved parameter and argument ownership kinds.
- Returning a unique variable marks that source binding moved. A temporary such
  as `return new Foo();` has no source symbol to invalidate. Copy/shared returns
  do not invalidate their source; a shared return will eventually emit `RETAIN`.
- In assignment, any writable place can appear on the left-hand side:
  1. Variable: `x = y`.
  2. Index: `a[i] = y`.
  3. Field: `object.x = y`.
  4. Dereference: `*pointer = y`.
- For all assignments, the right-hand side is moved, copied, or retained according
  to its ownership kind. Only a direct destination binding such as `x` is marked
  available in the binding-state map. An index, field, or dereference is updated
  through its place; it has no independent `VarSymbol`.
- Cases for the right-hand side:
  1. `y = x`: apply the normal whole-variable move/copy/retain rule.
  2. `y = a[i]`: copy and shared loads are possible; initially reject moving a
     unique element out because that would leave an uninitialized array slot.
  3. `y = object.x`: copy and shared loads are possible; initially reject moving
     a unique field out because that would leave a partially moved object.
  4. `y = *pointer`: raw memory has no statically tracked owner; treat this as an
     explicitly unchecked operation until safe-reference rules exist.
  5. `y = new Foo()` or `y = makeFoo()`: transfer a fresh temporary; no existing
     source binding becomes moved.

## Place: storage selected by an expression

A `VarSymbol` identifies a declared binding, but expressions such as `a[i]`,
`object.field`, and `*pointer` designate storage without having their own symbol.
Represent them as a root plus projections:

```cpp
enum class PlaceProjectionKind {
    Field,
    Index,
    Dereference
};

struct PlaceProjection {
    PlaceProjectionKind kind;
    Type *resultType = nullptr;
    int fieldIndex = -1; // Used only by Field.
};

struct Place {
    const VarSymbol *root = nullptr;
    Type *type = nullptr; // Type stored at the final projected location.
    std::vector<PlaceProjection> projections;
};
```

Examples:

```text
x                -> root x, []
object.field     -> root object, [Field(field)]
array[i]         -> root array, [Index]
object.items[i]  -> root object, [Field(items), Index]
(*pointer).field -> root pointer, [Dereference, Field(field)]
```

Build a place from an existing AST expression rather than adding a new AST node:

```cpp
std::optional<Place> resolvePlace(const ExprPtr& expression);
```

Initial rules:

- `Variable` produces a place rooted at its `VarSymbol`.
- `Get` appends a `Field` projection.
- `Index` appends an `Index` projection. The checker need not store or compare the
  runtime index expression yet.
- unary dereference appends a `Dereference` projection.
- literals, calls, `new`, and arithmetic expressions are values, not places.
- a move from a place with any projection is initially rejected as an unsupported
  partial move.

Later, the same representation can support explicit operations:

```cpp
enum class ValueUse {
    Read,
    Write,
    Move,
    BorrowShared,
    BorrowMutable,
    RawAddress
};

void checkPlaceUse(const Place& place, ValueUse use);
```

This keeps whole-variable availability simple now while leaving room for `&`,
`&mut`, overlapping-place checks, field-sensitive moves, and ownership-aware array
elements later.
