
#include "OrcaAst.h"
#include "OrcaLexerErrorListener.h"
#include <ANTLRInputStream.h>
#include <CommonTokenStream.h>
#include <fstream>

#include "OrcaAstBuilder.h"
#include "OrcaCodeGen.h"
#include "OrcaLexer.h"
#include "OrcaParser.h"
#include "OrcaParserErrorListener.h"
#include "OrcaTypeChecker.h"

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

  // Builds the AST, any semantic errors will be caught by the
  // AST builder
  void buildAst();

  void evaluateTypes();

  void codegen();

  OrcaAstProgramNode *getAst() const { return ast; }

  const std::string &getSourceCode() const { return source_code; }

private:
  const std::string getSourceLine(size_t line) const;
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
  OrcaAstBuilder *astBuilder;
  OrcaTypeChecker *typeChecker;
  OrcaCodeGen *codeGenerator;

  OrcaParser::ProgramContext *programContext;

  OrcaAstProgramNode *ast;

  friend class OrcaLexerErrorListener;
  friend class OrcaParserErrorListener;
  friend class OrcaError;
  friend class OrcaAstBuilder;
};
