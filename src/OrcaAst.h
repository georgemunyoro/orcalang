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

  /**
   * @brief The ANTLR context of the node in the source code. Used for error
   * reporting, and anywhere where the reference to the source code is needed.
   */
  ParserRuleContext *parseContext;

  /**
   * @brief The type of the node. This is set later by the type checker.
   */
  OrcaType *evaluatedType;

  /**
   * @brief Returns a string representation of the context of this node (with
   * colors). e.g. [1:0 - 1:5] 'hello'
   *
   * @return std::string
   */
  std::string contextString() {
    auto withoutType =
        std::string(KMAG) + "[" +
        std::to_string(parseContext->start->getLine()) + ":" +
        std::to_string(parseContext->start->getCharPositionInLine()) + " - " +
        std::to_string(parseContext->stop->getLine()) + ":" +
        std::to_string(parseContext->stop->getCharPositionInLine()) + "]" +
        KNRM + " " + KBLU + "'" + parseContext->getText() + "'" + KNRM;

    if (evaluatedType) {
      return withoutType + " " + KGRN + evaluatedType->toString() + KNRM;
    } else {
      return withoutType + " " + KRED + "???" + KNRM;
    }
  }
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
    evaluatedType = nullptr;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  // We store the context here instead of the type because
  // that's the job of the type checker, where ast type node
  // objects are mapped to the result of their evaluation.
  OrcaParser::TypeContext *typeContext;

  void print(int indent) override {
    printf("%*sTypeNode %s\n", indent, "", contextString().c_str());
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
    evaluatedType = nullptr;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sLetExpressionNode %s%s%s %s\n", indent, "", KYEL, name.c_str(),
           KNRM, contextString().c_str());
    type->print(indent + 2);
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
    evaluatedType = nullptr;
  }
  ~OrcaAstProgramNode() = default;

  std::any accept(OrcaAstVisitor &visitor) override;

  void addNode(OrcaAstNode *node) { nodes.push_back(node); }

  void print(int indent) override {
    printf("%*sProgramNode %s\n", indent, "", contextString().c_str());
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
    evaluatedType = nullptr;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sBinaryExpressionNode %s\n", indent, "", contextString().c_str());
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
    evaluatedType = nullptr;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sUnaryExpressionNode %s\n", indent, "", contextString().c_str());
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
    evaluatedType = nullptr;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sConditionalExpressionNode %s\n", indent, "",
           contextString().c_str());
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
    evaluatedType = nullptr;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void addExpression(OrcaAstExpressionNode *expression) {
    expressions.push_back(expression);
  }

  void print(int indent) override {
    printf("%*sExpressionListNode %s\n", indent, "", contextString().c_str());
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
    evaluatedType = nullptr;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sTypeDeclarationNode %s%s%s %s\n", indent, "", KYEL, name.c_str(),
           KNRM, contextString().c_str());
    type->print(indent + 2);
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
    evaluatedType = nullptr;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sTemplateTypeDeclarationNode %s\n", indent, "",
           contextString().c_str());
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
    evaluatedType = nullptr;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void addNode(OrcaAstNode *node) { nodes.push_back(node); }

  void print(int indent) override {
    printf("%*sCompoundStatementNode %s\n", indent, "",
           contextString().c_str());
    for (auto &node : nodes) {
      node->print(indent + 2);
    }
  }

private:
  std::vector<OrcaAstNode *> nodes;
};

class OrcaAstExpressionStatementNode : public OrcaAstStatementNode {
public:
  OrcaAstExpressionStatementNode(ParserRuleContext *pContext,
                                 OrcaAstExpressionNode *expr)
      : expr(expr) {
    this->parseContext = pContext;
    evaluatedType = nullptr;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  OrcaAstExpressionNode *expr;

  void print(int indent) override {
    printf("%*sExpressionStatementNode %s\n", indent, "",
           contextString().c_str());
    expr->print(indent + 4);
  }
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
    evaluatedType = nullptr;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sFunctionDeclarationNode %s%s%s %s\n", indent, "", KYEL,
           name.c_str(), KNRM, contextString().c_str());
    for (auto &arg : args) {
      printf("%*s%s%s%s", indent + 2, "", KYEL, arg.first.c_str(), KNRM);
      arg.second->print(1);
    }
    printf("%*s %s->%s ", indent + 2, "", KMAG, KNRM);
    returnType->print(1);

    body->print(indent + 2);
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
    evaluatedType = nullptr;
  }

  OrcaAstJumpStatementNode(ParserRuleContext *pContext,
                           const std::string &keyword)
      : keyword(keyword), expr(nullptr) {}

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sJumpStatementNode %s%s%s %s\n", indent, "", KYEL,
           keyword.c_str(), KNRM, contextString().c_str());
    expr->print(indent + 2);
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
    evaluatedType = nullptr;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sIndexExpressionNode %s\n", indent, "", contextString().c_str());
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
    evaluatedType = nullptr;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sMemberAccessExpressionNode %s\n", indent, "",
           contextString().c_str());
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
    evaluatedType = nullptr;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sFunctionCallExpressionNode %s\n", indent, "",
           contextString().c_str());
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
    evaluatedType = nullptr;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sPostfixExpressionNode %s\n", indent, "",
           contextString().c_str());
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
    evaluatedType = nullptr;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sIntegerLiteralExpressionNode %s%d%s %s\n", indent, "", KGRN,
           value, KNRM, contextString().c_str());
  }

private:
  int value;
};

class OrcaAstFloatLiteralExpressionNode : public OrcaAstExpressionNode {
public:
  OrcaAstFloatLiteralExpressionNode(ParserRuleContext *pContext, float value)
      : value(value) {
    this->parseContext = pContext;
    evaluatedType = nullptr;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sFloatLiteralExpressionNode %s\n", indent, "",
           contextString().c_str());
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
    evaluatedType = nullptr;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sStringLiteralExpressionNode %s\n", indent, "",
           contextString().c_str());
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
    evaluatedType = nullptr;
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
    evaluatedType = nullptr;
  }

  std::any accept(OrcaAstVisitor &visitor) override;

  void print(int indent) override {
    printf("%*sIdentifierExpressionNode %s%s%s %s\n", indent, "", KYEL,
           name.c_str(), KNRM, contextString().c_str());
  }

private:
  std::string name;
};
