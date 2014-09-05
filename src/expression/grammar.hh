/* A Bison parser, made by GNU Bison 2.7.  */

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

#ifndef YY_EXPRESSIONPARSER_GRAMMAR_HH_INCLUDED
# define YY_EXPRESSIONPARSER_GRAMMAR_HH_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int ExpressionParser_debug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     CONSTANT = 258,
     NEWTOKEN = 259,
     VAR = 260,
     FUNCCALL = 261,
     UCHROMA_IF = 262,
     UCHROMA_ELSE = 263,
     OR = 264,
     AND = 265,
     NEQ = 266,
     EQ = 267,
     LEQ = 268,
     GEQ = 269,
     UMINUS = 270
   };
#endif
/* Tokens.  */
#define CONSTANT 258
#define NEWTOKEN 259
#define VAR 260
#define FUNCCALL 261
#define UCHROMA_IF 262
#define UCHROMA_ELSE 263
#define OR 264
#define AND 265
#define NEQ 266
#define EQ 267
#define LEQ 268
#define GEQ 269
#define UMINUS 270



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2058 of yacc.c  */
#line 25 "grammar.yy"

	int functionId;			/* Function enum id */
	Dnchar* name;			/* Character pointer for names */
	Node* node;			/* node pointer */
	Variable* variable;		/* variable pointer */
	double doubleConst;		/* double constant value */


/* Line 2058 of yacc.c  */
#line 96 "grammar.hh"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE ExpressionParser_lval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int ExpressionParser_parse (void *YYPARSE_PARAM);
#else
int ExpressionParser_parse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int ExpressionParser_parse (void);
#else
int ExpressionParser_parse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_EXPRESSIONPARSER_GRAMMAR_HH_INCLUDED  */
