#include <gtest.h>
#include "lexer.h"
#include "parser.h"
#include "eval.h"
#include "token.h"
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>

// Вспомогательные функции для тестов
static std::string rpnToString(const std::vector<Token>& rpn)
{
    std::ostringstream oss;
    bool first = true;
    for (const auto& t : rpn)
    {
        if (!first) oss << ' ';
        first = false;
        // используем текст токена как представление
        oss << t.data_;
    }
    return oss.str();
}

static double evalExpr(const std::string& expr)
{
    
    Lexer lexer(expr);
  
    auto tokens = lexer.tokenize();
    auto rpn = toRPN(tokens);
    double result = evalRPN(rpn);
  
    return result;
}


TEST(LexerTests, TokenEndPresent)
{
    
    Lexer lexer("1+1");

    auto toks = lexer.tokenize();
    
    ASSERT_FALSE(toks.empty());
    EXPECT_EQ(toks.back().type_, TokenType::END);
}

TEST(LexerTests, ReadIntegerAndLeadingZero)
{
    
    Lexer lexer("045 0 7");

    auto toks = lexer.tokenize();
    
    ASSERT_GE(toks.size(), 2);
    EXPECT_EQ(toks[0].type_, TokenType::NUMBER);
    EXPECT_EQ(toks[0].data_, "045");
    // value преобразуется через stod
    EXPECT_DOUBLE_EQ(toks[0].value_, 45.0);
}

TEST(LexerTests, ReadDecimalNumber)
{
    
    Lexer lexer("123.4");
    
    auto toks = lexer.tokenize();
    
    ASSERT_GE(toks.size(), 1);
    EXPECT_EQ(toks[0].type_, TokenType::NUMBER);
    EXPECT_EQ(toks[0].data_, "123.4");
    EXPECT_DOUBLE_EQ(toks[0].value_, 123.4);
}

TEST(LexerTests, DecimalStartingWithDotIsError)
{
    
    Lexer lexer(".5");
    
    EXPECT_THROW(lexer.tokenize(), std::runtime_error);
}

TEST(LexerTests, InvalidCharacterThrows)
{
    
    Lexer lexer("1 + a");
    
    EXPECT_THROW(lexer.tokenize(), std::runtime_error);
}

// ========== Parser (RPN) tests ==========

TEST(ParserTests, PrecedenceWithoutParentheses)
{
    
    Lexer lexer("3+4*2");
    auto tokens = lexer.tokenize();
    
    auto rpn = toRPN(tokens);
    
    EXPECT_EQ(rpnToString(rpn), "3 4 2 * +");
}

TEST(ParserTests, ParenthesesOverridePrecedence)
{
    
    Lexer lexer("(3+4)*2");
    auto tokens = lexer.tokenize();
    
    auto rpn = toRPN(tokens);
    
    EXPECT_EQ(rpnToString(rpn), "3 4 + 2 *");
}

TEST(ParserTests, ExtraRightParenthesisThrows)
{
    
    Lexer lexer("1 + )");
    auto tokens = lexer.tokenize();
    
    EXPECT_THROW(toRPN(tokens), std::runtime_error);
}

TEST(ParserTests, MissingRightParenthesisThrows)
{
    
    Lexer lexer("(1+2");
    auto tokens = lexer.tokenize();
    
    EXPECT_THROW(toRPN(tokens), std::runtime_error);
}

// ========== Eval tests ==========

TEST(EvalTests, SimpleAddition)
{
    
    double res = evalExpr("1+2");
    
    EXPECT_DOUBLE_EQ(res, 3.0);
}

TEST(EvalTests, PrecedenceEvaluation)
{
     
    double res = evalExpr("3+4*2");
    
    EXPECT_DOUBLE_EQ(res, 11.0);
}

TEST(EvalTests, ParenthesesEvaluation)
{
     
    double res = evalExpr("(3+4)*2");
    
    EXPECT_DOUBLE_EQ(res, 14.0);
}

TEST(EvalTests, DecimalComputation)
{
     
    double res = evalExpr("123.4+0.6");

    EXPECT_DOUBLE_EQ(res, 124.0);
}

