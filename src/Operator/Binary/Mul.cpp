#include "Mul.h"
#include "../../OrcaCodeGen.h"

namespace orca {

MulOperator *MulOperator::instance = nullptr;

OrcaType *MulOperator::getResultingType(OrcaType *lhs, OrcaType *rhs) {
  auto lKind = lhs->getKind();
  auto rKind = rhs->getKind();

  if (lKind == OrcaTypeKind::Integer) {
    if (lKind != rKind)
      throw std::string("Cannot multiply integer with non-integer type");

    auto lInt = lhs->getIntegerType();
    auto rInt = rhs->getIntegerType();

    if (lInt.getIsSigned() != rInt.getIsSigned())
      throw std::string("Cannot multiply integers of different signedness");

    if (lInt.getBits() != rInt.getBits())
      throw std::string("Cannot multiply integers of different sizes");

    return lhs;
  }

  throw std::string("Cannot multiply non-integer types");
}

llvm::Value *MulOperator::codegen(OrcaCodeGen &cg, llvm::Value *lhs,
                                  llvm::Value *rhs) {
  return cg.builder->CreateMul(lhs, rhs);
}

} // namespace orca
