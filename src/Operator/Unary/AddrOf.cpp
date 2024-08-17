#include "AddrOf.h"
#include "../../OrcaCodeGen.h"
#include <llvm-14/llvm/IR/Instructions.h>

namespace orca {

AddrOfOperator *AddrOfOperator::instance = nullptr;

OrcaType *AddrOfOperator::getResultingType(OrcaType *operandType) {
  return new OrcaType(OrcaPointerType(operandType));
}

llvm::Value *AddrOfOperator::codegen(OrcaCodeGen &cg, llvm::Value *operand) {
  if (llvm::isa<llvm::LoadInst>(operand)) {
    auto loadInst = llvm::cast<llvm::LoadInst>(operand);
    return loadInst->getPointerOperand();
  }

  return operand;
}

} // namespace orca
