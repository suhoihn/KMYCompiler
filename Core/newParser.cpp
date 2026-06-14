#include "newParser.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "errorhandler.hpp"
#include "type.hpp"
#include "operators.hpp"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

// program → statement* EOF
FunctionExprPtr Parser::parse() {
    std::vector<StmtPtr> statements;
    while (!is_at_end()) {
        statements.push_back(parse_statement());
    }
    return std::make_shared<FunctionExpr>(
        std::vector<Parameter>{}, // No params for global scope
        std::make_shared<Block>(move(statements)),
        nullptr // No return type annotation for global scope
    );
}

Token Parser::peek() const {
    return tokens[current];
}

bool Parser::is_at_end() const {
    return peek().type == TokenType::EndOfFile;
}

// Unused (previous()).

Token Parser::previous() const {
    if (current - 1 < 0) {
        return Token{TokenType::Unknown, "", peek().line, peek().column, peek().startIdx, peek().endIdx};
    }
    return tokens[current - 1];
}

// Advances current and returns the previous token.
Token Parser::advance() {
    if (!is_at_end()) {
        ++current;
    }
    return tokens[current - 1];
}

// Compares peek token's type and the given tokentype
bool Parser::check(TokenType type) const {
    if (is_at_end()) return false;
    return peek().type == type;
}

// Returns whether the peek token's type is the given tokentype.
// IMPORTANT: match consumes the token if it matches!
bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

// Consumes the peek token if it matches the given type. 
// If matched, Returns the consumed token.
// Otherwise, throws given error message.
Token Parser::consume(TokenType token, const std::string& errMsg) {
    if (!check(token)) {
        Token top = previous();
        throw KMYParseError(errMsg, top.line, top.startIdx, top.endIdx);
    }
    return advance();
}

void Parser::consumeSemicolon() {
    consume(TokenType::Semicolon, "Expected ';' after statement");
}

// block → '{' statement* '}'
StmtPtr Parser::parse_block() {
    std::vector<StmtPtr> statements;
    
    consume(TokenType::LeftBrace, "Expected '{'");

    while (!check(TokenType::RightBrace) && !is_at_end()) {
        statements.push_back(parse_statement());
    }
    
    consume(TokenType::RightBrace, "Expected '}'");
    
    return std::make_shared<Block>( move(statements) );
}

/*
statement → (
    print_stmt |
    if_stmt |
    while_stmt | 
    block |
    break_stmt |
    continue_stmt |
    let_stmt |
    funtionDecl |
    for_stmt |
    class_stmt |
    expression (wrapped in ExprStmt) ";"
)

print_stmt → "print" '(' expression ')' ';'
// You can have statement instead of blocks, which is more complex.
if_stmt → "if" '(' expression ')' block else_clause?
else_clause -> "else" (block | if_stmt)
while_stmt → "while" '(' expression ')' block
break_stmt -> "break" ';'
continue_stmt -> "continue" ';'
let_stmt -> "let" "const"? IDENTIFIER (':' type)? ( '=' expression )? ';'
*/ 
StmtPtr Parser::parse_statement() {
    if (check(TokenType::LeftBrace)) {
        // Do not consume the '{' here since parse_block expects to see it and will consume it.
        return parse_block();

    } else if (match(TokenType::KeywordPrint)) {
        consume(TokenType::LeftParen, "Expected '(' after print statement");
        ExprPtr expr = parse_expression();
        consume(TokenType::RightParen, "Expected ')' after expression");
        consumeSemicolon();

        return std::make_shared<Print>(move(expr));

    } else if (match(TokenType::KeywordIf)) {
        consume(TokenType::LeftParen, "Expected '(' after if statement");
        ExprPtr condition = parse_expression();
        consume(TokenType::RightParen, "Expected ')' after condition expression");

        StmtPtr thenbranch = parse_block();
        StmtPtr elsebranch = nullptr;
        
        if (match(TokenType::KeywordElse)) {
            elsebranch = parse_statement(); // TODO: statement or block?
        }

        return std::make_shared<If>(
            move(condition),
            move(thenbranch),
            move(elsebranch)
        );

    } else if (match(TokenType::KeywordWhile)) {
        consume(TokenType::LeftParen, "Expected '(' after while statement");
        ExprPtr condition = parse_expression();
        consume(TokenType::RightParen, "Expected ')' after condition expression");
        StmtPtr body = parse_block();

        return std::make_shared<While>(
            move(condition),
            move(body)
        );

    } else if (match(TokenType::KeywordFor)) {
        return parse_for();

    } else if (match(TokenType::KeywordBreak)) {
        consumeSemicolon();
        return std::make_shared<Break>();

    } else if (match(TokenType::KeywordContinue)) {
        consumeSemicolon();
        return std::make_shared<Continue>();
    
    } else if (match(TokenType::KeywordLet)) {
        return parse_let();
    } else if (match(TokenType::KeywordFun)) {
        return parse_functionDecl();
    } else if (match(TokenType::KeywordReturn)) {
        if (check(TokenType::Semicolon)) {
            // Return without expression.
            consumeSemicolon();
            return std::make_shared<Return>(nullptr);
        }
        ExprPtr expr = parse_expression();
        consumeSemicolon();
        return std::make_shared<Return>(move(expr));
    } else if (match(TokenType::KeywordClass)) {
        return parse_aggregate(AggregateKind::CLASS);
    } else if (match(TokenType::KeywordRecord)) {
        return parse_aggregate(AggregateKind::RECORD);
    } else if (match(TokenType::KeywordTypealias)) {
        return parse_typeAlias();
    } else if (match(TokenType::KeywordEnum)) {
        return parse_enum();
    } else {
        ExprPtr expr = parse_expression();
        consumeSemicolon();
        return std::make_shared<ExprStmt>(move(expr));
    }
}

