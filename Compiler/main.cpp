#include <iostream>
#include <fstream>
#include <exception>

#include "../Core/lexer.hpp"
#include "../Core/utils.hpp"
#include "../Core/newParser.hpp"
#include "../Core/Ast.hpp"
#include "../Core/errorhandler.hpp"
#include "compiler.hpp"
#include "../Semantics/SemanticAnalyser.hpp"
#include "../Semantics/Resolver.hpp"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: mylang <source-file>\n";
        return 1;
    }

    const std::string filename = argv[1];

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
        
        std::cout << "[DEBUG]: Parsing finished. Ready to compile." << std::endl;

        // 2.5. (TODO lol) Semantic Analysis
        SemanticAnalyser analysis(program);
        analysis.analyse();

        // 3. compile
        Compiler compiler(program);
        Chunk chunk = compiler.compile();

        std::cout << "[DEBUG]: Compilation done. Ready to run VM." << std::endl;
        std::cout << chunkToString(chunk) << std::endl;

        // 4. run
        VM vm;
        vm.load(chunk);
        vm.run();

        std::cout << "[DEBUG]: VM halted." << std::endl;
        return 0;

    } catch (const KMYParseError& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
        return 1;
    } catch (const KMYCompileError& e) {
        std::cerr << "Compile error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return 1;
    }

    std::cerr << "Usage: ./comp <file>\n";
    return 1;
}