#pragma once

#include "OrcaAstVisitor.h"
#include "OrcaType.h"

class OrcaTypeChecker : OrcaAstVisitor {
  ~OrcaTypeChecker() = default;

  std::any visitLetExpression(OrcaAstLetExpressionNode *node) override {
    return std::any();
  };

  std::any visitProgram(OrcaAstProgramNode *node) override {
    return std::any();
  };

  std::any
  visitAssignmentExpression(OrcaAstAssignmentExpressionNode *node) override {
    return std::any();
  };

  std::any
  visitConditionalExpression(OrcaAstConditionalExpressionNode *node) override {
    return std::any();
  };

  std::any visitUnaryExpression(OrcaAstUnaryExpressionNode *node) override {
    return std::any();
  };

  std::any visitExpressionList(OrcaAstExpressionListNode *node) override {
    return std::any();
  };

  std::any visitTypeDeclaration(OrcaAstTypeDeclarationNode *node) override {
    return std::any();
  };

  std::any visitTemplateTypeDeclaration(
      OrcaAstTemplateTypeDeclarationNode *node) override {
    return std::any();
  };

  std::any visitType(OrcaAstTypeNode *node) override { return std::any(); };

  std::any visitCompoundStatement(OrcaAstCompoundStatementNode *node) override {
    return std::any();
  };

  std::any visitFunctionDeclarationStatement(
      OrcaAstFunctionDeclarationNode *node) override {
    return std::any();
  };

  std::any visitJumpStatement(OrcaAstJumpStatementNode *node) override {
    return std::any();
  };

  std::any visitBinaryExpression(OrcaAstBinaryExpressionNode *node) override {
    return std::any();
  };

  std::any
  visitIdentifierExpression(OrcaAstIdentifierExpressionNode *node) override {
    return std::any();
  };

  std::any visitIntegerLiteralExpression(
      OrcaAstIntegerLiteralExpressionNode *node) override {
    return std::any();
  };

  std::any visitFloatLiteralExpression(
      OrcaAstFloatLiteralExpressionNode *node) override {
    return std::any();
  };

  std::any visitStringLiteralExpression(
      OrcaAstStringLiteralExpressionNode *node) override {
    return std::any();
  };

  std::any visitBooleanLiteralExpression(
      OrcaAstBooleanLiteralExpressionNode *node) override {
    return std::any();
  };

  std::any
  visitExpressionStatement(OrcaAstExpressionStatementNode *node) override {
    return std::any();
  };

  // Type map for mapping ast nodes to types
  std::map<OrcaAstNode *, OrcaType *> typeMap;

public:
  void run(OrcaAstNode *node) {
    node->accept(*this);

    typeMap[node] = new OrcaType(OrcaCharType());
  }

  void printTypeMap() {
    for (auto &pair : typeMap) {
      printf("%p | %s\n", pair.first, pair.second->toString().c_str());
    }
  }
};
