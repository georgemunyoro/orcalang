#pragma once

#include "Binary.h"

namespace orca {

class AddOperator : public BinaryOperator {
public:
  AddOperator() {
    opSymbol = "+";
    instance = this;
  }
  ~AddOperator() = default;

  OrcaType *getResultingType(OrcaType *left, OrcaType *right) override;

  llvm::Value *codegen(OrcaCodeGen &cg, llvm::Value *lhs,
                       llvm::Value *rhs) override;

  /**
   * @brief Get the instance of the OrcaAdditionOperator.
   */
  static AddOperator *getInstance() {
    if (instance == nullptr)
      instance = new AddOperator();
    return instance;
  };

protected:
  static AddOperator *instance;
};

} // namespace orca
