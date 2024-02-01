#pragma once

#include "Binary.h"

namespace orca {
class AssignOperator : public BinaryOperator {
public:
  AssignOperator() {
    opSymbol = "=";
    instance = this;
  }
  ~AssignOperator() = default;

  OrcaType *getResultingType(OrcaType *left, OrcaType *right) override;

  llvm::Value *codegen(OrcaCodeGen &cg, OrcaAstExpressionNode *lhs,
                       OrcaAstExpressionNode *rhs) override;

  /**
   * @brief Get the instance of the OrcaAssignitionOperator.
   */
  static AssignOperator *getInstance() {
    if (instance == nullptr)
      instance = new AssignOperator();
    return instance;
  };

protected:
  static AssignOperator *instance;
};

} // namespace orca
