# Standard-library regression sources

`collections.kmy`, `maps.kmy`, `utilities.kmy`, and `smoke.kmy` are concrete
expected-output tests for the source-level standard library. They are wired
into `run-x86-tests.ps1`, which runs every generated program hidden, with its
output captured and a five-second hard timeout.

`smoke.kmy` imports every implementation module together. It specifically
regresses imported aggregate methods that access `this.field`: Resolver must
re-establish the `ThisExpr` type rather than assuming an earlier semantic pass
left it in place.
