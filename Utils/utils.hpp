#pragma once
#include <string>
#include <vector>
#include "../Core/tokens.hpp"
#include "../Core/AstBaseForward.hpp"
#include "../Core/Ast.hpp"

void log(const std::string& msg);
void printTokens(const std::vector<Token>& tokens);
void printAST(const std::vector<StmtPtr>& stmts);
void printAST(const ExprPtr expr);
