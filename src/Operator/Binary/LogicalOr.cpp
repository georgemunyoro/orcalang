#include "LogicalOr.h"
#include "../../OrcaAst.h"
#include "../../OrcaCodeGen.h"
#include "Binary.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"

namespace orca {

LogicalOrOperator *LogicalOrOperator::instance = nullptr;

OrcaType *LogicalOrOperator::getResultingType(OrcaType *lhs, OrcaType *rhs) {
  auto lKind = lhs->getKind();
  auto rKind = rhs->getKind();

  if (lKind == OrcaTypeKind::Boolean) {
    if (lKind != rKind)
      throw std::string("Cannot logical and boolean with non-boolean type");

    return new OrcaType(OrcaBooleanType());
  }

  throw std::string("Cannot logical and non-boolean types");
}

llvm::Value *LogicalOrOperator::codegen(OrcaCodeGen &cg,
                                        OrcaAstExpressionNode *lhs,
                                        OrcaAstExpressionNode *rhs) {

  auto lhsVal = std::any_cast<llvm::Value *>(lhs->accept(cg));

  // Will be used to store the result of the logical or operation.
  // This should be optimized away into a PHI node by LLVM.
  llvm::AllocaInst *resVal =
      cg.builder->CreateAlloca(lhsVal->getType(), nullptr, "or.res");

  cg.builder->CreateStore(lhsVal, resVal);

  llvm::Function *currFunc = cg.builder->GetInsertBlock()->getParent();

  if (!currFunc)
    throw std::string("No function in current scope");

  llvm::BasicBlock *rhsBlock =
      llvm::BasicBlock::Create(*cg.llvmContext, "or.rhs");
  llvm::BasicBlock *endBlock =
      llvm::BasicBlock::Create(*cg.llvmContext, "or.end");

  currFunc->getBasicBlockList().push_back(rhsBlock);
  cg.builder->CreateCondBr(lhsVal, endBlock, rhsBlock);
  cg.builder->SetInsertPoint(rhsBlock);
  auto rhsVal = std::any_cast<llvm::Value *>(rhs->accept(cg));
  cg.builder->CreateStore(rhsVal, resVal);

  cg.builder->CreateBr(endBlock);
  currFunc->getBasicBlockList().push_back(endBlock);
  cg.builder->SetInsertPoint(endBlock);
  return (llvm::Value *)cg.builder->CreateLoad(resVal->getAllocatedType(),
                                               resVal);
}

} // namespace orca
