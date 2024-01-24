#include <fstream>
#include <string>
#include <string_view>

#include "OrcaContext.h"
#include "OrcaParser.h"

std::ifstream OrcaContext::openFile(const std::string &filepath) {
  std::ifstream stream;

  stream.open(filepath);
  if (!stream.is_open()) {
    std::cerr << "Could not open file: " << filepath << std::endl;
    exit(1);
  }

  return stream;
}

OrcaContext::~OrcaContext() {
  delete lexerErrorListener;
  delete inputStream;
  delete tokenStream;
  delete lexer;
}

void OrcaContext::lex() { tokenStream->fill(); }

void OrcaContext::parse() {
  parser = new OrcaParser(tokenStream);
  parserErrorListener = new OrcaParserErrorListener(*this);
  parser->removeErrorListeners();
  parser->addErrorListener(parserErrorListener);
  programContext = parser->program();
}

void OrcaContext::buildAst() {
  astBuilder = new OrcaAstBuilder(*this);
  auto ast = astBuilder->build();
  ast->print(0);
}

void OrcaContext::readSourceCode() {
  std::string line;
  while (std::getline(fileStream, line)) {
    source_code += line + "\n";
  }
}

const std::string_view OrcaContext::getSourceLine(size_t line) const {
  size_t lineStart = 0;
  size_t currentLine = 1;

  while (currentLine < line && lineStart < source_code.length()) {
    lineStart = source_code.find('\n', lineStart) + 1;
    if (lineStart == std::string::npos) {
      // Line number out of range
      return std::string_view();
    }
    ++currentLine;
  }

  // Find the end of the line
  size_t lineEnd = source_code.find('\n', lineStart);
  if (lineEnd == std::string::npos) {
    lineEnd = source_code.length();
  }

  return std::string_view(source_code).substr(lineStart, lineEnd - lineStart);
}
