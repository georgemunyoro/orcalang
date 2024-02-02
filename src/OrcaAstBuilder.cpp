#include <any>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>

#include "OrcaAst.h"
#include "OrcaAstBuilder.h"
#include "OrcaContext.h"
#include "OrcaError.h"
#include "OrcaParser.h"

OrcaAstProgramNode *OrcaAstBuilder::build() {
  auto programNode = visitProgram(compileContext.programContext);
  return std::any_cast<OrcaAstProgramNode *>(programNode);
}

std::any OrcaAstBuilder::visitProgram(OrcaParser::ProgramContext *context) {

  OrcaAstProgramNode *program = new OrcaAstProgramNode(context, {});

  for (auto &statement : context->statement()) {
    auto node = visit(statement);
    if (node.has_value()) {
      program->addNode(std::any_cast<OrcaAstNode *>(node));
    }
  }

  if (program->getNodes().size() == 0) {
    throw OrcaError(compileContext, "Program has no statements.",
                    context->getStart()->getLine(),
                    context->getStart()->getCharPositionInLine());
  }

  return std::any(program);
}

std::any
OrcaAstBuilder::visitExpression(OrcaParser::ExpressionContext *context) {
  assert(context->children.size() == 1);
  return visit(context->children.at(0));
}

std::any OrcaAstBuilder::visitExpressionList(
    OrcaParser::ExpressionListContext *context) {
  OrcaAstExpressionListNode *expressionList =
      new OrcaAstExpressionListNode(context, {});

  for (auto &child : context->expression()) {
    auto node = visit(child);
    if (node.has_value()) {
      expressionList->addExpression(
          std::any_cast<OrcaAstExpressionNode *>(node));
    } else
      printf("ERROR: visitExpressionList: node has no value\n");
  }

  return std::any(expressionList);
}

std::any
OrcaAstBuilder::visitLetExpression(OrcaParser::LetExpressionContext *context) {
  std::string varName = context->varName->getText();
  auto type = std::any_cast<OrcaAstTypeNode *>(visit(context->varType));
  return (OrcaAstExpressionNode *)new OrcaAstLetExpressionNode(context, varName,
                                                               type);
}

std::any OrcaAstBuilder::visitAssignmentExpression(
    OrcaParser::AssignmentExpressionContext *context) {

  if (context->children.size() == 1) {
    return visit(context->children.at(0));
  }

  assert(context->children.size() == 3);

  auto lhs = visit(context->children.at(0));
  auto rhs = visit(context->children.at(2));
  auto op = context->children.at(1)->getText();

  OrcaAstAssignmentExpressionNode *assignmentNode =
      new OrcaAstAssignmentExpressionNode(
          context, std::any_cast<OrcaAstExpressionNode *>(lhs),
          std::any_cast<OrcaAstExpressionNode *>(rhs), op);

  return std::any((OrcaAstExpressionNode *)assignmentNode);
}

std::any OrcaAstBuilder::visitUnaryExpression(
    OrcaParser::UnaryExpressionContext *context) {
  if (context->children.size() == 1) {
    return visit(context->children.at(0));
  }

  assert(context->children.size() == 2);

  auto op = context->op->getText();
  auto expr = visit(context->expr);

  if (!expr.has_value()) {
    throw OrcaError(compileContext, "Expected expression after unary operator.",
                    context->getStart()->getLine(),
                    context->getStart()->getCharPositionInLine());
  }

  OrcaAstUnaryExpressionNode *unaryNode = new OrcaAstUnaryExpressionNode(
      context, std::any_cast<OrcaAstExpressionNode *>(expr), op);

  return std::any((OrcaAstExpressionNode *)unaryNode);
}

