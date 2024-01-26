#include "OrcaCodeGen.h"
#include "OrcaError.h"
#include "OrcaScope.h"
#include "OrcaType.h"
#include <any>

std::any OrcaCodeGen::visitProgram(OrcaAstProgramNode *node) {
  for (auto &node : node->getNodes()) {
    node->accept(*this);
  }

  return std::any();
}

std::any OrcaCodeGen::visitFunctionDeclarationStatement(
    OrcaAstFunctionDeclarationNode *node) {

  // Convert OrcaType to LLVMType
  auto functionType = generateType(node->evaluatedType);

  // Create function
  auto function = llvm::Function::Create(
      llvm::cast<llvm::FunctionType>(functionType),
      llvm::Function::ExternalLinkage, node->getName(), module.get());

  // Create entry block
  auto entryBlock = llvm::BasicBlock::Create(*llvmContext, "entry", function);
  builder->SetInsertPoint(entryBlock);

  // Enter a new scope
  namedValues = new OrcaScope<llvm::AllocaInst *>(namedValues);

  // Create function arguments
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

  // Check if the function is terminated
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

std::any
OrcaCodeGen::visitCompoundStatement(OrcaAstCompoundStatementNode *node) {
  for (auto &node : node->getNodes()) {
    node->accept(*this);
  }

  return std::any();
}

std::any OrcaCodeGen::visitJumpStatement(OrcaAstJumpStatementNode *node) {

  if (node->getKeyword() == "return") {
    if (node->getExpr() == nullptr) {
      return builder->CreateRetVoid();
    }

    auto returnValue =
        std::any_cast<llvm::Value *>(node->getExpr()->accept(*this));
    return builder->CreateRet(returnValue);
  }

  throw OrcaError(compileContext, "Unknown jump statement",
                  node->parseContext->getStart()->getLine(),
                  node->parseContext->getStart()->getCharPositionInLine());

  return std::any();
}

std::any OrcaCodeGen::visitIntegerLiteralExpression(
    OrcaAstIntegerLiteralExpressionNode *node) {
  return (llvm::Value *)llvm::ConstantInt::get(
      *llvmContext, llvm::APInt(node->evaluatedType->getIntegerType().getBits(),
                                node->getValue()));
}
