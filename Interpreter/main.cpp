#include <iostream>
#include <fstream>
#include "../Core/Ast.hpp"
#include "../Core/lexer.hpp"
#include "../Core/utils.hpp"
#include "../Core/newParser.hpp"
#include "../Core/errorhandler.hpp"
#include "repl.hpp"
#include "interpreter.hpp"


int main(int argn, char *argv[]) {
    Interpreter interpreter;
    if (argn == 1) {
        std::cout << "\033[31mREPL mode.\033[0m" << std::endl;
        //std::cout << "REPL mode.\n";
        start_repl(interpreter);
        return 0;
    }

    if (argn == 2 || argn == 3) {

        bool runRepl = false;
        std::string filename;

        for (int i = 1; i < argn; i++) {
            std::string arg = argv[i];

            if (arg == "-i" || arg == "--repl") {
                runRepl = true;
            } 
            else {
                filename = arg;
            }
        }

        std::cout << "[DEBUG]: Reading file..." << std::endl;

        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Could not open file: " << filename << "\n";
            return 1;
        }

        
        std::string source((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
        
        std::cout << "[DEBUG]: Reading done. Source file string created." << std::endl;

        try {
            // 1. tokenize
            Lexer lexer(source);
            std::vector<Token> tokens = lexer.tokenise();

            std::cout << "[DEBUG]: Lexing finished. Ready to parse." << std::endl;
            
            // 2. parse
            Parser parser(tokens);
            std::vector<StmtPtr> program = parser.parse();
            
            std::cout << "[DEBUG]: Parsing finished. Ready to run." << std::endl;

            // 3. run
            interpreter.interpret(program);
        } catch (const KMYParseError& e) {
            std::cerr << "Parse error: " << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }

        if (runRepl) {
            start_repl(interpreter);
        }

        return 0;
    }

    std::cerr << "Usage: ./interpreter <file>\n";
    return 1;
}