StmtPtr Parser::parse_let() {
    // consume(TokenType::KeywordLet, "Expected Let keyword. If this error is thrown in normal var decl, contact KMY");

    bool isMutable = !match(TokenType::KeywordConst);

    Token varToken = consume(TokenType::Identifier, "Expected an identifier");

    TypeNodePtr type = nullptr;
    if (match(TokenType::Colon)) {
        // Type signature exists.
        type = parse_type();
    }
    
    ExprPtr initialiser = nullptr;

    if (match(TokenType::Assign)) {
        // let x = (smth) form.
        initialiser = parse_expression();
    }
    // "let x;" form
    consumeSemicolon();
    return std::make_shared<Let>(
        type,
        varToken.lexeme,
        move(initialiser),
        isMutable
    );
}

// functionDecl → "fun" IDENTIFIER (funtionExpr without initial "fun")
StmtPtr Parser::parse_functionDecl() {
    // consume(TokenType::KeywordFun, "Expected fun keyword. If this error is thrown in normal func decl, contact KMY");

    Token name = consume(TokenType::Identifier, "Expected an identifier");

    ExprPtr fnExpr = parse_functionExpr();

    // desugar: 
    return std::make_shared<Let>(
        nullptr, // Function type left blank in parser phase.
        name.lexeme,
        move(fnExpr),
        false // Function declarations are immutable for default.
    );
}

// for_stmt → "for" '(' initialiser? ';' condition? ';' increment? ')' block
StmtPtr Parser::parse_for() {
    consume(TokenType::LeftParen, "Expected '(' at the start of a for loop");

    StmtPtr initialiser = nullptr;
    ExprPtr condition = nullptr;
    ExprPtr increment = nullptr;

    if (match(TokenType::Semicolon)) {
        initialiser = nullptr;
    } else if (match(TokenType::KeywordLet)) {
        // Kinda cheating, but only allowing let statements.
        initialiser = parse_let(); // This consumes ';'
    } else {
        initialiser = std::make_shared<ExprStmt>(parse_expression());
        consumeSemicolon();
    }
        
    if (!match(TokenType::Semicolon)) {
        // If there is something...
        condition = parse_expression();
        consumeSemicolon();
    }

    // If there is something...
    if (!check(TokenType::RightParen)) {
        increment = parse_expression();
    }
    
    consume(TokenType::RightParen, "Expected ')' at the end of a for loop");

    StmtPtr body = parse_block();

    // add increment at end of body
    if (increment != nullptr) {
        body = std::make_shared<Block>(
            std::vector<StmtPtr>{
                body,
                std::make_shared<ExprStmt>(increment)
            }
        );
    }

    // default condition = true
    if (condition == nullptr) {
        condition = std::make_shared<Literal>(true);
    }

    // while loop
    body = std::make_shared<While>(move(condition), move(body));

    // wrap with initializer
    if (initialiser != nullptr) {
        body = std::make_shared<Block>(
            std::vector<StmtPtr>{
                initialiser,
                body
            }
        );
    }

    return body;
}

