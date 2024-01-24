#include "TypeChecker.h"
#include "OrcaParser.h"
#include <any>

std::any TypeChecker::visitProgram(OrcaParser::ProgramContext *context) {

  for (auto &statement : context->children) {
    // Print the type of the statement
    std::cout << visit(statement).type().name() << std::endl;
  }

  return std::any();
}
