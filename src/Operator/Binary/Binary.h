#pragma once

#include "../Operator.h"
#include "llvm/IR/Value.h"

class OrcaAstExpressionNode;

namespace orca {

class BinaryOperator : public Operator {
public:
  virtual ~BinaryOperator() = default;

  virtual OrcaType *getResultingType(OrcaType *left,
                                     OrcaType *right) override = 0;

  virtual llvm::Value *codegen(OrcaCodeGen &cg, OrcaAstExpressionNode *lhs,
                               OrcaAstExpressionNode *rhs) = 0;

  /**
   * @brief This will throw an exception, as this is a binary operator.
   */
  OrcaType *getResultingType(OrcaType *operand) override {
    throw std::string("Cannot perform unary operation '" + this->toString() +
                      "' on multiple operands.");
  }
};

} // namespace orca
