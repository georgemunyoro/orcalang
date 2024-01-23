lexer grammar OrcaLexer;

Constant: Integer | Float;
Identifier: [a-zA-Z_][a-zA-Z0-9_]*;
Integer: DIGIT_NON_ZERO DIGIT*;
Float: DIGIT+ '.' DIGIT+;

WS: [ \t\r\n]+ -> skip;

fragment DIGIT_NON_ZERO: [1-9];
fragment DIGIT: [0-9];