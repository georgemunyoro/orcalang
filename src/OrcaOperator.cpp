#include "OrcaOperator.h"
#include "OrcaType.h"

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
