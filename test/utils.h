#pragma once

#include <string>

#include "../src/OrcaContext.h"

std::string replaceString(std::string &subject, const std::string &search,
                          const std::string &replace);

// Remove colors from string
std::string cleanString(std::string s);

typedef struct {
  std::string astStringWithoutTypes;
  std::string astStringWithTypes;
  std::string mainFunctionLLVMIr;
} RunResult;

RunResult runAndGetAstStrings(const std::string &filename);
