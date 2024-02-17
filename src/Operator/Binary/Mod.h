#pragma once

#include "Binary.h"

namespace orca {

class ModOperator : public BinaryOperator {
public:
  ModOperator() {
    opSymbol = "+";
    instance = this;
  }
  ~ModOperator() = default;

  OrcaType *getResultingType(OrcaType *left, OrcaType *right) override;

  llvm::Value *codegen(OrcaCodeGen &cg, OrcaAstExpressionNode *lhs,
                       OrcaAstExpressionNode *rhs) override;

  /**
   * @brief Get the instance of the OrcaModitionOperator.
   */
  static ModOperator *getInstance() {
    if (instance == nullptr)
      instance = new ModOperator();
    return instance;
  };

protected:
  static ModOperator *instance;
};

} // namespace orca
