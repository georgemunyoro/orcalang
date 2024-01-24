
#include "OrcaContext.h"

using namespace antlr4;
using namespace orcagrammar;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    return 1;
  }

  OrcaContext context(argv[1]);
  context.lex();
  context.parse();
}
