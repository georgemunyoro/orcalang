#pragma once

#include "Binary.h"

namespace orca {

class SubOperator : public BinaryOperator {
public:
  SubOperator() {
    opSymbol = "-";
    instance = this;
  }
  ~SubOperator() = default;

  OrcaType *getResultingType(OrcaType *left, OrcaType *right) override;

  llvm::Value *codegen(OrcaCodeGen &cg, llvm::Value *lhs,
                       llvm::Value *rhs) override;

  /**
   * @brief Get the instance of the OrcaSubtractionOperator.
   */
  static SubOperator *getInstance() {
    if (instance == nullptr)
      instance = new SubOperator();
    return instance;
  };

protected:
  static SubOperator *instance;
};

} // namespace orca
