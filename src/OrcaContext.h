
#include "OrcaLexerErrorListener.h"
#include <ANTLRInputStream.h>
#include <CommonTokenStream.h>
#include <fstream>
#include <string_view>

#include "OrcaLexer.h"

using namespace antlr4;
using namespace orcagrammar;

class OrcaContext {
public:
  OrcaContext(const std::string &entryFilepath) : entryFilepath(entryFilepath) {
    fileStream = openFile(entryFilepath);
    readSourceCode();
    inputStream = new ANTLRInputStream(source_code);
    lexer = new OrcaLexer(inputStream);
    lexerErrorListener = new OrcaLexerErrorListener(this);
    lexer->removeErrorListeners();
    lexer->addErrorListener(lexerErrorListener);
    tokenStream = new CommonTokenStream(lexer);
  };
  ~OrcaContext();

  // Runs the lexer, populating the token stream, any lexical
  // syntax errors will be caught by the lexer error listener
  void lex();

  // Returns a reference to the source code
  std::string &getSourceCode() { return source_code; }

  // Returns the line of source code at the given line number
  // without making a copy, or an empty string if the line number
  // is out of range
  std::string_view getSourceLine(size_t line);

  // Returns the filepath of the entry file
  std::string &getEntryFilepath() { return entryFilepath; }

private:
  static std::ifstream openFile(const std::string &filepath);
  void readSourceCode();

  std::string entryFilepath;
  std::ifstream fileStream;
  std::string source_code;

  OrcaLexerErrorListener *lexerErrorListener;
  ANTLRInputStream *inputStream;
  CommonTokenStream *tokenStream;
  OrcaLexer *lexer;
};
