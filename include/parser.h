#pragma once
#include "token.h"
#include <vector>

std::vector<Token> toRPN(const std::vector<Token>& tokens);
void syntaxAnalyzer(const std::vector<Token>& tokens);