#include "Sub.h"
#include "../../OrcaAst.h"
#include "../../OrcaCodeGen.h"

namespace orca {

SubOperator *SubOperator::instance = nullptr;

OrcaType *SubOperator::getResultingType(OrcaType *lhs, OrcaType *rhs) {
  auto lKind = lhs->getKind();
  auto rKind = rhs->getKind();

  if (lKind == OrcaTypeKind::Integer) {
    if (lKind != rKind)
      throw std::string("Cannot subtract integer from non-integer type");

    auto lInt = lhs->getIntegerType();
    auto rInt = rhs->getIntegerType();

    if (lInt.getIsSigned() != rInt.getIsSigned())
      throw std::string("Cannot subtract integers of different signedness");

    if (lInt.getBits() != rInt.getBits())
      throw std::string("Cannot subtract integers of different sizes");

    return lhs;
  }

  throw std::string("Cannot subtract non-integer types");
}

llvm::Value *SubOperator::codegen(OrcaCodeGen &cg, OrcaAstExpressionNode *lhs,
                                  OrcaAstExpressionNode *rhs) {
  auto lhsVal = std::any_cast<llvm::Value *>(lhs->accept(cg));
  auto rhsVal = std::any_cast<llvm::Value *>(rhs->accept(cg));

  return cg.builder->CreateSub(lhsVal, rhsVal);
}

} // namespace orca
