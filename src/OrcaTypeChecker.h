#pragma once

#include <any>
#include <utility>

#include "OrcaAst.h"
#include "OrcaAstVisitor.h"
#include "OrcaError.h"
#include "OrcaParser.h"
#include "OrcaScope.h"
#include "OrcaType.h"

class OrcaTypeChecker : OrcaAstVisitor {
  ~OrcaTypeChecker() = default;

  std::any visitLetExpression(OrcaAstLetExpressionNode *node) override {
    node->evaluatedType = std::any_cast<OrcaType *>(node->type->accept(*this));
    typeScope->set(node->name, node->evaluatedType);
    node->type->evaluatedType = node->evaluatedType;
    return std::any(node->evaluatedType);
  };

  std::any visitProgram(OrcaAstProgramNode *node) override {
    for (auto &node : node->getNodes())
      node->accept(*this);

    node->evaluatedType = new OrcaType(OrcaVoidType());
    return std::any((OrcaType *)node->evaluatedType);
  };

  std::any
  visitAssignmentExpression(OrcaAstAssignmentExpressionNode *node) override {
    OrcaType *lhs = std::any_cast<OrcaType *>(node->lhs->accept(*this));
    OrcaType *rhs = std::any_cast<OrcaType *>(node->rhs->accept(*this));

    if (node->opSymbol == "=") {

      // Cannot assign to type 'void'
      if (lhs->getKind() == OrcaTypeKind::Void) {
        throw OrcaError(
            compileContext,
            "Cannot assign to lvalue of type '" + lhs->toString() + "'.",
            node->parseContext->getStart()->getLine(),
            node->parseContext->getStart()->getCharPositionInLine());
      }

      // If types are perfectly equal, then we can assign
      // without any further checks.
      if (lhs->isEqual(rhs)) {
        node->evaluatedType = lhs;
        return std::any(node->evaluatedType);
      }

      if (lhs->getKind() == OrcaTypeKind::Integer) {

        switch (rhs->getKind()) {

        case OrcaTypeKind::Integer:
        case OrcaTypeKind::Boolean:
        case OrcaTypeKind::Float:
        case OrcaTypeKind::Pointer:
        case OrcaTypeKind::Char: {

          // If attempting to assign compatible types of a different size,
          // require an explicit cast.
          if (lhs->sizeOf() != rhs->sizeOf()) {
            throw OrcaError(
                compileContext,
                "Cannot assign value of type '" + rhs->toString() +
                    "' to type '" + lhs->toString() +
                    "'. If this is "
                    "intentional, you must explicitly cast the value.",
                node->parseContext->getStart()->getLine(),
                node->parseContext->getStart()->getCharPositionInLine());
          }

          // Everything is fine, the type of rhs is assignable to rhs
          node->evaluatedType = lhs;
          return std::any(node->evaluatedType);
        }

        default: {
          throw OrcaError(
              compileContext,
              "Cannot assign value of type '" + rhs->toString() +
                  "' to type '" + lhs->toString() + "'.",
              node->parseContext->getStart()->getLine(),
              node->parseContext->getStart()->getCharPositionInLine());
        }
        }
      }
    }

    // Unreachable
    throw OrcaError(compileContext,
                    "Unhandled assignment operator '" + node->opSymbol +
                        "'. This is a bug.",
                    node->parseContext->getStart()->getLine(),
                    node->parseContext->getStart()->getCharPositionInLine());
  };

  std::any
  visitConditionalExpression(OrcaAstConditionalExpressionNode *node) override {
    throw "TODO";
  };

  std::any visitUnaryExpression(OrcaAstUnaryExpressionNode *node) override {
    OrcaType *operandType =
        std::any_cast<OrcaType *>(node->getExpr()->accept(*this));

    try {
      node->evaluatedType = node->getOperator()->getResultingType(operandType);
      return std::any(node->evaluatedType);
    } catch (std::string &e) {
      throw OrcaError(compileContext, e,
                      node->parseContext->getStart()->getLine(),
                      node->parseContext->getStart()->getCharPositionInLine());
    }
  };

  std::any visitExpressionList(OrcaAstExpressionListNode *node) override {
    throw "TODO";
  };

  std::any visitTypeDeclaration(OrcaAstTypeDeclarationNode *node) override {
    throw "TODO";
  };

  std::any visitTemplateTypeDeclaration(
      OrcaAstTemplateTypeDeclarationNode *node) override {
    throw "TODO";
  };

  std::any visitType(OrcaAstTypeNode *node) override {
    node->evaluatedType = evaluateTypeContext(node->typeContext);
    return std::any(node->evaluatedType);
  };

