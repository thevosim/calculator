#include "lexer.h"
#include <stdexcept>
#include <cctype>

Lexer::Lexer(const std::string& input) : s(input), i(0) {}
void Lexer::skipSpaces()
{
    while(i < s.size() && s[i] == ' ') ++i;
}
// Конечный автомат для чисел: число должно начинаться с цифры
Token Lexer::readNumber()
{
    enum class State
    {
        START,
        INT_PART,
        DOT_SEEN,
        FRAC_PART,
        END,
        ERROR
    };

    State st = State::START;
    size_t start = i;
    while(i < s.size())
    {
        char c = s[i];
        switch(st)
        {
            case State::START:
                if(isDigit(c))
                {
                    st = State::INT_PART;
                    ++i;
                }
                else st = State::ERROR;
                break;

            case State::INT_PART:
                if(isDigit(c)) ++i;
                else if(c == '.')
                {
                    st = State::DOT_SEEN;
                    ++i;
                }
                else st = State::END;
                break;

            case State::DOT_SEEN:
                if(isDigit(c))
                {
                    st = State::FRAC_PART;
                    ++i;
                }
                else st = State::ERROR; // точка должна сопровождаться цифрой
                break;

            case State::FRAC_PART:
                if(isDigit(c)) ++i; 
                else st = State::END;
                break;

            default:
                st = State::ERROR;
                break;
        }
        if(st == State::END || st == State::ERROR) break;
    }

    if(st == State::INT_PART || st == State::FRAC_PART || st == State::END)
    {
        std::string tok = s.substr(start, i - start);
        try
        {
            double val = std::stod(tok);
            return Token(TokenType::NUMBER, val, tok);
        }
        catch(...)
        {
            throw std::runtime_error("Ошибка преобразования числа: " + tok);
        }
    }
    throw std::runtime_error(std::string("Лексическая ошибка при разборе числа на позиции ") + std::to_string(start));
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> out;
    while(true)
    {
        skipSpaces();
        if(i >= s.size())
        {
            out.emplace_back(TokenType::END, 0.0, "");
            return out;
        }
        char c = s[i];
        if(isDigit(c))
        {
            out.push_back(readNumber());
            continue;
        }
        if(c == '+') { out.emplace_back(TokenType::PLUS, 0.0, "+"); ++i; continue; }
        if(c == '-') { out.emplace_back(TokenType::MINUS, 0.0, "-"); ++i; continue; }
        if(c == '*') { out.emplace_back(TokenType::MUL, 0.0, "*"); ++i; continue; }
        if(c == '/') { out.emplace_back(TokenType::DIV, 0.0, "/"); ++i; continue; }
        if(c == '(') { out.emplace_back(TokenType::LPAREN, 0.0, "("); ++i; continue; }
        if(c == ')') { out.emplace_back(TokenType::RPAREN, 0.0, ")"); ++i; continue; }
        throw std::runtime_error(std::string("Неподдерживаемый символ: '") + c + "' на позиции " + std::to_string(i));
    }
}