std::any OrcaAstBuilder::visitConditionalExpression(
    OrcaParser::ConditionalExpressionContext *context) {

  // Ternary conditional expression
  if (context->condition) {
    auto condition = visit(context->condition);
    auto thenExpr = visit(context->thenExpr);
    auto elseExpr = visit(context->elseExpr);

    assert(condition.has_value());
    assert(thenExpr.has_value());
    assert(elseExpr.has_value());

    OrcaAstConditionalExpressionNode *conditionalNode =
        new OrcaAstConditionalExpressionNode(
            context, std::any_cast<OrcaAstExpressionNode *>(condition),
            std::any_cast<OrcaAstExpressionNode *>(thenExpr),
            std::any_cast<OrcaAstExpressionNode *>(elseExpr));

    return std::any((OrcaAstExpressionNode *)conditionalNode);
  }

  assert(context->children.size() == 1);

  // Fall through to logical or expression
  return visit(context->logicalOrExpression());
}

std::any OrcaAstBuilder::visitExpressionStatement(
    OrcaParser::ExpressionStatementContext *context) {
  auto expr =
      std::any_cast<OrcaAstExpressionNode *>(visit(context->expression()));
  return std::any(
      (OrcaAstNode *)new OrcaAstExpressionStatementNode(context, expr));
}

std::any OrcaAstBuilder::visitLogicalOrExpression(
    OrcaParser::LogicalOrExpressionContext *context) {
  // Logical or expression
  if (context->rhs) {
    assert(context->children.size() == 3);

    auto lhs = std::any_cast<OrcaAstExpressionNode *>(visit(context->lhs));
    auto rhs = std::any_cast<OrcaAstExpressionNode *>(visit(context->rhs));

    std::string opSymbol = context->children.at(1)->getText();

    return std::any((OrcaAstExpressionNode *)new OrcaAstBinaryExpressionNode(
        context, lhs, rhs, opSymbol));
  }

  assert(context->children.size() == 1);

  // Fall through to logical and expression
  return visit(context->children.at(0));
}

std::any OrcaAstBuilder::visitLogicalAndExpression(
    OrcaParser::LogicalAndExpressionContext *context) {
  // Logical and expression
  if (context->rhs) {
    assert(context->children.size() == 3);

    auto lhs = std::any_cast<OrcaAstExpressionNode *>(visit(context->lhs));
    auto rhs = std::any_cast<OrcaAstExpressionNode *>(visit(context->rhs));

    std::string op = context->children.at(1)->getText();

    return std::any((OrcaAstExpressionNode *)new OrcaAstBinaryExpressionNode(
        context, lhs, rhs, op));
  }

  assert(context->children.size() == 1);

  // Fall through to inclusive or expression
  return visit(context->children.at(0));
}

std::any OrcaAstBuilder::visitInclusiveOrExpression(
    OrcaParser::InclusiveOrExpressionContext *context) {
  // Inclusive or expression
  if (context->rhs) {
    assert(context->children.size() == 3);

    auto lhs = std::any_cast<OrcaAstExpressionNode *>(visit(context->lhs));
    auto rhs = std::any_cast<OrcaAstExpressionNode *>(visit(context->rhs));

    std::string op = context->children.at(1)->getText();

    return std::any((OrcaAstExpressionNode *)new OrcaAstBinaryExpressionNode(
        context, lhs, rhs, op));
  }

  assert(context->children.size() == 1);

  // Fall through to exclusive or expression
  return visit(context->children.at(0));
}

std::any OrcaAstBuilder::visitExclusiveOrExpression(
    OrcaParser::ExclusiveOrExpressionContext *context) {
  // Exclusive or expression
  if (context->rhs) {
    assert(context->children.size() == 3);

    auto lhs = std::any_cast<OrcaAstExpressionNode *>(visit(context->lhs));
    auto rhs = std::any_cast<OrcaAstExpressionNode *>(visit(context->rhs));

    std::string op = context->children.at(1)->getText();

    return std::any((OrcaAstExpressionNode *)new OrcaAstBinaryExpressionNode(
        context, lhs, rhs, op));
  }

  assert(context->children.size() == 1);

  // Fall through to and expression
  return visit(context->children.at(0));
}

