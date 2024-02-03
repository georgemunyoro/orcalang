#pragma once

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetIntrinsicInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"
#include "llvm/Transforms/Utils.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "OrcaAst.h"
#include "OrcaAstVisitor.h"
#include "OrcaError.h"
#include "OrcaLLVMBuilder.h"
#include "OrcaLexerErrorListener.h"
#include "OrcaScope.h"
#include "OrcaType.h"

typedef struct {
  llvm::BasicBlock *header;
  llvm::BasicBlock *body;
  llvm::BasicBlock *after;
} LoopInfo;

class OrcaContext;

class OrcaCodeGen : public OrcaAstVisitor {
public:
  OrcaCodeGen(OrcaContext &context) : compileContext(context) {
    llvmContext = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<OrcaLLVMBuilder>(*llvmContext);
    // module = std::make_unique<llvm::Module>("Orca", *llvmContext);
    module = new llvm::Module("Orca", *llvmContext);

    // Instrumentation
    pic = std::make_unique<llvm::PassInstrumentationCallbacks>();
    si = std::make_unique<llvm::StandardInstrumentations>(true);

    si->registerCallbacks(*pic, fam.get());

    // Initialize the pass managers
    fpm = std::make_unique<llvm::FunctionPassManager>();
    fpmLegacy = std::make_unique<llvm::legacy::FunctionPassManager>(module);

    // Initialize the analysis managers
    fam = std::make_unique<llvm::FunctionAnalysisManager>();
    mam = std::make_unique<llvm::ModuleAnalysisManager>();
    cgam = std::make_unique<llvm::CGSCCAnalysisManager>();
    lam = std::make_unique<llvm::LoopAnalysisManager>();

    fpm->addPass(llvm::ReassociatePass());
    fpm->addPass(llvm::GVNPass());
    fpm->addPass(llvm::SimplifyCFGPass());
    fpm->addPass(llvm::InstCombinePass());

    // Promote allocas to registers.
    fpmLegacy->add(llvm::createPromoteMemoryToRegisterPass());

    llvm::PassBuilder pb;
    pb.registerModuleAnalyses(*mam);
    pb.registerFunctionAnalyses(*fam);
    pb.crossRegisterProxies(*lam, *fam, *cgam, *mam);
  };

  ~OrcaCodeGen() = default;

  std::any visitLetExpression(OrcaAstLetExpressionNode *node) override;

  std::any visitProgram(OrcaAstProgramNode *node) override;

  std::any
  visitAssignmentExpression(OrcaAstAssignmentExpressionNode *node) override;

  std::any
  visitConditionalExpression(OrcaAstConditionalExpressionNode *node) override;

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

  std::any
  visitFunctionCallExpression(OrcaAstFunctionCallExpressionNode *node) override;

  std::any visitJumpStatement(OrcaAstJumpStatementNode *node) override;

  std::any
  visitIterationStatement(OrcaAstIterationStatementNode *node) override;

  std::any visitBinaryExpression(OrcaAstBinaryExpressionNode *node) override;

  std::any
  visitIdentifierExpression(OrcaAstIdentifierExpressionNode *node) override;

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
      OrcaAstBooleanLiteralExpressionNode *node) override;

  std::any
  visitExpressionStatement(OrcaAstExpressionStatementNode *node) override;

  std::any
  visitSelectionStatement(OrcaAstSelectionStatementNode *node) override;

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

    case OrcaTypeKind::Boolean:
      return llvm::Type::getInt1Ty(*llvmContext);

    case OrcaTypeKind::Void:
      return llvm::Type::getVoidTy(*llvmContext);

    default:
      throw "TODO";
    }
  }

  void generateCode(OrcaAstProgramNode *node) {
    try {
      node->accept(*this);
      llvm::verifyModule(*module, &llvm::errs());

      if (false) {

        // TODO: Move this into it's own module. This isn't scalable,
        // and doesn't allow for multiple compilation units. We need to
        // be able to link multiple modules together.

        std::string TargetTriple = llvm::sys::getDefaultTargetTriple();

        llvm::InitializeAllTargetInfos();
        llvm::InitializeAllTargets();
        llvm::InitializeAllTargetMCs();
        llvm::InitializeAllAsmParsers();
        llvm::InitializeAllAsmPrinters();

        std::string Error;
        auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

        if (!Target) {
          llvm::errs() << Error;
          throw std::runtime_error("Failed to lookup target");
        }

        auto CPU = "generic";
        auto Features = "";

        llvm::TargetOptions opt;
        auto TargetMachine = Target->createTargetMachine(
            TargetTriple, CPU, Features, opt, llvm::Reloc::PIC_);

        this->module->setDataLayout(TargetMachine->createDataLayout());
        this->module->setTargetTriple(TargetTriple);

        auto Filename = "output.o";
        std::error_code EC;
        llvm::raw_fd_ostream dest(Filename, EC, llvm::sys::fs::OF_None);

        if (EC) {
          llvm::errs() << "Could not open file: " << EC.message();
          throw std::runtime_error("Failed to open file");
        }

        llvm::legacy::PassManager pass;
        auto FileType = llvm::CodeGenFileType::CGFT_ObjectFile;

        if (TargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
          llvm::errs() << "TargetMachine can't emit a file of this type";
          throw std::runtime_error("Failed to emit file");
        }

        pass.run(*this->module);
        dest.flush();

        // Convert the object file into an executable
        std::string LinkerCommand = "clang++ -o output output.o";
        if (std::system(LinkerCommand.c_str()) == -1) {
          throw std::runtime_error("Failed to link object file");
        }

        printf("Successfully compiled\n");
      }
    } catch (OrcaError &error) {
      error.print();
    }
  }

  std::unique_ptr<llvm::LLVMContext> llvmContext;
  std::unique_ptr<OrcaLLVMBuilder> builder;
  // std::unique_ptr<llvm::Module> module;
  llvm::Module *module;

  std::unique_ptr<llvm::StandardInstrumentations> si;
  std::unique_ptr<llvm::PassInstrumentationCallbacks> pic;

  // Pass managers
  std::unique_ptr<llvm::FunctionPassManager> fpm;
  std::unique_ptr<llvm::legacy::FunctionPassManager> fpmLegacy;

  // Analysis managers
  std::unique_ptr<llvm::FunctionAnalysisManager> fam;
  std::unique_ptr<llvm::ModuleAnalysisManager> mam;
  std::unique_ptr<llvm::CGSCCAnalysisManager> cgam;
  std::unique_ptr<llvm::LoopAnalysisManager> lam;

  // Stores information about the current loop
  std::vector<LoopInfo> loopStack;

  OrcaScope<llvm::AllocaInst *> *namedValues =
      new OrcaScope<llvm::AllocaInst *>();

  std::string getUniqueLabel(std::string name) {
    if (uniqueLabelCounter.find(name) == uniqueLabelCounter.end())
      uniqueLabelCounter[name] = 0;

    return name + std::to_string(uniqueLabelCounter[name]++);
  }

  std::unordered_map<std::string, int> uniqueLabelCounter;

private:
  OrcaContext &compileContext;
};
