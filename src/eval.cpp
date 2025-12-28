#include "eval.h"
#include "Stack.h"
#include <stdexcept>

double evalRPN(const std::vector<Token>& rpn)
{
    Stack<double, std::vector<double>> st;
    for(const Token& tk : rpn)
    {
        if(tk.type_ == TokenType::NUMBER)
        {
            st.push(tk.value_);
            continue;
        }
        if(tk.type_ == TokenType::PLUS || tk.type_ == TokenType::MINUS ||
           tk.type_ == TokenType::MUL || tk.type_ == TokenType::DIV)
        {
            if(st.size() < 2) throw std::runtime_error("Синтаксическая ошибка: недостаточно операндов для оператора " + tk.data_);
            double b = st.top(); st.pop();
            double a = st.top(); st.pop();
            double res = 0.0;
            switch(tk.type_)
            {
                case TokenType::PLUS: res = a + b; break;
                case TokenType::MINUS: res = a - b; break;
                case TokenType::MUL: res = a * b; break;
                case TokenType::DIV:
                    if(b == 0.0) throw std::runtime_error("Ошибка: деление на ноль");
                    res = a / b;
                    break;
                default: break;
            }
            st.push(res);
            continue;
        }
        throw std::runtime_error("Неожиданный токен в ОПН: " + tk.data_);
    }
    if(st.size() != 1) throw std::runtime_error("Синтаксическая ошибка: выражение некорректно");
    return st.top();
}
