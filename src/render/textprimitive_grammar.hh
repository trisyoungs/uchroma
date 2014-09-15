/* A Bison parser, made by GNU Bison 2.6.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_TEXTPRIMITIVEPARSER_TEXTPRIMITIVE_GRAMMAR_HH_INCLUDED
# define YY_TEXTPRIMITIVEPARSER_TEXTPRIMITIVE_GRAMMAR_HH_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int TextPrimitiveParser_debug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TEXT = 258,
     ESCAPE = 259,
     FAIL = 260
   };
#endif
/* Tokens.  */
#define TEXT 258
#define ESCAPE 259
#define FAIL 260



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2042 of yacc.c  */
#line 22 "textprimitive_grammar.yy"

	TextPrimitive::EscapeSequence escSeq;	/* Escape Sequence id */
	const char* text;			/* Text string */
	bool success;				/* TextFragment pointer */


/* Line 2042 of yacc.c  */
#line 74 "textprimitive_grammar.hh"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE TextPrimitiveParser_lval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int TextPrimitiveParser_parse (void *YYPARSE_PARAM);
#else
int TextPrimitiveParser_parse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int TextPrimitiveParser_parse (void);
#else
int TextPrimitiveParser_parse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_TEXTPRIMITIVEPARSER_TEXTPRIMITIVE_GRAMMAR_HH_INCLUDED  */
