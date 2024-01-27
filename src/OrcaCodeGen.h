#pragma once

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <memory>

#include "OrcaAst.h"
#include "OrcaAstVisitor.h"
#include "OrcaError.h"
#include "OrcaLexerErrorListener.h"
#include "OrcaScope.h"
#include "OrcaType.h"

class OrcaContext;

class OrcaCodeGen : public OrcaAstVisitor {
public:
  OrcaCodeGen(OrcaContext &context) : compileContext(context) {
    llvmContext = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<llvm::IRBuilder<>>(*llvmContext);
    module = std::make_unique<llvm::Module>("Orca", *llvmContext);
  };

  ~OrcaCodeGen() = default;

  std::any visitLetExpression(OrcaAstLetExpressionNode *node) override {
    throw "TODO";
  };

  std::any visitProgram(OrcaAstProgramNode *node) override;

  std::any
  visitAssignmentExpression(OrcaAstAssignmentExpressionNode *node) override {
    throw "TODO";
  };
  std::any
  visitConditionalExpression(OrcaAstConditionalExpressionNode *node) override {
    throw "TODO";
  };

  std::any visitUnaryExpression(OrcaAstUnaryExpressionNode *node) override;

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
  std::any visitType(OrcaAstTypeNode *node) override { throw "TODO"; };

  std::any visitCompoundStatement(OrcaAstCompoundStatementNode *node) override;

  std::any visitFunctionDeclarationStatement(
      OrcaAstFunctionDeclarationNode *node) override;

  std::any visitJumpStatement(OrcaAstJumpStatementNode *node) override;

  std::any visitBinaryExpression(OrcaAstBinaryExpressionNode *node) override;

  std::any
  visitIdentifierExpression(OrcaAstIdentifierExpressionNode *node) override {
    throw "TODO";
  };
  std::any visitIntegerLiteralExpression(
      OrcaAstIntegerLiteralExpressionNode *node) override;

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
    throw "TODO";
  };
  std::any
  visitExpressionStatement(OrcaAstExpressionStatementNode *node) override {
    throw "TODO";
  };

  std::any visitCastExpression(OrcaAstCastExpressionNode *node) override;

  llvm::Type *generateType(OrcaType *type) {
    switch (type->getKind()) {

    case OrcaTypeKind::Integer:
      return llvm::Type::getIntNTy(*llvmContext,
                                   type->getIntegerType().getBits());

    case OrcaTypeKind::Function: {
      auto functionType = type->getFunctionType();

      std::vector<llvm::Type *> parameterTypes;
      for (auto &parameterType : functionType.getParameterTypes()) {
        parameterTypes.push_back(generateType(parameterType));
      }

      return llvm::FunctionType::get(generateType(functionType.getReturnType()),
                                     parameterTypes, false);
    }

    default:
      throw "TODO";
    }
  }

  void generateCode(OrcaAstProgramNode *node) {
    node->accept(*this);
    module->print(llvm::errs(), nullptr);
  }

  std::unique_ptr<llvm::LLVMContext> llvmContext;
  std::unique_ptr<llvm::IRBuilder<>> builder;
  std::unique_ptr<llvm::Module> module;

  OrcaScope<llvm::AllocaInst *> *namedValues =
      new OrcaScope<llvm::AllocaInst *>();

private:
  OrcaContext &compileContext;
};
