#pragma once

#include "OrcaAst.h"
#include <any>

class OrcaAstVisitor {
public:
  virtual ~OrcaAstVisitor() = default;
  virtual std::any visitLetExpression(OrcaAstLetExpressionNode *node) = 0;
  virtual std::any visitProgram(OrcaAstProgramNode *node) = 0;
  virtual std::any
  visitAssignmentExpression(OrcaAstAssignmentExpressionNode *node) = 0;
  virtual std::any
  visitConditionalExpression(OrcaAstConditionalExpressionNode *node) = 0;
  virtual std::any visitUnaryExpression(OrcaAstUnaryExpressionNode *node) = 0;
  virtual std::any visitExpressionList(OrcaAstExpressionListNode *node) = 0;
  virtual std::any visitTypeDeclaration(OrcaAstTypeDeclarationNode *node) = 0;
  virtual std::any
  visitTemplateTypeDeclaration(OrcaAstTemplateTypeDeclarationNode *node) = 0;
  virtual std::any visitType(OrcaAstTypeNode *node) = 0;
  virtual std::any
  visitCompoundStatement(OrcaAstCompoundStatementNode *node) = 0;
  virtual std::any
  visitFunctionDeclarationStatement(OrcaAstFunctionDeclarationNode *node) = 0;
  virtual std::any visitJumpStatement(OrcaAstJumpStatementNode *node) = 0;
  virtual std::any visitBinaryExpression(OrcaAstBinaryExpressionNode *node) = 0;
  virtual std::any
  visitIdentifierExpression(OrcaAstIdentifierExpressionNode *node) = 0;
  virtual std::any
  visitIntegerLiteralExpression(OrcaAstIntegerLiteralExpressionNode *node) = 0;
  virtual std::any
  visitFloatLiteralExpression(OrcaAstFloatLiteralExpressionNode *node) = 0;
  virtual std::any
  visitStringLiteralExpression(OrcaAstStringLiteralExpressionNode *node) = 0;
  virtual std::any
  visitBooleanLiteralExpression(OrcaAstBooleanLiteralExpressionNode *node) = 0;
  virtual std::any
  visitExpressionStatement(OrcaAstExpressionStatementNode *node) = 0;
  virtual std::any visitCastExpression(OrcaAstCastExpressionNode *node) = 0;
  virtual std::any
  visitSelectionStatement(OrcaAstSelectionStatementNode *node) = 0;
  virtual std::any
  visitIterationStatement(OrcaAstIterationStatementNode *node) = 0;
  virtual std::any
  visitFunctionCallExpression(OrcaAstFunctionCallExpressionNode *node) = 0;
  virtual std::any visitIndexExpression(OrcaAstIndexExpressionNode *node) = 0;
  virtual std::any
  visitMemberAccessExpression(OrcaAstMemberAccessExpressionNode *node) = 0;
  virtual std::any visitFieldMap(OrcaAstFieldMapNode *node) = 0;
};
