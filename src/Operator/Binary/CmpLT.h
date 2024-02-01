#pragma once

#include "Binary.h"

namespace orca {

class CmpLTOperator : public BinaryOperator {
public:
  CmpLTOperator() {
    opSymbol = "<";
    instance = this;
  }
  ~CmpLTOperator() = default;

  OrcaType *getResultingType(OrcaType *left, OrcaType *right) override;

  llvm::Value *codegen(OrcaCodeGen &cg, OrcaAstExpressionNode *lhs,
                       OrcaAstExpressionNode *rhs) override;

  /**
   * @brief Get the instance of the OrcaLTOperator.
   */
  static CmpLTOperator *getInstance() {
    if (instance == nullptr)
      instance = new CmpLTOperator();
    return instance;
  };

protected:
  static CmpLTOperator *instance;
};

} // namespace orca
