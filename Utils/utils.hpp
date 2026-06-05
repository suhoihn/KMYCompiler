#pragma once
#include <string>
#include <vector>
#include "../Core/tokens.hpp"
#include "../Core/AstBaseForward.hpp"
#include "../Core/Ast.hpp"

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR
};

void printLog(LogLevel level, const std::string& msg);
void printTokens(const std::vector<Token>& tokens);
void printAST(const std::vector<StmtPtr>& stmts);
void printAST(const ExprPtr expr);
