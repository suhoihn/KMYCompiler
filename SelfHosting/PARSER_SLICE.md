# First KMY-written parser slice

`Examples/KmyParser.kmy` is a single-file lexer and recursive-descent parser
written in KMY. It reads `Examples/KmyParserInput.kmy`, builds linked AST nodes
in memory, prints a KMY-style source preview followed by an indented AST with
named child roles, and writes that view to `build/kmy_parser_ast.txt`. It parses
source; it does not evaluate or type-check
the parsed program. The older standalone `Examples/KmyLexer.kmy` is unchanged.

This first slice accepts integer literals, names, parentheses, `+ - * /`,
unary `+ - & *`, `let` with an optional simple/pointer type annotation,
assignment to a name or dereference, `//` comments, and calls with zero or one
argument (enough to represent `malloc(8)`). Multiplication/division bind more
tightly than addition/subtraction; assignment is right-associative. Functions,
control flow, classes, arrays, and multi-argument calls are later stages.

The host compiler now accepts pointer annotations such as `int*`, `int**`,
`int*[]`, and `int[]*`. `malloc(bytes: int)` returns `any*`; it calls C `malloc`
on the x86 path and does **not** initialize the returned bytes. Assigning that
opaque result to a concrete pointer type, for example
`let p: int* = malloc(8);`, is an unsafe programmer choice. Raw `any*` cannot
be dereferenced. There is no `free` yet, so each allocation leaks, and the
compiler does not check that the requested byte count fits the chosen type.
The bytecode VM does not implement raw pointers or `malloc`.

Run the guarded native regression suite with `tests/run-x86-tests.ps1`. Its
`kmy_parser` case checks both stdout and the written AST file; `malloc` checks
heap reads/writes, while two rejection cases check wrong argument types and
opaque dereference. Native test processes are time-limited to five seconds.
