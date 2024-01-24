#include "OrcaAst.h"

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
};