TEST(EvalTests, DivisionByZeroThrows)
{
    
    Lexer lexer("1/0");
    auto tokens = lexer.tokenize();
    auto rpn = toRPN(tokens);
    
    EXPECT_THROW(evalRPN(rpn), std::runtime_error);
}

TEST(EvalTests, UnaryMinusNotSupportedThrows)
{
    
    Lexer lexer("-1+2");
    auto tokens = lexer.tokenize();
    auto rpn = toRPN(tokens);

    EXPECT_THROW(evalRPN(rpn), std::runtime_error);
}

TEST(EvalTests, ComplexExpression)
{
     
    double res = evalExpr("3+(4-2)*5/2");
    // Assert: (4-2)=2 -> *5=10 -> /2=5 -> +3 = 8
    EXPECT_DOUBLE_EQ(res, 8.0);
}

TEST(ComplexExprTests, DeeplyNestedSingleValue)
{
    
    const int depth = 20;
    std::string expr;
    for(int i = 0; i < depth; ++i) expr.push_back('(');
    expr += "1";
    for(int i = 0; i < depth; ++i) expr.push_back(')');
    
    double res = evalExpr(expr);

    EXPECT_DOUBLE_EQ(res, 1.0);
}

TEST(ComplexExprTests, LongChainAdditions)
{

    const int N = 50;
    std::string expr;
    for(int i = 0; i < N; ++i)
    {
        if(i) expr.push_back('+');
        expr += "1";
    }

    double res = evalExpr(expr);

    EXPECT_DOUBLE_EQ(res, static_cast<double>(N));
}

TEST(ComplexExprTests, LeftAssociativeSubtraction)
{
    std::string expr = "1-2-3-4"; // ((1-2)-3)-4 = -8

    double res = evalExpr(expr);

    EXPECT_DOUBLE_EQ(res, -8.0);
}

TEST(ComplexExprTests, ChainDivisionLeftAssociative)
{
    std::string expr = "100/5/2"; // (100/5)/2 = 10

    double res = evalExpr(expr);

    EXPECT_DOUBLE_EQ(res, 10.0);
}

TEST(ComplexExprTests, MixedPrecedenceAndParentheses)
{
 
    std::string expr = "3+4*2/(1-5)+6";
    // 4*2=8; 1-5=-4; 8/(-4)=-2; 3-2+6 = 7

    double res = evalExpr(expr);

    EXPECT_DOUBLE_EQ(res, 7.0);
}

TEST(ComplexExprTests, WhitespaceVariations)
{
    std::string expr = " 12  * (  3  + 4 )  ";

    double res = evalExpr(expr);

    EXPECT_DOUBLE_EQ(res, 84.0); // 12 * 7
}

TEST(ComplexExprTests, ConsecutiveOperatorsThrows)
{
    EXPECT_THROW(evalExpr("1++2"), std::runtime_error);
    EXPECT_THROW(evalExpr("5--3"), std::runtime_error); // унарный минус не поддерживается
}

TEST(ComplexExprTests, TrailingOperatorThrows)
{
    EXPECT_THROW(evalExpr("1+2*"), std::runtime_error);
}

TEST(ComplexExprTests, EmptyParenthesesThrows)
{
    EXPECT_THROW(evalExpr("()"), std::runtime_error);
    EXPECT_THROW(evalExpr("1 + ()"), std::runtime_error);
}

TEST(ComplexExprTests, MultipleDotsInNumberThrows)
{
    Lexer lx("1.2.3");
    EXPECT_THROW(lx.tokenize(), std::runtime_error);
}

TEST(ComplexExprTests, LeadingZerosAndDecimals)
{
    std::string expr = "000.050 + 0.05"; // 0.05 + 0.05 = 0.1

    double res = evalExpr(expr);

    EXPECT_NEAR(res, 0.1, 1e-12);
}

TEST(ComplexExprTests, LongChainMulDiv)
{
    // Arrange
    // 1024 / 2 десять раз = 1
    std::string expr = "1024";
    for(int i = 0; i < 10; ++i) expr += "/2";

    double res = evalExpr(expr);

    EXPECT_DOUBLE_EQ(res, 1.0);
}