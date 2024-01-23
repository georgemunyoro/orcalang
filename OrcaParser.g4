parser grammar OrcaParser;

options {
	tokenVocab = OrcaLexer;
}

// Actual grammar start.

program: expression;

expression: Constant | Identifier;