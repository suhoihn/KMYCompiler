#pragma once
#include <string>
#include <vector>
#include "tokens.hpp"
#include "AstBaseForward.hpp"

void log(const std::string& msg);
void printTokens(const std::vector<Token>& tokens);
void printAST(const std::vector<StmtPtr>& stmts);
