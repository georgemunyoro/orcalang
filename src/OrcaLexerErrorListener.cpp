#include "OrcaLexerErrorListener.h"
#include "OrcaContext.h"

void OrcaLexerErrorListener::syntaxError(Recognizer *recognizer,
                                         Token *offendingSymbol, size_t line,
                                         size_t charPositionInLine,
                                         const std::string &msg,
                                         std::exception_ptr e) {

  auto sourceLine = context->getSourceLine(line).data();

  printf("error: encountered unrecognized token\n");
  printf("--> %s [%zu:%zu]\n", context->getEntryFilepath().c_str(), line,
         charPositionInLine);
  printf("     | \n");
  printf(" %3d | %s", (int)line, sourceLine);
  printf("     | %*s\n", (int)charPositionInLine, "^");
  printf("     |\n");
}
