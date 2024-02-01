#include "Add.h"
#include "../../OrcaAst.h"
#include "../../OrcaCodeGen.h"
#include "Binary.h"

namespace orca {

AddOperator *AddOperator::instance = nullptr;

OrcaType *AddOperator::getResultingType(OrcaType *lhs, OrcaType *rhs) {
  auto lKind = lhs->getKind();
  auto rKind = rhs->getKind();

  if (lhs->is(OrcaTypeKind::Integer)) {
    auto lhsInt = lhs->getIntegerType();

    switch (rKind) {

    // iX + iY = uZ where Z = max(X, Y)
    // uX + uX = uX
    // sX + sX = sX
    // uX + sX = sX
    case OrcaTypeKind::Integer: {
      auto rhsInt = rhs->getIntegerType();

      size_t largerBitSize = std::max(lhs->getIntegerType().getBits(),
                                      rhs->getIntegerType().getBits());

      return new OrcaType(OrcaIntegerType(
          lhsInt.getIsSigned() || rhsInt.getIsSigned(), largerBitSize));
    }

    // int + float = float
    case OrcaTypeKind::Float:
      return rhs;

    default:
      break;
    }
  }

  if (lhs->is(OrcaTypeKind::Float)) {
    auto lhsFloat = lhs->getFloatType();

    switch (rKind) {

    // float + float = float
    case OrcaTypeKind::Float: {
      auto rhsFloat = rhs->getFloatType();

      size_t largerBitSize = std::max(lhs->getFloatType().getBits(),
                                      rhs->getFloatType().getBits());

      return new OrcaType(OrcaFloatType(largerBitSize));
    }

    // float + int = float
    case OrcaTypeKind::Integer:
      return lhs;

    default:
      break;
    }
  }

  throw std::string("Cannot add types '" + lhs->toString() + "' and '" +
                    rhs->toString() + "'.");
}

llvm::Value *AddOperator::codegen(OrcaCodeGen &cg, OrcaAstExpressionNode *lhs,
                                  OrcaAstExpressionNode *rhs) {
  auto lhsVal = std::any_cast<llvm::Value *>(lhs->accept(cg));
  auto rhsVal = std::any_cast<llvm::Value *>(rhs->accept(cg));

  return cg.builder->CreateAdd(lhsVal, rhsVal);
}

} // namespace orca
