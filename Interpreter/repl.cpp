#include "repl.hpp"

#include <iostream>
#include <string>
#include "../Core/lexer.hpp"
#include "../Core/utils.hpp"
#include "../Core/newParser.hpp"
#include "../Core/Ast.hpp"
#include "../Core/errorhandler.hpp"
#include "interpreter.hpp"

using namespace std;

void start_repl(Interpreter& intp) {
    string line;
    cout << ">>> ";
    while (getline(cin, line)) {
        if (line == "exit") {
            break;
        }

        Lexer lexer(line);
        vector<Token> tokens = lexer.tokenise();
        printTokens(tokens);
        try {
            Parser parser(tokens);
            vector<StmtPtr> stmts = parser.parse();

            printAST(stmts);

            intp.interpret(stmts);
            intp.printEnv();

        } catch (const KMYParseError& e) {
            cerr << "Parse error: " << e.what() << endl;
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
        }

        cout << ">>> ";
    }
}

