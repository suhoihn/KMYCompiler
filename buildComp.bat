del .\Interpreter\comp.exe
cls
g++ .\Compiler\main.cpp .\Semantics\MethodLower.cpp .\Semantics\SymbolScopeBuilder.cpp .\Semantics\Resolver.cpp .\Semantics\ClosureAnalyser.cpp .\Semantics\TypeInterner.cpp .\Compiler\compiler.cpp .\BytecodeVM\vm.cpp .\Utils\utils.cpp .\Utils\SymbolPrinter.cpp .\Core\lexer.cpp .\Core\newParser.cpp .\Core\Ast.cpp .\Core\value.cpp -o comp.exe
