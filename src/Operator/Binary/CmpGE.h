#pragma once

#include "Binary.h"

namespace orca {

class CmpGEOperator : public BinaryOperator {
public:
  CmpGEOperator() {
    opSymbol = ">=";
    instance = this;
  }
  ~CmpGEOperator() = default;

  OrcaType *getResultingType(OrcaType *left, OrcaType *right) override;

  llvm::Value *codegen(OrcaCodeGen &cg, OrcaAstExpressionNode *lhs,
                       OrcaAstExpressionNode *rhs) override;

  /**
   * @brief Get the instance of the OrcaGEOperator.
   */
  static CmpGEOperator *getInstance() {
    if (instance == nullptr)
      instance = new CmpGEOperator();
    return instance;
  };

protected:
  static CmpGEOperator *instance;
};

} // namespace orca
