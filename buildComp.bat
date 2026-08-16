del .\Interpreter\comp.exe
cls
g++ .\Compiler\main.cpp .\Semantics\MethodLower.cpp .\Semantics\SymbolScopeBuilder.cpp .\Semantics\Resolver.cpp .\Semantics\ClosureAnalyser.cpp .\Semantics\TypeInterner.cpp .\Semantics\DeclTypeResolver.cpp .\Compiler\compiler.cpp .\BytecodeVM\vm.cpp .\Utils\NativeFunctionImpl.cpp .\Utils\utils.cpp .\Utils\SymbolPrinter.cpp .\Utils\DefaultVisitor.cpp .\Core\lexer.cpp .\Core\newParser.cpp .\Core\Ast.cpp .\Core\value.cpp .\CodegenIR\IRBuilder.cpp  -o comp.exe

REM .\MachineIR\MIRBuilder.cpp .\X86Codegen\x86Builder.cpp