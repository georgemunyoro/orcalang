#include "OrcaParserVisitor.h"

using namespace orcagrammar;

class TypeChecker : public OrcaParserVisitor {
public:
  std::any visitProgram(OrcaParser::ProgramContext *context) override;

  std::any visitType(OrcaParser::TypeContext *context) override;

  std::any visitStructFieldDeclarationList(
      OrcaParser::StructFieldDeclarationListContext *context) override;

  std::any visitStructFieldDeclaration(
      OrcaParser::StructFieldDeclarationContext *context) override;

  std::any
  visitTypeSpecifier(OrcaParser::TypeSpecifierContext *context) override;

  std::any visitTypeList(OrcaParser::TypeListContext *context) override;

  std::any visitExpression(OrcaParser::ExpressionContext *context) override;

  std::any visitAssignmentExpression(
      OrcaParser::AssignmentExpressionContext *context) override;

  std::any visitAssignmentOperator(
      OrcaParser::AssignmentOperatorContext *context) override;

  std::any visitConditionalExpression(
      OrcaParser::ConditionalExpressionContext *context) override;

  std::any visitLogicalOrExpression(
      OrcaParser::LogicalOrExpressionContext *context) override;

  std::any visitLogicalAndExpression(
      OrcaParser::LogicalAndExpressionContext *context) override;

  std::any visitInclusiveOrExpression(
      OrcaParser::InclusiveOrExpressionContext *context) override;

  std::any visitExclusiveOrExpression(
      OrcaParser::ExclusiveOrExpressionContext *context) override;

  std::any
  visitAndExpression(OrcaParser::AndExpressionContext *context) override;

  std::any visitEqualityExpression(
      OrcaParser::EqualityExpressionContext *context) override;

  std::any visitRelationalExpression(
      OrcaParser::RelationalExpressionContext *context) override;

  std::any
  visitShiftExpression(OrcaParser::ShiftExpressionContext *context) override;

  std::any visitAdditiveExpression(
      OrcaParser::AdditiveExpressionContext *context) override;

  std::any visitMultiplicativeExpression(
      OrcaParser::MultiplicativeExpressionContext *context) override;

  std::any
  visitCastExpression(OrcaParser::CastExpressionContext *context) override;

  std::any
  visitUnaryExpression(OrcaParser::UnaryExpressionContext *context) override;

  std::any
  visitUnaryOperator(OrcaParser::UnaryOperatorContext *context) override;

  std::any visitPostfixExpression(
      OrcaParser::PostfixExpressionContext *context) override;

  std::any visitArgumentExpressionList(
      OrcaParser::ArgumentExpressionListContext *context) override;

  std::any visitPrimaryExpression(
      OrcaParser::PrimaryExpressionContext *context) override;

  std::any
  visitLetExpression(OrcaParser::LetExpressionContext *context) override;

  std::any
  visitArrayExpression(OrcaParser::ArrayExpressionContext *context) override;

  std::any visitFieldMap(OrcaParser::FieldMapContext *context) override;

  std::any
  visitFieldMapEntry(OrcaParser::FieldMapEntryContext *context) override;
};
