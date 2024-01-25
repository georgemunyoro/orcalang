#pragma once

#include <any>
#include <map>
#include <string>

#include "./utils/printfColors.h"
#include "OrcaParser.h"
#include "OrcaType.h"

using namespace orcagrammar;
using namespace antlr4;

class OrcaAstVisitor;

class OrcaAstNode {
public:
  virtual ~OrcaAstNode() = default;
  virtual std::any accept(OrcaAstVisitor &visitor) = 0;
  virtual void print(int indent) = 0;

  ParserRuleContext *parseContext;
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

class OrcaAstTypeNode : public OrcaAstNode {
public:
  OrcaAstTypeNode(OrcaParser::TypeContext *typeContext)
      : typeContext(typeContext) {
    parseContext = typeContext;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  // We store the context here instead of the type because
  // that's the job of the type checker, where ast type node
  // objects are mapped to the result of their evaluation.
  OrcaParser::TypeContext *typeContext;

  void print(int indent) override {
    printf("%*sTypeNode <%d\n", indent, "");
    printf("%*s%s\n", indent + 2, "", typeContext->getText().c_str());
  }
};

/**
 * @brief Represents a let expression.
 * e.g. let x: int;
 *          ^   ^
 *   name --/   \-- type
 */
class OrcaAstLetExpressionNode : public OrcaAstExpressionNode {
public:
  OrcaAstLetExpressionNode(ParserRuleContext *pContext, const std::string &name,
                           OrcaAstTypeNode *type)
      : name(name), type(type) {
    this->parseContext = pContext;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sLetExpressionNode\n", indent, "");
    printf("%*sname: %s\n", indent + 2, "", name.c_str());
    printf("%*stype:\n", indent + 2, "");
    type->print(indent + 4);
  }

private:
  std::string name;
  OrcaAstTypeNode *type;
};

class OrcaAstProgramNode : public OrcaAstNode {
public:
  OrcaAstProgramNode(ParserRuleContext *pContext,
                     std::vector<OrcaAstNode *> nodes)
      : nodes(nodes) {
    this->parseContext = pContext;
  }
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
  OrcaAstBinaryExpressionNode(ParserRuleContext *pContext,
                              OrcaAstExpressionNode *lhs,
                              OrcaAstExpressionNode *rhs, std::string op)
      : lhs(lhs), rhs(rhs), op(op) {
    this->parseContext = pContext;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sBinaryExpressionNode %s%s%s %s[%d:%d - %d:%d]%s %s'%s'%s\n",
           indent, "", KYEL, op.c_str(), KNRM, KMAG,
           parseContext->start->getLine(),
           parseContext->start->getCharPositionInLine(),
           parseContext->stop->getLine(),
           parseContext->stop->getCharPositionInLine(), KNRM, KBLU,
           parseContext->getText().c_str(), KNRM);
    lhs->print(indent + 4);
    rhs->print(indent + 4);
  }

private:
  OrcaAstExpressionNode *lhs;
  OrcaAstExpressionNode *rhs;
  std::string op;
};

class OrcaAstAssignmentExpressionNode : public OrcaAstBinaryExpressionNode {
public:
  OrcaAstAssignmentExpressionNode(ParserRuleContext *pContext,
                                  OrcaAstExpressionNode *lhs,
                                  OrcaAstExpressionNode *rhs, std::string op)
      : OrcaAstBinaryExpressionNode(pContext, lhs, rhs, op) {}

  std::any accept(OrcaAstVisitor &visitor) override;
};

class OrcaAstUnaryExpressionNode : public OrcaAstExpressionNode {
public:
  OrcaAstUnaryExpressionNode(ParserRuleContext *pContext,
                             OrcaAstExpressionNode *expr, std::string op)
      : expr(expr), op(op) {
    this->parseContext = pContext;
  }

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
  OrcaAstConditionalExpressionNode(ParserRuleContext *pContext,
                                   OrcaAstExpressionNode *condition,
                                   OrcaAstExpressionNode *thenExpr,
                                   OrcaAstExpressionNode *elseExpr)
      : condition(condition), thenExpr(thenExpr), elseExpr(elseExpr) {
    this->parseContext = pContext;
  }

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
  OrcaAstExpressionListNode(ParserRuleContext *pContext,
                            std::vector<OrcaAstExpressionNode *> expressions)
      : expressions(expressions) {
    this->parseContext = pContext;
  }

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

class OrcaAstTypeDeclarationNode : public OrcaAstStatementNode {
public:
  OrcaAstTypeDeclarationNode(ParserRuleContext *pContext,
                             const std::string &name, OrcaAstTypeNode *type)
      : name(name), type(type) {
    this->parseContext = pContext;
  }

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
  OrcaAstTemplateTypeDeclarationNode(ParserRuleContext *pContext,
                                     const std::vector<std::string> params,
                                     const std::string &name,
                                     OrcaAstTypeNode *type)
      : params(params), name(name), type(type) {
    this->parseContext = pContext;
  }

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
  OrcaAstCompoundStatementNode(ParserRuleContext *pContext,
                               std::vector<OrcaAstNode *> nodes)
      : nodes(nodes) {
    this->parseContext = pContext;
  }

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
  OrcaAstFunctionDeclarationNode(ParserRuleContext *pContext,
                                 const std::string &name,
                                 OrcaAstTypeNode *returnType,
                                 std::map<std::string, OrcaAstTypeNode *> args,
                                 OrcaAstCompoundStatementNode *body)
      : name(name), returnType(returnType), args(args), body(body) {
    this->parseContext = pContext;
  }

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
  OrcaAstJumpStatementNode(ParserRuleContext *pContext,
                           const std::string &keyword,
                           OrcaAstExpressionNode *expr)
      : keyword(keyword), expr(expr) {
    this->parseContext = pContext;
  }

