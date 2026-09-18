# KMY grammar sheet (current parser)

The main sheet describes what `Core/newParser.cpp` accepts today. A separate proposed-import section below is not yet parsed. Quoted text is a literal token; `IDENTIFIER`, `INTEGER`, `DOUBLE`, and `STRING` are lexer tokens. `?` after a grammar item means optional, `*` means zero or more, and `+` means one or more. These grammar markers are not KMY operators.

```text
/*
Format---------- Program and statements

program         → statement* EOF
statement       → block
                | printStmt | ifStmt | whileStmt | forStmt
                | breakStmt | continueStmt | returnStmt
                | letStmt | functionDecl
                | classDecl | recordDecl | enumDecl | typealiasDecl
                | expression ";"

block           → "{" statement* "}"
printStmt       → "print" "(" expression ")" ";"
ifStmt          → "if" "(" expression ")" block ("else" statement)?
whileStmt       → "while" "(" expression ")" block
forStmt         → "for" "(" forInit expression? ";" expression? ")" block
forInit         → ";" | letStmt | expression ";"
breakStmt       → "break" ";"
continueStmt    → "continue" ";"
returnStmt      → "return" expression? ";"

letStmt         → "let" "const"? IDENTIFIER (":" type)? ("=" expression)? ";"
functionDecl    → "fun" IDENTIFIER "(" parameters? ")" (":" type)? block
typealiasDecl   → "typealias" IDENTIFIER "=" type ";"
enumDecl        → "enum" IDENTIFIER "{" enumValues? "}" ";"
enumValues      → IDENTIFIER ("," IDENTIFIER)*

classDecl       → "class" IDENTIFIER "{" aggregateMember* "}" ";"
recordDecl      → "record" IDENTIFIER "{" aggregateMember* "}" ";"
aggregateMember → letStmt | functionDecl | initDecl | enumDecl
initDecl        → "init" "(" parameters? ")" (":" type)? block

parameters      → parameter ("," parameter)*
parameter       → "const"? IDENTIFIER (":" type)? ("=" expression)?
                | "const"? "..." IDENTIFIER (":" type)?

*/
```

`if`, `while`, and `for` require a block for their main body. `else` calls the general statement parser, so `else if (...) { ... }` works. A variadic parameter must be last, and a parameter without a default cannot follow one with a default. Parameter types are syntactically optional, but later semantic passes currently require explicit types in function signatures. `for` is lowered by the parser into a block plus `while`.

```text
/*
Format---------- Expressions

expression      → assignment
assignment      → logicalOr (assignmentOp assignment)?
logicalOr       → coalesce ("||" coalesce)*
coalesce        → logicalAnd ("??" logicalAnd)*
logicalAnd      → bitOr ("&&" bitOr)*
bitOr           → bitXor ("|" bitXor)*
bitXor          → bitAnd ("^" bitAnd)*
bitAnd          → equality ("&" equality)*
equality        → comparison (("==" | "!=") comparison)*
comparison      → shift (("<" | "<=" | ">" | ">=") shift)*
shift           → term (("<<" | ">>") term)*
term            → factor (("+" | "-") factor)*
factor          → prefix (("*" | "/" | "%") prefix)*
prefix          → ("+" | "-" | "!" | "~" | "&" | "*") prefix
                | postfix
postfix         → primary (callSuffix | indexSuffix | fieldSuffix | "!!")*
callSuffix      → "(" arguments? ")"
indexSuffix     → "[" expression "]"
fieldSuffix     → "." IDENTIFIER
arguments       → expression ("," expression)*

primary         → INTEGER | DOUBLE | STRING
                | "true" | "false" | "null" | "this"
                | scopedName
                | "(" expression ")"
                | arrayLiteral | recordLiteral | functionExpr | newExpr
scopedName      → IDENTIFIER ("::" IDENTIFIER)*
arrayLiteral    → "[" arguments? "]"
recordLiteral   → "{" recordFields? "}"
recordFields    → recordField ("," recordField)*
recordField     → IDENTIFIER ":" expression
functionExpr    → "fun" "(" parameters? ")" (":" type)? block

newExpr         → "new" IDENTIFIER "(" arguments? ")"
                | "new" arrayElementType "[" expression "]"
arrayElementType→ typeAtom "*"*

assignmentOp    → "=" | "+=" | "-=" | "*=" | "/="
                | "&=" | "|=" | "^=" | "<<=" | ">>="
                | "&&=" | "||="

*/
```

