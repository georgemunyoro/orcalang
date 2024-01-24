#pragma once

#include <any>
#include <map>
#include <string>

#include "OrcaParser.h"
#include "OrcaType.h"

using namespace orcagrammar;

class OrcaAstVisitor;

class OrcaAstNode {
public:
  virtual ~OrcaAstNode() = default;
  virtual std::any accept(OrcaAstVisitor &visitor) = 0;
  virtual void print(int indent) = 0;
};

class OrcaAstExpressionNode : public OrcaAstNode {
public:
  virtual ~OrcaAstExpressionNode() = default;
  virtual std::any accept(OrcaAstVisitor &visitor) = 0;
};

class OrcaAstStatementNode : public OrcaAstNode {
public:
  virtual ~OrcaAstStatementNode() = default;
  virtual std::any accept(OrcaAstVisitor &visitor) = 0;
};

/**
 * @brief Represents a let expression.
 * e.g. let x: int;
 *          ^   ^
 *   name --/   \-- type
 */
class OrcaAstLetExpressionNode : public OrcaAstExpressionNode {
public:
  OrcaAstLetExpressionNode(const std::string &name, OrcaType *type)
      : name(name), type(type) {}

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sLetExpressionNode\n", indent, "");
    printf("%*sname: %s\n", indent + 2, "", name.c_str());
    printf("%*stype: %d\n", indent + 2, "", (int)type->getKind());
  }

private:
  std::string name;
  OrcaType *type;
};

class OrcaAstProgramNode : public OrcaAstNode {
public:
  OrcaAstProgramNode(std::vector<OrcaAstNode *> nodes) : nodes(nodes) {}
  ~OrcaAstProgramNode() = default;

  std::any accept(OrcaAstVisitor &visitor) override;

  void addNode(OrcaAstNode *node) { nodes.push_back(node); }

  void print(int indent) override {
    printf("%*sProgramNode\n", indent, "");
    for (auto &node : nodes) {
      node->print(indent + 2);
    }
  }

  std::vector<OrcaAstNode *> nodes;
};

class OrcaAstBinaryExpressionNode : public OrcaAstExpressionNode {
public:
  OrcaAstBinaryExpressionNode(OrcaAstExpressionNode *lhs,
                              OrcaAstExpressionNode *rhs, std::string op)
      : lhs(lhs), rhs(rhs) {}
  
  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sBinaryExpressionNode\n", indent, "");
    printf("%*slhs:\n", indent + 2, "");
    lhs->print(indent + 4);
    printf("%*srhs:\n", indent + 2, "");
    rhs->print(indent + 4);
  }

private:
  OrcaAstExpressionNode *lhs;
  OrcaAstExpressionNode *rhs;
  std::string op;
};

class OrcaAstAssignmentExpressionNode : public OrcaAstBinaryExpressionNode {
public:
  OrcaAstAssignmentExpressionNode(OrcaAstExpressionNode *lhs,
                                  OrcaAstExpressionNode *rhs, std::string op)
      : OrcaAstBinaryExpressionNode(lhs, rhs, op) {}

  std::any accept(OrcaAstVisitor &visitor) override;
};

class OrcaAstUnaryExpressionNode : public OrcaAstExpressionNode {
public:
  OrcaAstUnaryExpressionNode(OrcaAstExpressionNode *expr, std::string op)
      : expr(expr), op(op) {}

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sUnaryExpressionNode\n", indent, "");
    printf("%*sexpr:\n", indent + 2, "");
    expr->print(indent + 4);
  }

private:
  OrcaAstExpressionNode *expr;
  std::string op;
};

class OrcaAstConditionalExpressionNode : public OrcaAstExpressionNode {
public:
  OrcaAstConditionalExpressionNode(OrcaAstExpressionNode *condition,
                                   OrcaAstExpressionNode *thenExpr,
                                   OrcaAstExpressionNode *elseExpr)
      : condition(condition), thenExpr(thenExpr), elseExpr(elseExpr) {}

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sConditionalExpressionNode\n", indent, "");
    printf("%*scondition:\n", indent + 2, "");
    condition->print(indent + 4);
    printf("%*sthenExpr:\n", indent + 2, "");
    thenExpr->print(indent + 4);
    printf("%*selseExpr:\n", indent + 2, "");
    elseExpr->print(indent + 4);
  }

private:
  OrcaAstExpressionNode *condition;
  OrcaAstExpressionNode *thenExpr;
  OrcaAstExpressionNode *elseExpr;
};

class OrcaAstExpressionListNode : public OrcaAstExpressionNode {
public:
  OrcaAstExpressionListNode(std::vector<OrcaAstExpressionNode *> expressions)
      : expressions(expressions) {}

