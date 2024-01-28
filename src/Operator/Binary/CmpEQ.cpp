#include "CmpEQ.h"
#include "../../OrcaCodeGen.h"

namespace orca {

CmpEQOperator *CmpEQOperator::instance = nullptr;

OrcaType *CmpEQOperator::getResultingType(OrcaType *lhs, OrcaType *rhs) {
  auto lKind = lhs->getKind();
  auto rKind = rhs->getKind();

  if (lKind == OrcaTypeKind::Integer) {
    if (lKind != rKind)
      throw std::string("Cannot compare integer with non-integer type");

    auto lInt = lhs->getIntegerType();
    auto rInt = rhs->getIntegerType();

    return new OrcaType(OrcaBooleanType());
  }

  if (lKind == OrcaTypeKind::Float) {
    if (lKind != rKind)
      throw std::string("Cannot compare float with non-float type");

    return new OrcaType(OrcaBooleanType());
  }

  if (lKind == OrcaTypeKind::Boolean) {
    if (lKind != rKind)
      throw std::string("Cannot compare boolean with non-boolean type");

    return new OrcaType(OrcaBooleanType());
  }

  throw std::string("Cannot compare non-numeric types");
}

llvm::Value *CmpEQOperator::codegen(OrcaCodeGen &cg, llvm::Value *lhs,
                                    llvm::Value *rhs) {
  // Compare and return a boolean

  return cg.builder->CreateICmpEQ(lhs, rhs);
}

} // namespace orca