`assignment` is right-associative (`a = b = 1` means `a = (b = 1)`). The other binary operators are left-associative. The prefix `&` and `*` mean address-of and dereference; the same characters are binary bitwise-AND and multiplication in infix position. `!!` is postfix; `??` is binary. Function calls, indexing, fields, and `!!` bind tighter than prefix operators. `new Foo(...)` requires an unqualified class/record name; `new int[count + 1]` accepts an expression for the array length. A bare integer length retains a fixed-length array type. A record literal beginning with `{` at the start of a statement is parsed as a block; parenthesize it if you need it there as an expression.

```text
/*
Format---------- Types

type            → typeAtom typeSuffix* "?"?
typeAtom        → builtInType | scopedType | functionType | recordType
builtInType     → "int" | "double" | "bool" | "string" | "void" | "any"
                | "i64" | "u64" | "f64" | "byte"
scopedType      → IDENTIFIER ("::" IDENTIFIER)*
functionType    → "(" typeList? ")" "->" type
typeList        → type ("," type)*
recordType      → "{" typeFields? "}"
typeFields      → typeField ("," typeField)*
typeField       → IDENTIFIER ":" type
typeSuffix      → "*" | "[" INTEGER? "]"

Examples:
    int*        pointer to int
    int**       pointer to pointer to int
    int[]       unsized array type
    int[4]      fixed-length array type
    int*[]      array of int pointers
    int[]*      pointer to an int array
    int*?       nullable int pointer
    (int, int) -> bool
    {left: int, right: int}

*/
```

Each `*` or array suffix wraps the type built so far; a single `?` may follow the complete type. `int?*` and repeated `??` are not currently type syntax. `new T[expression]` uses the expression grammar for the length, whereas `T[INTEGER]` in a type annotation accepts only an integer token. The lexer recognizes `...` in parameter lists, but `T[...]` is not currently parsed as a type.

```text
/*
Format---------- Lexer and current edges

IDENTIFIER      → (letter | "_") (letter | digit | "_")*
INTEGER         → digit+
DOUBLE          → digit+ "." digit* | "." digit+
STRING          → '"' characters-until-next-quote '"'
lineComment     → "//" characters-until-newline

Whitespace and // comments are ignored between tokens.

*/
```

String escapes and `/* ... */` comments are not implemented by this lexer. The lexer recognizes `%=` but the expression binding-power table does not currently consume it as an assignment operator, so it is intentionally omitted from `assignmentOp` above. This sheet is syntax only: the parser can build an AST for constructs that later semantic or backend passes may reject.

## Proposed compile-time imports (not yet parsed)

```text
/*
Format---------- Modules

module          → importDecl* statement* EOF
importDecl      → "import" STRING "as" IDENTIFIER ";"

Example:
    import "collections/HashMap.kmy" as collections;
    let map: collections::HashMap = new collections::HashMap();

*/
```

The `STRING` is a source-file path. Resolve it relative to the **directory of the file containing this import**, never relative to the command-line working directory. For example, `src/main.kmy` importing `"lib/math.kmy"` finds `src/lib/math.kmy`; if that file imports `"../util.kmy"`, it finds `src/util.kmy`. Canonicalize paths so the same file is loaded only once. The alias is required and names from the imported module are accessed through `Alias::Name`. Imports form a compile-time dependency graph, not runtime module values or C-style text substitution. Imports may appear only at file top level, before any other statements; imported files should contain declarations rather than executable top-level statements. Dependency files are loaded transitively, but their names are not automatically re-exported.

`import`, `as`, and `from` are lexer keywords. The parser still does not accept `importDecl`. The proposed rule deliberately has no `from` or optional component; `from` is reserved for possible selective imports later.
