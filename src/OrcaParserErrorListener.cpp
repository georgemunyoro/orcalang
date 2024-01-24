#include <cstdio>

#include "OrcaContext.h"
#include "OrcaError.h"
#include "OrcaParserErrorListener.h"

void OrcaParserErrorListener::syntaxError(Recognizer *recognizer,
                                          Token *offendingSymbol, size_t line,
                                          size_t charPositionInLine,
                                          const std::string &msg,
                                          std::exception_ptr e) {
  OrcaError err(context, msg, line, charPositionInLine);
  err.print();
}
