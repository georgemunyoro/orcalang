#pragma once

#include "OrcaAst.h"
#include "OrcaLexerErrorListener.h"
#include "OrcaParser.h"
#include "OrcaParserVisitor.h"
#include <vector>

using namespace orcagrammar;

class OrcaAstBuilder : public OrcaParserVisitor {
public:
  OrcaAstBuilder(OrcaContext &compileContext)
      : compileContext(compileContext) {}

  OrcaAstProgramNode *build();

  std::any visitProgram(OrcaParser::ProgramContext *context) override;

  std::any visitType(OrcaParser::TypeContext *context) override;

  std::any visitStructFieldDeclarationList(
      OrcaParser::StructFieldDeclarationListContext *context) override {
    printf("visitStructFieldDeclarationList\n");
    throw "TODO";
  };

  std::any visitStructFieldDeclaration(
      OrcaParser::StructFieldDeclarationContext *context) override {
    printf("visitStructFieldDeclaration\n");
    throw "TODO";
  };

  std::any
  visitTypeSpecifier(OrcaParser::TypeSpecifierContext *context) override {
    printf("visitTypeSpecifier\n");
    throw "TODO";
  };

  std::any visitTypeList(OrcaParser::TypeListContext *context) override {
    printf("visitTypeList\n");
    throw "TODO";
  };

  std::any visitExpression(OrcaParser::ExpressionContext *context) override;

  std::any visitAssignmentExpression(
      OrcaParser::AssignmentExpressionContext *context) override;

  std::any visitAssignmentOperator(
      OrcaParser::AssignmentOperatorContext *context) override {
    printf("visitAssignmentOperator\n");
    throw "TODO";
  };

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
  visitUnaryOperator(OrcaParser::UnaryOperatorContext *context) override {
    printf("visitUnaryOperator\n");
    throw "TODO";
  };

  std::any visitPostfixExpression(
      OrcaParser::PostfixExpressionContext *context) override;

  std::any visitArgumentExpressionList(
      OrcaParser::ArgumentExpressionListContext *context) override {
    printf("visitArgumentExpressionList\n");
    throw "TODO";
  };

  std::any visitPrimaryExpression(
      OrcaParser::PrimaryExpressionContext *context) override;

  std::any
  visitLetExpression(OrcaParser::LetExpressionContext *context) override;

  std::any
  visitArrayExpression(OrcaParser::ArrayExpressionContext *context) override;

  std::any visitFieldMap(OrcaParser::FieldMapContext *context) override {
    printf("visitFieldMap\n");
    throw "TODO";
  };

  std::any
  visitFieldMapEntry(OrcaParser::FieldMapEntryContext *context) override {
    printf("visitFieldMapEntry\n");
    throw "TODO";
  };

  std::any
  visitSizeofExpression(OrcaParser::SizeofExpressionContext *context) override;

  std::any
  visitExpressionList(OrcaParser::ExpressionListContext *context) override;

  std::any visitStatement(OrcaParser::StatementContext *context) override;

  std::any visitDeclarationStatement(
      OrcaParser::DeclarationStatementContext *context) override;

  std::any
  visitTypeDeclaration(OrcaParser::TypeDeclarationContext *context) override;

  std::any
  visitIdentifierList(OrcaParser::IdentifierListContext *context) override {
    printf("visitIdentifierList\n");
    throw "TODO";
  };

  std::any visitFunctionDeclarationStatement(
      OrcaParser::FunctionDeclarationStatementContext *context) override;

  std::any visitFunctionArg(OrcaParser::FunctionArgContext *context) override {
    printf("visitFunctionArg\n");
    throw "TODO";
  };

  std::any
  visitFunctionArgs(OrcaParser::FunctionArgsContext *context) override {
    printf("visitFunctionArgs\n");
    throw "TODO";
  };

  std::any
  visitJumpStatement(OrcaParser::JumpStatementContext *context) override;

  std::any
  visitLabeledStatement(OrcaParser::LabeledStatementContext *context) override {
    printf("visitLabeledStatement\n");
    throw "TODO";
  };

  std::any visitCompoundStatement(
      OrcaParser::CompoundStatementContext *context) override;

  std::any visitSelectionStatement(
      OrcaParser::SelectionStatementContext *context) override;

  std::any
  visitIfStatementBody(OrcaParser::IfStatementBodyContext *context) override;

  std::any visitIterationStatement(
      OrcaParser::IterationStatementContext *context) override;

  std::any visitExpressionStatement(
      OrcaParser::ExpressionStatementContext *context) override;

private:
  std::vector<OrcaAstNode *> nodes;
  const OrcaContext &compileContext;
};
