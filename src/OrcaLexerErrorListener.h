#pragma once

#include <ANTLRInputStream.h>
#include <BaseErrorListener.h>
#include <CommonTokenStream.h>
#include <Recognizer.h>
#include <exception>

using namespace antlr4;

class OrcaContext;

class OrcaLexerErrorListener : public BaseErrorListener {
public:
  OrcaLexerErrorListener(const OrcaContext &context) : context(context) {}

  void syntaxError(Recognizer *recognizer, Token *offendingSymbol, size_t line,
                   size_t charPositionInLine, const std::string &msg,
                   std::exception_ptr e) override;

private:
  const OrcaContext &context;
};
