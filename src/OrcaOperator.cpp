#include "OrcaOperator.h"
#include "OrcaType.h"
#include <stdexcept>

// =======================================================================
// =========================== Binary Operators ==========================
// =======================================================================

OrcaAdditionOperator *OrcaAdditionOperator::instance = nullptr;

OrcaType *OrcaAdditionOperator::getResultingType(OrcaType *lhs, OrcaType *rhs) {
  auto lKind = lhs->getKind();
  auto rKind = rhs->getKind();

  if (lKind == OrcaTypeKind::Integer) {
    if (lKind != rKind)
      throw std::string("Cannot add integer to non-integer type");

    auto lInt = lhs->getIntegerType();
    auto rInt = rhs->getIntegerType();

    if (lInt.getIsSigned() != rInt.getIsSigned())
      throw std::string("Cannot add integers of different signedness");

    if (lInt.getBits() != rInt.getBits())
      throw std::string("Cannot add integers of different sizes");

    return lhs;
  }

  throw std::string("Cannot add non-integer types");
}

llvm::Value *
OrcaAdditionOperator::codegen(std::unique_ptr<llvm::IRBuilder<>> &builder,
                              llvm::Value *lhs, llvm::Value *rhs) {
  return builder->CreateAdd(lhs, rhs);
}

OrcaSubtractionOperator *OrcaSubtractionOperator::instance = nullptr;

OrcaType *OrcaSubtractionOperator::getResultingType(OrcaType *lhs,
                                                    OrcaType *rhs) {
  auto lKind = lhs->getKind();
  auto rKind = rhs->getKind();

  if (lKind == OrcaTypeKind::Integer) {
    if (lKind != rKind)
      throw std::string("Cannot subtract integer from non-integer type");

    auto lInt = lhs->getIntegerType();
    auto rInt = rhs->getIntegerType();

    if (lInt.getIsSigned() != rInt.getIsSigned())
      throw std::string("Cannot subtract integers of different signedness");

    if (lInt.getBits() != rInt.getBits())
      throw std::string("Cannot subtract integers of different sizes");

    return lhs;
  }

  throw std::string("Cannot subtract non-integer types");
}

llvm::Value *
OrcaSubtractionOperator::codegen(std::unique_ptr<llvm::IRBuilder<>> &builder,
                                 llvm::Value *lhs, llvm::Value *rhs) {
  return builder->CreateSub(lhs, rhs);
}

OrcaMultiplicationOperator *OrcaMultiplicationOperator::instance = nullptr;

OrcaType *OrcaMultiplicationOperator::getResultingType(OrcaType *lhs,
                                                       OrcaType *rhs) {
  auto lKind = lhs->getKind();
  auto rKind = rhs->getKind();

  if (lKind == OrcaTypeKind::Integer) {
    if (lKind != rKind)
      throw std::string("Cannot multiply integer with non-integer type");

    auto lInt = lhs->getIntegerType();
    auto rInt = rhs->getIntegerType();

    if (lInt.getIsSigned() != rInt.getIsSigned())
      throw std::string("Cannot multiply integers of different signedness");

    if (lInt.getBits() != rInt.getBits())
      throw std::string("Cannot multiply integers of different sizes");

    return lhs;
  }

  throw std::string("Cannot multiply non-integer types");
}

llvm::Value *
OrcaMultiplicationOperator::codegen(std::unique_ptr<llvm::IRBuilder<>> &builder,
                                    llvm::Value *lhs, llvm::Value *rhs) {
  return builder->CreateMul(lhs, rhs);
}

OrcaDivisionOperator *OrcaDivisionOperator::instance = nullptr;

