#pragma once

#include "Binary.h"

namespace orca {

class LogicalOrOperator : public BinaryOperator {
public:
  LogicalOrOperator() {
    opSymbol = "&&";
    instance = this;
  }
  ~LogicalOrOperator() = default;

  OrcaType *getResultingType(OrcaType *left, OrcaType *right) override;

  llvm::Value *codegen(OrcaCodeGen &cg, OrcaAstExpressionNode *lhs,
                       OrcaAstExpressionNode *rhs) override;

  /**
   * @brief Get the instance of the OrcaLogicalOrOperator.
   */
  static LogicalOrOperator *getInstance() {
    if (instance == nullptr)
      instance = new LogicalOrOperator();
    return instance;
  };

protected:
  static LogicalOrOperator *instance;
};

} // namespace orca