  std::any visitCompoundStatement(OrcaAstCompoundStatementNode *node) override {

    assert(typeScope != nullptr);

    // Enter a new scope
    typeScope = new OrcaScope<OrcaType *>(typeScope);

    for (auto statement : node->nodes) {
      auto t = statement->accept(*this);
      assert(t.has_value());
      statement->evaluatedType = std::any_cast<OrcaType *>(t);
    }

    // Leave the scope
    typeScope = typeScope->getParent();

    node->evaluatedType = new OrcaType(OrcaVoidType());
    return std::any(node->evaluatedType);
  };

  std::any visitFunctionDeclarationStatement(
      OrcaAstFunctionDeclarationNode *node) override {

    std::vector<std::pair<std::string, OrcaType *>> parameters;
    for (auto &parameter : node->args) {
      auto parameterName = parameter.first;
      auto parameterType =
          std::any_cast<OrcaType *>(parameter.second->accept(*this));

      parameter.second->evaluatedType = parameterType;
      parameters.push_back(std::make_pair(parameterName, parameterType));
    }

    OrcaType *returnType =
        std::any_cast<OrcaType *>(node->returnType->accept(*this));
    OrcaType *functionType =
        new OrcaType(OrcaFunctionType(returnType, parameters));

    node->evaluatedType = functionType;
    node->returnType->evaluatedType = returnType;

    // Add the function to the current scope
    typeScope->set(node->name, new OrcaType(OrcaPointerType(functionType)));

    currentFunctionInfo = {
        .declaration = node,
        .type = functionType,
        .isInFunction = true,
    };

    assert(typeScope != nullptr);

    // Enter a new scope
    typeScope = new OrcaScope<OrcaType *>(typeScope);

    // Add parameters to the scope
    for (auto &parameter : parameters) {
      typeScope->set(parameter.first, parameter.second);
    }

    // Evaluate body
    node->body->accept(*this);

    // Leave the scope
    typeScope = typeScope->getParent();

    currentFunctionInfo = {
        .declaration = nullptr,
        .type = nullptr,
        .isInFunction = false,
    };

    return std::any(functionType);
  };

  std::any visitJumpStatement(OrcaAstJumpStatementNode *node) override {

    if (node->keyword == "return") {
      if (!currentFunctionInfo.isInFunction) {
        throw OrcaError(
            compileContext, "Cannot return from outside of a function.",
            node->parseContext->getStart()->getLine(),
            node->parseContext->getStart()->getCharPositionInLine());
      }

      auto retType = std::any_cast<OrcaType *>(node->expr->accept(*this));

      if (!retType->isEqual(
              currentFunctionInfo.type->getFunctionType().getReturnType())) {
        throw OrcaError(
            compileContext,
            "Cannot return value of type '" + retType->toString() +
                "' from function of type '" +
                currentFunctionInfo.type->toString() + "'.",
            node->parseContext->getStart()->getLine(),
            node->parseContext->getStart()->getCharPositionInLine());
      }
    }

    node->evaluatedType = T_void;
    return std::any(node->evaluatedType);
  };

  std::any visitBinaryExpression(OrcaAstBinaryExpressionNode *node) override {
    OrcaType *lhs = std::any_cast<OrcaType *>(node->lhs->accept(*this));
    OrcaType *rhs = std::any_cast<OrcaType *>(node->rhs->accept(*this));

    try {
      node->evaluatedType = node->getOperator()->getResultingType(lhs, rhs);
      return std::any(node->evaluatedType);
    } catch (std::string &e) {
      throw OrcaError(compileContext, e,
                      node->parseContext->getStart()->getLine(),
                      node->parseContext->getStart()->getCharPositionInLine());
    }
  };

  std::any
  visitIdentifierExpression(OrcaAstIdentifierExpressionNode *node) override {
    auto type = typeScope->get(node->getName());

    if (type == nullptr) {
      throw OrcaError(compileContext,
                      "Referenced unknown symbol '" + node->getName() + "'",
                      node->parseContext->getStart()->getLine(),
                      node->parseContext->getStart()->getCharPositionInLine());
    }

    assert(*type != nullptr);

    node->evaluatedType = *type;
    return std::any(node->evaluatedType);
  };

  std::any visitIntegerLiteralExpression(
      OrcaAstIntegerLiteralExpressionNode *node) override {
    node->evaluatedType = T_u32;
    return std::any(node->evaluatedType);
  };

  std::any visitFloatLiteralExpression(
      OrcaAstFloatLiteralExpressionNode *node) override {
    throw "TODO";
  };

  std::any visitStringLiteralExpression(
      OrcaAstStringLiteralExpressionNode *node) override {
    throw "TODO";
  };

