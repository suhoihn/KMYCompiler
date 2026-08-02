del .\Interpreter\comp.exe
cls
g++ -Wall .\Compiler\main.cpp .\Semantics\MethodLower.cpp .\Semantics\SymbolScopeBuilder.cpp .\Semantics\Resolver.cpp .\Semantics\ClosureAnalyser.cpp .\Semantics\TypeInterner.cpp .\Semantics\DeclTypeResolver.cpp .\Compiler\compiler.cpp .\BytecodeVM\vm.cpp .\Utils\NativeFunctionImpl.cpp .\Utils\utils.cpp .\Utils\SymbolPrinter.cpp .\Utils\DefaultVisitor.cpp .\Core\lexer.cpp .\Core\newParser.cpp .\Core\Ast.cpp .\Core\value.cpp .\CodegenIR\IRBuilder.cpp .\MachineIR\MIRBuilder.cpp -o comp.exe