  std::any accept(OrcaAstVisitor &visitor) override;

  void addExpression(OrcaAstExpressionNode *expression) {
    expressions.push_back(expression);
  }

  void print(int indent) override {
    printf("%*sExpressionListNode\n", indent, "");
    for (auto &expression : expressions) {
      expression->print(indent + 2);
    }
  }

private:
  std::vector<OrcaAstExpressionNode *> expressions;
};

class OrcaAstTypeNode : public OrcaAstNode {
public:
  OrcaAstTypeNode(OrcaParser::TypeContext *typeContext)
      : typeContext(typeContext) {}

  std::any accept(OrcaAstVisitor &visitor) override;

  // We store the context here instead of the type because
  // that's the job of the type checker, where ast type node
  // objects are mapped to the result of their evaluation.
  OrcaParser::TypeContext *typeContext;

  void print(int indent) override {
    printf("%*sTypeNode\n", indent, "");
    printf("%*s%s\n", indent + 2, "", typeContext->getText().c_str());
  }
};

class OrcaAstTypeDeclarationNode : public OrcaAstStatementNode {
public:
  OrcaAstTypeDeclarationNode(const std::string &name, OrcaAstTypeNode *type)
      : name(name), type(type) {}

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sTypeDeclarationNode\n", indent, "");
    printf("%*sname: %s\n", indent + 2, "", name.c_str());
    printf("%*stype:\n", indent + 2, "");
    type->print(indent + 4);
  }

private:
  std::string name;
  OrcaAstTypeNode *type;
};

class OrcaAstTemplateTypeDeclarationNode : public OrcaAstStatementNode {
public:
  OrcaAstTemplateTypeDeclarationNode(const std::vector<std::string> params,
                                     const std::string &name,
                                     OrcaAstTypeNode *type)
      : params(params), name(name), type(type) {}

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sTemplateTypeDeclarationNode\n", indent, "");
    printf("%*sparams:\n", indent + 2, "");
    for (auto &param : params) {
      printf("%*s%s\n", indent + 4, "", param.c_str());
    }
    printf("%*sname: %s\n", indent + 2, "", name.c_str());
    printf("%*stype:\n", indent + 2, "");
    type->print(indent + 4);
  }

private:
  std::vector<std::string> params;
  std::string name;
  OrcaAstTypeNode *type;
};

class OrcaAstCompoundStatementNode : public OrcaAstStatementNode {
public:
  OrcaAstCompoundStatementNode(std::vector<OrcaAstNode *> nodes)
      : nodes(nodes) {}

  std::any accept(OrcaAstVisitor &visitor) override;

  void addNode(OrcaAstNode *node) { nodes.push_back(node); }

  void print(int indent) override {
    printf("%*sCompoundStatementNode\n", indent, "");
    for (auto &node : nodes) {
      node->print(indent + 2);
    }
  }

private:
  std::vector<OrcaAstNode *> nodes;
};

class OrcaAstFunctionDeclarationNode : public OrcaAstStatementNode {
public:
  OrcaAstFunctionDeclarationNode(const std::string &name,
                                 OrcaAstTypeNode *returnType,
                                 std::map<std::string, OrcaAstTypeNode *> args,
                                 OrcaAstCompoundStatementNode *body)
      : name(name), returnType(returnType), args(args), body(body) {}

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sFunctionDeclarationNode\n", indent, "");
    printf("%*sname: %s\n", indent + 2, "", name.c_str());
    printf("%*sreturnType:\n", indent + 2, "");
    returnType->print(indent + 4);
    printf("%*sargs:\n", indent + 2, "");
    for (auto &arg : args) {
      printf("%*s%s:\n", indent + 4, "", arg.first.c_str());
      arg.second->print(indent + 6);
    }
    printf("%*sbody:\n", indent + 2, "");
    body->print(indent + 4);
  }

private:
  std::string name;
  OrcaAstTypeNode *returnType;
  std::map<std::string, OrcaAstTypeNode *> args;
  OrcaAstCompoundStatementNode *body;
};

class OrcaAstJumpStatementNode : public OrcaAstStatementNode {
public:
  OrcaAstJumpStatementNode(const std::string &keyword,
                           OrcaAstExpressionNode *expr)
      : keyword(keyword), expr(expr) {}

  OrcaAstJumpStatementNode(const std::string &keyword)
      : keyword(keyword), expr(nullptr) {}

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sJumpStatementNode\n", indent, "");
    printf("%*skeyword: %s\n", indent + 2, "", keyword.c_str());
    printf("%*sexpr:\n", indent + 2, "");
    expr->print(indent + 4);
  }

private:
  std::string keyword;
  OrcaAstExpressionNode *expr;
};
