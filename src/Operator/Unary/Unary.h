#pragma once

#include "../Operator.h"
#include "llvm/IR/Value.h"

namespace orca {

class UnaryOperator : public Operator {
public:
  // OrcaUnaryOperator(std::string opSymbol) : OrcaOperator(opSymbol) {}
  virtual ~UnaryOperator() = default;

  virtual OrcaType *getResultingType(OrcaType *operand) override = 0;

  virtual llvm::Value *codegen(OrcaCodeGen &cg, llvm::Value *operand) = 0;

  /**
   * @brief This will throw an exception, as this is a unary operator.
   */
  OrcaType *getResultingType(OrcaType *left, OrcaType *right) override {
    throw std::string("Cannot perform binary operation '" + this->toString() +
                      "' on a single operands.");
  }
};

} // namespace orca
