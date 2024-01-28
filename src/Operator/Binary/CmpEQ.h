#pragma once

#include "Binary.h"

namespace orca {

class CmpEQOperator : public BinaryOperator {
public:
  CmpEQOperator() {
    opSymbol = "==";
    instance = this;
  }
  ~CmpEQOperator() = default;

  OrcaType *getResultingType(OrcaType *left, OrcaType *right) override;

  llvm::Value *codegen(OrcaCodeGen &cg, llvm::Value *lhs,
                       llvm::Value *rhs) override;

  /**
   * @brief Get the instance of the OrcaEQOperator.
   */
  static CmpEQOperator *getInstance() {
    if (instance == nullptr)
      instance = new CmpEQOperator();
    return instance;
  };

protected:
  static CmpEQOperator *instance;
};

} // namespace orca
