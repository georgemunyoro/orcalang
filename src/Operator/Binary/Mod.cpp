#include "Mod.h"
#include "../../OrcaAst.h"
#include "../../OrcaCodeGen.h"
#include "Binary.h"

namespace orca {

ModOperator *ModOperator::instance = nullptr;

OrcaType *ModOperator::getResultingType(OrcaType *lhs, OrcaType *rhs) {
  auto lKind = lhs->getKind();
  auto rKind = rhs->getKind();

  if (lhs->is(OrcaTypeKind::Integer) && rhs->is(OrcaTypeKind::Integer)) {
    if (lhs->getIntegerType().getIsSigned() &&
        rhs->getIntegerType().getIsSigned()) {
      return new OrcaType(OrcaIntegerType(true, 64));
    }

    if (!lhs->getIntegerType().getIsSigned() &&
        !rhs->getIntegerType().getIsSigned()) {
      return new OrcaType(OrcaIntegerType(false, 64));
    }
  }

  throw std::string("Cannot mod types '" + lhs->toString() + "' and '" +
                    rhs->toString() + "'.");
}

llvm::Value *ModOperator::codegen(OrcaCodeGen &cg, OrcaAstExpressionNode *lhs,
                                  OrcaAstExpressionNode *rhs) {
  auto lhsVal = std::any_cast<llvm::Value *>(lhs->accept(cg));
  auto rhsVal = std::any_cast<llvm::Value *>(rhs->accept(cg));

  if (lhs->evaluatedType->getIntegerType().getIsSigned()) {
    return cg.builder->CreateSRem(lhsVal, rhsVal);
  } else {
    return cg.builder->CreateURem(lhsVal, rhsVal);
  }
}

} // namespace orca
