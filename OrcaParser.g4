parser grammar OrcaParser;

options {
	tokenVocab = OrcaLexer;
}

program: expression EOF;

type:
	typeSpecifier
	| '(' wrappedType = type ')'
	| pointeeType = type STAR
	| elementType = type '[]'
	| '{' fields = structFieldDeclarationList '}'
	| opaqueType = type '<' params = typeList '>';

structFieldDeclarationList:
	structFieldDeclaration (',' structFieldDeclaration)*;
structFieldDeclaration: field = Identifier ':' fieldType = type;

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
	('++' | '--')* (
		postfixExpression
		| operator = unaryOperator expr = unaryExpression
		| 'sizeof' '(' typeToGetSizeOf = type ')'
		| Identifier
	);

unaryOperator: '&' | '*' | '+' | '-' | '~' | '!';

/*
 * A postfix expression is a primary expression followed by zero or more postfix operators.
 */
postfixExpression:
	primaryExpression (
		'[' index = expression ']'
		| '.' field = Identifier
		| '->' field = Identifier
		| '(' args = argumentExpressionList? ')'
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
arrayExpression: '[' (expression (',' expression)*)? ']';

/*
 * A field map is a map of identifiers to expressions. The identifiers are the keys, and the
 * expressions are the values. e.g. {a: 1, b: 2}
 */
fieldMap: '{' fieldMapEntry+ '}';
fieldMapEntry: key = Identifier ':' value = expression ',';