#pragma once

#include <string>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

#include "OrcaType.h"

/**
 * @brief OrcaOperator is used to represent the operators in the Orca
 * language.
 */
class OrcaOperator {
public:
  virtual ~OrcaOperator() = default;

  virtual OrcaType *getResultingType(OrcaType *left, OrcaType *right) = 0;
  virtual OrcaType *getResultingType(OrcaType *operand) = 0;

  std::string toString() const { return opSymbol; };

  static OrcaOperator *getInstance();

protected:
  std::string opSymbol;
};

/**
 * @brief OrcaBinaryOperator is used to represent the binary operators in the
 * Orca language.
 */
class OrcaBinaryOperator : public OrcaOperator {
public:
  virtual ~OrcaBinaryOperator() = default;

  virtual OrcaType *getResultingType(OrcaType *left,
                                     OrcaType *right) override = 0;

  virtual llvm::Value *codegen(std::unique_ptr<llvm::IRBuilder<>> &builder,
                               llvm::Value *lhs, llvm::Value *rhs) = 0;

  /**
   * @brief This will throw an exception, as this is a binary operator.
   */
  OrcaType *getResultingType(OrcaType *operand) override {
    throw std::string("Cannot perform unary operation '" + this->toString() +
                      "' on multiple operands.");
  }
};

/**
 * @brief OrcaUnaryOperator is used to represent the unary operators in the
 * Orca language.
 */
class OrcaUnaryOperator : public OrcaOperator {
public:
  // OrcaUnaryOperator(std::string opSymbol) : OrcaOperator(opSymbol) {}
  virtual ~OrcaUnaryOperator() = default;

  virtual OrcaType *getResultingType(OrcaType *operand) override = 0;

  virtual llvm::Value *codegen(std::unique_ptr<llvm::IRBuilder<>> &builder,
                               llvm::Value *operand) = 0;

  /**
   * @brief This will throw an exception, as this is a unary operator.
   */
  OrcaType *getResultingType(OrcaType *left, OrcaType *right) override {
    throw std::string("Cannot perform binary operation '" + this->toString() +
                      "' on a single operands.");
  }
};

// =======================================================================
// =========================== Binary Operators ==========================
// =======================================================================

/**
 * @brief OrcaAdditionOperator is used to represent the addition operator in
 * the Orca language.
 */
class OrcaAdditionOperator : public OrcaBinaryOperator {
public:
  OrcaAdditionOperator() {
    opSymbol = "+";
    instance = this;
  }
  ~OrcaAdditionOperator() = default;

  OrcaType *getResultingType(OrcaType *left, OrcaType *right) override;

  llvm::Value *codegen(std::unique_ptr<llvm::IRBuilder<>> &builder,
                       llvm::Value *lhs, llvm::Value *rhs) override;

  /**
   * @brief Get the instance of the OrcaAdditionOperator.
   */
  static OrcaAdditionOperator *getInstance() {
    if (instance == nullptr)
      instance = new OrcaAdditionOperator();
    return instance;
  };

protected:
  static OrcaAdditionOperator *instance;
};

/**
 * @brief OrcaSubtractionOperator is used to represent the subtraction operator
 * in the Orca language.
 */
class OrcaSubtractionOperator : public OrcaBinaryOperator {
public:
  OrcaSubtractionOperator() {
    opSymbol = "-";
    instance = this;
  }
  ~OrcaSubtractionOperator() = default;

  OrcaType *getResultingType(OrcaType *left, OrcaType *right) override;

  llvm::Value *codegen(std::unique_ptr<llvm::IRBuilder<>> &builder,
                       llvm::Value *lhs, llvm::Value *rhs) override;

  /**
   * @brief Get the instance of the OrcaSubtractionOperator.
   */
  static OrcaSubtractionOperator *getInstance() {
    if (instance == nullptr)
      instance = new OrcaSubtractionOperator();
    return instance;
  };

protected:
  static OrcaSubtractionOperator *instance;
};

/**
 * @brief OrcaMultiplicationOperator is used to represent the multiplication
 * operator in the Orca language.
 */
