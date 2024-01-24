parser grammar OrcaParser;

options {
	tokenVocab = OrcaLexer;
}

program: statement* EOF;

statement:
	labeledStatement
	| compoundStatement
	| selectionStatement
	| expressionStatement
	| iterationStatement
	| jumpStatement
	| declarationStatement;

declarationStatement:
	functionDeclarationStatement
	| typeDeclaration;

typeDeclaration:
	'type' name = Identifier typeToAlias = type ';'
	| 'type' '<' params = identifierList '>' name = Identifier typeToAlias = type ';';

identifierList: Identifier (',' Identifier)*;

functionDeclarationStatement:
	'func' name = Identifier ('(' args = functionArgs ')' | '()') '->' returnType = type body =
		compoundStatement
	| 'func' name = Identifier '<' typeParams = identifierList '>' (
		'(' args = functionArgs ')'
		| '()'
	) '->' returnType = type body = compoundStatement;

functionArgs: functionArg (',' functionArg)*;
functionArg: name = Identifier ':' type;

jumpStatement:
	'break' ';'
	| 'continue' ';'
	| 'return' ';'
	| 'return' expression ';';

labeledStatement: Identifier ':' statement;

compoundStatement: '{' statement* '}';

selectionStatement:
	'if' condition = expression then = compoundStatement (
		'else' else = compoundStatement
	)?;

expressionStatement: expression ';';

iterationStatement:
	'while' condition = expression body = compoundStatement
	| 'for' init = expression ';' condition = expression ';' update = expression body =
		compoundStatement;

type:
	typeSpecifier
	| '(' wrappedType = type ')'
	| pointeeType = type STAR
	| elementType = type '[' arraySize = Integer ']'
	| '{' fields = structFieldDeclarationList methods = functionDeclarationStatement* '}'
	| opaqueType = type '<' params = typeList '>'
	| '$' nameOfFunctionToGetReturnTypeOf = Identifier;

structFieldDeclarationList: structFieldDeclaration*;
structFieldDeclaration:
	field = Identifier ':' fieldType = type ';';

typeSpecifier:
	T_U8
	| T_U16
	| T_U32
	| T_U64
	| T_S8
	| T_S16
	| T_S32
	| T_S64
	| T_F32
	| T_F64
	| T_BOOL
	| T_VOID
	| T_CHAR
	| Identifier
	| '(' argsType = typeList ')' '->' returnType = type;

typeList: type (',' type)*;

expression: assignmentExpression;

assignmentExpression:
	conditionalExpression
	| lhs = unaryExpression operator = assignmentOperator rhs = assignmentExpression;

assignmentOperator:
	'='
	| '*='
	| '/='
	| '%='
	| '+='
	| '-='
	| '<<='
	| '>>='
	| '&='
	| '^='
	| '|=';

conditionalExpression:
	condition = logicalOrExpression (
		'?' trueExpr = expression ':' elseExpr = conditionalExpression
	)?;

logicalOrExpression:
	lhs = logicalAndExpression ('||' rhs = logicalAndExpression)*;

logicalAndExpression:
	lhs = inclusiveOrExpression (
		'&&' rhs = inclusiveOrExpression
	)*;

inclusiveOrExpression:
	lhs = exclusiveOrExpression ('|' rhs = exclusiveOrExpression)*;

exclusiveOrExpression:
	lhs = andExpression ('^' rhs = andExpression)*;

andExpression:
	lhs = equalityExpression ('&' rhs = equalityExpression)*;

equalityExpression:
	lhs = relationalExpression (
		('==' | '!=') rhs = relationalExpression
	)*;

relationalExpression:
	lhs = shiftExpression (
		('<' | '>' | '<=' | '>=') rhs = shiftExpression
	)*;

shiftExpression:
	lhs = additiveExpression (
		('<<' | '>>') rhs = additiveExpression
	)*;

additiveExpression:
	lhs = multiplicativeExpression (
		('+' | '-') rhs = multiplicativeExpression
	)*;

multiplicativeExpression:
	lhs = castExpression (('*' | '/' | '%') rhs = castExpression)*;

castExpression:
	expr = castExpression 'as' typeToCastTo = type
	| unaryExpression;

unaryExpression:
	| postfixExpression
	| sizeofExpression
	| operator = unaryOperator expr = unaryExpression;

sizeofExpression: 'sizeof' '(' typeToGetSizeOf = type ')';

unaryOperator: '&' | '*' | '+' | '-' | '~' | '!' | '++' | '--';

/*
 * A postfix expression is a primary expression followed by zero or more postfix operators.
 */
postfixExpression:
	primaryExpression (
		'[' index = expression ']'
		| '.' field = Identifier
		| '->' field = Identifier
		| '(' args = argumentExpressionList ')'
		| '()'
		| '++'
		| '--'
	)*;

argumentExpressionList: expression (',' expression)*;

primaryExpression:
	letExpression
	| Constant
	| String
	| Identifier
	| arrayExpression
	| fieldMap
	| '(' expression ')';

letExpression: 'let' varName = Identifier ':' varType = type;

/*
 * An array expression is a list of expressions. e.g. [1, 2, 3]
 */
arrayExpression: '[' expressionList ']';

expressionList: expression (',' expression)*;

/*
 * A field map is a map of identifiers to expressions. The identifiers are the keys, and the
 * expressions are the values. e.g. {a: 1, b: 2}
 */
fieldMap: '{' fieldMapEntry+ functionDeclarationStatement* '}';
fieldMapEntry:
	key = Identifier ':' value = expression ','
	| typeOfField = type key = Identifier ',';