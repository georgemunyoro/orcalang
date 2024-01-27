#include "utils.h"
#include "../src/utils/printfColors.h"

RunResult runAndGetAstStrings(const std::string &filename) {
  RunResult result;

  OrcaContext *ctx = new OrcaContext(filename);

  ctx->lex();
  ctx->parse();
  ctx->buildAst();

  std::string withoutTypes = ctx->getAst()->toString(0);
  result.astStringWithoutTypes = cleanString(withoutTypes);

  ctx->evaluateTypes();

  std::string withTypes = ctx->getAst()->toString(0);
  result.astStringWithTypes = cleanString(withTypes);

  ctx->codegen();

  const auto mainFuncStream =
      new llvm::raw_string_ostream(result.mainFunctionLLVMIr);
  ctx->getCodeGenerator().module->getFunction("main")->print(*mainFuncStream);

  return result;
}

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
std::string replaceString(std::string &subject, const std::string &search,
                          const std::string &replace) {
  size_t pos = 0;
  while ((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
  }
  return subject;
}