std::any
OrcaAstBuilder::visitAndExpression(OrcaParser::AndExpressionContext *context) {
  // And expression
  if (context->rhs) {
    assert(context->children.size() == 3);

    auto lhs = std::any_cast<OrcaAstExpressionNode *>(visit(context->lhs));
    auto rhs = std::any_cast<OrcaAstExpressionNode *>(visit(context->rhs));

    std::string op = context->children.at(1)->getText();

    return std::any((OrcaAstExpressionNode *)new OrcaAstBinaryExpressionNode(
        context, lhs, rhs, op));
  }

  assert(context->children.size() == 1);

  // Fall through to equality expression
  return visit(context->children.at(0));
}

std::any OrcaAstBuilder::visitEqualityExpression(
    OrcaParser::EqualityExpressionContext *context) {
  // Equality expression
  if (context->rhs) {
    assert(context->children.size() == 3);

    auto lhs = std::any_cast<OrcaAstExpressionNode *>(visit(context->lhs));
    auto rhs = std::any_cast<OrcaAstExpressionNode *>(visit(context->rhs));

    std::string op = context->children.at(1)->getText();

    return std::any((OrcaAstExpressionNode *)new OrcaAstBinaryExpressionNode(
        context, lhs, rhs, op));
  }

  assert(context->children.size() == 1);

  // Fall through to relational expression
  return visit(context->children.at(0));
}

std::any OrcaAstBuilder::visitRelationalExpression(
    OrcaParser::RelationalExpressionContext *context) {
  // Relational expression
  if (context->rhs) {
    assert(context->children.size() == 3);

    auto lhs = std::any_cast<OrcaAstExpressionNode *>(visit(context->lhs));
    auto rhs = std::any_cast<OrcaAstExpressionNode *>(visit(context->rhs));

    std::string op = context->children.at(1)->getText();

    return std::any((OrcaAstExpressionNode *)new OrcaAstBinaryExpressionNode(
        context, lhs, rhs, op));
  }

  assert(context->children.size() == 1);

  // Fall through to shift expression
  return visit(context->children.at(0));
}

std::any OrcaAstBuilder::visitShiftExpression(
    OrcaParser::ShiftExpressionContext *context) {
  // Shift expression
  if (context->rhs) {
    assert(context->children.size() == 3);

    auto lhs = std::any_cast<OrcaAstExpressionNode *>(visit(context->lhs));
    auto rhs = std::any_cast<OrcaAstExpressionNode *>(visit(context->rhs));

    std::string op = context->children.at(1)->getText();

    return std::any((OrcaAstExpressionNode *)new OrcaAstBinaryExpressionNode(
        context, lhs, rhs, op));
  }

  assert(context->children.size() == 1);

  // Fall through to additive expression
  return visit(context->children.at(0));
}

std::any OrcaAstBuilder::visitAdditiveExpression(
    OrcaParser::AdditiveExpressionContext *context) {
  // Additive expression
  if (context->rhs) {
    assert(context->children.size() == 3);

    auto lhs = std::any_cast<OrcaAstExpressionNode *>(visit(context->lhs));
    auto rhs = std::any_cast<OrcaAstExpressionNode *>(visit(context->rhs));

    std::string op = context->children.at(1)->getText();

    return std::any((OrcaAstExpressionNode *)new OrcaAstBinaryExpressionNode(
        context, lhs, rhs, op));
  }

  assert(context->children.size() == 1);

  // Fall through to multiplicative expression
  return visit(context->children.at(0));
}

std::any OrcaAstBuilder::visitMultiplicativeExpression(
    OrcaParser::MultiplicativeExpressionContext *context) {
  // Multiplicative expression
  if (context->rhs) {
    assert(context->children.size() == 3);

    auto lhs = std::any_cast<OrcaAstExpressionNode *>(visit(context->lhs));
    auto rhs = std::any_cast<OrcaAstExpressionNode *>(visit(context->rhs));

    std::string op = context->children.at(1)->getText();

    return std::any((OrcaAstExpressionNode *)new OrcaAstBinaryExpressionNode(
        context, lhs, rhs, op));
  }

  assert(context->children.size() == 1);

  assert(context->castExpression());

  // Fall through to cast expression
  return visit(context->castExpression());
}

std::any OrcaAstBuilder::visitSizeofExpression(
    OrcaParser::SizeofExpressionContext *context) {

  printf("VISITING SIZEOF\n");

  return std::any();
}

