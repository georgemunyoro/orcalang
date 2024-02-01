#include "Assign.h"

namespace orca {

AssignOperator *AssignOperator::instance = nullptr;

OrcaType *AssignOperator::getResultingType(OrcaType *lhs, OrcaType *rhs) {
  auto lKind = lhs->getKind();
  auto rKind = rhs->getKind();
}

llvm::Value *AssignOperator::codegen(OrcaCodeGen &cg,
                                     OrcaAstExpressionNode *lhs,
                                     OrcaAstExpressionNode *rhs) {}

} // namespace orca
