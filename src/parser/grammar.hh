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

#ifndef YY_COMMANDPARSER_GRAMMAR_HH_INCLUDED
# define YY_COMMANDPARSER_GRAMMAR_HH_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int CommandParser_debug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INTCONST = 258,
     DOUBLECONST = 259,
     NEWTOKEN = 260,
     STEPTOKEN = 261,
     VAR = 262,
     LOCALVAR = 263,
     FUNCCALL = 264,
     VTYPE = 265,
     UCHROMA_DO = 266,
     UCHROMA_WHILE = 267,
     UCHROMA_FOR = 268,
     UCHROMA_IF = 269,
     UCHROMA_RETURN = 270,
     UCHROMA_CONTINUE = 271,
     UCHROMA_BREAK = 272,
     UCHROMA_ELSE = 273,
     OR = 274,
     AND = 275,
     DEQ = 276,
     TEQ = 277,
     MEQ = 278,
     PEQ = 279,
     NEQ = 280,
     EQ = 281,
     LEQ = 282,
     GEQ = 283,
     UMINUS = 284,
     MINUSMINUS = 285,
     PLUSPLUS = 286
   };
#endif
/* Tokens.  */
#define INTCONST 258
#define DOUBLECONST 259
#define NEWTOKEN 260
#define STEPTOKEN 261
#define VAR 262
#define LOCALVAR 263
#define FUNCCALL 264
#define VTYPE 265
#define UCHROMA_DO 266
#define UCHROMA_WHILE 267
#define UCHROMA_FOR 268
#define UCHROMA_IF 269
#define UCHROMA_RETURN 270
#define UCHROMA_CONTINUE 271
#define UCHROMA_BREAK 272
#define UCHROMA_ELSE 273
#define OR 274
#define AND 275
#define DEQ 276
#define TEQ 277
#define MEQ 278
#define PEQ 279
#define NEQ 280
#define EQ 281
#define LEQ 282
#define GEQ 283
#define UMINUS 284
#define MINUSMINUS 285
#define PLUSPLUS 286



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2058 of yacc.c  */
#line 31 "grammar.yy"

	int functionId;			/* Function enum id */
	Dnchar *name;			/* character pointer for names */
	TreeNode *node;			/* node pointer */
	Variable *variable;		/* variable pointer */
	Tree *tree;			/* function (tree) pointer */
	VTypes::DataType vtype;		/* variable type for next declarations */
	int intconst;			/* integer constant value */
	double doubleconst;		/* double constant value */


/* Line 2058 of yacc.c  */
#line 131 "grammar.hh"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE CommandParser_lval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int CommandParser_parse (void *YYPARSE_PARAM);
#else
int CommandParser_parse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int CommandParser_parse (void);
#else
int CommandParser_parse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_COMMANDPARSER_GRAMMAR_HH_INCLUDED  */
