#pragma once

#include "Unary.h"

namespace orca {

/**
 * @brief OrcaNegOperator is used to represent the negate operator in the Orca
 */
class NegOperator : public UnaryOperator {
public:
  NegOperator() {
    opSymbol = "-";
    instance = this;
  }
  ~NegOperator() = default;

  OrcaType *getResultingType(OrcaType *operand) override;

  llvm::Value *codegen(OrcaCodeGen &cg, llvm::Value *operand) override;

  /**
   * @brief Get the instance of the OrcaNegOperator.
   */
  static NegOperator *getInstance() {
    if (instance == nullptr)
      instance = new NegOperator();
    return instance;
  };

protected:
  static NegOperator *instance;
};

} // namespace orca
