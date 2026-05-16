del .\Interpreter\intp.exe
cls
g++ .\Interpreter\main.cpp .\Interpreter\env.cpp .\Interpreter\repl.cpp .\Core\utils.cpp .\Core\lexer.cpp .\Core\newParser.cpp .\Core\Ast.cpp .\Core\value.cpp -o intp.exe
