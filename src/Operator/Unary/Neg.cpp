#include "Neg.h"
#include "../../OrcaCodeGen.h"

namespace orca {

NegOperator *NegOperator::instance = nullptr;

OrcaType *NegOperator::getResultingType(OrcaType *operandType) {
  if (operandType->getKind() != OrcaTypeKind::Integer)
    throw std::string("neg operator can only be applied to integers");

  // Negating a value will always result in a signed value
  return new OrcaType(
      OrcaIntegerType(true, operandType->getIntegerType().getBits()));
}

llvm::Value *NegOperator::codegen(OrcaCodeGen &cg, llvm::Value *operand) {
  return cg.builder->CreateNeg(operand);
}

} // namespace orca
