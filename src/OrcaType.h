#pragma once

#include <cstddef>
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
  Void
};

class OrcaIntegerType {
public:
  OrcaIntegerType(bool isSigned, int bits) : isSigned(isSigned), bits(bits) {}

  OrcaTypeKind getKind() const { return OrcaTypeKind::Integer; }

private:
  bool isSigned;
  int bits;

  friend class OrcaType;
};

class OrcaFloatType {
public:
  OrcaFloatType(int bits) : bits(bits) {}

  OrcaTypeKind getKind() const { return OrcaTypeKind::Float; }

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

  std::string toString() const {
    switch (kind) {
    case OrcaTypeKind::Integer:
      return "integer";
    case OrcaTypeKind::Float:
      return "float";
    case OrcaTypeKind::Pointer:
      return "pointer";
    case OrcaTypeKind::Array:
      return "array";
    case OrcaTypeKind::Boolean:
      return "boolean";
    case OrcaTypeKind::Char:
      return "char";
    case OrcaTypeKind::Struct:
      return "struct";
    case OrcaTypeKind::Void:
      return "void";
    }
  }

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
  };
};
