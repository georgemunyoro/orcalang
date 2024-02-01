#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

class OrcaType;

enum class OrcaTypeKind {
  Integer,
  Float,
  Pointer,
  Array,
  Boolean,
  Char,
  Struct,
  Void,
  Function,
};

class OrcaIntegerType {
public:
  OrcaIntegerType(bool isSigned, int bits) : isSigned(isSigned), bits(bits) {}

  OrcaTypeKind getKind() const { return OrcaTypeKind::Integer; }

  std::string toString() const {
    return (isSigned ? std::string("s") : std::string("u")) +
           std::to_string(bits);
  }

  int getBits() const { return bits; }
  bool getIsSigned() const { return isSigned; }

private:
  bool isSigned;
  int bits;

  friend class OrcaType;
};

class OrcaFloatType {
public:
  OrcaFloatType(int bits) : bits(bits) {}

  OrcaTypeKind getKind() const { return OrcaTypeKind::Float; }

  std::string toString() const {
    return std::string("f") + std::to_string(bits);
  }

  int getBits() const { return bits; }

private:
  int bits;

  friend class OrcaType;
};

class OrcaPointerType {
public:
  OrcaPointerType(OrcaType *pointee) : pointee(pointee) {}

  OrcaTypeKind getKind() const { return OrcaTypeKind::Pointer; }

private:
  OrcaType *pointee;

  friend class OrcaType;
};

class OrcaArrayType {
public:
  OrcaArrayType(OrcaType *elementType, size_t length)
      : elementType(elementType), length(length) {}

  OrcaTypeKind getKind() const { return OrcaTypeKind::Array; }

private:
  OrcaType *elementType;
  size_t length;

  friend class OrcaType;
};

class OrcaBooleanType {
public:
  OrcaBooleanType() {}

  OrcaTypeKind getKind() const { return OrcaTypeKind::Boolean; }
};

class OrcaCharType {
public:
  OrcaCharType() {}

  OrcaTypeKind getKind() const { return OrcaTypeKind::Char; }
};

class OrcaStructType {
public:
  OrcaStructType(std::vector<std::pair<std::string, OrcaType *>> fields)
      : fields(fields) {}

  OrcaTypeKind getKind() const { return OrcaTypeKind::Struct; }

private:
  std::vector<std::pair<std::string, OrcaType *>> fields;

  friend class OrcaType;
};

class OrcaVoidType {
public:
  OrcaVoidType() {}

  OrcaTypeKind getKind() const { return OrcaTypeKind::Void; }
};

class OrcaFunctionType {
public:
  OrcaFunctionType(OrcaType *returnType,
                   std::vector<std::pair<std::string, OrcaType *>> parameters)
      : returnType(returnType), parameters(parameters) {}

  OrcaTypeKind getKind() const { return OrcaTypeKind::Function; }

  OrcaType *getReturnType() const { return returnType; }

  std::vector<OrcaType *> getParameterTypes() const {
    std::vector<OrcaType *> parameterTypes;
    for (auto &parameter : parameters) {
      parameterTypes.push_back(parameter.second);
    }
    return parameterTypes;
  }

private:
  OrcaType *returnType;
  std::vector<std::pair<std::string, OrcaType *>> parameters;

  friend class OrcaType;
};

class OrcaType {
public:
  OrcaType() {}
  ~OrcaType() {}

  OrcaType(OrcaIntegerType integerType)
      : integerType(integerType), kind(OrcaTypeKind::Integer) {}

  OrcaType(OrcaFloatType floatType)
      : floatType(floatType), kind(OrcaTypeKind::Float) {}

  OrcaType(OrcaPointerType pointerType)
      : pointerType(pointerType), kind(OrcaTypeKind::Pointer) {}

  OrcaType(OrcaArrayType arrayType)
      : arrayType(arrayType), kind(OrcaTypeKind::Array) {}

  OrcaType(OrcaBooleanType booleanType)
      : booleanType(booleanType), kind(OrcaTypeKind::Boolean) {}

