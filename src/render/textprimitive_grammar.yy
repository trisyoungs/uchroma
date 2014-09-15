/* -------- */
/* Prologue */
/* -------- */
%{

/* Includes */

#include "render/textprimitive.h"
#include "base/dnchar.h"
#include "base/messenger.h"

/* Prototypes */
int TextPrimitiveParser_lex(void);
void TextPrimitiveParser_error(char *s);

%}

// Redeclare function names
%name-prefix="TextPrimitiveParser_"

/* Type Definition */
%union {
	TextPrimitive::EscapeSequence escSeq;	/* Escape Sequence id */
	const char* text;			/* Text string */
	bool success;				/* TextFragment pointer */
};

%token <text> TEXT
%token <escSeq> ESCAPE
%token FAIL

%type <success> fragment pushescape

%%

/* ------------------------------ */
/* Fragment Sequence Construction */
/* ------------------------------ */

/* Fragment Sequence */
fragmentsequence:
	fragment					{ }
	| fragmentsequence fragment			{ }
	;

/* ---------- */
/* Components */
/* ---------- */

fragment:
	TEXT						{
		$$ = TextPrimitive::target()->addFragment($1);
		if (! $$) YYABORT;
		}
	| ESCAPE pushescape '{' fragmentsequence '}' popescape	{
		$$ = $2;
		}
	| FAIL						{
		YYABORT;
		}
	;

/* Semantics */

pushescape:
	/* empty */					{ $$ = TextPrimitive::target()->addEscape(yylval.escSeq); }
	;

popescape:
	/* empty */					{ TextPrimitive::target()->removeEscape(); }
	;

%%

void yyerror(char *s)
{
}
