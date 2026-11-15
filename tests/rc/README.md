# Shared-local RC tests

This suite tests the current milestone only: initialized, uncaptured `shared X`
locals, direct local copies, and normal block exits. Objects have only an int
field. No shared parameters/returns, captured objects, nested object fields,
reassignment, or early-exit cleanup is claimed to work.

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

Each passing runtime test must finish with `live=0`. Runtime totals differ from
assembly call-site counts: a loop has one retain instruction executed three times;
only one branch runs on each call.

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

## Known regression: cleanup-list pop on an early-exit path

`known_gap_return.kmy` calls probe(false), so no return actually executes. Its
ordinary local owners should still be cleaned correctly. But merely COMPILING
the other branch can leave the compiler's cleanup-list stack unbalanced if
`sharedLocalVarStk.pop_back()` stays inside the no-terminator check.

```powershell
pwsh -NoProfile -File .\tests\run-rc-tests.ps1 -IncludeKnownGaps -Run
```

This case is excluded from the supported suite and is expected to fail until that
bookkeeping bug is fixed. It is NOT hidden or counted as a pass. Move the pop out
of the conditional before proceeding to parameter ownership. Correct early-return
runtime cleanup itself remains a separate feature.
The current implementation failed this compile-only regression with
`SSA variable 'a' has no reaching definition`: the leftover list lets function-local
cleanup escape into the enclosing function. The test is not executed if compilation fails.

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
