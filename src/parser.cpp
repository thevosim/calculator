#include "parser.h"
#include "Stack.h"
#include <stdexcept>

static int priority(TokenType t)
{
    if(t == TokenType::MUL || t == TokenType::DIV) return 2;
    if(t == TokenType::PLUS || t == TokenType::MINUS) return 1;
    return 0;
}
bool isOp(Token tk)
{
    return (tk.type_ == TokenType::PLUS || tk.type_ == TokenType::MINUS \
    || tk.type_ == TokenType::MUL || tk.type_ == TokenType::DIV);
}
bool isNum(Token tk) {return tk.type_ == TokenType::NUMBER;}
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
                while(!ops.empty())
                {
                    Token top = ops.top(); ops.pop();
                    if(top.type_ == TokenType::LPAREN) break;
                    output.push_back(top);
                }
                break;
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


enum class State
{
    START,
    EXPECT_TERM,
    EXPECT_OP,
    AFTER_LPAREN,
    AFTER_LPAREN_NUMBER,
    AFTER_LPAREN_EXPECT_TERM,
    AFTER_LPAREN_DONE_TERM,
    END
};

void syntaxAnalyzer(const std::vector<Token>& tokens)
{
    Stack<Token, std::vector<Token>> parens;
    for(size_t i = 0; i < tokens.size(); ++i)
    {
        Token tk = tokens[i];
        if(tk.type_ == TokenType::LPAREN) parens.push(tk);
        if(tk.type_ == TokenType::RPAREN)
        {
            if(parens.empty()) throw std::runtime_error("Скобки не согласованы");
            else parens.pop();
        }
    }
    if(!(parens.empty())) throw std::runtime_error("Скобки не согласованы");
    State st = State::START;
    for(size_t i = 0; i < tokens.size(); ++i)
    {
        const Token &tk = tokens[i];
        if(tk.type_ == TokenType::END) 
        {
            st = State::END;
            break;
        }
        switch(st)
        {
            case State::START:
            {
                if(tk.type_ == TokenType::NUMBER) st = State::EXPECT_OP;
                else if(tk.type_ == TokenType::LPAREN) st = State::AFTER_LPAREN;
                else throw std::runtime_error("Синтаксическая ошибка");
                break;
            }

            case State::EXPECT_TERM:
            {
                if(tk.type_ == TokenType::NUMBER) st = State::EXPECT_OP;
                else if(tk.type_ == TokenType::LPAREN) st = State::AFTER_LPAREN;
                else throw std::runtime_error("Синтаксическая ошибка");
                break;
            }

            case State::EXPECT_OP:
            {
                if(tk.type_ == TokenType::PLUS || tk.type_ == TokenType::MINUS || tk.type_ == TokenType::MUL  || tk.type_ == TokenType::DIV)
                {
                    st = State::EXPECT_TERM;
                }
                else if(tk.type_ == TokenType::RPAREN)
                {
                    st = State::EXPECT_OP;
                }
                else throw std::runtime_error("Синтаксическая ошибка");
                break;
            }

            case State::AFTER_LPAREN:
            {
                if(tk.type_ == TokenType::NUMBER) st = State::AFTER_LPAREN_NUMBER;
                else if(tk.type_ == TokenType::LPAREN) st = State::AFTER_LPAREN;
                else throw std::runtime_error("Синтаксическая ошибка");
                break;
            }

            case State::AFTER_LPAREN_NUMBER:
            {
                if(tk.type_ == TokenType::PLUS || tk.type_ == TokenType::MINUS ||
                   tk.type_ == TokenType::MUL  || tk.type_ == TokenType::DIV)
                {
                    st = State::AFTER_LPAREN_EXPECT_TERM;
                }
                else throw std::runtime_error("Синтаксическая ошибка");
                break;
            }

            case State::AFTER_LPAREN_EXPECT_TERM:
            {
                if(tk.type_ == TokenType::NUMBER) st = State::AFTER_LPAREN_DONE_TERM;
                else if(tk.type_ == TokenType::LPAREN) st = State::AFTER_LPAREN;
                else throw std::runtime_error("Синтаксическая ошибка");
                break;
            }
            case State::AFTER_LPAREN_DONE_TERM:
            {
                if(tk.type_ == TokenType::PLUS || tk.type_ == TokenType::MINUS || tk.type_ == TokenType::MUL  || tk.type_ == TokenType::DIV)
                    st = State::AFTER_LPAREN_EXPECT_TERM;
                if(tk.type_ == TokenType::RPAREN)
                    st = State::EXPECT_OP;
            }
        }
    }
    if(st != State::END) throw std::runtime_error("Входные токены не содержат END");
}