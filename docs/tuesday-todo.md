# Tuesday TODO — 2026-11-17

## Goal: basic Shared globals

Finish one small extension of the existing RC model. You implement; the assistant
reviews your changes and helps write tests. Do not add arrays or record layout
changes in the same session.

### 1. Refresh and trace the existing implementation

- [ ] Run the current RC suite before changing code.
- [ ] Find `VarSymbol::moduleGlobalSlot`, `LOAD_GLOBAL`, and `STORE_GLOBAL`.
- [ ] Find where the native entry path finishes running module initializers and
      top-level code. This is where normal program-shutdown cleanup must run.

```powershell
pwsh -NoProfile -ExecutionPolicy Bypass -File .\tests\run-rc-tests.ps1 -Run
```

### 2. Make one global own one count

- [ ] Start with one global initialized by `new shared Box()`.
- [ ] Record successfully initialized Shared globals in initialization order.
- [ ] Release them once, in reverse initialization order, at normal program exit.
- [ ] Keep these globals out of function-local cleanup lists.

Expected trace: allocate count 1, program runs, shutdown releases to 0 and frees.
The static global slot itself is not heap memory and must not be freed.

### 3. Copy that global into a local

- [ ] Support `let local = globalX;` in a function.
- [ ] Fix the binding-copy condition that currently requires a `VarRef` operand.
      `LOAD_GLOBAL` produces an `IRValue`, but reading the global still supplies
      a borrowed pointer: copying it into another owner requires retain.
- [ ] Test that local cleanup leaves the global's count alive, and only shutdown
      performs the final free.

Expected trace: allocation 1 -> local copy 2 -> local exit 1 -> shutdown 0/free.

### 4. Verify and stop

- [ ] Test the global with no local copies.
- [ ] Test one local copy and repeated calls that each create a local copy.
- [ ] Test early return from a function holding a local copy.
- [ ] Test two independent globals for reverse shutdown cleanup order.
- [ ] Run the existing RC suite again and review the diff.
- [ ] Commit the milestone after reviewing the results yourself.

**Done means:** ordinary same-module global initialization, local copies, and
normal shutdown cleanup balance their counts. Do not claim all Shared globals
work yet. Explicitly keep unsupported operations outside this milestone.

## Following small steps

1. Global reassignment: save old pointer, acquire replacement, release old,
   `STORE_GLOBAL`. Include self-assignment and fresh/call replacements.
2. Passing and returning globals: check the existing visitor paths and add tests.
3. Imported global reads such as `module::globalX`: these are borrowed reads too,
   even though their AST kind is `ScopeAccess` rather than `Variable`.
4. Global initialization from another Shared variable or a Shared-returning call;
   verify initialization order across modules before extending cleanup assumptions.

Avoid duplicating the ownership rules at every site. Keep track of which
expression forms supply a count, and reject unsupported forms rather than
silently treating every non-variable expression as an owned result.

## Why globals before arrays and records?

| Next feature | New questions to settle |
|---|---|
| Shared globals | Existing RC rules, static slots, program-lifetime cleanup |
| Arrays containing Shared values | Slot ownership, element overwrite, releasing every initialized element, container destruction |
| Value-oriented records | Size/alignment/field offsets, copies and moves, argument/return representation, cleanup of managed fields |

Recommended order: **basic globals -> Shared elements in one fixed-size array
model -> value-oriented records**. Define the array ownership rules before
implementing them; Shared elements and Shared backing storage are different.

Records do not need packed, byte-sized fields immediately. A first version can
deliberately keep word-sized storage slots. Smaller fields and padding can follow
behind a central layout calculation. Value-copy and calling rules still need an
explicit design; padding is not the only record problem.

## If another RC session sounds exhausting

Fix the existing `for` + `continue` issue instead: continue currently skips the
increment appended to the loop body. Use the guarded
`tests/rc/known_gap_for_continue.kmy` regression. This is an alternative session
goal, not extra work required on top of globals.
