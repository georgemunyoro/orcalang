#pragma once

#include "../Operator.h"
#include "Binary.h"

namespace orca {

/**
 * @brief OrcaDivisionOperator is used to represent the division operator in
 * the Orca language.
 */
class DivOperator : public BinaryOperator {
public:
  DivOperator() {
    opSymbol = "/";
    instance = this;
  }
  ~DivOperator() = default;

  OrcaType *getResultingType(OrcaType *left, OrcaType *right) override;

  llvm::Value *codegen(OrcaCodeGen &cg, llvm::Value *lhs,
                       llvm::Value *rhs) override;

  /**
   * @brief Get the instance of the OrcaDivisionOperator.
   */
  static DivOperator *getInstance() {
    if (instance == nullptr)
      instance = new DivOperator();
    return instance;
  };

protected:
  static DivOperator *instance;
};

} // namespace orca