/*
class_stmt   → "class" IDENTIFIER '{' aggregate_member* '}' ';'
record_stmt → "record" IDENTIFIER '{' aggregate_member* '}' ';'
aggregate_member → functionDecl | let_stmt (TODO: change to fieldDecl. its actually not letstmt.)
*/
StmtPtr Parser::parse_aggregate(AggregateKind kind) {
    //consume(TokenType::KeywordClass, "Expected class keyword");

    Token name = consume(TokenType::Identifier, "Expected an identifier");

    consume(TokenType::LeftBrace, "Expected '{'");

    std::vector<FieldMember> fieldMembers;
    std::vector<MethodMember> methodMembers;
    std::vector<ConstructorMember> constructorMembers;

    // NOTICE: the expressions will be shared among all constructors.
    std::vector<StmtPtr> initStmts;

    while (!match(TokenType::RightBrace)) {
        if (match(TokenType::KeywordInit)) {
            // Constructors
            FunctionExprPtr constructorFunc = std::static_pointer_cast<FunctionExpr>(parse_functionExpr());
            constructorMembers.push_back(
                ConstructorMember(constructorFunc)
            );

        } else if (match(TokenType::KeywordLet)) {
            std::shared_ptr<Let> letStmt = std::static_pointer_cast<Let>(parse_let());
            /*
            Token name = consume(TokenType::Identifier, "Expected an identifier");
            
            TypeNodePtr typeAnnotation = nullptr;
            if (match(TokenType::Colon)) {
                typeAnnotation = parse_type();
            }
            
            ExprPtr initExpr = nullptr;
            if (match(TokenType::Assign)) {
                initExpr = parse_expression();
            }

            consumeSemicolon();
            
            if (initExpr) {
                initStmts.push_back(std::make_shared<ExprStmt>(
                    std::make_shared<Assignment>(
                        AssignmentOp::Assign,
                        std::make_shared<Get>(
                            std::make_shared<ThisExpr>(),
                            name.lexeme
                        ),
                        std::move(initExpr)
                    )
                ));
            }
            */

            if (letStmt->expr) {
                initStmts.push_back(std::make_shared<ExprStmt>(
                    std::make_shared<Assignment>(
                        AssignmentOp::Assign,
                        std::make_shared<Get>(
                            std::make_shared<ThisExpr>(),
                            letStmt->name
                        ),
                        std::move(letStmt->expr)
                    )
                ));
            }

            fieldMembers.push_back(
                FieldMember(
                    move(letStmt->annotatedType),
                    letStmt->name,
                    move(letStmt->expr),
                    // TODO: Unify parsing [modifiers] [name] ':' [type] '=' [expr] format.
                    letStmt->isMutable
                )
            );
        } else if (match(TokenType::KeywordFun)) {
            // Desugared form: let [functionName] = [functionExpr]
            std::shared_ptr<Let> letStmt = std::static_pointer_cast<Let>(parse_functionDecl());
            methodMembers.push_back(
                MethodMember(
                    move(letStmt->name),
                    // This must be FunctionExpr.
                    std::static_pointer_cast<FunctionExpr>(move(letStmt->expr))
                )
            );
        } else {
            throw KMYParseError(
                "Only field declarations (let) and method declarations (fun) are allowed in aggregates",
                peek().line, // This isnt previous().
                peek().startIdx,
                peek().endIdx
            );
        }
    }

    consumeSemicolon();

    /*
    // SHARED AST ISSUE
    // We inject field initialisation statements in front of user-defined constructor body
    for (auto& constrMem : constructorMembers) {
        auto constrBody = std::static_pointer_cast<Block>(constrMem.initFuncExpr->body);
        constrBody->statements.insert(
            constrBody->statements.begin(),
            initStmts.begin(),
            initStmts.end()
        );
    }
    */

    auto fieldInitFunc = std::make_shared<FunctionExpr>(
        std::vector<Parameter> {},
        std::make_shared<Block>(std::move(initStmts)),
        nullptr
    );
    
    return std::make_shared<Aggregate>(
        kind,
        move(name.lexeme),
        move(fieldMembers),
        move(methodMembers),
        move(constructorMembers),
        move(fieldInitFunc)
    );
}

