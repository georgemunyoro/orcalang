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
