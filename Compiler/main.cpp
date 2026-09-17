#include <iostream>
#include <fstream>
#include <exception>
#include <cstdlib>
#include <streambuf>

#include "../Utils/utils.hpp"
#include "../Utils/SymbolPrinter.hpp"
#include "../Utils/FrontendJson.hpp"
#include "../Utils/PrettyAst.hpp"
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
#include "../SSA/SSABuilder.hpp" // Pass 7
#include "../MachineIR/MIRBuilder.hpp" // Pass X
#include "../X86Codegen/x86Builder.hpp" // The ultimate pass... right?

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

static void printUsage(std::ostream& out) {
    out << "KMY compiler (kmyc)\n"
        << "2026 (c) KMY\n\n"
        << "Usage:\n"
        << "  kmyc <source-file> [options]\n"
        << "  kmyc --help\n\n"
        << "Options:\n"
        << "  -h, --help             Show this help text and exit.\n"
        << "  -asm, --asm            Compile through the native x86-64 backend.\n"
        << "  -o, --output <path>    Set the output executable/assembly base path.\n"
        << "  --no-run               Generate output but do not link/run it.\n"
        << "  --frontend-json        Emit frontend tokens/trace JSON and stop.\n"
        << "  -d, --debug            Print intermediate compiler information.\n"
        << "  -t, --strict-types     Enable the legacy strict-type switch.\n\n"
        << "Examples:\n"
        << "  kmyc Examples/Hello.kmy\n"
        << "  kmyc Examples/Hello.kmy -asm -o hello\n"
        << "  kmyc Examples/Hello.kmy --frontend-json\n";
}

// Older passes still print their internal tracing directly to stdout. Keep it
// out of both normal builds and the curated -d view without hiding stderr.
class QuietPassOutput {
    class Sink : public std::streambuf {
        int overflow(int c) override { return traits_type::not_eof(c); }
    } sink;
    std::streambuf* previous;

public:
    QuietPassOutput() : previous(std::cout.rdbuf(&sink)) {}
    ~QuietPassOutput() { std::cout.rdbuf(previous); }
    QuietPassOutput(const QuietPassOutput&) = delete;
    QuietPassOutput& operator=(const QuietPassOutput&) = delete;
};

template <typename F>
auto runQuietly(const char* passName, F&& pass) {
    setAstTracePass(passName);
    QuietPassOutput quiet;
    return pass();
}

static void printDebugHeading(const std::string& name) {
    std::cout << "\n" << name << "\n" << std::string(name.size(), '-') << "\n";
}

