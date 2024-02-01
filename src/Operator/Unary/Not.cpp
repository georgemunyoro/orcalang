#include "Not.h"
#include "../../OrcaCodeGen.h"

namespace orca {

NotOperator *NotOperator::instance = nullptr;

OrcaType *NotOperator::getResultingType(OrcaType *operandType) {
  auto kind = operandType->getKind();

  if (kind == OrcaTypeKind::Array || kind == OrcaTypeKind::Function ||
      kind == OrcaTypeKind::Void || kind == OrcaTypeKind::Struct)
    throw std::string(
        "not operator cannot be applied to array, function, void or struct");

  return operandType;
}

llvm::Value *NotOperator::codegen(OrcaCodeGen &cg, llvm::Value *operand) {

  // Determine if the operand is non-zero
  llvm::Value *zero = llvm::ConstantInt::get(operand->getType(), 0);
  llvm::Value *result = cg.builder->CreateICmpEQ(operand, zero);

  // Convert the result back to the original type
  return cg.builder->CreateZExt(result, operand->getType());
}

} // namespace orca