  OrcaType(OrcaCharType charType)
      : charType(charType), kind(OrcaTypeKind::Char) {}

  OrcaType(OrcaStructType structType)
      : structType(structType), kind(OrcaTypeKind::Struct) {}

  OrcaType(OrcaVoidType voidType)
      : voidType(voidType), kind(OrcaTypeKind::Void) {}

  OrcaType(OrcaFunctionType functionType)
      : functionType(functionType), kind(OrcaTypeKind::Function) {}

  static OrcaType Void() { return OrcaType(OrcaVoidType()); }
  static OrcaType Boolean() { return OrcaType(OrcaBooleanType()); }
  static OrcaType Char() { return OrcaType(OrcaCharType()); }
  static OrcaType Integer(bool isSigned, int bits) {
    return OrcaType(OrcaIntegerType(isSigned, bits));
  }
  static OrcaType Float(int bits) { return OrcaType(OrcaFloatType(bits)); }
  static OrcaType Pointer(OrcaType *pointee) {
    return OrcaType(OrcaPointerType(pointee));
  }
  static OrcaType Array(OrcaType *elementType, size_t length) {
    return OrcaType(OrcaArrayType(elementType, length));
  }
  static OrcaType
  Struct(std::vector<std::pair<std::string, OrcaType *>> fields) {
    return OrcaType(OrcaStructType(fields));
  }

  OrcaTypeKind getKind() const { return kind; }

  size_t sizeOf() const {
    switch (kind) {
    case OrcaTypeKind::Integer:
      return integerType.bits / 8;
    case OrcaTypeKind::Float:
      return floatType.bits / 8;
    case OrcaTypeKind::Pointer:
      return 8; // For now, assume an address size of 64-bit
    case OrcaTypeKind::Array:
      return arrayType.elementType->sizeOf() * arrayType.length;
    case OrcaTypeKind::Boolean:
      return 1;
    case OrcaTypeKind::Char:
      return 1;
    case OrcaTypeKind::Struct: {
      size_t s_size = 0;
      for (auto f : structType.fields)
        s_size += f.second->sizeOf();

      return s_size;
    }
    case OrcaTypeKind::Void:
      return 0;
    case OrcaTypeKind::Function:
      return 0;
    }
  }

  std::string toString() const {
    switch (kind) {
    case OrcaTypeKind::Integer:
      return this->integerType.toString();
    case OrcaTypeKind::Float:
      return "float";
    case OrcaTypeKind::Pointer:
      return pointerType.pointee->toString() + "*";
    case OrcaTypeKind::Array:
      return arrayType.elementType->toString() + "[" +
             std::to_string(arrayType.length) + "]";
    case OrcaTypeKind::Boolean:
      return "bool";
    case OrcaTypeKind::Char:
      return "char";
    case OrcaTypeKind::Struct:
      return "struct";
    case OrcaTypeKind::Void:
      return "void";
    case OrcaTypeKind::Function: {
      std::string paramsStr = "(";
      for (size_t i = 0; i < functionType.parameters.size(); ++i) {
        paramsStr += functionType.parameters.at(i).second->toString();
        if (i != functionType.parameters.size() - 1) {
          paramsStr += ", ";
        }
      }
      return paramsStr + ") -> " + functionType.returnType->toString();
    }
    }
  }

  OrcaFunctionType getFunctionType() const {
    if (kind == OrcaTypeKind::Function) {
      return functionType;
    } else {
      throw std::runtime_error("Type is not a function type.");
    }
  }

  OrcaIntegerType getIntegerType() const {
    if (kind == OrcaTypeKind::Integer) {
      return integerType;
    } else {
      throw std::runtime_error("Type is not an integer type.");
    }
  }

  OrcaFloatType getFloatType() const {
    if (kind == OrcaTypeKind::Float) {
      return floatType;
    } else {
      throw std::runtime_error("Type is not a float type.");
    }
  }