int main(int argc, char *argv[]) {
    // Switches
    bool debugOutput = false;
    bool strictTypes = false;
    bool isBuildingASM = false;
    bool frontendJson = false;
    bool run = true;
    const char* output = nullptr;
    const char* filename = nullptr;

    if (argc == 1) {
        printUsage(std::cout);
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        const char* str = argv[i];

        if (strcmp(str, "-h") == 0 || strcmp(str, "--help") == 0) {
            printUsage(std::cout);
            return 0;
        }

        if (strcmp(str, "--frontend-json") == 0) {
            if (frontendJson) {
                printLog(LogLevel::WARN, "Duplicate switch (--frontend-json) detected.");
            }
            frontendJson = true;
            run = false;
            continue;
        }

        if (strcmp(str, "-d") == 0 || strcmp(str, "--debug") == 0) {
            if (debugOutput) {
                printLog(LogLevel::WARN, "Duplicate switch (--debug) detected.");
            }
            debugOutput = true;
            continue;
        }

        if (strcmp(str, "-t") == 0 || strcmp(str, "--strict-types") == 0) {
            if (strictTypes) {
                printLog(LogLevel::WARN, "Duplicate switch (--strict-types) detected.");
            }
            strictTypes = true;
            continue;
        }

        if (strcmp(str, "-asm") == 0 || strcmp(str, "--asm") == 0) {
            if (isBuildingASM) {
                printLog(LogLevel::WARN, "Duplicate switch (--asm) detected.");
            }
            isBuildingASM = true;
            continue;
        }

        if (strcmp(str, "--no-run") == 0) {
            if (!run) {
                printLog(LogLevel::WARN, "Duplicate switch (--no-run) detected.");
            }
            run = false;
            continue;
        }

        if (strcmp(str, "-o") == 0 || strcmp(str, "--output") == 0) {
            if (output) {
                printLog(LogLevel::WARN, "Duplicate output switch detected; using the last path.");
            }
            if (i + 1 >= argc || argv[i + 1][0] == '-') {
                printLog(LogLevel::ERROR, "Output switch requires a path: -o <path>");
                return 1;
            }
            output = argv[++i];
            continue;
        }

        if (str[0] == '-') {
            std::cerr << "Unknown switch: " << str << "\n\n";
            printUsage(std::cerr);
            return 1;
        }

        if (filename) {
            std::cerr << "Unexpected extra source file: " << str << "\n\n";
            printUsage(std::cerr);
            return 1;
        }
        filename = str;
    }

    if (!filename) {
        std::cerr << "No source file specified.\n\n";
        printUsage(std::cerr);
        return 1;
    }

    setAstTraceEnabled(debugOutput);

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << "\n";
        return 1;
    }

    std::string source((std::istreambuf_iterator<char>(file)),
    std::istreambuf_iterator<char>());
    
    std::vector<Token> tokens;
    std::vector<ParserTraceEvent> parserTrace;

    try {
        // 1. Tokenize
        Lexer lexer(source);
        tokens = lexer.tokenise();

        if (debugOutput && !frontendJson) {
            printDebugHeading("Tokens");
            printTokens(tokens);
        }

        // 2. Parse
        Parser parser(tokens, frontendJson ? &parserTrace : nullptr);
        FunctionExprPtr program = parser.parse();
        if (frontendJson) {
            writeFrontendJson(std::cout, source, tokens, parserTrace, program);
            std::cout << '\n';
            return 0;
        }
        if (debugOutput) {
            printDebugHeading("Parsed AST");
            printPrettyAST(std::cout, program);
        }
        
        // 3-1. Symbol building
        auto globalScope = runQuietly("SymbolScopeBuilder", [&] {
            SymbolScopeBuilder builder(program);
            return builder.analyse();
        });

        // 3-2. Type declaration and function signature builder
        runQuietly("DeclTypeResolver", [&] {
            DeclTypeResolver temp(program, globalScope);
            temp.resolve(); // TODO: Better name
        });

        // 3-3. Variable resolvance
        runQuietly("Resolver", [&] {
            Resolver resolver(program, globalScope);
            resolver.resolve();
        });

        
        //3-3.5(?). Method lowering
        runQuietly("MethodLower", [&] {
            MethodLower lower(program);
            lower.lower();
        });

        if (debugOutput) {
            printDebugHeading("Resolved AST");
            printPrettyAST(std::cout, program);
        }

        // 3-4. Closure analysis and slot allocation (VM).
        runQuietly("ClosureAnalyser", [&] {
            ClosureAnalyser analyser(program);
            analyser.analyse();
        });
        
        // 3-4. Type check
        if (strictTypes) {
            std::cerr << "Warning: --strict-types is deprecated; the resolver already checks types.\n";
            // TypeChecker checker(program);
            // checker.check();
            // std::cout << "[DEBUG]: Type checks done." << std::endl;
        }

        
        if (isBuildingASM) {
            // 4-a. Code gen (CFG IR)
            IRBuilder builder(program);
            auto funcs = runQuietly("IRBuilder", [&] { return builder.compile(); });
            if (debugOutput) {
                printDebugHeading("HIR (before SSA)");
                for (const auto& func : funcs) std::cout << *func << "\n";
            }

            // 4-b. Phi computation and SSA renaming
            runQuietly("SSABuilder", [&] {
                SSABuilder ssaBuilder(funcs);
                ssaBuilder.build();
            });

            if (debugOutput) {
                printDebugHeading("HIR (after SSA)");
                for (const auto& func : funcs) std::cout << *func << "\n";
            }

            MIRBuilder mirBuilder(funcs);
            auto mirFuncs = runQuietly("MIRBuilder", [&] { return mirBuilder.lower(); });
            if (debugOutput) {
                printDebugHeading("MIR");
                for (const auto& func : mirFuncs) std::cout << *func << "\n";
            }

            std::ostringstream buffer;

            X86Builder x86Builder(mirFuncs, buffer, builder.getStringPool());
            runQuietly("X86Builder", [&] { x86Builder.build(); });

            std::string assembly = buffer.str();

            if (debugOutput) {
                printDebugHeading("x86-64 assembly");
                std::cout << assembly;
            }

            
            if (!output) {
                output = "out";
            }
            
            std::string asmFile = std::string(output) + ".s";
            {
                std::ofstream file(asmFile);
                if (!file) {
                    std::cerr << "Could not write assembly file: " << asmFile << "\n";
                    return 1;
                }
                file << assembly;
            } // close file.

            std::cout << "Assembly written to " << asmFile << "\n";

            if (run) {
                std::string cmd = "gcc \"" + asmFile + "\" \"Runtime C Functions\\runtime.o\" -o \"" + std::string(output) + "\"";

                int result = std::system(cmd.c_str());

                if (result != 0) {
                    std::cerr << "gcc failed\n";
                    return 1;
                }

                std::cout << "Built " << output << "\n";
            }
            return 0;
        }

        // 4. Code gen (Stack VM)
        Compiler compiler(program);
        auto fnProtos = runQuietly("VMCompiler", [&] { return compiler.compile(); });

        if (debugOutput) {
            printDebugHeading("VM bytecode");
            int fnProtoId = 0;
            for (auto& fnProto : fnProtos) {
                std::cout << "Function " << fnProtoId++
                          << " (" << fnProto.upValueCnt << " captures):\n"
                          << chunkToString(fnProto.chunk) << '\n';
            }
        }

        if (run) {
            // 4. run
            setAstTracePass(nullptr);
            VM vm;
            vm.load(fnProtos);
            vm.run();
    
            //std::exit(0);
        }
        return 0;

    } catch (const KMYParseError& e) {
        if (frontendJson) {
            writeFrontendErrorJson(
                std::cout, source, tokens, parserTrace,
                e.what(), e.line(), e.start(), e.end()
            );
            std::cout << '\n';
            return 1;
        }
        printDiagnostic(e, source);
        return 1;
    } catch (const KMYCompileError& e) {
        std::cerr << RED << "Compile error: " << e.what() << RESET << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << RED << "Runtime error: " << e.what() << RESET << std::endl;
        return 1;
    }

    printUsage(std::cerr);
    return 1;
}
