#include "Add.h"
#include "../../OrcaCodeGen.h"

namespace orca {

AddOperator *AddOperator::instance = nullptr;

OrcaType *AddOperator::getResultingType(OrcaType *lhs, OrcaType *rhs) {
  auto lKind = lhs->getKind();
  auto rKind = rhs->getKind();

  if (lKind == OrcaTypeKind::Integer) {
    if (lKind != rKind)
      throw std::string("Cannot add integer to non-integer type");

    auto lInt = lhs->getIntegerType();
    auto rInt = rhs->getIntegerType();

    if (lInt.getIsSigned() != rInt.getIsSigned())
      throw std::string("Cannot add integers of different signedness");

    if (lInt.getBits() != rInt.getBits())
      throw std::string("Cannot add integers of different sizes");

    return lhs;
  }

  throw std::string("Cannot add non-integer types");
}

llvm::Value *AddOperator::codegen(OrcaCodeGen &cg, llvm::Value *lhs,
                                  llvm::Value *rhs) {
  return cg.builder->CreateAdd(lhs, rhs);
}

} // namespace orca
