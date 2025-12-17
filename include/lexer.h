#pragma once
#include "token.h"
#include <string>
#include <vector>

class Lexer
{
public:
    explicit Lexer(const std::string& input);
    std::vector<Token> tokenize(); // бросает std::runtime_error при ошибке
private:
    std::string s;
    size_t i = 0;
    void skipSpaces();
    Token readNumber();
    bool isDigit(char c) const { return c >= '0' && c <= '9'; }
};
