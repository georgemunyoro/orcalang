#include "OrcaCodeGen.h"
#include "OrcaAst.h"
#include "OrcaError.h"
#include "OrcaScope.h"
#include "OrcaType.h"
#include <any>

std::any OrcaCodeGen::visitProgram(OrcaAstProgramNode *node) {
  for (auto &node : node->getNodes())
    node->accept(*this);

  return std::any();
}

std::any
OrcaCodeGen::visitCompoundStatement(OrcaAstCompoundStatementNode *node) {
  for (auto &node : node->getNodes())
    node->accept(*this);

  return std::any();
}

std::any OrcaCodeGen::visitFunctionDeclarationStatement(
    OrcaAstFunctionDeclarationNode *node) {

  llvm::Type *functionType = generateType(node->evaluatedType);

  auto function = llvm::Function::Create(
      llvm::cast<llvm::FunctionType>(functionType),
      llvm::Function::ExternalLinkage, node->getName(), module.get());

  auto entryBlock = llvm::BasicBlock::Create(*llvmContext, "entry", function);
  builder->SetInsertPoint(entryBlock);

  // Enter a new scope
  namedValues = new OrcaScope<llvm::AllocaInst *>(namedValues);

  // Allocate space for the parameters, and store them in the scope
  auto argument = function->arg_begin();
  for (auto &parameter : node->getParameters()) {
    auto paramAlloca =
        builder->CreateAlloca(generateType(parameter.second->evaluatedType));
    namedValues->set(parameter.first, paramAlloca);
    builder->CreateStore(argument, paramAlloca);
    argument++;
  }

  // Generate function body
  node->getBody()->accept(*this);

  if (entryBlock->getTerminator() == nullptr) {

    // If the function is not terminated and the return type is
    // not void, throw an error.
    if (!function->getReturnType()->isVoidTy())
      throw OrcaError(compileContext,
                      "Function '" + node->getName() +
                          "' does not return a value",
                      node->parseContext->getStart()->getLine(),
                      node->parseContext->getStart()->getCharPositionInLine());

    // else implicitly return void. This is necessary because
    // Orca does not support explicit void returns.
    builder->CreateRetVoid();
  }

  // Leave the scope
  namedValues = namedValues->getParent();

  return function;
}

std::any OrcaCodeGen::visitJumpStatement(OrcaAstJumpStatementNode *node) {

  if (node->getKeyword() == "return") {
    if (node->getExpr() == nullptr)
      return builder->CreateRetVoid();

    auto returnValue =
        std::any_cast<llvm::Value *>(node->getExpr()->accept(*this));
    return builder->CreateRet(returnValue);
  }

  throw OrcaError(compileContext,
                  "Unknown jump statement '" + node->getKeyword() +
                      "'. This is a bug.",
                  node->parseContext->getStart()->getLine(),
                  node->parseContext->getStart()->getCharPositionInLine());
}

std::any OrcaCodeGen::visitIntegerLiteralExpression(
    OrcaAstIntegerLiteralExpressionNode *node) {
  return (llvm::Value *)llvm::ConstantInt::get(
      *llvmContext, llvm::APInt(node->evaluatedType->getIntegerType().getBits(),
                                node->getValue()));
}

std::any OrcaCodeGen::visitUnaryExpression(OrcaAstUnaryExpressionNode *node) {
  llvm::Value *operand =
      std::any_cast<llvm::Value *>(node->getExpr()->accept(*this));

  return node->getOperator()->codegen(builder, operand);
};

std::any OrcaCodeGen::visitBinaryExpression(OrcaAstBinaryExpressionNode *node) {
  llvm::Value *lhs =
      std::any_cast<llvm::Value *>(node->getLhs()->accept(*this));
  llvm::Value *rhs =
      std::any_cast<llvm::Value *>(node->getRhs()->accept(*this));

  return node->getOperator()->codegen(builder, lhs, rhs);
};

std::any OrcaCodeGen::visitCastExpression(OrcaAstCastExpressionNode *node) {
  llvm::Value *operand =
      std::any_cast<llvm::Value *>(node->getExpr()->accept(*this));

  OrcaType *fromType = node->getExpr()->evaluatedType;
  OrcaType *toType = node->evaluatedType;

  if (toType->is(OrcaTypeKind::Integer)) {
    switch (fromType->getKind()) {
    case OrcaTypeKind::Integer:
    case OrcaTypeKind::Boolean:
    case OrcaTypeKind::Char:
    case OrcaTypeKind::Float: {
      if (fromType->sizeOf() < toType->sizeOf())
        return builder->CreateSExt(operand, generateType(toType));

      if (fromType->sizeOf() > toType->sizeOf())
        return builder->CreateTrunc(operand, generateType(toType));

      return builder->CreateBitCast(operand, generateType(toType));
    }

    case OrcaTypeKind::Pointer: {
      return builder->CreatePtrToInt(operand,
                                     generateType(node->evaluatedType));
    }

    default:
      break;
    }
  }

  throw OrcaError(compileContext,
                  "Cannot cast from '" +
                      node->getExpr()->evaluatedType->toString() + "' to '" +
                      node->evaluatedType->toString() + "'",
                  node->parseContext->getStart()->getLine(),
                  node->parseContext->getStart()->getCharPositionInLine());
}

std::any OrcaCodeGen::visitBooleanLiteralExpression(
    OrcaAstBooleanLiteralExpressionNode *node) {
  // Booleans are represented as 1 bit unsigned integers
  return (llvm::Value *)llvm::ConstantInt::get(
      *llvmContext, llvm::APInt(1, node->getValue()));
}
