#include "BitNot.h"
#include "../../OrcaCodeGen.h"

namespace orca {

BitNotOperator *BitNotOperator::instance = nullptr;

OrcaType *BitNotOperator::getResultingType(OrcaType *operandType) {
  if (operandType->getKind() != OrcaTypeKind::Integer)
    throw std::string("bitwise not operator can only be applied to integers");

  return operandType;
}

llvm::Value *BitNotOperator::codegen(OrcaCodeGen &cg, llvm::Value *operand) {
  return cg.builder->CreateNot(operand);
}

} // namespace orca
