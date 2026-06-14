#pragma once

#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include "tokens.hpp"
#include "Ast.hpp"
#include "type.hpp"


// Precedence values (higher = tighter binding)
enum BindingPower {
    BP_NONE = 0,

    BP_ASSIGNMENT = 10,     // = += -= ...
    BP_LOGICAL_OR = 20,     // ||
    BP_LOGICAL_AND = 30,    // &&
    BP_BITWISE_OR = 40,     // |
    BP_BITWISE_XOR = 50,    // ^
    BP_BITWISE_AND = 60,    // &
    BP_EQUALITY = 70,       // == !=
    BP_COMPARISON = 80,     // < > <= >=
    BP_SHIFT = 90,          // << >>
    BP_TERM = 100,          // + -
    BP_FACTOR = 110,        // * / %
    BP_UNARY = 120,         // ! - + ~
    BP_POSTFIX = 130        // (), [], .
};

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    // Entry point
    FunctionExprPtr parse();

private:
    // =============================
    // Token Helpers
    // =============================

    Token peek() const;
    Token previous() const;
    Token advance();
    bool match(TokenType type);
    bool check(TokenType type) const;
    bool is_at_end() const;
    Token consume(TokenType token, const std::string& errMsg);

    // =============================
    // Core Pratt Parsing (Expressions)
    // =============================

    ExprPtr parse_expression(int minBP=BP_ASSIGNMENT);
    ExprPtr parse_functionExpr();
    ExprPtr parse_recordExpr();
    ExprPtr parse_newExpr();
    ExprPtr parse_prefix();

    TypeNodePtr parseTypeToken(Token t);
    TypeNodePtr parse_type();
    TypeNodePtr parse_functionType();
    TypeNodePtr parse_arraySuffix();
    TypeNodePtr parse_recordType();
    
    ExprPtr finishCall(ExprPtr callee);

    int get_binding_power(TokenType type);
    bool is_right_associative(TokenType type);

    // =============================
    // Parsing statements
    // =============================
    
    StmtPtr parse_block();
    void consumeSemicolon();
    StmtPtr parse_statement();
    StmtPtr parse_let();
    StmtPtr parse_functionDecl(); // "fun f(a,b) {}" form
    StmtPtr parse_for();
    StmtPtr parse_aggregate(AggregateKind kind);
    StmtPtr parse_typeAlias();
    StmtPtr parse_enum();

    const std::vector<Token> tokens;
    size_t current = 0;
};
