#include "parser.h"
#include "Stack.h"
#include <stdexcept>

static int priority(TokenType t)
{
    if(t == TokenType::MUL || t == TokenType::DIV) return 2;
    if(t == TokenType::PLUS || t == TokenType::MINUS) return 1;
    return 0;
}
void syntaxAnalyzer(const std::vector<Token>& tokens)
{
    enum class State
    {
        START,
        NUM,
        OP,
        ERROR
    };
    Stack<Token, std::vector<Token>> parentheses_check;
    Token prev;
    for(size_t i = 1; i < tokens.size(); ++i)
    {   
        prev = tokens[i - 1];
        if(tokens[i].type_ == TokenType::LPAREN) parentheses_check.push(tokens[i]);
        if(tokens[i].type_ == TokenType::RPAREN)
        {
            if(parentheses_check.empty()) throw std::runtime_error("Скобки не согласованы");
            if(prev.type_ == TokenType::NUMBER) throw std::runtime_error("Только число в скобках \"(num)\"");
            if(prev.type_ == TokenType::LPAREN) throw std::runtime_error("Пустое выражение в скобках \"()\"");
            else parentheses_check.pop();
        } 
    }
    size_t idx = 0;
    State st = State::START;
    while(idx < tokens.size())
    {
        Token tk = tokens[idx++];
        if(tk.type_ == TokenType::LPAREN || tk.type_ == TokenType::RPAREN) continue;
        switch(st)
        {
            case State::START:
            case State::OP:
                if(tk.type_ == TokenType::NUMBER)
                {
                    st = State::NUM;
                    break;
                }
                else
                {
                    st = State::ERROR;
                    break;
                }
            case State::NUM:
                if(tk.type_ == TokenType::PLUS || tk.type_ == TokenType::MINUS \
                || tk.type_ == TokenType::MUL || tk.type_ == TokenType::DIV)
                {
                    st = State::OP;
                    break;
                }
                else 
                {
                    st = State::ERROR;
                    break;
                }
            default: 
                st = State::ERROR; 
                break;
        }
        if(st == State::ERROR) break;
    }
    if(st == State::ERROR) throw std::runtime_error("Синтаксическая ошибка");
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

            case TokenType::END:
                while(!ops.empty())
                {
                    Token top = ops.top(); ops.pop();
                    output.push_back(top);
                }
                return output;

            default:
                throw std::runtime_error("Неожиданный токен при парсинге: " + tk.data_);
        }
    }
    throw std::runtime_error("Входной набор токенов не содержит END");
}