/*
typealias_stmt → "typealias" IDENTIFIER '=' type ';'
*/
StmtPtr Parser::parse_typeAlias() {
    Token name = consume(TokenType::Identifier, "Expected an identifier");

    consume(TokenType::Assign, "Expected '='");

    TypeNodePtr type = parse_type();
    std::cout << type << "\n";

    consumeSemicolon();

    return std::make_shared<TypeAlias>(
        name.lexeme,
        std::move(type)
    );
}


/*
enum_stmt → "enum" IDENTIFIER '{' enum_list? '}' ';'
enum_list → IDENTIFIER (',' IDENTIFIER)*
*/
StmtPtr Parser::parse_enum() {
    Token name = consume(TokenType::Identifier, "Expected an identifier for the enum's name");
    
    consume(TokenType::LeftBrace, "Expected '{' after enum keyword");

    std::vector<std::string> variants;
    if (!check(TokenType::RightBrace)) {
        do {
            Token var = consume(TokenType::Identifier, "Expected an identifier for the enum's variant");
            variants.push_back(var.lexeme);
        } while (match(TokenType::Comma));
    }

    consume(TokenType::RightBrace, "Expected '}' after enum definition");

    consumeSemicolon();

    return std::make_shared<Enum>(name.lexeme, std::move(variants));
}

// =============================
// Helpers
// =============================

// TODO: INCOMPLETE!!!! &= fails. (Really?)
int Parser::get_binding_power(TokenType type) {
    switch (type) {
        case TokenType::Assign:
        case TokenType::PlusAssign:
        case TokenType::MinusAssign:
        case TokenType::StarAssign:
        case TokenType::SlashAssign:
        case TokenType::BitAndAssign:
        case TokenType::BitOrAssign:
        case TokenType::BitXorAssign:
        case TokenType::LShiftAssign:
        case TokenType::RShiftAssign:
        case TokenType::LogicalAndAssign:
        case TokenType::LogicalOrAssign:
            return BP_ASSIGNMENT;

        case TokenType::LogicalOr: return BP_LOGICAL_OR;
        case TokenType::LogicalAnd: return BP_LOGICAL_AND;

        case TokenType::BitOr: return BP_BITWISE_OR;
        case TokenType::BitXor: return BP_BITWISE_XOR;
        case TokenType::BitAnd: return BP_BITWISE_AND;

        case TokenType::EqualEqual:
        case TokenType::NotEqual:
            return BP_EQUALITY;

        case TokenType::Greater:
        case TokenType::GreaterEqual:
        case TokenType::Less:
        case TokenType::LessEqual:
            return BP_COMPARISON;

        case TokenType::LShift:
        case TokenType::RShift:
            return BP_SHIFT;

        case TokenType::Plus:
        case TokenType::Minus:
            return BP_TERM;

        case TokenType::Star:
        case TokenType::Slash:
        case TokenType::Percent:
            return BP_FACTOR;

        case TokenType::LeftParen:
        case TokenType::LeftBracket:
        case TokenType::Dot:
            return BP_POSTFIX;

        default:
            return BP_NONE;
    }
}

