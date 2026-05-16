del .\Interpreter\comp.exe
cls
g++ .\Compiler\main.cpp .\Compiler\compiler.cpp .\BytecodeVM\vm.cpp .\Core\utils.cpp .\Core\lexer.cpp .\Core\newParser.cpp .\Core\Ast.cpp .\Core\value.cpp -o comp.exe
