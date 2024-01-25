lexer grammar OrcaLexer;

// Constant: Integer | Float | Char | Boolean;
Integer: DIGIT_NON_ZERO DIGIT* | DIGIT;
Float: DIGIT+ '.' DIGIT+;
String: '"' .*? '"';
Boolean: 'true' | 'false';
Char: '\'' . '\'';

WS: [ \t\r\n]+ -> skip;

fragment DIGIT_NON_ZERO: [1-9];
fragment DIGIT: [0-9];

// Symbols
SEMICOLON: ';';
DOT: '.';
COMMA: ',';
COLON: ':';
LBRACE: '{';
RBRACE: '}';
LPAREN: '(';
RPAREN: ')';
LBRACK: '[';
RBRACK: ']';
INCREMENT: '++';
DECREMENT: '--';
ARROW: '->';
SIZEOF: 'sizeof';
STAR: '*';
GT: '>';
LT: '<';
GE: '>=';
LE: '<=';
EQ: '==';
NE: '!=';
BANG: '!';
AND: '&';
PLUS: '+';
MINUS: '-';
NOT: '~';
EQUALS: '=';
MUL_EQUALS: '*=';
DIV_EQUALS: '/=';
MOD_EQUALS: '%=';
PLUS_EQUALS: '+=';
MINUS_EQUALS: '-=';
AND_EQUALS: '&=';
OR_EQUALS: '|=';
XOR_EQUALS: '^=';
SHL_EQUALS: '<<=';
SHR_EQUALS: '>>=';
QUESTION: '?';
LOGICAL_AND: '&&';
LOGICAL_OR: '||';
PIPE: '|';
CARET: '^';
SHL: '<<';
SHR: '>>';
MOD: '%';
DIV: '/';
CALL: '()';
DOLLAR: '$';

// Types
T_U8: 'u8';
T_U16: 'u16';
T_U32: 'u32';
T_U64: 'u64';
T_S8: 's8';
T_S16: 's16';
T_S32: 's32';
T_S64: 's64';
T_F32: 'f32';
T_F64: 'f64';
T_BOOL: 'bool';
T_CHAR: 'char';
T_VOID: 'void';
STRUCT: 'struct';
ARRAY: '[]';

// Keywords
IF: 'if';
ELSE: 'else';
WHILE: 'while';
FOR: 'for';
RETURN: 'return';
BREAK: 'break';
CONTINUE: 'continue';
LET: 'let';
AS: 'as';
FN: 'func';
TYPE: 'type';
TRAIT: 'trait';
IMPLEMENT: 'impl';

Identifier: [a-zA-Z_][a-zA-Z0-9_]*;

LineComment: '//' ~[\r\n]* -> channel(HIDDEN);