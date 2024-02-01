#pragma once

#include "Binary.h"

namespace orca {

class CmpLEOperator : public BinaryOperator {
public:
  CmpLEOperator() {
    opSymbol = "<";
    instance = this;
  }
  ~CmpLEOperator() = default;

  OrcaType *getResultingType(OrcaType *left, OrcaType *right) override;

  llvm::Value *codegen(OrcaCodeGen &cg, OrcaAstExpressionNode *lhs,
                       OrcaAstExpressionNode *rhs) override;

  /**
   * @brief Get the instance of the OrcaLEOperator.
   */
  static CmpLEOperator *getInstance() {
    if (instance == nullptr)
      instance = new CmpLEOperator();
    return instance;
  };

protected:
  static CmpLEOperator *instance;
};

} // namespace orca
