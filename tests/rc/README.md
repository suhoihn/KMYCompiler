# Shared-local RC tests

For the retain/release rules and count-by-count examples, read the
[Shared RC guide](../../docs/shared-rc-guide.md).

This suite tests the current milestone only: initialized, uncaptured `shared X`
locals, direct local copies/assignments, normal exits, returns, and break/continue
cleanup. Function-boundary cases cover Shared parameters, returned objects,
nested calls, recursion, and discarded owned results. Objects have only an int
field. Captured owners, Shared globals, managed fields/array elements, and general
RHS forms beyond variable/new/call are outside this suite. These finite tests do
not prove every ownership scenario correct.

## Run it yourself (Windows, PowerShell 7)

From the repository root:

```powershell
# Builds the current compiler, emits assembly, checks RC call sites, and links.
# Does NOT execute generated programs.
pwsh -NoProfile -File .\tests\run-rc-tests.ps1

# Also runs the programs hidden, with redirected logs and 5-second timeouts.
pwsh -NoProfile -File .\tests\run-rc-tests.ps1 -Run
```

The runner prints its unique `build/rc-tests/...` artifact directory. It never
overwrites the checked-in runtime.o. Reuse a compiler YOU JUST REBUILT to skip
the compiler build:

```powershell
pwsh -NoProfile -File .\tests\run-rc-tests.ps1 -CompilerPath .\kmyc.exe -Run

# Run just one case (still uses hidden execution and the timeout).
pwsh -NoProfile -File .\tests\run-rc-tests.ps1 -CompilerPath .\kmyc.exe -TestName copy -Run
```

An old compiler binary will test old code. By default the suite builds current
sources specifically to avoid that confusion.

## Cases and expected runtime totals

| Case | What it proves | Alloc | Retain | Release | Free |
|---|---|---:|---:|---:|---:|
| single | Fresh owner released at function-body fallthrough | 1 | 0 | 1 | 1 |
| copy | Copy adds an owner; two releases free exactly once | 1 | 1 | 2 | 1 |
| nested_alias | Inner releases preserve outer owner; copying it later works | 1 | 3 | 4 | 1 |
| reverse_order | Independent locals are released in reverse declaration order | 2 | 0 | 2 | 2 |
| shadow | Same-spelled bindings have different symbol identities | 2 | 2 | 4 | 2 |
| branches | Both outcomes get their own local cleanup | 2 | 2 | 4 | 2 |
| loop | Body-local alias released each iteration; outer owner survives | 1 | 3 | 4 | 1 |
| known_gap_return | Former cleanup-stack imbalance; untaken return path | 1 | 1 | 2 | 1 |
| assignment | Replace x's old allocation with another local's owner | 2 | 1 | 3 | 2 |
| known_gap_fresh_assignment | Fresh replacement releases old x without retaining new x | 2 | 0 | 2 | 2 |
| self_assignment | Retain before release prevents x = x from freeing x | 1 | 1 | 2 | 1 |
| alias_assignment | x and y already point to the same object | 1 | 2 | 3 | 1 |
| integer_assignment | No RC on ordinary integer variable assignment | 1 | 0 | 1 | 1 |
| return_nested | Release inner-to-outer while preserving integer 73 | 2 | 1 | 3 | 2 |
| return_void | Empty return cleans up all active function scopes | 1 | 1 | 2 | 1 |
| return_paths | Early-return and surviving branch both clean correctly | 2 | 2 | 4 | 2 |
| break_nested | Release body/nested owners, preserve outer owner | 2 | 2 | 4 | 2 |
| continue_nested | Release body/nested owners on each iteration | 3 | 3 | 6 | 3 |
| nested_loop_jumps | Inner break and outer continue use different boundaries | 3 | 4 | 7 | 3 |
| for_initializer | Body cleanup plus initializer cleanup after break | 1 | 2 | 3 | 1 |

Each passing runtime test must finish with `live=0`. Runtime totals differ from
assembly call-site counts: a loop has one retain instruction executed three times;
only one branch runs on each call. The newer control-flow cases assert complete
runtime traces rather than fixing the exact static instruction count.

## Function-boundary coverage

These 20 additional cases compare the entire runtime trace, including object IDs,
intermediate counts, release order, actual frees, and a final `live=0` summary.
They do not merely check successful compilation or process exit.

