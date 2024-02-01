#include "OrcaCodeGen.h"
#include "OrcaAst.h"
#include "OrcaError.h"
#include "OrcaScope.h"
#include "OrcaType.h"
#include <any>
#include <cstdlib>

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

  // Validate the function
  llvm::verifyFunction(*function);

  // Optimize the function
  fpm->run(*function, *fam);

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

  size_t bitSize = node->evaluatedType->getIntegerType().getBits();
  size_t value = node->getValue();

  auto llvmInt = node->evaluatedType->getIntegerType().getIsSigned()
                     ? llvm::APSInt(bitSize, value)
                     : llvm::APInt(bitSize, value);

  return (llvm::Value *)llvm::ConstantInt::get(*llvmContext, llvmInt);
}

std::any OrcaCodeGen::visitUnaryExpression(OrcaAstUnaryExpressionNode *node) {
  llvm::Value *operand =
      std::any_cast<llvm::Value *>(node->getExpr()->accept(*this));

  return node->getOperator()->codegen(*this, operand);
};

std::any OrcaCodeGen::visitBinaryExpression(OrcaAstBinaryExpressionNode *node) {
  return node->getOperator()->codegen(*this, node->getLhs(), node->getRhs());
};

std::any OrcaCodeGen::visitCastExpression(OrcaAstCastExpressionNode *node) {
  llvm::Value *operand =
      std::any_cast<llvm::Value *>(node->getExpr()->accept(*this));

  OrcaType *fromType = node->getExpr()->evaluatedType;
  OrcaType *toType = node->evaluatedType;

  if (fromType->is(OrcaTypeKind::Integer)) {
    switch (toType->getKind()) {

    case OrcaTypeKind::Integer: {
      if (fromType->sizeOf() < toType->sizeOf())
        return builder->CreateSExt(operand, generateType(toType));

      if (fromType->sizeOf() > toType->sizeOf())
        return builder->CreateTrunc(operand, generateType(toType));

      return builder->CreateBitCast(operand, generateType(toType));
    }

    default:
      break;
    }
  }

  if (fromType->is(OrcaTypeKind::Boolean)) {
    switch (toType->getKind()) {
    case OrcaTypeKind::Integer: {
      auto bitSize = toType->getIntegerType().getBits();
      return builder->CreateZExt(operand,
                                 llvm::IntegerType::get(*llvmContext, bitSize));
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

std::any
OrcaCodeGen::visitExpressionStatement(OrcaAstExpressionStatementNode *node) {
  return node->expr->accept(*this);
}

std::any
OrcaCodeGen::visitAssignmentExpression(OrcaAstAssignmentExpressionNode *node) {
  llvm::Value *value =
      std::any_cast<llvm::Value *>(node->getRhs()->accept(*this));
  llvm::Value *assignee =
      std::any_cast<llvm::Value *>(node->getLhs()->accept(*this));

  if (llvm::isa<llvm::LoadInst>(assignee)) {
    llvm::LoadInst *loadInst = llvm::cast<llvm::LoadInst>(assignee);
    builder->CreateStore(value, loadInst->getPointerOperand());
    return value;
  }

  builder->CreateStore(value, assignee);
  return value;
};

std::any OrcaCodeGen::visitLetExpression(OrcaAstLetExpressionNode *node) {
  // If we're in a function, we allocate space for the variable
  // in the function's entry block. Otherwise, we have a global
  // variable.

  if (!builder->GetInsertBlock())
    throw OrcaError(compileContext,
                    "Global variables are not supported yet. This is a bug.",
                    node->parseContext->getStart()->getLine(),
                    node->parseContext->getStart()->getCharPositionInLine());

  if (namedValues->isInImmediateScope(node->getName()))
    throw OrcaError(compileContext,
                    "Variable '" + node->getName() +
                        "' already declared in this scope.",
                    node->parseContext->getStart()->getLine(),
                    node->parseContext->getStart()->getCharPositionInLine());

  // Get the current function
  llvm::Function *currentFunction = builder->GetInsertBlock()->getParent();

  llvm::IRBuilder<> tmpBuilder(&currentFunction->getEntryBlock(),
                               currentFunction->getEntryBlock().begin());

  // Allocate space for the variable in the entry block
  llvm::AllocaInst *alloca = tmpBuilder.CreateAlloca(
      generateType(node->evaluatedType), nullptr, node->getName());

  namedValues->set(node->getName(), alloca);

  return (llvm::Value *)alloca;
}

std::any
OrcaCodeGen::visitIdentifierExpression(OrcaAstIdentifierExpressionNode *node) {
  if (!namedValues->isInScope(node->getName()))
    throw OrcaError(compileContext,
                    "Variable '" + node->getName() + "' not declared.",
                    node->parseContext->getStart()->getLine(),
                    node->parseContext->getStart()->getCharPositionInLine());

  // Get the variable from the scope
  llvm::AllocaInst *alloca = *namedValues->get(node->getName());

  return (llvm::Value *)builder->CreateLoad(alloca->getAllocatedType(), alloca);
}