  OrcaPointerType getPointerType() const {
    if (kind == OrcaTypeKind::Pointer) {
      return pointerType;
    } else {
      throw std::runtime_error("Type is not a pointer type.");
    }
  }

  OrcaArrayType getArrayType() const {
    if (kind == OrcaTypeKind::Array) {
      return arrayType;
    } else {
      throw std::runtime_error("Type is not an array type.");
    }
  }

  OrcaStructType getStructType() const {
    if (kind == OrcaTypeKind::Struct) {
      return structType;
    } else {
      throw std::runtime_error("Type is not a struct type.");
    }
  }

  OrcaBooleanType getBooleanType() const {
    if (kind == OrcaTypeKind::Boolean) {
      return booleanType;
    } else {
      throw std::runtime_error("Type is not a boolean type.");
    }
  }

  OrcaCharType getCharType() const {
    if (kind == OrcaTypeKind::Char) {
      return charType;
    } else {
      throw std::runtime_error("Type is not a char type.");
    }
  }

  OrcaVoidType getVoidType() const {
    if (kind == OrcaTypeKind::Void) {
      return voidType;
    } else {
      throw std::runtime_error("Type is not a void type.");
    }
  }

  bool isEqual(OrcaType *other) {
    if (kind != other->kind) {
      return false;
    }

    switch (kind) {
    case OrcaTypeKind::Integer:
      return integerType.isSigned == other->integerType.isSigned &&
             integerType.bits == other->integerType.bits;
    case OrcaTypeKind::Float:
      return floatType.bits == other->floatType.bits;
    case OrcaTypeKind::Pointer:
      return pointerType.pointee->isEqual(other->pointerType.pointee);
    case OrcaTypeKind::Array:
      return arrayType.elementType->isEqual(other->arrayType.elementType) &&
             arrayType.length == other->arrayType.length;
    case OrcaTypeKind::Boolean:
      return true;
    case OrcaTypeKind::Char:
      return true;
    case OrcaTypeKind::Struct: {
      if (structType.fields.size() != other->structType.fields.size()) {
        return false;
      }

      for (size_t i = 0; i < structType.fields.size(); ++i) {
        if (structType.fields.at(i).first !=
            other->structType.fields.at(i).first) {
          return false;
        }

        if (!structType.fields.at(i).second->isEqual(
                other->structType.fields.at(i).second)) {
          return false;
        }
      }

      return true;
    }
    case OrcaTypeKind::Void:
      return true;
    case OrcaTypeKind::Function: {
      if (!functionType.returnType->isEqual(other->functionType.returnType)) {
        return false;
      }

      if (functionType.parameters.size() !=
          other->functionType.parameters.size()) {
        return false;
      }

      for (size_t i = 0; i < functionType.parameters.size(); ++i) {
        if (functionType.parameters.at(i).first !=
            other->functionType.parameters.at(i).first) {
          return false;
        }

        if (!functionType.parameters.at(i).second->isEqual(
                other->functionType.parameters.at(i).second)) {
          return false;
        }
      }

      return true;
    }
    }
  }

  bool is(OrcaTypeKind kind) { return this->kind == kind; }

  static OrcaIntegerType u8;
  static OrcaIntegerType u16;
  static OrcaIntegerType u32;
  static OrcaIntegerType u64;

  static OrcaIntegerType s8;
  static OrcaIntegerType s16;
  static OrcaIntegerType s32;
  static OrcaIntegerType s64;

  static OrcaFloatType f32;
  static OrcaFloatType f64;

  static OrcaBooleanType boolean;
  static OrcaCharType _char;
  static OrcaVoidType _void;

private:
  OrcaTypeKind kind;

  union {
    OrcaIntegerType integerType;
    OrcaFloatType floatType;
    OrcaPointerType pointerType;
    OrcaArrayType arrayType;
    OrcaBooleanType booleanType;
    OrcaCharType charType;
    OrcaStructType structType;
    OrcaVoidType voidType;
    OrcaFunctionType functionType;
  };
};
