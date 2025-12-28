#include "lexer.h"
#include "parser.h"
#include "eval.h"
#include <iostream>
#include <string>
#include <replxx.hxx>

int main()
{
    replxx::Replxx rx;

    while(true)
    {
        const char* line_c = rx.input("calc> ");
        if(!line_c) { std::cout << "\n"; break; } // Ctrl-D
        std::string line(line_c);

        if(line.empty()) continue;

        rx.history_add(line.c_str()); // поддержка стрелки вверх

        try
        {
            Lexer lex(line);
            auto tokens = lex.tokenize();
            syntaxAnalyzer(tokens);
            auto rpn = toRPN(tokens);
            double res = evalRPN(rpn);
            std::cout << res << '\n';
        }
        catch(const std::exception& ex)
        {
            std::cout << ex.what() << '\n';
        }
    }
    return 0;
}
