#pragma once

#include "../Operator.h"
#include "Binary.h"

namespace orca {

class MulOperator : public BinaryOperator {
public:
  MulOperator() {
    opSymbol = "*";
    instance = this;
  }
  ~MulOperator() = default;

  OrcaType *getResultingType(OrcaType *left, OrcaType *right) override;

  llvm::Value *codegen(OrcaCodeGen &cg, llvm::Value *lhs,
                       llvm::Value *rhs) override;

  /**
   * @brief Get the instance of the OrcaMultiplicationOperator.
   */
  static MulOperator *getInstance() {
    if (instance == nullptr)
      instance = new MulOperator();
    return instance;
  };

protected:
  static MulOperator *instance;
};

} // namespace orca
