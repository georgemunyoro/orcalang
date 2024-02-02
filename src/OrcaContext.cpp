#include <DiagnosticErrorListener.h>
#include <atn/ParserATNSimulator.h>
#include <atn/PredictionMode.h>
#include <fstream>
#include <iostream>
#include <string>

#include "OrcaCodeGen.h"
#include "OrcaContext.h"
#include "OrcaLexerErrorListener.h"
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

  parser->addErrorListener(new DiagnosticErrorListener());
  parser->getInterpreter<atn::ParserATNSimulator>()->setPredictionMode(
      atn::PredictionMode::SLL);

  programContext = parser->program();
}

void OrcaContext::buildAst() {
  astBuilder = new OrcaAstBuilder(*this);
  ast = astBuilder->build();
  // printf("\n");
  // ast->print(0);
  // printf("\n");

  try {
    std::cout << "========== AST built successfully ===========" << std::endl;
    std::cout << ast->toString(0) << std::endl;
  } catch (char *c) {
    std::cout << "========= AST Failed to build ==========" << std::endl;
    std::cout << c << std::endl;
  }
}

void OrcaContext::evaluateTypes() {
  typeChecker = new OrcaTypeChecker(*this);
  typeChecker->run(ast);
  // printf("\n");
  // ast->print(0);
  // printf("\n");

  try {
    std::cout << "========== Type checking completed successfully ==========="
              << std::endl;
    std::cout << ast->toString(0) << std::endl;
  } catch (char *c) {
    std::cout << "========= Type checking failed ==========" << std::endl;
    std::cout << c << std::endl;
  }
}

void OrcaContext::codegen() {
  codeGenerator = new OrcaCodeGen(*this);
  codeGenerator->generateCode(ast);

  try {
    std::cout << "========== Code generation completed successfully ==========="
              << std::endl;
    codeGenerator->module->print(llvm::errs(), nullptr);
  } catch (char *c) {
    std::cout << "========= Code generation failed ==========" << std::endl;
    std::cout << c << std::endl;
  }
}

void OrcaContext::readSourceCode() {
  std::string line;
  while (std::getline(fileStream, line)) {
    source_code += line + "\n";
  }
}

std::vector<std::string> split(std::string s, std::string delimiter) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }

  res.push_back(s.substr(pos_start));
  return res;
}

const std::string OrcaContext::getSourceLine(size_t line) const {
  return split(source_code, "\n")[line - 1];
}
