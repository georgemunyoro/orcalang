#include "OrcaCodeGen.h"
#include "OrcaAst.h"
#include "OrcaError.h"
#include "OrcaScope.h"
#include "OrcaType.h"
#include <alloca.h>
#include <any>
#include <cassert>
#include <cstdio>
#include <cstdlib>

std::any OrcaCodeGen::visitProgram(OrcaAstProgramNode *node) {
  for (auto &node : node->getNodes())
    node->accept(*this);

  return std::any();
}

std::any
OrcaCodeGen::visitCompoundStatement(OrcaAstCompoundStatementNode *node) {
  // Enter a new scope
  namedValues = new OrcaScope<llvm::AllocaInst *>(namedValues);

  for (auto &node : node->getNodes())
    node->accept(*this);

  // Leave the scope
  namedValues = namedValues->getParent();

  return std::any();
}

std::any OrcaCodeGen::visitFunctionDeclarationStatement(
    OrcaAstFunctionDeclarationNode *node) {

  llvm::Type *functionType = generateType(node->evaluatedType);

  auto function = llvm::Function::Create(
      llvm::cast<llvm::FunctionType>(functionType),
      llvm::Function::ExternalLinkage, node->getName(), module);

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

  if (function->getBasicBlockList().back().getTerminator() == nullptr) {

    // If the function is not terminated and the return type is
    // not void, throw an error.
    if (!function->getReturnType()->isVoidTy())
      throw OrcaError(compileContext,
                      "Function '" + node->getName() +
                          "' does not return a value on all paths.",
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
  fpmLegacy->run(*function);

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

  if (node->getKeyword() == "break") {
    if (loopStack.empty())
      throw OrcaError(compileContext, "Break statement not in a loop.",
                      node->parseContext->getStart()->getLine(),
                      node->parseContext->getStart()->getCharPositionInLine());

    return builder->CreateBr(loopStack.back().after);
  }

  if (node->getKeyword() == "continue") {
    if (loopStack.empty())
      throw OrcaError(compileContext, "Continue statement not in a loop.",
                      node->parseContext->getStart()->getLine(),
                      node->parseContext->getStart()->getCharPositionInLine());

    return builder->CreateBr(loopStack.back().header);
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

  if (module->getFunction(node->getName())) {
    // If the identifier is a function, return the function
    return (llvm::Value *)module->getFunction(node->getName());
  }

  if (!namedValues->isInScope(node->getName()))
    throw OrcaError(compileContext,
                    "Variable '" + node->getName() + "' not declared.",
                    node->parseContext->getStart()->getLine(),
                    node->parseContext->getStart()->getCharPositionInLine());

  // Get the variable from the scope
  llvm::AllocaInst *alloca = *namedValues->get(node->getName());

  return (llvm::Value *)builder->CreateLoad(alloca->getAllocatedType(), alloca);
}

std::any
OrcaCodeGen::visitSelectionStatement(OrcaAstSelectionStatementNode *node) {
  auto condVal =
      std::any_cast<llvm::Value *>(node->getCondition()->accept(*this));

  llvm::Function *currentFunction = builder->GetInsertBlock()->getParent();
  if (!currentFunction) {
    throw OrcaError(compileContext,
                    "Selection statements are not supported in global scope.",
                    node->parseContext->getStart()->getLine(),
                    node->parseContext->getStart()->getCharPositionInLine());
  }

  llvm::BasicBlock *thenBlock = llvm::BasicBlock::Create(
      *llvmContext, getUniqueLabel("if.then"), currentFunction);
  llvm::BasicBlock *elseBlock =
      llvm::BasicBlock::Create(*llvmContext, getUniqueLabel("if.else"));
  llvm::BasicBlock *contBlock =
      llvm::BasicBlock::Create(*llvmContext, getUniqueLabel("if.cont"));

  builder->CreateCondBr(condVal, thenBlock, elseBlock);

  builder->SetInsertPoint(thenBlock);
  node->getThenStatement()->accept(*this);
  if (!currentFunction->getBasicBlockList().back().getTerminator())
    builder->CreateBr(contBlock);

  elseBlock->insertInto(currentFunction);
  builder->SetInsertPoint(elseBlock);
  if (node->getElseStatement())
    node->getElseStatement()->accept(*this);
  if (!currentFunction->getBasicBlockList().back().getTerminator())
    builder->CreateBr(contBlock);

  if (contBlock->hasNPredecessorsOrMore(1)) {
    contBlock->insertInto(currentFunction);
    builder->SetInsertPoint(contBlock);
  }

  return std::any();
}

std::any OrcaCodeGen::visitConditionalExpression(
    OrcaAstConditionalExpressionNode *node) {
  auto condVal =
      std::any_cast<llvm::Value *>(node->getCondition()->accept(*this));

  llvm::Function *currentFunction = builder->GetInsertBlock()->getParent();
  if (!currentFunction) {
    throw OrcaError(compileContext,
                    "Selection statements are not supported in global scope.",
                    node->parseContext->getStart()->getLine(),
                    node->parseContext->getStart()->getCharPositionInLine());
  }

  // Create a temporary alloca to store the result.
  // Will change this to a phi node later, but this is easier, and it's
  // optimized away by LLVM anyway.
  auto alloca =
      builder->CreateAlloca(generateType(node->evaluatedType), nullptr);

  llvm::BasicBlock *thenBlock = llvm::BasicBlock::Create(
      *llvmContext, getUniqueLabel("tern.then"), currentFunction);
  llvm::BasicBlock *elseBlock =
      llvm::BasicBlock::Create(*llvmContext, getUniqueLabel("tern.else"));
  llvm::BasicBlock *contBlock =
      llvm::BasicBlock::Create(*llvmContext, getUniqueLabel("tern.cont"));

  builder->CreateCondBr(condVal, thenBlock, elseBlock);
  builder->SetInsertPoint(thenBlock);
  auto thenVal =
      std::any_cast<llvm::Value *>(node->getThenExpr()->accept(*this));
  builder->CreateStore(thenVal, alloca);
  builder->CreateBr(contBlock);

  elseBlock->insertInto(currentFunction);
  builder->SetInsertPoint(elseBlock);
  auto elseVal =
      std::any_cast<llvm::Value *>(node->getElseExpr()->accept(*this));
  builder->CreateStore(elseVal, alloca);
  builder->CreateBr(contBlock);

  contBlock->insertInto(currentFunction);
  builder->SetInsertPoint(contBlock);

  auto loadInst = builder->CreateLoad(alloca->getAllocatedType(), alloca);

  return (llvm::Value *)loadInst;
}

std::any
OrcaCodeGen::visitIterationStatement(OrcaAstIterationStatementNode *node) {
  llvm::Function *currentFunction = builder->GetInsertBlock()->getParent();
  if (!currentFunction) {
    throw OrcaError(compileContext,
                    "Iteration statements are not supported in global scope.",
                    node->parseContext->getStart()->getLine(),
                    node->parseContext->getStart()->getCharPositionInLine());
  }

  llvm::BasicBlock *condBlock = llvm::BasicBlock::Create(
      *llvmContext, getUniqueLabel("w.cond"), currentFunction);
  llvm::BasicBlock *bodyBlock =
      llvm::BasicBlock::Create(*llvmContext, getUniqueLabel("w.body"));
  llvm::BasicBlock *contBlock =
      llvm::BasicBlock::Create(*llvmContext, getUniqueLabel("w.cont"));

  loopStack.push_back({
      .header = condBlock,
      .body = bodyBlock,
      .after = contBlock,
  });

  builder->CreateBr(condBlock);
  builder->SetInsertPoint(condBlock);
  builder->CreateCondBr(
      std::any_cast<llvm::Value *>(node->getCondition()->accept(*this)),
      bodyBlock, contBlock);

  bodyBlock->insertInto(currentFunction);
  builder->SetInsertPoint(bodyBlock);
  node->getBody()->accept(*this);
  if (!currentFunction->getBasicBlockList().back().getTerminator())
    builder->CreateBr(condBlock);

  if (contBlock->hasNPredecessorsOrMore(1)) {
    contBlock->insertInto(currentFunction);
    builder->SetInsertPoint(contBlock);
  }

  loopStack.pop_back();

  return std::any();
}

std::any OrcaCodeGen::visitFunctionCallExpression(
    OrcaAstFunctionCallExpressionNode *node) {
  auto func = (llvm::Function *)std::any_cast<llvm::Value *>(
      node->getCallee()->accept(*this));

  std::vector<llvm::Value *> args;
  for (auto &arg : node->getArgs())
    args.push_back(std::any_cast<llvm::Value *>(arg->accept(*this)));

  return (llvm::Value *)builder->CreateCall(func, args);
}

std::any OrcaCodeGen::visitExpressionList(OrcaAstExpressionListNode *node) {
  llvm::Function *currentFunction = builder->GetInsertBlock()->getParent();
  llvm::IRBuilder<> tmpBuilder(&currentFunction->getEntryBlock(),
                               currentFunction->getEntryBlock().begin());

  auto elementType = generateType(node->getElements().at(0)->evaluatedType);
  auto arrayType =
      llvm::ArrayType::get(elementType, node->getElements().size());

  llvm::AllocaInst *arrayAlloc = tmpBuilder.CreateAlloca(
      arrayType, nullptr, getUniqueLabel("array.alloc"));

  auto zero = llvm::ConstantInt::get(*llvmContext, llvm::APInt(32, 0));

  for (size_t i = 0; i < node->getElements().size(); ++i) {
    auto elementValue =
        std::any_cast<llvm::Value *>(node->getElements().at(i)->accept(*this));

    auto index = llvm::ConstantInt::get(*llvmContext, llvm::APInt(32, i));
    auto gep = builder->CreateInBoundsGEP(arrayType, arrayAlloc, {zero, index});
    auto store = builder->CreateStore(elementValue, gep);
  }

  auto firstElementPtr =
      builder->CreateInBoundsGEP(arrayType, arrayAlloc, {zero, zero});
  return (llvm::Value *)firstElementPtr;
}

std::any OrcaCodeGen::visitIndexExpression(OrcaAstIndexExpressionNode *node) {
  auto indexee = std::any_cast<llvm::Value *>(node->getExpr()->accept(*this));
  auto index = std::any_cast<llvm::Value *>(node->getIndex()->accept(*this));
  auto gep = builder->CreateGEP(index->getType(), indexee, index);
  auto load = builder->CreateLoad(gep->getType()->getPointerElementType(), gep);
  return (llvm::Value *)load;
}
