# SSA `varStack` and Phi Placement

This note explains the `varStack` failure fixed in `SSA/SSABuilder.cpp`.

## The pieces involved

The native backend builds a control-flow graph (CFG) in `CodegenIR/IRBuilder.cpp`.
Each `HIRBlock` has:

- `preds`: blocks that can flow into it;
- `succs`: blocks it can flow into;
- `defs`: variables assigned/defined in that block;
- `code`: HIR instructions and a terminator such as branch or jump.

`IRBuilder` creates those blocks and edges. For example, `while` creates:

```text
preheader -> condition -> body -> condition
                    \-> exit
```

`bindLocalDefinition()` records assignments in `block->defs`, so SSA can later
find where each variable is defined.

## What `varStack` means

`SSABuilder::rename()` walks the dominator tree. It maintains:

```cpp
std::unordered_map<VarSymbol*, std::vector<IRValue>> varStack;
```

For each source variable, the vector contains the currently visible SSA values.
The top value is the definition that reaches the current block.

When entering a block, the pass pushes phi results and ordinary definitions. When
leaving, it pops them again. A variable use is converted with:

```cpp
lookupVar(varStack, symbol)
```

If there is no value, the CFG/phi setup is asking for a variable before it has a
definition.

## What dominance frontiers do

`computePhiPos()` uses dominance frontiers to find *candidate* phi locations.
This is standard SSA construction: if different definitions can meet at a block,
that block may need a phi.

However, a dominance-frontier candidate is not automatically a valid phi. The
original implementation inserted every candidate immediately.

## The failure case

Consider a local declared inside an outer loop:

```kmy
for (let outer = 0; outer < 3; outer += 1) {
    let inner = 0;

    while (inner < 3) {
        inner += 1;
        if (inner == 2) {
            continue;
        }
        if (outer == 1) {
            if (inner == 3) {
                break;
            }
        }
    }
}
```

`inner` has real definitions in the outer-loop body and inner-loop backedge.
The correct phi is at the inner-loop header, where every incoming edge has a
definition of `inner`.

The old dominance-frontier propagation could also nominate the outer-loop header.
That header has two incoming paths:

```text
preheader: inner does not exist yet
backedge:  inner has a value
```

The old code created a phi with an impossible preheader input. During renaming,
the preheader had no `varStack[inner]`, so this assertion failed:

```cpp
assert(it != varStack.end() && !it->second.empty());
```

## The fix

`insertPhis()` now validates each candidate before creating the phi:

1. Find all definition blocks for the variable.
2. Inspect every predecessor of the candidate block.
3. Require at least one definition block to dominate each predecessor.
4. Skip the candidate if any predecessor lacks a reaching definition.
5. Create the phi only after all incoming paths are valid.

The key check is:

```cpp
if (dom.at(pred).count(defBlock) != 0) {
    hasDefinition = true;
}
```

This preserves valid loop phis while rejecting phis for locals that are not in
scope on all incoming paths.

`lookupVar()` now reports a compiler error instead of aborting the process:

```text
SSA variable 'inner' has no reaching definition
```

That makes any future genuine SSA bug diagnosable rather than producing an
assertion dialog.

## Regression coverage

The scoped-loop case is covered in:

- `tests/x86/control_flow.kmy`
- `tests/x86/control_flow.expected`

The test includes a loop-local variable, nested control flow, `continue`, and
`break`. The automated native suite now confirms:

```text
PASS control_flow
PASS closures
All x86 tests passed.
```

The important distinction is: dominance frontiers find where phis may be needed;
reaching definitions determine whether a candidate can actually be materialized
safely.