// Right-associative operators
bool Parser::is_right_associative(TokenType type) {
    switch (type) {
        case TokenType::Assign:
        case TokenType::PlusAssign:
        case TokenType::MinusAssign:
        case TokenType::StarAssign:
        case TokenType::SlashAssign:
        case TokenType::BitAndAssign:
        case TokenType::BitOrAssign:
        case TokenType::BitXorAssign:
        case TokenType::LShiftAssign:
        case TokenType::RShiftAssign:
        case TokenType::LogicalAndAssign:
        case TokenType::LogicalOrAssign:
            return true;
        default:
            return false;
    }
}

// =============================
// Main Pratt Parse (Expressions)
// =============================

/*
Parse left
Let stronger operators steal right side
Return when weaker operator appears

Do it with 1 + 2 * 3 - 4.

result = [ recursion 1 / minBp = 10]
First, parse 1
    left = 1
See +
Check can I continue?
    Since bp(100) >= minBp(0), we can continue consuming.
Consume +
parse rest as right1 and build +(1, right1) = +(1, -(*(2, 3), 4))
We are at EOF. (Continue check omitted) return left.
Final tree:
    +
   / \
  1   -
     / \
    *   4
   / \
  2   3

right1 = [ recursion 2 / minBp = 100 ]
First, parse 2
    left = 2
See *
Check can I continue?
    Since bp(110) >= minBp(100), we can continue consuming.
Consume *
parse rest as right and left = *(2, right2) = *(2, 3)
See -
Check can I continue?
    Since bp(100) >= minBp(100), we can continue consuming.
Consume -
parse rest as right3 and left = -(*(2, 3), 4)
We are at EOF. (Continue check omitted) return left


right2 = [ recursion 3 / minBp = 110 ]
First, parse 3
    left = 3
See -
Check can I continue?
    Since bp(100) < minBp(110), we STOP.
We DONT consume - and return left = 3

right3 = [ recursion 4 / minBp = 100 ]
First, parse 4
    left = 4
Check can I continue?
    Since bp(0) < minBp(100), we STOP.
We are at EOF. Return left = 4

*/

/*
e.g. a = arr[i] = 5 * 3 + 2 * 1

minBp = 10
left = a
bp = 10(=)
nextBp = 10 + 1 (Since = is r-associative)
    [right]
    minBp = 11
    left = arr
    bp = 10(=)
    index = i (We skip this for convenience)
    left = index(arr, i)
    nextBp = 10 + 1
        [right]
        minBp = 11
        left = 5
        bp = 110(*)
        nextBp = 110
            [right]
            minBp = 110
            left = 3
            bp = 100(+)
            return left
        right = 3
        left = *(5, 3)
        bp = 100(+)
        nextBp = 100
            [right]
            minBp = 100
            left = 2
            bp = 110(*)
            nextBp = 110
                [right]
                minBp = 110
                left = 1
                bp = 0
                return left
            right = 1
            left = *(2, 1)
            bp = 0
            return left
        right = *(2, 1)
        left = +(*(5, 3), *(2, 1))
        bp = 0
        return left
    right = +(*(5, 3), *(2, 1))
    left = =(index(arr, i), +(*(5, 3), *(2, 1)))
    bp = 0
    return left
right = =(index(arr, i), +(*(5, 3), *(2, 1)))
left = =(a, =(index(arr, i), +(*(5, 3), *(2, 1))))
bp = 0
return left
*/
ExprPtr Parser::parse_expression(int minBP) {
    ExprPtr left = parse_prefix();

    while (true) {
        TokenType op = peek().type;
        int bp = get_binding_power(op);

        if (bp < minBP)
            break;

        advance();

        // Call
        if (op == TokenType::LeftParen) {
            left = finishCall(std::move(left));
            continue;
        }

        // Index
        if (op == TokenType::LeftBracket) {
            ExprPtr index = parse_expression();
            consume(TokenType::RightBracket, "Expected ']'");
            left = std::make_shared<Index>(std::move(left), std::move(index));
            continue;
        }

        // Get
        if (op == TokenType::Dot) {
            Token name = consume(TokenType::Identifier, "Expected property name");
            left = std::make_shared<Get>(std::move(left), name.lexeme);
            continue;
        }

        // For left-associative operators, increasing minBP prevents
        // equal-precedence operators from binding on the right.
        int nextMinBP = bp + (is_right_associative(op) ? 0 : 1);

        // IMPORTANT OBSERVATION:
        // Parse the RHS until encountering an operator
        // with lower precedence (or equal precedence for left-associative ops).
        // e.g. 1 + 2 * 3 / 1 - 3
        //          ^^^^^^^^^right
        //      ^left
        // e.g. 1 + 2 * 3 / 1 - 3
        //              ^right
        //          ^left
        // e.g. x = y += z = 1 + 2 * 3
        //          ^^^^^^^^^^^^^^^^^^right (doesn't stop at += or = since its r-associative)         
        //      ^left
        ExprPtr right = parse_expression(nextMinBP);

        // Assignment operators
        if (bp == BP_ASSIGNMENT) {
            left = std::make_shared<Assignment>(
                toAssignmentOp(op),
                std::move(left),
                std::move(right)
            );
        }
        else {
            left = std::make_shared<BinaryExpr>(
                toBinaryOp(op),
                std::move(left),
                std::move(right)
            );
        }
    }

    return left;
}

