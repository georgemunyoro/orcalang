#pragma once

#include <string>

#include "../OrcaType.h"

class OrcaCodeGen;

namespace orca {

class Operator {
public:
  virtual ~Operator() = default;

  virtual OrcaType *getResultingType(OrcaType *left, OrcaType *right) = 0;
  virtual OrcaType *getResultingType(OrcaType *operand) = 0;

  std::string toString() const { return opSymbol; };

  static Operator *getInstance();

protected:
  std::string opSymbol;
};

} // namespace orca
