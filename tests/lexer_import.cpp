#include "../Core/lexer.hpp"

#include <cassert>
#include <string>

int main() {
    const std::string source = "import \"collections/HashMap.kmy\" as collections; from importer fromage";
    const auto tokens = Lexer(source).tokenise();
    assert(tokens.size() == 9);
    assert(tokens[0].type == TokenType::KeywordImport);
    assert(tokens[1].type == TokenType::StringLiteral);
    assert(tokens[2].type == TokenType::KeywordAs);
    assert(tokens[3].type == TokenType::Identifier);
    assert(tokens[4].type == TokenType::Semicolon);
    assert(tokens[5].type == TokenType::KeywordFrom);
    assert(tokens[6].type == TokenType::Identifier); // `importer` is not a keyword.
    assert(tokens[7].type == TokenType::Identifier); // `fromage` is not a keyword.
    assert(tokens[8].type == TokenType::EndOfFile);
}
