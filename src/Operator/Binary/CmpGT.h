#pragma once

#include "Binary.h"

namespace orca {

class CmpGTOperator : public BinaryOperator {
public:
  CmpGTOperator() {
    opSymbol = ">";
    instance = this;
  }
  ~CmpGTOperator() = default;

  OrcaType *getResultingType(OrcaType *left, OrcaType *right) override;

  llvm::Value *codegen(OrcaCodeGen &cg, OrcaAstExpressionNode *lhs,
                       OrcaAstExpressionNode *rhs) override;

  /**
   * @brief Get the instance of the OrcaGTOperator.
   */
  static CmpGTOperator *getInstance() {
    if (instance == nullptr)
      instance = new CmpGTOperator();
    return instance;
  };

protected:
  static CmpGTOperator *instance;
};

} // namespace orca
