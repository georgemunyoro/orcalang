#include <string>

#include "OrcaLexerErrorListener.h"

class OrcaContext;

/**
 * @brief Represents an error that occurred during the compilation process.
 *
 * This class is used to store information about errors that occurred during the
 * compilation process. It is used to print out the error message and the
 * location of the error in the source code.
 *
 * @see OrcaLexerErrorListener
 * @see OrcaParserErrorListener
 */
class OrcaError {
public:
  OrcaError(const OrcaContext &context, const std::string &message, size_t line,
            size_t charPositionInLine)
      : context(context), message(message), line(line),
        charPositionInLine(charPositionInLine) {}

  /**
   * @brief Prints the error message and the location of the error in the source
   * code.
   */
  void print() const;

private:
  const OrcaContext &context;

  std::string message;
  size_t line;
  size_t charPositionInLine;

  friend class OrcaLexerErrorListener;
  friend class OrcaParserErrorListener;
};
