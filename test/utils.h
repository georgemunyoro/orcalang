#pragma once

#include <string>

#include "../src/OrcaContext.h"
#include "../src/utils/printfColors.h"

std::string replaceString(std::string &subject, const std::string &search,
                          const std::string &replace) {
  size_t pos = 0;
  while ((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
  }
  return subject;
}

// Remove colors from string
std::string cleanString(std::string s) {
  s = replaceString(s, KNRM, "");
  s = replaceString(s, KRED, "");
  s = replaceString(s, KGRN, "");
  s = replaceString(s, KYEL, "");
  s = replaceString(s, KBLU, "");
  s = replaceString(s, KMAG, "");
  s = replaceString(s, KCYN, "");
  s = replaceString(s, KWHT, "");
  s = replaceString(s, KPRP, "");

  return s;
}

std::pair<std::string, std::string>
runAndGetAstStrings(const std::string &filename) {
  OrcaContext ctx(filename);

  ctx.lex();
  ctx.parse();
  ctx.buildAst();
  ctx.evaluateTypes();

  std::string withoutTypes = ctx.getAst()->toString(0);
  withoutTypes = cleanString(withoutTypes);

  std::string withTypes = ctx.getAst()->toString(0);
  withTypes = cleanString(withTypes);

  return {withoutTypes, withTypes};
}
