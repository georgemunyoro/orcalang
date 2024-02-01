#include "CmpLE.h"
#include "../../OrcaAst.h"
#include "../../OrcaCodeGen.h"
#include "Binary.h"

namespace orca {

CmpLEOperator *CmpLEOperator::instance = nullptr;

OrcaType *CmpLEOperator::getResultingType(OrcaType *lhs, OrcaType *rhs) {
  auto lKind = lhs->getKind();
  auto rKind = rhs->getKind();

  if (lKind == OrcaTypeKind::Integer) {
    if (lKind != rKind)
      throw std::string("Cannot compare integer with non-integer type");

    auto lInt = lhs->getIntegerType();
    auto rInt = rhs->getIntegerType();

    if (lInt.getIsSigned() != rInt.getIsSigned())
      throw std::string("Cannot compare signed and unsigned integers");

    return new OrcaType(OrcaBooleanType());
  }

  if (lKind == OrcaTypeKind::Float) {
    if (lKind != rKind)
      throw std::string("Cannot compare float with non-float type");

    return new OrcaType(OrcaBooleanType());
  }

  throw std::string("Cannot compare types '" + lhs->toString() + "' and '" +
                    rhs->toString() + "'");
}

llvm::Value *CmpLEOperator::codegen(OrcaCodeGen &cg, OrcaAstExpressionNode *lhs,
                                    OrcaAstExpressionNode *rhs) {
  auto lhsVal = std::any_cast<llvm::Value *>(lhs->accept(cg));
  auto rhsVal = std::any_cast<llvm::Value *>(rhs->accept(cg));

  auto lhsType = lhs->evaluatedType;
  auto rhsType = rhs->evaluatedType;

  switch (lhsType->getKind()) {
  case OrcaTypeKind::Integer: {
    auto lInt = lhsType->getIntegerType();
    auto rInt = rhsType->getIntegerType();

    if (lInt.getIsSigned())
      return cg.builder->CreateICmpSLE(lhsVal, rhsVal);

    return cg.builder->CreateICmpULE(lhsVal, rhsVal);
  }

  case OrcaTypeKind::Float: {
    return cg.builder->CreateFCmpOLE(lhsVal, rhsVal);
  }

  default:
    break;
  }

  throw std::string("Cannot compare types '" + lhsType->toString() + "' and '" +
                    rhsType->toString() + "'");
}

} // namespace orca
