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

  OrcaAstProgramNode *program = new OrcaAstProgramNode({});

  for (auto &statement : context->statement()) {
    auto node = visit(statement);
    if (node.has_value())
      program->addNode(std::any_cast<OrcaAstNode *>(node));
  }

  if (program->nodes.size() == 0) {
    OrcaError(compileContext, "Program has no statements.",
              context->getStart()->getLine(),
              context->getStart()->getCharPositionInLine())
        .print();
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
  OrcaAstExpressionListNode *expressionList = new OrcaAstExpressionListNode({});

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

  printf("VISITING LET\n");

  return std::any();
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
          std::any_cast<OrcaAstExpressionNode *>(lhs),
          std::any_cast<OrcaAstExpressionNode *>(rhs), op);

  return std::any(assignmentNode);
}

std::any OrcaAstBuilder::visitUnaryExpression(
    OrcaParser::UnaryExpressionContext *context) {
  printf("VISITING UNARY\n");

  if (context->children.size() == 1) {
    return visit(context->children.at(0));
  }

  assert(context->children.size() == 2);

  auto op = context->children.at(0)->getText();
  auto expr = visit(context->children.at(1));

  if (!expr.has_value()) {
    OrcaError(compileContext, "Expected expression after unary operator.",
              context->getStart()->getLine(),
              context->getStart()->getCharPositionInLine())
        .print();
  }

  OrcaAstUnaryExpressionNode *unaryNode = new OrcaAstUnaryExpressionNode(
      std::any_cast<OrcaAstExpressionNode *>(expr), op);

  return std::any(unaryNode);
}

std::any OrcaAstBuilder::visitConditionalExpression(
    OrcaParser::ConditionalExpressionContext *context) {

  printf("VISITING CONDITIONAL\n");

  return std::any();
}

std::any OrcaAstBuilder::visitSizeofExpression(
    OrcaParser::SizeofExpressionContext *context) {

  printf("VISITING SIZEOF\n");

  return std::any();
}

std::any OrcaAstBuilder::visitPostfixExpression(
    OrcaParser::PostfixExpressionContext *context) {

  if (context->children.size() == 1) {
    return visit(context->children.at(0));
  }

  printf("VISITING POSTFIX\n");

  std::cout << context->toStringTree() << std::endl;

  return std::any();
}

std::any OrcaAstBuilder::visitPrimaryExpression(
    OrcaParser::PrimaryExpressionContext *context) {

  if (context->children.size() == 1) {
    return visit(context->children.at(0));
  }

  if (context->LPAREN()) {
    if (!context->RPAREN()) {
      OrcaError(compileContext,
                "Expected closing parenthesis after expression.",
                context->getStart()->getLine(),
                context->getStart()->getCharPositionInLine())
          .print();
    }

    return visit(context->children.at(1));
  }

  OrcaError(compileContext,
            "Encountered unknown primary expression. This is a bug.",
            context->getStart()->getLine(),
            context->getStart()->getCharPositionInLine())
      .print();

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
            params, name, std::any_cast<OrcaAstTypeNode *>(type));

    return std::any((OrcaAstNode *)typeDecl);
  }

  // Generic type declaration
  auto name = context->name->getText();
  auto type = visit(context->typeToAlias);

  OrcaAstTypeDeclarationNode *typeDecl = new OrcaAstTypeDeclarationNode(
      name, std::any_cast<OrcaAstTypeNode *>(type));

  return std::any((OrcaAstNode *)typeDecl);
}

std::any OrcaAstBuilder::visitType(OrcaParser::TypeContext *context) {
  return std::any(new OrcaAstTypeNode(context));
}
