#include "lexer.h"
#include "parser.h"
#include "eval.h"
#include <iostream>
#include <string>
#include "linenoise.h"

int main()
{
    while(true)
    {
        char* line_c = linenoise("calc> ");
        if(!line_c) { std::cout << "\n"; break; } // Ctrl-D
        std::string line(line_c);
        free(line_c);

        if(line.empty()) continue;

        linenoiseHistoryAdd(line.c_str()); // поддержка стрелки вверх

        try
        {
            Lexer lex(line);
            auto tokens = lex.tokenize();
            auto rpn = toRPN(tokens);
            double res = evalRPN(rpn);
            std::cout << res << '\n';
            /* 
            for(int i = 0; i < rpn.size(); ++i)
            {
                std::cout << rpn[i].data_;
            }
            std::cout << '\n';
        `   */
        }
        catch(const std::exception& ex)
        {
            std::cout << "Ошибка: " << ex.what() << '\n';
        }
    }
    return 0;
}