| Test | Purpose |
|---|---|
| param_local | Variable argument; implicit parameter cleanup |
| param_fresh | Fresh argument; explicit parameter cleanup |
| param_same_twice | One object passed into two owning parameters |
| param_distinct | Two independent objects passed together |
| param_early_return | Both early-return and fallthrough parameter cleanup |
| return_param | Returned parameter survives callee cleanup |
| return_local_shared | Returned local survives its lexical scope |
| return_fresh_shared | Fresh return transfers allocation's initial count |
| return_forward_shared | Returned call result forwards without another retain |
| nested_shared_argument | Returned owner transfers into another call |
| discard_fresh_shared | Unused allocation released immediately |
| discard_returned_param | Unused identity result released while caller owner survives |
| discard_returned_fresh | Unused freshly returned object freed |
| return_shared_branches | Variable-return and fresh-return paths |
| return_shared_nested_cleanup | Returned alias survives nested cleanup |
| loop_shared_calls | Parameter count acquired/released each iteration |
| recursive_shared_return | Recursive forwarding balances each frame |
| shared_stack_arguments | Seven mixed arguments exercise register and stack passing |
| shared_param_reassignment | Mutable parameter replaces its owner safely |
| assign_returned_shared | Returned object replaces a local's previous object |

Run one case (the default rebuilds the current compiler):

```powershell
pwsh -NoProfile -ExecutionPolicy Bypass -File .\tests\run-rc-tests.ps1 -TestName return_param -Run
```

Read `<printed artifact directory>/return_param.stderr.log` for the RC trace.
All generated test programs run hidden with a five-second timeout and redirected
output; timed-out processes and their children are terminated.

## How we verify counts and actual frees

The runner links `tests/runtime/rc_trace_runtime.c` INSTEAD OF the production
runtime object. This test-only translation unit includes the real runtime.c,
wraps its RC entry points, and intercepts its call to C `free`.

It reads actual header counts while allocations are live, logs stable object IDs,
checks non-final releases preserve the object, checks final releases reach free,
and rejects leaked RC allocations at process exit. It never reads a freed header.
The runner compares the complete event order and totals against expected traces.
It does not infer correctness merely from successful execution or assembly counts.
Three C calibration runs also prove the tracer accepts balanced ownership and
rejects both a deliberately leaked allocation and a double release.

For copy.kmy, stderr must be:

```text
RC alloc id=1 count=1
RC retain id=1 count=2
RC release id=1 count=1
RC release id=1 count=0
RC free id=1
RC summary alloc=1 retain=1 release=2 free=1 live=0
```

These checks cover RC-managed allocations only, not all raw heap allocations.
The backend still allocates ordinary function descriptors without automatic cleanup;
a whole-program leak checker may report those unrelated leaks too.

## One test by hand

```powershell
.\kmyc.exe .\tests\rc\copy.kmy --check-ownership -asm --no-run -d -o .\build\rc-copy
gcc -g -O0 .\build\rc-copy.s .\tests\runtime\rc_trace_runtime.c -o .\build\rc-copy.exe
```

Inspect the IR output or assembly for one allocation, one retain, two releases.
Only when ready, run `build/rc-copy.exe`; the trace goes to stderr. Prefer the
runner's `-Run` switch for hidden execution and automatic termination on timeout.
Runtime errors exit nonzero; no generated program should be left running.

## Known gaps and a former regression

`known_gap_return.kmy` retains its historical name but is now in the regular
suite. Its untaken early-return branch must not unbalance compiler cleanup lists.
Block pops are unconditional, and cleanup state is function-local.

`known_gap_fresh_assignment.kmy` also keeps its historical name but is now a regular
regression: a fresh Shared replacement releases old x and needs no extra retain.

One opt-in regression target describes remaining work; failures are not swallowed:

- `known_gap_for_continue`: continue must execute the for-loop increment. Current
  parser lowering skips it. A separate guard bounds the test anyway; the expected
  final value is 2, not 0. RC counts can balance while this control-flow bug remains.

```powershell
pwsh -NoProfile -ExecutionPolicy Bypass -File .\tests\run-rc-tests.ps1 -IncludeKnownGaps -TestName known_gap_for_continue -Run
```

The regular suite should pass; this opt-in case should expose unfinished
features until fixed. A compile/link success alone does not test the expected
runtime counts or output: add -Run. The runner stops at the first failure.

## Debuggers and leak detectors

GDB is useful to step through a failing retain/release ([breakpoint manual](https://sourceware.org/gdb/current/onlinedocs/gdb/Set-Breaks.html)). Compile with `-g -O0`
and set breakpoints on `kmy_rc_alloc`, `kmy_rc_retain`, and `kmy_rc_release`.
Stop only in live allocations when inspecting the header. The traced runtime's
underlying implementation names start with `rc_test_real_`.

```text
gdb build/rc-copy.exe
(gdb) break kmy_rc_retain
(gdb) break kmy_rc_release
(gdb) run
(gdb) next
(gdb) continue
```

On a native Linux build, [Valgrind Memcheck](https://valgrind.org/docs/manual/mc-manual.html)
is an additional whole-program check:

```bash
valgrind --leak-check=full --show-leak-kinds=all ./program
```

Do not mistake unrelated function-descriptor leaks for RC payload leaks. Matching
these finite test traces does not prove all ownership scenarios are correct.
