#include "Mul.h"
#include "../../OrcaAst.h"
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

llvm::Value *MulOperator::codegen(OrcaCodeGen &cg, OrcaAstExpressionNode *lhs,
                                  OrcaAstExpressionNode *rhs) {
  auto lhsVal = std::any_cast<llvm::Value *>(lhs->accept(cg));
  auto rhsVal = std::any_cast<llvm::Value *>(rhs->accept(cg));

  return cg.builder->CreateMul(lhsVal, rhsVal);
}

} // namespace orca