// =============================
// Prefix(Primary) Parsing
// =============================

/*
array_elements → expression ("," expression)*
primary → 
    LITERAL |
    '(' expression ')'
    '[' array_elements? ']' |
    record |
    fnExpr |
    newExpr
*/
ExprPtr Parser::parse_prefix() {
    Token tok = advance();

    switch (tok.type) {

        // Literals
        case TokenType::Int:
            return std::make_shared<Literal>(stoi(tok.lexeme));

        case TokenType::Double:
            return std::make_shared<Literal>(stod(tok.lexeme));

        case TokenType::StringLiteral:
            return std::make_shared<Literal>(tok.lexeme);

        case TokenType::KeywordTrue:
            return std::make_shared<Literal>(true);

        case TokenType::KeywordFalse:
            return std::make_shared<Literal>(false);

        case TokenType::KeywordNull:
            return std::make_shared<Literal>(nullptr);

        case TokenType::Identifier: {
            // Check for IDENTIFIER "::" IDENTIFIER
            if (match(TokenType::ColonColon)) {
                std::vector<std::string> parts;
                parts.push_back(tok.lexeme);
                do {
                    Token memberName = consume(TokenType::Identifier, "Expected an identifier after \"::\"");
                    parts.push_back(memberName.lexeme);
                } while(match(TokenType::ColonColon));
                
                return std::make_shared<ScopeAccessExpr>(std::move(parts));
            }

            return std::make_shared<Variable>(tok.lexeme);
        }

        // Grouping
        case TokenType::LeftParen: {
            ExprPtr expr = parse_expression();
            consume(TokenType::RightParen, "Expected ')'");
            return expr;
        }

        // Arrays
        case TokenType::LeftBracket: {
            std::vector<ExprPtr> elements;
            if (!check(TokenType::RightBracket)) {
                do {
                    elements.push_back(parse_expression());
                } while(match(TokenType::Comma));
            }
            
            consume(TokenType::RightBracket, "Expected ']' after array literal");

            return std::make_shared<ArrayLiteral>( move(elements) );
        }

        // Function Expressions ( e.g., fun() {} )
        case TokenType::KeywordFun:
            return parse_functionExpr();
        
        case TokenType::LeftBrace:
            return parse_recordExpr();

        case TokenType::KeywordThis:
            return std::make_shared<ThisExpr>();
        
        case TokenType::KeywordNew:
            return parse_newExpr();

        // Unary operators
        case TokenType::Plus:
            return parse_expression(BP_UNARY);
        case TokenType::Minus:
        case TokenType::Bang:
        case TokenType::BitNot: { // ~
            ExprPtr right = parse_expression(BP_UNARY);
            return std::make_shared<UnaryExpr>(toUnaryOp(tok.type), std::move(right));
        }

        default:
            throw KMYParseError("Unexpected token in expression", tok.line, tok.startIdx, tok.endIdx);
    }
}


