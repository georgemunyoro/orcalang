#pragma once

#include <any>
#include <set>
#include <string>
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

      if (lhs->is(OrcaTypeKind::Array)) {
        if (!lhs->isEqual(rhs)) {
          throw OrcaError(
              compileContext,
              "Cannot assign value of type '" + rhs->toString() +
                  "' to type '" + lhs->toString() + "'.",
              node->parseContext->getStart()->getLine(),
              node->parseContext->getStart()->getCharPositionInLine());
        }
      }

      if (lhs->getKind() == OrcaTypeKind::Pointer) {
        switch (rhs->getKind()) {
        case OrcaTypeKind::Pointer: {
          if (!lhs->getPointerType().getPointee()->isEqual(
                  rhs->getPointerType().getPointee())) {
            throw OrcaError(
                compileContext,
                "Cannot assign value of type '" + rhs->toString() +
                    "' to type '" + lhs->toString() + "'.",
                node->parseContext->getStart()->getLine(),
                node->parseContext->getStart()->getCharPositionInLine());
          }
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
    auto conditionType =
        std::any_cast<OrcaType *>(node->getCondition()->accept(*this));

    if (!conditionType->is(OrcaTypeKind::Boolean)) {
      throw OrcaError(
          compileContext,
          "Condition of ternary expression must be of type 'bool'. Got '" +
              conditionType->toString() + "'.",
          node->parseContext->getStart()->getLine(),
          node->parseContext->getStart()->getCharPositionInLine());
    }

    auto thenTy = std::any_cast<OrcaType *>(node->getThenExpr()->accept(*this));
    auto elseTy = std::any_cast<OrcaType *>(node->getElseExpr()->accept(*this));

    if (!thenTy->isEqual(elseTy)) {
      throw OrcaError(
          compileContext,
          "Ternary expression branches should be of same type. Got '" +
              thenTy->toString() + "' and '" + elseTy->toString() + "'.",
          node->parseContext->getStart()->getLine(),
          node->parseContext->getStart()->getCharPositionInLine());
    }

    node->evaluatedType = thenTy;
    return std::any(node->evaluatedType);
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
    std::vector<OrcaType *> elementTypes;
    for (auto &expr : node->getElements()) {
      auto t = std::any_cast<OrcaType *>(expr->accept(*this));
      elementTypes.push_back(t);
      if (elementTypes.size() > 0) {
        if (!elementTypes.front()->isEqual(t)) {
          throw OrcaError(
              compileContext,
              "All elements of an array must be of the same type.",
              node->parseContext->getStart()->getLine(),
              node->parseContext->getStart()->getCharPositionInLine());
        }
      }
    }

    node->evaluatedType =
        new OrcaType(OrcaArrayType(elementTypes.front(), elementTypes.size()));
    return node->evaluatedType;
  };

  std::any visitTypeDeclaration(OrcaAstTypeDeclarationNode *node) override {
    node->getType()->accept(*this);
    node->evaluatedType = evaluateTypeContext(node->getType()->typeContext);
    typeScope->set(node->getName(), node->evaluatedType);
    return std::any(node->evaluatedType);
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

  std::any
  visitIterationStatement(OrcaAstIterationStatementNode *node) override {
    node->getCondition()->accept(*this);
    node->getBody()->accept(*this);
    node->evaluatedType = T_void;
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

  std::any visitFunctionCallExpression(
      OrcaAstFunctionCallExpressionNode *node) override {
    auto callee = std::any_cast<OrcaType *>(node->getCallee()->accept(*this));

    if (!callee->getPointerType().getPointee()->is(OrcaTypeKind::Function)) {
      throw OrcaError(compileContext, "Trying to call a non-function type.",
                      node->parseContext->getStart()->getLine(),
                      node->parseContext->getStart()->getCharPositionInLine());
    }

    if (callee->getPointerType()
            .getPointee()
            ->getFunctionType()
            .getParameterTypes()
            .size() != node->getArgs().size()) {
      throw OrcaError(
          compileContext,
          "Function call has incorrect number of arguments. Expected " +
              std::to_string(callee->getPointerType()
                                 .getPointee()
                                 ->getFunctionType()
                                 .getParameterTypes()
                                 .size()) +
              " but got " + std::to_string(node->getArgs().size()) + ".",
          node->parseContext->getStart()->getLine(),
          node->parseContext->getStart()->getCharPositionInLine());
    }

    for (size_t i = 0; i < node->getArgs().size(); ++i) {
      auto argType =
          std::any_cast<OrcaType *>(node->getArgs()[i]->accept(*this));
      auto paramType = callee->getPointerType()
                           .getPointee()
                           ->getFunctionType()
                           .getParameterTypes()[i];

      if (!argType->isEqual(paramType)) {
        throw OrcaError(
            compileContext,
            "Function call argument " + std::to_string(i) +
                " has incorrect type. Expected '" + paramType->toString() +
                "' but got '" + argType->toString() + "'.",
            node->parseContext->getStart()->getLine(),
            node->parseContext->getStart()->getCharPositionInLine());
      }
    }

    node->evaluatedType = callee->getPointerType()
                              .getPointee()
                              ->getFunctionType()
                              .getReturnType();
    return std::any(node->evaluatedType);
  }

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

    // Remove the quotes from the string
    auto strValue = node->getValue().substr(1, node->getValue().size() - 2);

    auto strType = new OrcaType(
        OrcaArrayType(new OrcaType(OrcaIntegerType(false, 8)),
                      (size_t)(strValue.size() + 1))); // +1 for null terminator
    node->evaluatedType = strType;
    return std::any(node->evaluatedType);
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

  std::any
  visitSelectionStatement(OrcaAstSelectionStatementNode *node) override {
    node->getCondition()->accept(*this);
    auto thenTy =
        std::any_cast<OrcaType *>(node->getThenStatement()->accept(*this));
    if (node->getElseStatement()) {
      auto elseTy =
          std::any_cast<OrcaType *>(node->getElseStatement()->accept(*this));

      if (!thenTy->isEqual(elseTy)) {
        throw OrcaError(
            compileContext,
            "Branches of if statement must have the same type. Got '" +
                thenTy->toString() + "' and '" + elseTy->toString() + "'.",
            node->parseContext->getStart()->getLine(),
            node->parseContext->getStart()->getCharPositionInLine());
      }
    }

    node->evaluatedType = T_void;
    return node->evaluatedType;
  };

  std::any visitIndexExpression(OrcaAstIndexExpressionNode *node) override {
    auto arrayType = std::any_cast<OrcaType *>(node->getExpr()->accept(*this));
    auto indexType = std::any_cast<OrcaType *>(node->getIndex()->accept(*this));

    if (!arrayType->is(OrcaTypeKind::Array)) {
      throw OrcaError(compileContext,
                      "Indexing expression must be applied to an array. Got '" +
                          arrayType->toString() + "'.",
                      node->parseContext->getStart()->getLine(),
                      node->parseContext->getStart()->getCharPositionInLine());
    }

    if (!indexType->is(OrcaTypeKind::Integer)) {
      throw OrcaError(
          compileContext,
          "Indexing expression must be applied with an integer. Got '" +
              indexType->toString() + "'.",
          node->parseContext->getStart()->getLine(),
          node->parseContext->getStart()->getCharPositionInLine());
    }

    node->evaluatedType = arrayType->getArrayType().getElementType();
    return std::any(node->evaluatedType);
  }

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

  std::any visitMemberAccessExpression(
      OrcaAstMemberAccessExpressionNode *node) override {
    if (node->getKind() ==
        OrcaAstMemberAccessExpressionNode::OrcaAstMemberAccessKind::Dot) {
      const auto &expr =
          std::any_cast<OrcaType *>(node->getExpr()->accept(*this));

      if (expr->isValidDotAccess(node->getMember())) {
        for (const auto &member : expr->getStructType().getFields()) {
          if (member.first == node->getMember()) {
            node->evaluatedType = member.second;
            return std::any(node->evaluatedType);
          }
        }
      }

      throw OrcaError(compileContext,
                      "Cannot access member '" + node->getMember() +
                          "' of type '" + expr->toString() + "'.",
                      node->parseContext->getStart()->getLine(),
                      node->parseContext->getStart()->getCharPositionInLine());
    }

    throw "TODO";
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

    if (tContext->LBRACK()) {
      auto elementType = evaluateTypeContext(tContext->elementType);
      size_t arraySize = std::stoi(tContext->arraySize->getText());
      return new OrcaType(OrcaArrayType(elementType, arraySize));
    }

    if (tContext->LBRACE()) {
      const auto &fieldDecls =
          tContext->structFieldDeclarationList()->structFieldDeclaration();

      std::set<std::string> fieldNames;
      for (auto &fieldDecl : fieldDecls) {
        auto fieldName = fieldDecl->Identifier()->getText();
        if (fieldNames.find(fieldName) != fieldNames.end()) {
          throw OrcaError(compileContext,
                          "Duplicate field name '" + fieldName + "'.",
                          fieldDecl->getStart()->getLine(),
                          fieldDecl->getStart()->getCharPositionInLine());
        }
        fieldNames.insert(fieldName);
      }

      std::vector<std::pair<std::string, OrcaType *>> fields;
      fields.resize(fieldDecls.size());

      for (size_t i = 0; i < fieldDecls.size(); ++i) {
        auto fieldDecl = fieldDecls[i];
        fields[i] = std::make_pair(fieldDecl->Identifier()->getText(),
                                   evaluateTypeContext(fieldDecl->type()));
      }

      return new OrcaType(OrcaStructType(fields));
    }

    if (typeScope->isInScope(tContext->getText())) {
      return *typeScope->get(tContext->getText());
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
