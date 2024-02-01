#pragma once

#include "Binary.h"

namespace orca {

class CmpNEOperator : public BinaryOperator {
public:
  CmpNEOperator() {
    opSymbol = "==";
    instance = this;
  }
  ~CmpNEOperator() = default;

  OrcaType *getResultingType(OrcaType *left, OrcaType *right) override;

  llvm::Value *codegen(OrcaCodeGen &cg, OrcaAstExpressionNode *lhs,
                       OrcaAstExpressionNode *rhs) override;

  /**
   * @brief Get the instance of the OrcaNEOperator.
   */
  static CmpNEOperator *getInstance() {
    if (instance == nullptr)
      instance = new CmpNEOperator();
    return instance;
  };

protected:
  static CmpNEOperator *instance;
};

} // namespace orca