  std::any visitBooleanLiteralExpression(
      OrcaAstBooleanLiteralExpressionNode *node) override {
    node->evaluatedType = new OrcaType(OrcaBooleanType());
    return std::any(node->evaluatedType);
  };

  std::any
  visitExpressionStatement(OrcaAstExpressionStatementNode *node) override {
    node->expr->accept(*this);

    node->evaluatedType = T_void;
    return node->evaluatedType;
  };

  std::any visitCastExpression(OrcaAstCastExpressionNode *node) override {

    auto type = std::any_cast<OrcaType *>(node->getType()->accept(*this));
    auto exprType = std::any_cast<OrcaType *>(node->getExpr()->accept(*this));

    if (exprType->is(OrcaTypeKind::Integer)) {
      switch (type->getKind()) {
      case OrcaTypeKind::Integer:
      case OrcaTypeKind::Boolean:
      case OrcaTypeKind::Char:
      case OrcaTypeKind::Float:
        node->evaluatedType = type;
        return std::any(node->evaluatedType);
      default:
        break;
      }
    }

    if (exprType->is(OrcaTypeKind::Boolean)) {
      switch (type->getKind()) {
      case OrcaTypeKind::Integer:
      case OrcaTypeKind::Boolean:
      case OrcaTypeKind::Char:
      case OrcaTypeKind::Float:
        node->evaluatedType = type;
        return std::any(node->evaluatedType);
      default:
        break;
      }
    }

    throw OrcaError(compileContext,
                    "Cannot cast from type '" + exprType->toString() +
                        "' to type '" + type->toString() + "'.",
                    node->parseContext->getStart()->getLine(),
                    node->parseContext->getStart()->getCharPositionInLine());
  }

  // Type map for mapping ast nodes to types
  std::map<OrcaAstNode *, OrcaType *> typeMap;

  OrcaType *evaluateTypeContext(OrcaParser::TypeContext *tContext) {
    if (tContext->typeSpecifier() && tContext->children.size() == 1) {
      auto ts = tContext->typeSpecifier();

      if (ts->T_BOOL())
        return new OrcaType(OrcaBooleanType());
      if (ts->T_CHAR())
        return new OrcaType(OrcaCharType());
      if (ts->T_VOID())
        return new OrcaType(OrcaVoidType());

      // Unsigned integers
      if (ts->T_U8())
        return new OrcaType(OrcaIntegerType(false, 8));
      if (ts->T_U16())
        return new OrcaType(OrcaIntegerType(false, 16));
      if (ts->T_U32())
        return T_u32;
      if (ts->T_U64())
        return new OrcaType(OrcaIntegerType(false, 64));

      // Signed integers
      if (ts->T_S8())
        return new OrcaType(OrcaIntegerType(true, 8));
      if (ts->T_S16())
        return new OrcaType(OrcaIntegerType(true, 16));
      if (ts->T_S32())
        return new OrcaType(OrcaIntegerType(true, 32));
      if (ts->T_S64())
        return new OrcaType(OrcaIntegerType(true, 64));

      if (ts->T_F32())
        return new OrcaType(OrcaFloatType(32));
      if (ts->T_F64())
        return new OrcaType(OrcaFloatType(64));
    }

    if (tContext->STAR()) {
      auto pointeeType = evaluateTypeContext(tContext->pointeeType);
      return new OrcaType(OrcaPointerType(pointeeType));
    }

    throw OrcaError(compileContext, "Unhandled type. This is a bug.",
                    tContext->getStart()->getLine(),
                    tContext->getStart()->getCharPositionInLine());
  }

  struct {
    OrcaAstFunctionDeclarationNode *declaration;
    OrcaType *type;
    bool isInFunction = false;
  } currentFunctionInfo;

  OrcaType *currentFunctionType = nullptr;

  OrcaScope<OrcaType *> *typeScope;

  OrcaContext &compileContext;

  OrcaType *T_void = new OrcaType(OrcaVoidType());
  OrcaType *T_u32 = new OrcaType(OrcaIntegerType(false, 32));

public:
  OrcaTypeChecker(OrcaContext &context)
      : compileContext(context), typeScope(new OrcaScope<OrcaType *>()),
        currentFunctionType(nullptr), currentFunctionInfo({
                                          .declaration = nullptr,
                                          .type = nullptr,
                                          .isInFunction = false,
                                      }) {}

  void run(OrcaAstNode *node) {
    try {
      node->accept(*this);
    } catch (OrcaError &e) {
      e.print();
      exit(1);
    }
  }

  void printTypeMap() {
    for (auto &pair : typeMap) {
      printf("%p | %s\n", pair.first, pair.second->toString().c_str());
    }
  }
};