/*
type        → ( basicType | functionType | recordType ) arraySuffix*
basicType   → "int" | "string" | "bool" | "array" | "object" | "any" | "void"

functionType → '(' typeList? ')' "->" type
typeList    → type (',' type)*
arraySuffix  → '[' ( integer | "..." )? ']'

recordType → '{'  typePairs?  '}'
typePairs → typePair (',' typePair)*
typePair → IDENTIFIER ':' type
*/

TypeNodePtr Parser::parseTypeToken(Token t) {
    switch (t.type) {
        case TokenType::KeywordInt:
            return std::make_shared<NamedTypeNode>("int");

        case TokenType::KeywordDouble:
            return std::make_shared<NamedTypeNode>("double");

        case TokenType::KeywordBool:
            return std::make_shared<NamedTypeNode>("bool");

        case TokenType::KeywordString:
            return std::make_shared<NamedTypeNode>("string");

        case TokenType::KeywordAny:
            return std::make_shared<NamedTypeNode>("any");

        case TokenType::KeywordVoid:
            return std::make_shared<NamedTypeNode>("void");

        case TokenType::Identifier:
            return std::make_shared<NamedTypeNode>(t.lexeme);
        
        default: 
            // TODO: previous() here gets the invalid token, not ':'
            throw KMYParseError("Expected identifier or type keyword after ':'", previous().line, previous().startIdx, previous().endIdx);
    }
}

TypeNodePtr Parser::parse_recordType() {
    consume(TokenType::LeftBrace, "Expected '{'");

    std::vector<std::pair<std::string, TypeNodePtr>> paramTypePairs; 

    if (!check(TokenType::RightBrace)) {
        do {
            Token name = consume(TokenType::Identifier, "Expected an identifier");
            
            consume(TokenType::Colon, "Expected ':'");

            TypeNodePtr type = parse_type();

            paramTypePairs.push_back( { name.lexeme, type } );
        } while(match(TokenType::Comma));
    }

    consume(TokenType::RightBrace, "Expected '}'");

    return std::make_shared<RecordTypeNode>(
        std::move(paramTypePairs)
    );
}

TypeNodePtr Parser::parse_functionType() {
    consume(TokenType::LeftParen, "Expected '('");
    std::vector<TypeNodePtr> paramTypes; 
    if (!check(TokenType::RightParen)) {
        do {
            paramTypes.push_back( parse_type() );
        } while(match(TokenType::Comma));
    }
    consume(TokenType::RightParen, "Expected ')'");
    consume(TokenType::Arrow, "Expected \"->\"");
    
    TypeNodePtr returnType = parse_type();
    return std::make_shared<FunctionTypeNode>(
        std::move(paramTypes),
        std::move(returnType)
    );
}

TypeNodePtr Parser::parse_type() {
    TypeNodePtr result = nullptr;
    if (check(TokenType::LeftParen)) {
        result = parse_functionType();
    } else if (check(TokenType::LeftBrace)) {
        result = parse_recordType();
    } else {
        Token typeToken = peek();
        result = parseTypeToken(typeToken);
        advance(); // It is like this due to proper usage of previous() in parseTypeToken() error
    }

    while (match(TokenType::LeftBracket)) {
        if (match(TokenType::RightBracket)) {
            // Static array, e.g., int[]
            result = std::make_shared<ArrayTypeNode>(result, -1, false, false);
        } else if (false) {
            // TODO: Dynamic array will have diff expr...
            // Dynamic array, e.g., int[...]
            consume(TokenType::RightBracket, "Expected ']' after '...'");
            result = std::make_shared<ArrayTypeNode>(result, 0, true, true);
        } else if (check(TokenType::Int)) {
            // Bounded array, e.g., int[5]
            Token sizeToken = consume(TokenType::Int, "Expected integer for array size");
            consume(TokenType::RightBracket, "Expected ']' after array size");
            result = std::make_shared<ArrayTypeNode>(result, std::stoi(sizeToken.lexeme), true, false);
        } else {
            throw KMYParseError(
                "Invalid array type syntax. Expected ']' or an integer",
                previous().line,
                previous().startIdx, 
                previous().endIdx
            );
        }
    }

    return result;
}

