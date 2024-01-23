parser grammar OrcaParser;

options {
	tokenVocab = OrcaLexer;
}

// Actual grammar start.

program: expression EOF;

expression: Constant | Identifier;