#include "OrcaError.h"
#include "OrcaContext.h"

#include "./utils/printfColors.h"

void OrcaError::print() const {
  printf("%serror%s: %s\n", KRED, KNRM, message.c_str());
  printf("--> %s [%zu:%zu]\n", context.getEntryFilepath().c_str(), line,
         charPositionInLine);
  printf("     | \n");
  printf(" %3d | %s", (int)line, context.getSourceLine(line).data());
  printf("     | %*s\n\n", (int)charPositionInLine + 1, "^");
}