/*
fnExpr → "fun" '(' params? ')' block
params → param (',' param)*
param  → "const"? ( ( IDENTIFIER (':' type)? ('=' expression)? ) | "..." IDENTIFIER (':' type)? )

fnExpr is more like a lambda function!
*/
ExprPtr Parser::parse_functionExpr() {
    consume(TokenType::LeftParen, "Expected '(' after function declaration");

    std::vector<Parameter> params;
    bool defaultSeen = false;
    if (!check(TokenType::RightParen)) {
        do {
            bool isMutable = !match(TokenType::KeywordConst);
            if (match(TokenType::Ellipsis)) {

                Token name = consume(TokenType::Identifier, "Expected an identifier");

                if (!check(TokenType::RightParen)) {
                    throw KMYParseError(
                        "Variadic parameter should come at the end of the parameter list. Didn't see ')'",
                        previous().line,
                        previous().startIdx,
                        previous().endIdx
                    );
                }

                TypeNodePtr varargType = match(TokenType::Colon) ? parse_type() : nullptr;

                params.push_back(
                    Parameter(std::move(varargType), name.lexeme, true, isMutable, false, nullptr) 
                );
                break;
            }

            Token name = consume(TokenType::Identifier, "Expected an identifier");
            
            TypeNodePtr paramType = match(TokenType::Colon) ? parse_type() : nullptr;
            
            ExprPtr defaultValue = nullptr;
            bool isDefault;
            if (isDefault = match(TokenType::Assign)) {
                defaultSeen = true;
                defaultValue = parse_expression();
            } else if (defaultSeen) {
                throw KMYParseError(
                    "Non-default parameter detected after a default parameter",
                    previous().line,
                    previous().startIdx,
                    previous().endIdx
                );
            }
            
            params.push_back(
                Parameter(std::move(paramType), name.lexeme, false, isMutable, isDefault, defaultValue) 
            );
        } while (match(TokenType::Comma));
    }

    consume(TokenType::RightParen, "Expected ')' after function parameters");
    
    TypeNodePtr returnType = match(TokenType::Colon) ? parse_type() : nullptr;

    StmtPtr body = parse_block();

    return std::make_shared<FunctionExpr>(
        std::move(params),
        std::move(body),
        std::move(returnType)
    );
}


/*
record → '{' (pair (',' pair)*)? '}'
pair   → IDENTIFIER ':' expression
*/
ExprPtr Parser::parse_recordExpr() {
    std::vector<std::pair<std::string, ExprPtr>> fields;

    if (check(TokenType::RightBrace)) {
        advance();
        return std::make_shared<RecordLiteral>(
            std::move(fields)
        );
    }

    do {
        Token name = consume(TokenType::Identifier, "Expected an identifier");
        
        consume(TokenType::Colon, "Expected ':'");
        
        ExprPtr expr = parse_expression();
        fields.push_back({name.lexeme, expr});

    } while (match(TokenType::Comma));
    
    consume(TokenType::RightBrace, "Expected '}' after record declaration");

    return std::make_shared<RecordLiteral>(
        std::move(fields)
    ); 
}  


/*
newExpr → "new" IDENTIFIER '(' args? ')'
args    → expression ("," expression)*
*/
ExprPtr Parser::parse_newExpr() {
    Token name = consume(TokenType::Identifier, "Expected an identifier");

    consume(TokenType::LeftParen, "Expected '(' after class/record name");
    std::vector<ExprPtr> args;

    if (!check(TokenType::RightParen)) {
        do {
            args.push_back(parse_expression());
        } while (match(TokenType::Comma));
    }
    
    consume(TokenType::RightParen, "Expected ')' after class constructor args");

    return std::make_shared<NewExpr>(
        name.lexeme,
        std::move(args)
    );
}


ExprPtr Parser::finishCall(ExprPtr callee) {
    std::vector<ExprPtr> args;

    if (!check(TokenType::RightParen)) {
        do {
            args.push_back(parse_expression());
        } while (match(TokenType::Comma));
    }

    consume(TokenType::RightParen, "Expected ')'");

    return std::make_shared<Call>(
        std::move(callee),
        std::move(args)
    );
}
