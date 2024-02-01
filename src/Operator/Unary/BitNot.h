#pragma once

#include "Unary.h"

namespace orca {

class BitNotOperator : public UnaryOperator {
public:
  BitNotOperator() {
    opSymbol = "!";
    instance = this;
  }
  ~BitNotOperator() = default;

  OrcaType *getResultingType(OrcaType *operand) override;

  llvm::Value *codegen(OrcaCodeGen &cg, llvm::Value *operand) override;

  /**
   * @brief Get the instance of the OrcaNotOperator.
   */
  static BitNotOperator *getInstance() {
    if (instance == nullptr)
      instance = new BitNotOperator();
    return instance;
  };

protected:
  static BitNotOperator *instance;
};

} // namespace orca