  OrcaAstJumpStatementNode(ParserRuleContext *pContext,
                           const std::string &keyword)
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

class OrcaAstIndexExpressionNode : public OrcaAstExpressionNode {
public:
  OrcaAstIndexExpressionNode(ParserRuleContext *pContext,
                             OrcaAstExpressionNode *expr,
                             OrcaAstExpressionNode *index)
      : expr(expr), index(index) {
    this->parseContext = pContext;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sIndexExpressionNode\n", indent, "");
    printf("%*sexpr:\n", indent + 2, "");
    expr->print(indent + 4);
    printf("%*sindex:\n", indent + 2, "");
    index->print(indent + 4);
  }

private:
  OrcaAstExpressionNode *expr;
  OrcaAstExpressionNode *index;
};

class OrcaAstMemberAccessExpressionNode : public OrcaAstExpressionNode {
public:
  enum class OrcaAstMemberAccessKind {
    Dot,
    Arrow,
  };

  OrcaAstMemberAccessExpressionNode(ParserRuleContext *pContext,
                                    OrcaAstExpressionNode *expr,
                                    const std::string &member,
                                    OrcaAstMemberAccessKind kind)
      : expr(expr), member(member), kind(kind) {
    this->parseContext = pContext;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sMemberAccessExpressionNode\n", indent, "");
    printf("%*sexpr:\n", indent + 2, "");
    expr->print(indent + 4);
    printf("%*smember: %s\n", indent + 2, "", member.c_str());
    printf("%*skind: %d\n", indent + 2, "", (int)kind);
  }

private:
  OrcaAstExpressionNode *expr;
  std::string member;
  OrcaAstMemberAccessKind kind;
};

class OrcaAstFunctionCallExpressionNode : public OrcaAstExpressionNode {
public:
  OrcaAstFunctionCallExpressionNode(ParserRuleContext *pContext,
                                    OrcaAstExpressionNode *expr,
                                    std::vector<OrcaAstExpressionNode *> args)
      : expr(expr), args(args) {
    this->parseContext = pContext;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sFunctionCallExpressionNode\n", indent, "");
    printf("%*sexpr:\n", indent + 2, "");
    expr->print(indent + 4);
    printf("%*sargs:\n", indent + 2, "");
    for (auto &arg : args) {
      arg->print(indent + 4);
    }
  }

private:
  OrcaAstExpressionNode *expr;
  std::vector<OrcaAstExpressionNode *> args;
};

class OrcaAstPostfixExpressionNode : public OrcaAstExpressionNode {
public:
  OrcaAstPostfixExpressionNode(ParserRuleContext *pContext,
                               OrcaAstExpressionNode *expr,
                               const std::string &op)
      : expr(expr), op(op) {
    this->parseContext = pContext;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sPostfixExpressionNode\n", indent, "");
    printf("%*sexpr:\n", indent + 2, "");
    expr->print(indent + 4);
    printf("%*sop: %s\n", indent + 2, "", op.c_str());
  }

private:
  OrcaAstExpressionNode *expr;
  std::string op;
};

class OrcaAstIntegerLiteralExpressionNode : public OrcaAstExpressionNode {
public:
  OrcaAstIntegerLiteralExpressionNode(ParserRuleContext *pContext, int value)
      : value(value) {
    this->parseContext = pContext;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sIntegerLiteralExpressionNode %s%d%s\n", indent, "", KGRN, value,
           KNRM);
  }

private:
  int value;
};

class OrcaAstFloatLiteralExpressionNode : public OrcaAstExpressionNode {
public:
  OrcaAstFloatLiteralExpressionNode(ParserRuleContext *pContext, float value)
      : value(value) {
    this->parseContext = pContext;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sFloatLiteralExpressionNode\n", indent, "");
    printf("%*svalue: %f\n", indent + 2, "", value);
  }

private:
  float value;
};

class OrcaAstStringLiteralExpressionNode : public OrcaAstExpressionNode {
public:
  OrcaAstStringLiteralExpressionNode(ParserRuleContext *pContext,
                                     const std::string &value)
      : value(value) {
    this->parseContext = pContext;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sStringLiteralExpressionNode\n", indent, "");
    printf("%*svalue: %s\n", indent + 2, "", value.c_str());
  }

private:
  std::string value;
};

class OrcaAstBooleanLiteralExpressionNode : public OrcaAstExpressionNode {
public:
  OrcaAstBooleanLiteralExpressionNode(ParserRuleContext *pContext, bool value)
      : value(value) {
    this->parseContext = pContext;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sBooleanLiteralExpressionNode\n", indent, "");
    printf("%*svalue: %s\n", indent + 2, "", value ? "true" : "false");
  }

private:
  bool value;
};

class OrcaAstIdentifierExpressionNode : public OrcaAstExpressionNode {
public:
  OrcaAstIdentifierExpressionNode(ParserRuleContext *pContext,
                                  const std::string &name)
      : name(name) {
    this->parseContext = pContext;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sIdentifierExpressionNode\n", indent, "");
    printf("%*sname: %s\n", indent + 2, "", name.c_str());
  }

private:
  std::string name;
};
