#pragma once

#include "Unary.h"

namespace orca {

/**
 * @brief OrcaAddrOfOperator is used to represent the not operator in the Orca
 */
class AddrOfOperator : public UnaryOperator {
public:
  AddrOfOperator() {
    opSymbol = "!";
    instance = this;
  }
  ~AddrOfOperator() = default;

  OrcaType *getResultingType(OrcaType *operand) override;

  llvm::Value *codegen(OrcaCodeGen &cg, llvm::Value *operand) override;

  /**
   * @brief Get the instance of the OrcaAddrOfOperator.
   */
  static AddrOfOperator *getInstance() {
    if (instance == nullptr)
      instance = new AddrOfOperator();
    return instance;
  };

protected:
  static AddrOfOperator *instance;
};

} // namespace orca