std::any OrcaAstBuilder::visitPostfixExpression(
    OrcaParser::PostfixExpressionContext *context) {

  assert(context->primaryExpression());

  OrcaAstExpressionNode *expr = std::any_cast<OrcaAstExpressionNode *>(
      visit(context->primaryExpression()));

  // Postfix expression
  // TODO: Implement postfix expression
  assert(context->children.size() == 1);

  return std::any(expr);
}

std::any OrcaAstBuilder::visitPrimaryExpression(
    OrcaParser::PrimaryExpressionContext *context) {

  // Identifier
  if (context->Identifier()) {
    return std::any(
        (OrcaAstExpressionNode *)new OrcaAstIdentifierExpressionNode(
            context, context->Identifier()->getText()));
  }

  // Constants
  if (context->Integer()) {
    int value = std::stoi(context->Integer()->getText());
    return std::any(
        (OrcaAstExpressionNode *)new OrcaAstIntegerLiteralExpressionNode(
            context, value));
  }

  if (context->Float()) {
    float value = std::stof(context->Float()->getText());
    return std::any(
        (OrcaAstExpressionNode *)new OrcaAstFloatLiteralExpressionNode(context,
                                                                       value));
  }

  if (context->String()) {
    std::string value = context->String()->getText();
    return std::any(
        (OrcaAstExpressionNode *)new OrcaAstStringLiteralExpressionNode(context,
                                                                        value));
  }

  if (context->Boolean()) {
    assert(context->Boolean()->getText() == "true" ||
           context->Boolean()->getText() == "false");

    bool value = context->Boolean()->getText() == "true";

    return std::any(
        (OrcaAstExpressionNode *)new OrcaAstBooleanLiteralExpressionNode(
            context, value));
  }

  // let expression / array expression / field map
  if (context->children.size() == 1) {
    return visit(context->children.at(0));
  }

  if (context->LPAREN()) {
    if (!context->RPAREN()) {
      throw OrcaError(compileContext,
                      "Expected closing parenthesis after expression.",
                      context->getStart()->getLine(),
                      context->getStart()->getCharPositionInLine());
    }

    return visit(context->expression());
  }

  throw OrcaError(compileContext,
                  "Encountered unknown primary expression. This is a bug.",
                  context->getStart()->getLine(),
                  context->getStart()->getCharPositionInLine());

  // unreachable
  exit(1);
}

std::any OrcaAstBuilder::visitStatement(OrcaParser::StatementContext *context) {
  assert(context->children.size() == 1);
  return visit(context->children.at(0));
}

std::any OrcaAstBuilder::visitDeclarationStatement(
    OrcaParser::DeclarationStatementContext *context) {
  assert(context->children.size() == 1);
  return visit(context->children.at(0));
}

std::any OrcaAstBuilder::visitTypeDeclaration(
    OrcaParser::TypeDeclarationContext *context) {

  // Template type declaration
  if (context->params) {
    std::vector<std::string> params;

    for (auto &param : context->params->Identifier()) {
      params.push_back(param->getText());
    }

    auto name = context->name->getText();
    auto type = visit(context->typeToAlias);

    assert(type.has_value());

    std::any_cast<OrcaAstTypeNode *>(type)->print(0);

    OrcaAstTemplateTypeDeclarationNode *typeDecl =
        new OrcaAstTemplateTypeDeclarationNode(
            context, params, name, std::any_cast<OrcaAstTypeNode *>(type));

    return std::any((OrcaAstNode *)typeDecl);
  }

  // Generic type declaration
  auto name = context->name->getText();
  auto type = visit(context->typeToAlias);

  OrcaAstTypeDeclarationNode *typeDecl = new OrcaAstTypeDeclarationNode(
      context, name, std::any_cast<OrcaAstTypeNode *>(type));

  return std::any((OrcaAstNode *)typeDecl);
}

std::any OrcaAstBuilder::visitType(OrcaParser::TypeContext *context) {
  return std::any(new OrcaAstTypeNode(context));
}

