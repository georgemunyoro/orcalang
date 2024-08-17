#include "OrcaAst.h"
#include "OrcaAstVisitor.h"
#include <any>

std::any OrcaAstLetExpressionNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitLetExpression(this);
}

std::any OrcaAstProgramNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitProgram(this);
}

std::any OrcaAstAssignmentExpressionNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitAssignmentExpression(this);
}

std::any OrcaAstConditionalExpressionNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitConditionalExpression(this);
}

std::any OrcaAstUnaryExpressionNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitUnaryExpression(this);
}

std::any OrcaAstExpressionListNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitExpressionList(this);
}

std::any OrcaAstTypeDeclarationNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitTypeDeclaration(this);
}

std::any OrcaAstTemplateTypeDeclarationNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitTemplateTypeDeclaration(this);
}

std::any OrcaAstTypeNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitType(this);
}

std::any OrcaAstJumpStatementNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitJumpStatement(this);
}

std::any OrcaAstCompoundStatementNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitCompoundStatement(this);
}

std::any OrcaAstFunctionDeclarationNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitFunctionDeclarationStatement(this);
}

std::any OrcaAstBinaryExpressionNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitBinaryExpression(this);
}

std::any OrcaAstIdentifierExpressionNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitIdentifierExpression(this);
}

std::any OrcaAstIntegerLiteralExpressionNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitIntegerLiteralExpression(this);
}

std::any OrcaAstFloatLiteralExpressionNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitFloatLiteralExpression(this);
}

std::any OrcaAstStringLiteralExpressionNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitStringLiteralExpression(this);
}

std::any OrcaAstBooleanLiteralExpressionNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitBooleanLiteralExpression(this);
}

std::any OrcaAstExpressionStatementNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitExpressionStatement(this);
}

std::any OrcaAstCastExpressionNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitCastExpression(this);
}

std::any OrcaAstSelectionStatementNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitSelectionStatement(this);
}

std::any OrcaAstIterationStatementNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitIterationStatement(this);
}

std::any OrcaAstFunctionCallExpressionNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitFunctionCallExpression(this);
}

std::any OrcaAstIndexExpressionNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitIndexExpression(this);
}

std::any OrcaAstMemberAccessExpressionNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitMemberAccessExpression(this);
}

std::any OrcaAstFieldMapNode::accept(OrcaAstVisitor &visitor) {
  return visitor.visitFieldMap(this);
}
