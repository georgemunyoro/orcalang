#include "LogicalAnd.h"
#include "../../OrcaCodeGen.h"

namespace orca {

LogicalAndOperator *LogicalAndOperator::instance = nullptr;

OrcaType *LogicalAndOperator::getResultingType(OrcaType *lhs, OrcaType *rhs) {
  auto lKind = lhs->getKind();
  auto rKind = rhs->getKind();

  if (lKind == OrcaTypeKind::Integer) {
    if (lKind != rKind)
      throw std::string("Cannot logical and integer with non-integer type");

    auto lInt = lhs->getIntegerType();
    auto rInt = rhs->getIntegerType();

    if (lInt.getIsSigned() != rInt.getIsSigned())
      throw std::string("Cannot logical and integers of different signedness");

    if (lInt.getBits() != rInt.getBits())
      throw std::string("Cannot logical and integers of different sizes");

    return new OrcaType(OrcaBooleanType());
  }

  if (lKind == OrcaTypeKind::Boolean) {
    if (lKind != rKind)
      throw std::string("Cannot logical and boolean with non-boolean type");

    return new OrcaType(OrcaBooleanType());
  }

  throw std::string("Cannot logical and non-boolean types");
}

llvm::Value *LogicalAndOperator::codegen(OrcaCodeGen &cg, llvm::Value *lhs,
                                         llvm::Value *rhs) {
  // In order to implement short-circuiting, we actually use a select
  // instruction instead of a logical and instruction. This is because the
  // select instruction will only evaluate the second operand if the first
  // operand is true.
  //
  // You can think of the select instruction as a ternary operator. e.g.
  // foo && bar is equivalent to foo ? bar : false

  return cg.builder->CreateSelect(lhs, rhs, cg.builder->getInt1(false));
}

} // namespace orca
