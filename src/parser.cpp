#include "parser.h"
#include "Stack.h"
#include <stdexcept>

static int priority(TokenType t)
{
    if(t == TokenType::MUL || t == TokenType::DIV) return 2;
    if(t == TokenType::PLUS || t == TokenType::MINUS) return 1;
    return 0;
}

std::vector<Token> toRPN(const std::vector<Token>& tokens)
{
    std::vector<Token> output;
    Stack<Token, std::vector<Token>> ops;
    size_t idx = 0;
    while(idx < tokens.size())
    {
        Token tk = tokens[idx++];
        switch(tk.type_)
        {
            case TokenType::NUMBER:
                output.push_back(tk);
                break;

            case TokenType::PLUS:
            case TokenType::MINUS:
            case TokenType::MUL:
            case TokenType::DIV:
            {
                while(!ops.empty())
                {
                    Token top = ops.top();
                    if((top.type_ == TokenType::PLUS || top.type_ == TokenType::MINUS ||
                        top.type_ == TokenType::MUL || top.type_ == TokenType::DIV) &&
                       priority(top.type_) >= priority(tk.type_))
                    {
                        output.push_back(top);
                        ops.pop();
                    }
                    else break;
                }
                ops.push(tk);
                break;
            }

            case TokenType::LPAREN:
                ops.push(tk);
                break;

            case TokenType::RPAREN:
            {
                bool found = false;
                while(!ops.empty())
                {
                    Token top = ops.top(); ops.pop();
                    if(top.type_ == TokenType::LPAREN) { found = true; break; }
                    output.push_back(top);
                }
                if(!found) throw std::runtime_error("Скобки не согласованы: лишняя \")\"");
                break;
            }

            case TokenType::END:
                while(!ops.empty())
                {
                    Token top = ops.top(); ops.pop();
                    if(top.type_ == TokenType::LPAREN || top.type_ == TokenType::RPAREN)
                        throw std::runtime_error("Скобки не согласованы");
                    output.push_back(top);
                }
                return output;

            default:
                throw std::runtime_error("Неожиданный токен при парсинге: " + tk.data_);
        }
    }
    throw std::runtime_error("Входной набор токенов не содержит END");
}
