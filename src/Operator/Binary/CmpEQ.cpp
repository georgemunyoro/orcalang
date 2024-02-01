#include "CmpEQ.h"
#include "../../OrcaAst.h"
#include "../../OrcaCodeGen.h"
#include "Binary.h"

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

  throw std::string("Cannot compare types '" + lhs->toString() + "' and '" +
                    rhs->toString() + "'");
}

llvm::Value *CmpEQOperator::codegen(OrcaCodeGen &cg, OrcaAstExpressionNode *lhs,
                                    OrcaAstExpressionNode *rhs) {
  auto lhsVal = std::any_cast<llvm::Value *>(lhs->accept(cg));
  auto rhsVal = std::any_cast<llvm::Value *>(rhs->accept(cg));

  return cg.builder->CreateICmpEQ(lhsVal, rhsVal);
}

} // namespace orca
