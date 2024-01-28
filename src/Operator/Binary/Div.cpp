#include "Div.h"
#include "../../OrcaCodeGen.h"

namespace orca {

DivOperator *DivOperator::instance = nullptr;

OrcaType *DivOperator::getResultingType(OrcaType *lhs, OrcaType *rhs) {
  auto lKind = lhs->getKind();
  auto rKind = rhs->getKind();

  if (lKind == OrcaTypeKind::Integer) {
    if (lKind != rKind)
      throw std::string("Cannot divide integer by non-integer type");

    auto lInt = lhs->getIntegerType();
    auto rInt = rhs->getIntegerType();

    if (lInt.getIsSigned() != rInt.getIsSigned())
      throw std::string("Cannot divide integers of different signedness");

    if (lInt.getBits() != rInt.getBits())
      throw std::string("Cannot divide integers of different sizes");

    return lhs;
  }

  if (lKind == OrcaTypeKind::Float) {
    if (lKind != rKind)
      throw std::string("Cannot divide float by non-float type");

    auto lFloat = lhs->getFloatType();
    auto rFloat = rhs->getFloatType();

    if (lFloat.getBits() != rFloat.getBits())
      throw std::string("Cannot divide floats of different sizes");

    return lhs;
  }

  throw std::string("Cannot divide non-numeric types");
}

llvm::Value *DivOperator::codegen(OrcaCodeGen &cg, llvm::Value *lhs,
                                  llvm::Value *rhs) {
  return cg.builder->CreateSDiv(lhs, rhs);
}

} // namespace orca
