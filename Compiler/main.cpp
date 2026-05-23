#include <iostream>
#include <fstream>
#include <exception>

#include "../Core/lexer.hpp"
#include "../Core/utils.hpp"
#include "../Core/newParser.hpp"
#include "../Core/Ast.hpp"
#include "../Core/errorhandler.hpp"
#include "../Semantics/SymbolScopeBuilder.hpp" // Pass 1
#include "../Semantics/Resolver.hpp" // Pass 2
#include "../Semantics/ClosureAnalyser.hpp" // Pass 3?
#include "../Semantics/typechecker.hpp" // Optional pass 4
#include "compiler.hpp" // Closure check and code gen in pass 4
#include <cstring>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: mylang <source-file>\n";
        return 1;
    }

    const std::string filename = argv[1];

    // Switches
    bool debugOutput = false;
    bool strictTypes = false;

    for (int i = 2; i < argc; i++) {
        char* str = argv[i];
        
        if (strcmp(str, "-d") == 0) {
            if (debugOutput) {
                std::cout << "[WARNING]: Duplicate switch (-d) detected." << std::endl;
            }
            debugOutput = true;
            std::cout << "[DEBUG]: Intermediate results will be outputed." << std::endl;
        } if (strcmp(str, "-t") == 0) {
            if (strictTypes) {
                std::cout << "[WARNING]: Duplicate switch (-t) detected." << std::endl;
            }
            strictTypes = true;
            std::cout << "[DEBUG]: Strict typing enabled." << std::endl;
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
        // 1. Tokenize
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenise();

        std::cout << "[DEBUG]: Lexing finished. Ready to parse." << std::endl;
        if (debugOutput) {
            std::cout << "[DEBUG]: Tokens:\n";
            printTokens(tokens);
        }

        // 2. Parse
        Parser parser(tokens);
        FunctionExprPtr program = parser.parse();
        
        std::cout << "[DEBUG]: Parsing finished. Ready to move onto semantic analysis." << std::endl;
        
        // 3-1. Symbol building
        SymbolScopeBuilder builder(program);
        auto globalScope = builder.analyse();

        std::cout << "[DEBUG]: Symbol building done. Ready to resolve variables." << std::endl;
        
        std::cout << "[DEBUG]: Global scope symbols:\n";
        std::cout << globalScope << std::endl;
        for (const auto& [name, sym] : globalScope->symbols) {
            std::cout << "  " << name << " (mutable: " << sym->isMutable << ")\n";
        }
        std::cout << "so it was empty" << std::endl;
        // 3-2. Variable resolvance
        Resolver resolver(program, globalScope);
        resolver.resolve();
        
        std::cout << "[DEBUG]: Variable resolvance done. Ready to allocate local slots and analyse closures." << std::endl;
        
        // 3-3. Closure analysis?
        ClosureAnalyser analyser(program);
        analyser.analyse();
        
        std::cout << "[DEBUG]: Slot allocation and closure analysis done." << std::endl;
        
        // 3-4. Type check
        if (strictTypes) {
            std::cout << "[DEBUG]: Strict type check enabled." << std::endl;
            TypeChecker checker(program);
            checker.check();
            std::cout << "[DEBUG]: Type checks done." << std::endl;
        }
        std::cout << "[DEBUG]: Ready for code generation." << std::endl;

        // 4. Code gen (Stack VM)
        Compiler compiler(program);
        auto fnProtos = compiler.compile();

        std::cout << "[DEBUG]: Compilation done. Ready to run VM." << std::endl;
        for (auto& fnProto : fnProtos) {
            std::cout << chunkToString(fnProto.chunk) << std::endl;
        }

        // 4. run
        VM vm;
        vm.load(fnProtos);
        vm.run();

        std::cout << "[DEBUG]: VM halted." << std::endl;
        //std::exit(0);
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