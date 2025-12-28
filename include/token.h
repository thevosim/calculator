#pragma once
#include <string>
enum class TokenType { NUMBER, PLUS, MINUS, MUL, DIV, LPAREN, RPAREN, END};
struct Token
{
	TokenType type_;
    double value_; // для чисел (NUMBER)
    std::string data_;
    Token(TokenType type, double value, std::string data) : type_{type}, value_{value}, data_{data} {}
    Token(){}
};