OrcaType *OrcaDivisionOperator::getResultingType(OrcaType *lhs, OrcaType *rhs) {
  auto lKind = lhs->getKind();
  auto rKind = rhs->getKind();

  if (lKind == OrcaTypeKind::Integer) {
    if (lKind != rKind)
      throw std::string("Cannot divide integer by non-integer type");

    auto lInt = lhs->getIntegerType();
    auto rInt = rhs->getIntegerType();

    if (lInt.getIsSigned() != rInt.getIsSigned())
      throw std::string("Cannot divide integers of different signedness");

    if (lInt.getBits() != rInt.getBits())
      throw std::string("Cannot divide integers of different sizes");

    return lhs;
  }

  if (lKind == OrcaTypeKind::Float) {
    if (lKind != rKind)
      throw std::string("Cannot divide float by non-float type");

    auto lFloat = lhs->getFloatType();
    auto rFloat = rhs->getFloatType();

    if (lFloat.getBits() != rFloat.getBits())
      throw std::string("Cannot divide floats of different sizes");

    return lhs;
  }

  throw std::string("Cannot divide non-numeric types");
}

llvm::Value *
OrcaDivisionOperator::codegen(std::unique_ptr<llvm::IRBuilder<>> &builder,
                              llvm::Value *lhs, llvm::Value *rhs) {
  return builder->CreateSDiv(lhs, rhs);
}

OrcaLogicalAndOperator *OrcaLogicalAndOperator::instance = nullptr;

OrcaType *OrcaLogicalAndOperator::getResultingType(OrcaType *lhs,
                                                   OrcaType *rhs) {
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

    return lhs;
  }

  if (lKind == OrcaTypeKind::Boolean) {
    if (lKind != rKind)
      throw std::string("Cannot logical and boolean with non-boolean type");

    return lhs;
  }

  throw std::string("Cannot logical and non-boolean types");
}

llvm::Value *
OrcaLogicalAndOperator::codegen(std::unique_ptr<llvm::IRBuilder<>> &builder,
                                llvm::Value *lhs, llvm::Value *rhs) {
  // In order to implement short-circuiting, we actually use a select
  // instruction instead of a logical and instruction. This is because the
  // select instruction will only evaluate the second operand if the first
  // operand is true.
  //
  // You can think of the select instruction as a ternary operator. e.g.
  // foo && bar is equivalent to foo ? bar : false

  return builder->CreateSelect(lhs, rhs, builder->getInt1(false));
}

// ======================================================================
// =========================== Unary Operators ==========================
// ======================================================================

OrcaBitwiseNotOperator *OrcaBitwiseNotOperator::instance = nullptr;

OrcaType *OrcaBitwiseNotOperator::getResultingType(OrcaType *operandType) {
  if (operandType->getKind() != OrcaTypeKind::Integer)
    throw std::string("bitwise not operator can only be applied to integers");

  return operandType;
}

llvm::Value *
OrcaBitwiseNotOperator::codegen(std::unique_ptr<llvm::IRBuilder<>> &builder,
                                llvm::Value *operand) {
  return builder->CreateNot(operand);
}

OrcaNegOperator *OrcaNegOperator::instance = nullptr;

OrcaType *OrcaNegOperator::getResultingType(OrcaType *operandType) {
  if (operandType->getKind() != OrcaTypeKind::Integer)
    throw std::string("neg operator can only be applied to integers");

  // Negating a value will always result in a signed value
  return new OrcaType(
      OrcaIntegerType(true, operandType->getIntegerType().getBits()));
}

llvm::Value *
OrcaNegOperator::codegen(std::unique_ptr<llvm::IRBuilder<>> &builder,
                         llvm::Value *operand) {
  return builder->CreateNeg(operand);
}

OrcaNotOperator *OrcaNotOperator::instance = nullptr;

OrcaType *OrcaNotOperator::getResultingType(OrcaType *operandType) {
  auto kind = operandType->getKind();

  if (kind == OrcaTypeKind::Array || kind == OrcaTypeKind::Function ||
      kind == OrcaTypeKind::Void || kind == OrcaTypeKind::Struct)
    throw std::string(
        "not operator cannot be applied to array, function, void or struct");

  return operandType;
}

llvm::Value *
OrcaNotOperator::codegen(std::unique_ptr<llvm::IRBuilder<>> &builder,
                         llvm::Value *operand) {

  // Determine if the operand is non-zero
  llvm::Value *zero = llvm::ConstantInt::get(operand->getType(), 0);
  llvm::Value *result = builder->CreateICmpEQ(operand, zero);

  // Convert the result back to the original type
  return builder->CreateZExt(result, operand->getType());
}
