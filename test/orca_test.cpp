#include "ANTLRInputStream.h"
#include "OrcaLexer.h"
#include <CommonTokenStream.h>
#include <gtest/gtest.h>

// Utility function to parse a string and return the tokens
antlr4::CommonTokenStream *tokenize(const std::string &input) {
  antlr4::ANTLRInputStream input_stream("123");
  orcagrammar::OrcaLexer lexer(&input_stream);
  auto tokens = new antlr4::CommonTokenStream(&lexer);
  tokens->fill();
  return tokens;
}

// Parser tests
// TEST(OrcaParserTest, NumericConstantExpression) { EXPECT_EQ(0, 1); }

// Lexer tests

TEST(OrcaLexerTest, numeric_constant_expression) {
  auto tokens = tokenize("123");
  EXPECT_EQ(tokens->getTokens().size(), 2);
}

TEST(OrcaLexerTest, numeric_float_constant_expression) {
  auto tokens = tokenize("123.456");
  EXPECT_EQ(tokens->getTokens().size(), 2);
}

TEST(OrcaLexerTest, float_constant_expression_with_leading_zero) {
  auto tokens = tokenize("0.123");
  EXPECT_EQ(tokens->getTokens().size(), 2);
}

TEST(OrcaLexerTest, integer_constant_expression_with_leading_zero) {
  auto tokens = tokenize("0123");
  EXPECT_EQ(tokens->getTokens().size(), 2);
}

TEST(OrcaLexerTest, IdentifierExpression) {
  auto tokens = tokenize("abc");
  EXPECT_EQ(tokens->getTokens().size(), 2);
}
