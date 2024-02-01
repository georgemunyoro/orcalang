#pragma once

#include "Binary.h"

namespace orca {

class LogicalAndOperator : public BinaryOperator {
public:
  LogicalAndOperator() {
    opSymbol = "&&";
    instance = this;
  }
  ~LogicalAndOperator() = default;

  OrcaType *getResultingType(OrcaType *left, OrcaType *right) override;

  llvm::Value *codegen(OrcaCodeGen &cg, OrcaAstExpressionNode *lhs,
                       OrcaAstExpressionNode *rhs) override;

  /**
   * @brief Get the instance of the OrcaLogicalAndOperator.
   */
  static LogicalAndOperator *getInstance() {
    if (instance == nullptr)
      instance = new LogicalAndOperator();
    return instance;
  };

protected:
  static LogicalAndOperator *instance;
};

} // namespace orca
