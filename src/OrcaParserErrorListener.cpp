#include "OrcaParserErrorListener.h"
#include "OrcaContext.h"
#include <cstdio>

#include "./utils/printfColors.h"

void OrcaParserErrorListener::syntaxError(Recognizer *recognizer,
                                          Token *offendingSymbol, size_t line,
                                          size_t charPositionInLine,
                                          const std::string &msg,
                                          std::exception_ptr e) {

  printf("%serror%s: encountered invalid token '%s'\n", KRED, KNRM,
         offendingSymbol->getText().data());
  printf("--> %s [%zu:%zu]\n", context->getEntryFilepath().c_str(), line,
         charPositionInLine);
  printf("     | \n");
  printf(" %3d | %s", (int)line, context->getSourceLine(line).data());
  printf("     | %*s\n\n", (int)charPositionInLine + 1, "^");
}
