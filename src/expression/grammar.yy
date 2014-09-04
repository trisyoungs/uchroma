/* -------- */
/* Prologue */
/* -------- */
%{

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "expression/expression.h"
#include "expression/functions.h"
#include "expression/variable.h"
#include "base/messenger.h"

/* Prototypes */
int ExpressionParser_lex(void);
void ExpressionParser_error(char *s);

%}

// Redeclare function names
%name-prefix="ExpressionParser_"

/* Type Definition */
%union {
	int functionId;			/* Function enum id */
	Dnchar* name;			/* Character pointer for names */
	Node* node;			/* node pointer */
	Variable* variable;		/* variable pointer */
	double doubleConst;		/* double constant value */
};

%token <doubleConst> CONSTANT
%token <name> NEWTOKEN
%token <variable> VAR
%token <functionId> FUNCCALL
%token UCHROMA_IF
%nonassoc UCHROMA_ELSE

%left AND OR
%left '='
%left GEQ LEQ EQ NEQ '>' '<'
%left '+' '-'
%left '*' '/' '%'
%right UMINUS
%right '!'
%right '^'

%type <node> constant expression expressionlist variable statement flowstatement statementlist block blockment
%type <node> function

%%

/* ------------------------- */
/* Main Program Construction */
/* ------------------------- */

/* Program List */
programlist:
	program						{ }
	| programlist program				{ }
	;

/* Single Program 'Statement' */
program:
	statementlist					{
		if (($1 != NULL) && (!Expression::target()->addStatement($1))) YYABORT;
		}
	| block						{
		if (($1 != NULL) && (!Expression::target()->addStatement($1))) YYABORT;
		}
	;

/* --------- */
/* Constants */
/* --------- */

constant:
	CONSTANT					{
		$$ = Expression::target()->createConstant($1);
		}
	;

/* ----------------- */
/* Variables & Paths */
/* ----------------- */

/* Pre-Existing Variable */
variable:
	VAR						{
		$$ = Expression::target()->addVariableNode($1);
		if ($$ == NULL) { printf("Error in variable expression (code 2)\n"); YYABORT; }
		}
	| variable '('					{
		msg.print("Tried to use a variable as a function.\n");
		YYABORT;
		}
	;

/* -------------- */
/* Function Calls */
/* -------------- */

/* Built-In Functions */
function:
	FUNCCALL '(' ')'				{
		$$ = Expression::target()->addFunctionNode( (Functions::Function) $1);
		msg.print(Messenger::Verbose, "PARSER: function : function '%s'\n", functions.data[(Functions::Function) $1].keyword);
		}
	| FUNCCALL '(' expressionlist ')'		{
		$$ = Expression::target()->addFunctionNodeWithArglist( (Functions::Function) $1, $3);
		msg.print(Messenger::Verbose, "PARSER: function : function '%s' with exprlist\n", functions.data[(Functions::Function) $1].keyword);
		}
	| FUNCCALL error				{
		msg.print("Error: Missing brackets after function call?\n");
		YYABORT;
		}
	;

/* ----------- */
/* Expressions */
/* ----------- */

/* Expression */
expression:
	constant					{ $$ = $1; }
	| function					{ $$ = $1; }
	| '-' expression %prec UMINUS			{ $$ = Expression::target()->addOperator(Functions::OperatorNegate, $2); }
	| variable					{ $$ = $1; }
	| expression '+' expression			{ $$ = Expression::target()->addOperator(Functions::OperatorAdd, $1, $3); }
	| expression '-' expression			{ $$ = Expression::target()->addOperator(Functions::OperatorSubtract, $1, $3); }
	| expression '*' expression			{ $$ = Expression::target()->addOperator(Functions::OperatorMultiply, $1, $3); }
	| expression '/' expression			{ $$ = Expression::target()->addOperator(Functions::OperatorDivide, $1, $3); }
	| expression '^' expression			{ $$ = Expression::target()->addOperator(Functions::OperatorPower, $1, $3); }
	| expression '%' expression			{ $$ = Expression::target()->addOperator(Functions::OperatorModulus, $1, $3); }
	| expression EQ expression			{ $$ = Expression::target()->addOperator(Functions::OperatorEqualTo, $1, $3); }
	| expression NEQ expression			{ $$ = Expression::target()->addOperator(Functions::OperatorNotEqualTo, $1, $3); }
	| expression '>' expression			{ $$ = Expression::target()->addOperator(Functions::OperatorGreaterThan, $1, $3); }
	| expression GEQ expression			{ $$ = Expression::target()->addOperator(Functions::OperatorGreaterThanEqualTo, $1, $3); }
	| expression '<' expression			{ $$ = Expression::target()->addOperator(Functions::OperatorLessThan, $1, $3); }
	| expression LEQ expression			{ $$ = Expression::target()->addOperator(Functions::OperatorLessThanEqualTo, $1, $3); }
	| expression AND expression			{ $$ = Expression::target()->addOperator(Functions::OperatorAnd, $1, $3); }
	| expression OR expression			{ $$ = Expression::target()->addOperator(Functions::OperatorOr, $1, $3); }
	| '(' expression ')'				{ $$ = $2; }
	| '!' expression				{ $$ = Expression::target()->addOperator(Functions::OperatorNot, $2); }
	| NEWTOKEN					{ printf("Error: '%s' has not been declared as a function or a variable.\n", yylval.name->get()); YYABORT; }
	;

/* Expression List */
expressionlist:
	expression					{
		$$ = $1;
		if ($$ == NULL) YYABORT;
		}
	| expressionlist ',' expression			{
		$$ = Expression::joinArguments($3,$1);
		}
	| expressionlist expression			{
		printf("Error: Missing comma between items.\n");
		YYABORT;
		}
	;

/* ---------- */
/* Statements */
/* ---------- */

/* Statement Delimiter */
delim:
	';'						{ }
	| delim ';'					{ }
	;

/* Single Statement */
statement:
	expression delim				{
		$$ = $1;
		}
	| function delim				{
		$$ = $1;
		}
	| flowstatement					{
		$$ = $1;
		}
	;

/* Statement List */
statementlist:
	statement					{
		$$ = $1;
		}
	| statementlist statement			{
		if ($2 == NULL) $$ = $1;
		else $$ = Expression::target()->joinCommands($1, $2);
		}
	;

/* Block Statement */
block:
	'{' statementlist '}' {
		$$ = $2;
		}
	| '{' '}'					{
		$$ = Expression::target()->addFunctionNode(Functions::NoFunction);
		}
	;

/* Block or Statement, but not Statement List */
blockment:
	statement					{
		$$ = $1;
		}
	| block						{
		$$ = $1;
		}
	;

/* Flow-Control Statement */
flowstatement:
	UCHROMA_IF '(' expression ')' blockment UCHROMA_ELSE blockment 	{
		$$ = Expression::target()->addFunctionNode(Functions::If,$3,$5,$7);
		}
	| UCHROMA_IF '(' expression ')' blockment 			{
		$$ = Expression::target()->addFunctionNode(Functions::If,$3,$5);
		}
	;

%%

void yyerror(char *s)
{
}