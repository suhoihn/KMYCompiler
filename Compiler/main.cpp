#include <iostream>
#include <fstream>
#include <exception>

#include "../Utils/utils.hpp"
#include "../Utils/SymbolPrinter.hpp"
#include "../Core/lexer.hpp"
#include "../Core/newParser.hpp"
#include "../Core/Ast.hpp"
#include "../Core/errorhandler.hpp"
#include "../Semantics/SymbolScopeBuilder.hpp" // Pass 1
#include "../Semantics/DeclTypeResolver.hpp" // Pass 2
#include "../Semantics/Resolver.hpp" // Pass 3
#include "../Semantics/ClosureAnalyser.hpp" // Pass 4
#include "../Semantics/MethodLower.hpp" // Pass 5
// #include "../Semantics/typechecker.hpp" // Planned pass 5
#include "compiler.hpp" // Code gen in pass 6
#include "../CodegenIR/IRBuilder.hpp" // Pass 6
#include <cstring>
#include <sstream>
#include "../MachineIR/MIRBuilder.hpp" // Pass X

const std::string RED = "\033[31m";
const std::string RESET = "\033[0m";
const std::string BOLD = "\033[1m";

static void printDiagnostic(
    const KMYParseError& e,
    const std::string& source
) {

    // -----------------------------
    // Find line text
    // -----------------------------
    std::istringstream ss(source);
    std::string lineStr;

    for (int i = 1; i <= e.line(); ++i) {
        if (!std::getline(ss, lineStr))
            return;
    }

    // -----------------------------
    // FIXED COLUMN CALCULATION
    // -----------------------------
    // DO NOT use e.start() directly as column.
    // Convert global offset -> line-local offset.

    int globalPos = e.start();
    int lineStart = globalPos;

    for (int i = globalPos; i >= 0; --i) {
        if (source[i] == '\n') {
            lineStart = i + 1;
            break;
        }
    }

    int column = globalPos - lineStart;

    // safety clamp
    column = std::max(0, column);

    int length = std::max(1, e.end() - e.start());

    std::cerr << RED
              << "Parse error: "
              << e.what()
              << " at line "
              << e.line()
              << ", column "
              << column + 1 // For display, column doesn't start at 0.
              << RESET
              << "\n";

    // -----------------------------
    // Print source line
    // -----------------------------
    std::cerr << RED << lineStr << RESET << "\n";

    // -----------------------------
    // Print underline
    // -----------------------------
    std::cerr << std::string(column, ' ')
              << BOLD << RED
              << std::string(length, '^')
              << RESET
              << "\n";
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: mylang <source-file>\n";
        return 1;
    }

    const std::string filename = argv[1];

    // Switches
    bool debugOutput = false;
    bool strictTypes = false;
    bool isBuildingASM = false;
    bool run = true;

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
        } if (strcmp(str, "-asm") == 0) {
            if (isBuildingASM) {
                printLog(LogLevel::WARN, "Duplicate switch (-asm) detected.");
            }
            isBuildingASM = true;
            std::cout << "[DEBUG]: Direct lowering enabled." << std::endl;
        }
        if (strcmp(str, "--no-run") == 0) {
            if (!run) {
                printLog(LogLevel::WARN, "Duplicate switch (--no-run) detected.");
            }
            run = false;
            std::cout << "[DEBUG]: Will not execute." << std::endl;
        }
    }

    printLog(LogLevel::INFO, "Reading file...\n");
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << "\n";
        return 1;
    }

    std::string source((std::istreambuf_iterator<char>(file)),
    std::istreambuf_iterator<char>());
    
    printLog(LogLevel::INFO, "Reading done. Source file string created.\n");

    try {
        // 1. Tokenize
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenise();

        printLog(LogLevel::INFO, "Lexing finished. Ready to parse.\n");
        if (debugOutput) {
            printLog(LogLevel::INFO, "Tokens:\n");
            printTokens(tokens);
        }

        // 2. Parse
        Parser parser(tokens);
        FunctionExprPtr program = parser.parse();
        if (debugOutput) {
            printLog(LogLevel::INFO, "AST:\n");
            // TODO: improve AST printing.
            printAST(program);
        }

        // Define symbol printer here.
        SymbolPrinter symPrinter(program);
        
        printLog(LogLevel::INFO, "Parsing finished. Ready to move onto semantic analysis.\n");
        
        // 3-1. Symbol building
        SymbolScopeBuilder builder(program);
        auto globalScope = builder.analyse();

        printLog(LogLevel::INFO, "Symbol building done. Ready to declare types and func signatures.\n");
        
        if (debugOutput) {
            printLog(LogLevel::INFO, "Symbols built: \n");
            symPrinter.print();
        }

        // 3-2. Type declaration and function signature builder
        DeclTypeResolver temp(program, globalScope);
        temp.resolve(); // TODO: Better name

        printLog(LogLevel::INFO, "Type declaration done. Ready to resolve variables and their types.\n");
        
        if (debugOutput) {
            printLog(LogLevel::INFO, "Symbols built: \n");
            symPrinter.print();
        }

        // 3-3. Variable resolvance
        Resolver resolver(program, globalScope);
        resolver.resolve();
        
        if (debugOutput) {
            printLog(LogLevel::INFO, "Symbols built: \n");
            symPrinter.print();
        }

        std::cout << "[DEBUG]: Variable resolvance done. Ready to lower methods if one exists." << std::endl;

        
        //3-3.5(?). Method lowering
        MethodLower lower(program);
        lower.lower();

        if (debugOutput) {
            printLog(LogLevel::INFO, "Symbols built: \n");
            symPrinter.print();
            printLog(LogLevel::INFO, "Modified AST: \n");
            printAST(program);
        }
        
       
        printLog(LogLevel::INFO, "Method lowering done. Ready to allocate local slots and analyse closures.\n");

        // 3-4. Closure analysis and slot allocation (VM).
        ClosureAnalyser analyser(program);
        analyser.analyse();

        if (debugOutput) {
            printLog(LogLevel::INFO, "Symbols built: \n");
            symPrinter.print();
        }
        
        printLog(LogLevel::INFO, "Slot allocation and closure analysis done.\n");
        
        // 3-4. Type check
        if (strictTypes) {
            std::cout << "[DEBUG]: Strict type check enabled." << std::endl;
            std::cout << "[WARNING]: DEPRECATED. Already done in resolver. Nothing will be done here.\n";
            // TypeChecker checker(program);
            // checker.check();
            // std::cout << "[DEBUG]: Type checks done." << std::endl;
        }

        
        printLog(LogLevel::INFO, "Ready for code generation.\n");

        if (isBuildingASM) {
            // 4-a. Code gen (CFG IR)
            IRBuilder builder(program);
            auto funcs = builder.compile();
            for (const auto& func: funcs) {
                std::cout << *func << "\n";
            }

            std::cout << "[DEBUG]: CFG IR generation done. Ready to lower to MIR." << std::endl;
            MIRBuilder mirBuilder(funcs);
            auto mirFuncs = mirBuilder.lower();
            for (const auto& func: mirFuncs) {
                std::cout << *func << "\n";
            }
            return 0;
        }

        // 4. Code gen (Stack VM)
        Compiler compiler(program);
        auto fnProtos = compiler.compile();

        if (debugOutput) {
            printLog(LogLevel::INFO, "Symbols built: \n");
            symPrinter.print();
        }

        std::cout << "[DEBUG]: Compilation done. Ready to run VM." << std::endl;
        int fnProtoId = 0;
        for (auto& fnProto : fnProtos) {
            std::cout << "Function proto " << fnProtoId << ":\n";
            std::cout << "Upvalue cnt " << fnProto.upValueCnt << "\n";
            std::cout << "Upvalue vec size " << fnProto.upvalues.size() << "\n";

            std::cout << chunkToString(fnProto.chunk) << std::endl;
            fnProtoId++;
        }

        if (run) {
            // 4. run
            VM vm;
            vm.load(fnProtos);
            vm.run();
    
            std::cout << "[DEBUG]: VM halted." << std::endl;
            //std::exit(0);
        }
        return 0;

    } catch (const KMYParseError& e) {
        printDiagnostic(e, source);
        return 1;
    } catch (const KMYCompileError& e) {
        std::cerr << RED << "Compile error: " << e.what() << RESET << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << RED << "Runtime error: " << e.what() << RESET << std::endl;
        return 1;
    }

    std::cerr << "Usage: ./comp <file>\n";
    return 1;
}