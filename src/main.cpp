
#include <ANTLRInputStream.h>
#include <fstream>
#include <iostream>

#include "OrcaLexer.h"
#include "OrcaParser.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    return 1;
  }

  std::ifstream stream;
  stream.open(argv[1]);

  antlr4::ANTLRInputStream input(stream);

  return 0;
}
