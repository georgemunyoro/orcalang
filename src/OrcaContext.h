
#include "OrcaLexerErrorListener.h"
#include <ANTLRInputStream.h>
#include <CommonTokenStream.h>
#include <fstream>
#include <string_view>

#include "OrcaLexer.h"
#include "OrcaParser.h"
#include "OrcaParserErrorListener.h"
#include "TypeChecker.h"

using namespace antlr4;
using namespace orcagrammar;

class OrcaContext {
public:
  OrcaContext(const std::string &entryFilepath) : entryFilepath(entryFilepath) {
    fileStream = openFile(entryFilepath);
    readSourceCode();
    inputStream = new ANTLRInputStream(source_code);
    lexer = new OrcaLexer(inputStream);
    lexerErrorListener = new OrcaLexerErrorListener(*this);
    lexer->removeErrorListeners();
    lexer->addErrorListener(lexerErrorListener);
    tokenStream = new CommonTokenStream(lexer);
  };
  ~OrcaContext();

  // Runs the lexer, populating the token stream, any lexical
  // syntax errors will be caught by the lexer error listener
  void lex();

  // Runs the parser, any syntax errors will be caught by the
  // default error listener
  void parse();

  // Runs the type checker, any type errors will be caught by
  // the type checker
  void typeCheck();

private:
  const std::string &getSourceCode() const { return source_code; }
  const std::string_view getSourceLine(size_t line) const;
  const std::string &getEntryFilepath() const { return entryFilepath; }

  static std::ifstream openFile(const std::string &filepath);
  void readSourceCode();

  std::string entryFilepath;
  std::ifstream fileStream;
  std::string source_code;

  // Error handling
  OrcaLexerErrorListener *lexerErrorListener;
  OrcaParserErrorListener *parserErrorListener;

  ANTLRInputStream *inputStream;
  CommonTokenStream *tokenStream;
  OrcaLexer *lexer;
  OrcaParser *parser;
  TypeChecker *typeChecker;

  OrcaParser::ProgramContext *programContext;

  friend class OrcaLexerErrorListener;
  friend class OrcaParserErrorListener;
  friend class OrcaError;
};