class OrcaMultiplicationOperator : public OrcaBinaryOperator {
public:
  OrcaMultiplicationOperator() {
    opSymbol = "*";
    instance = this;
  }
  ~OrcaMultiplicationOperator() = default;

  OrcaType *getResultingType(OrcaType *left, OrcaType *right) override;

  llvm::Value *codegen(std::unique_ptr<llvm::IRBuilder<>> &builder,
                       llvm::Value *lhs, llvm::Value *rhs) override;

  /**
   * @brief Get the instance of the OrcaMultiplicationOperator.
   */
  static OrcaMultiplicationOperator *getInstance() {
    if (instance == nullptr)
      instance = new OrcaMultiplicationOperator();
    return instance;
  };

protected:
  static OrcaMultiplicationOperator *instance;
};

/**
 * @brief OrcaDivisionOperator is used to represent the division operator in
 * the Orca language.
 */
class OrcaDivisionOperator : public OrcaBinaryOperator {
public:
  OrcaDivisionOperator() {
    opSymbol = "/";
    instance = this;
  }
  ~OrcaDivisionOperator() = default;

  OrcaType *getResultingType(OrcaType *left, OrcaType *right) override;

  llvm::Value *codegen(std::unique_ptr<llvm::IRBuilder<>> &builder,
                       llvm::Value *lhs, llvm::Value *rhs) override;

  /**
   * @brief Get the instance of the OrcaDivisionOperator.
   */
  static OrcaDivisionOperator *getInstance() {
    if (instance == nullptr)
      instance = new OrcaDivisionOperator();
    return instance;
  };

protected:
  static OrcaDivisionOperator *instance;
};

// ======================================================================
// =========================== Unary Operators ==========================
// ======================================================================

/**
 * @brief OrcaNotOperator is used to represent the not operator in the Orca
 */
class OrcaBitwiseNotOperator : public OrcaUnaryOperator {
public:
  OrcaBitwiseNotOperator() {
    opSymbol = "!";
    instance = this;
  }
  ~OrcaBitwiseNotOperator() = default;

  OrcaType *getResultingType(OrcaType *operand) override;

  llvm::Value *codegen(std::unique_ptr<llvm::IRBuilder<>> &builder,
                       llvm::Value *operand) override;

  /**
   * @brief Get the instance of the OrcaNotOperator.
   */
  static OrcaBitwiseNotOperator *getInstance() {
    if (instance == nullptr)
      instance = new OrcaBitwiseNotOperator();
    return instance;
  };

protected:
  static OrcaBitwiseNotOperator *instance;
};

/**
 * @brief OrcaNegOperator is used to represent the negate operator in the Orca
 */
class OrcaNegOperator : public OrcaUnaryOperator {
public:
  OrcaNegOperator() {
    opSymbol = "-";
    instance = this;
  }
  ~OrcaNegOperator() = default;

  OrcaType *getResultingType(OrcaType *operand) override;

  llvm::Value *codegen(std::unique_ptr<llvm::IRBuilder<>> &builder,
                       llvm::Value *operand) override;

  /**
   * @brief Get the instance of the OrcaNegOperator.
   */
  static OrcaNegOperator *getInstance() {
    if (instance == nullptr)
      instance = new OrcaNegOperator();
    return instance;
  };

protected:
  static OrcaNegOperator *instance;
};

/**
 * @brief OrcaNotOperator is used to represent the not operator in the Orca
 */
class OrcaNotOperator : public OrcaUnaryOperator {
public:
  OrcaNotOperator() {
    opSymbol = "!";
    instance = this;
  }
  ~OrcaNotOperator() = default;

  OrcaType *getResultingType(OrcaType *operand) override;

  llvm::Value *codegen(std::unique_ptr<llvm::IRBuilder<>> &builder,
                       llvm::Value *operand) override;

  /**
   * @brief Get the instance of the OrcaNotOperator.
   */
  static OrcaNotOperator *getInstance() {
    if (instance == nullptr)
      instance = new OrcaNotOperator();
    return instance;
  };

protected:
  static OrcaNotOperator *instance;
};