std::any OrcaAstBuilder::visitFunctionDeclarationStatement(
    OrcaParser::FunctionDeclarationStatementContext *context) {

  // TODO: Type params
  if (context->typeParams) {
    printf("TODO: Type params\n");
    exit(1);
  }

  std::map<std::string, OrcaAstTypeNode *> params;
  if (context->args) {
    for (auto &param : context->args->functionArg()) {
      auto type = visit(param->type());
      params[param->name->getText()] = std::any_cast<OrcaAstTypeNode *>(type);
    }
  }

  auto returnType =
      std::any_cast<OrcaAstTypeNode *>(visit(context->returnType));
  auto functionName = context->name->getText();
  auto body = (OrcaAstCompoundStatementNode *)std::any_cast<OrcaAstNode *>(
      visit(context->body));

  OrcaAstFunctionDeclarationNode *funcDecl = new OrcaAstFunctionDeclarationNode(
      context, functionName, returnType, params, body);

  return std::any((OrcaAstNode *)funcDecl);
}

std::any OrcaAstBuilder::visitCompoundStatement(
    OrcaParser::CompoundStatementContext *context) {

  std::vector<OrcaAstNode *> nodes;

  for (auto &statement : context->statement()) {
    auto node = visit(statement);

    if (!node.has_value()) {
      throw OrcaError(compileContext, "Expected statement.",
                      context->getStart()->getLine(),
                      context->getStart()->getCharPositionInLine());
    }

    nodes.push_back(std::any_cast<OrcaAstNode *>(node));
  }

  return std::any(
      (OrcaAstNode *)new OrcaAstCompoundStatementNode(context, nodes));
}

std::any
OrcaAstBuilder::visitJumpStatement(OrcaParser::JumpStatementContext *context) {
  if (context->BREAK()) {
    return std::any(new OrcaAstJumpStatementNode(context, "break"));
  }

  if (context->CONTINUE()) {
    return std::any(new OrcaAstJumpStatementNode(context, "continue"));
  }

  assert(context->RETURN());

  if (!context->expression()) {
    return std::any(
        (OrcaAstNode *)new OrcaAstJumpStatementNode(context, "return"));
  }

  auto expr = visit(context->expression());
  assert(expr.has_value());

  return std::any((OrcaAstNode *)new OrcaAstJumpStatementNode(
      context, "return", std::any_cast<OrcaAstExpressionNode *>(expr)));
}

std::any OrcaAstBuilder::visitCastExpression(
    OrcaParser::CastExpressionContext *context) {

  if (context->typeToCastTo) {
    auto type = std::any_cast<OrcaAstTypeNode *>(visit(context->typeToCastTo));
    auto expr = std::any_cast<OrcaAstExpressionNode *>(
        visit(context->castExpression()));

    auto node = new OrcaAstCastExpressionNode(context, type, expr);

    return std::any((OrcaAstExpressionNode *)node);
  }

  return visit(context->unaryExpression());
}

std::any OrcaAstBuilder::visitIfStatementBody(
    OrcaParser::IfStatementBodyContext *context) {
  if (context->compoundStatement()) {
    return std::any(std::any_cast<OrcaAstNode *>(
        context->compoundStatement()->accept(this)));
  }

  return std::any(std::any_cast<OrcaAstNode *>(
      context->selectionStatement()->accept(this)));
}

std::any OrcaAstBuilder::visitSelectionStatement(
    OrcaParser::SelectionStatementContext *context) {
  auto condition =
      std::any_cast<OrcaAstExpressionNode *>(visit(context->condition));
  auto thenStatement =
      (OrcaAstCompoundStatementNode *)std::any_cast<OrcaAstNode *>(
          visit(context->then));
  auto elseStatement = (OrcaAstCompoundStatementNode *)nullptr;

  if (context->else_)
    elseStatement =
        (OrcaAstCompoundStatementNode *)std::any_cast<OrcaAstNode *>(
            visit(context->else_));

  return std::any((OrcaAstNode *)new OrcaAstSelectionStatementNode(
      context, condition, thenStatement, elseStatement));
}
