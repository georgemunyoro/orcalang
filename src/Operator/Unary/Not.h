#pragma once

#include "Unary.h"

namespace orca {

/**
 * @brief OrcaNotOperator is used to represent the not operator in the Orca
 */
class NotOperator : public UnaryOperator {
public:
  NotOperator() {
    opSymbol = "!";
    instance = this;
  }
  ~NotOperator() = default;

  OrcaType *getResultingType(OrcaType *operand) override;

  llvm::Value *codegen(OrcaCodeGen &cg, llvm::Value *operand) override;

  /**
   * @brief Get the instance of the OrcaNotOperator.
   */
  static NotOperator *getInstance() {
    if (instance == nullptr)
      instance = new NotOperator();
    return instance;
  };

protected:
  static NotOperator *instance;
};

} // namespace orca
