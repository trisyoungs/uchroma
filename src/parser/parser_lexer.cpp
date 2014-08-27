/*
	*** Parser Lexer
	*** src/parser/parser_lexer.cpp
	Copyright T. Youngs 2010-2013

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "base/dnchar.h"
#include "parser/variable.h"
#include "parser/double.h"
#include "parser/parser.h"
#include "parser/grammar.hh"
#include "parser/tree.h"
#include "command/commands.h"
#include "base/sysfunc.h"
#include "base/messenger.h"
#include <string.h>

// Symbols
const char *SymbolTokenKeywords[CommandParser::nSymbolTokens] = { "==", ">=", "<=", "!=", "<>", "+=", "-=", "*=", "/=", "++", "--", "&&", "||" };
int SymbolTokenValues[CommandParser::nSymbolTokens] = { EQ, GEQ, LEQ, NEQ, NEQ, PEQ, MEQ, TEQ, DEQ, PLUSPLUS, MINUSMINUS, AND, OR };

// Bison-generated CommandParser_lex()
int CommandParser_lex()
{
	return cmdparser.lex();
}

// Parser lexer, called by yylex()
int CommandParser::lex()
{
	int n;
	bool done, integer, hasExp;
	static Dnchar token, name;
	char quotechar, c;
	token.clear();

	// Skip over whitespace
	while ((c = getChar()) == ' ' || c == '\t' || c == '\r' || c == '\n' );

	if (c == 0) return 0;

	// Set this point as the start of our new token (for error reporting)
	tokenStart_ = stringPos_-1;

	/*
	// A '.' followed by a character indicates a variable path - generate a step
	*/
	msg.print(Messenger::Verbose, "LEXER (%p): begins at [%c], peek = [%c]\n", tree_, c, peekChar());
	if ((c == '.') && isalpha(peekChar()))
	{
		msg.print(Messenger::Verbose, "LEXER (%p): found a '.' before an alpha character - expecting a path step next...\n",tree_);
		expectPathStep_ = true;
		return '.';
	}
	
	/*
	// Number Detection - Either '.' or  a digit begins a number
	*/
	if (c == '.' || isdigit(c))
	{
		msg.print(Messenger::Verbose, "LEXER (%p): found the start of a number...\n", tree_);
		// Default to integer, unless first char is '.'
		integer = (c == '.' ? false : true);
		      hasExp = false;
		token += c;
		done = false;
		do
		{
			c = getChar();
			if (isdigit(c)) token += c;
			else if (c == '.')
			{
				integer = false;
				token += '.';
			}
			else if ((c == 'e') || (c == 'E'))
			{
				// Check for previous exponential in number
				if (hasExp)
				{
					printf("Error: Number has two exponentiations (e/E).\n");
					return 0;
				}
				token += 'E';
				            hasExp = true;
			}
			else if ((c == '-') || (c == '+'))
			{
				// We allow '-' or '+' only as part of an exponentiation, so if it is not preceeded by 'E' we stop parsing
				if ((!token.isEmpty()) && (token.lastChar() != 'E'))
				{
					unGetChar();
					done = true;
				}
				else token += c;
			}
			else
			{
				unGetChar();
				done = true;
			}
		} while (!done);
		// We now have the number as a text token...
		if (!hasExp)
		{
			if (integer) CommandParser_lval.intconst = atol(token);
			else CommandParser_lval.doubleconst = atof(token);
		}
		else
		{
			// Exponentiations are always returned as a double
			integer = false;
			CommandParser_lval.doubleconst = atof(beforeChar(token,'E')) * pow(10.0, atof(afterChar(token,'E')));
		}
		if (integer) msg.print(Messenger::Verbose, "LEXER (%p): found an integer constant [%s] [%i]\n", tree_, token.get(), CommandParser_lval.intconst);
		else msg.print(Messenger::Verbose, "LEXER (%p): found a floating-point constant [%s] [%e]\n", tree_, token.get(), CommandParser_lval.doubleconst);
		return (integer ? INTCONST : DOUBLECONST);
	}

	/*
	// Alphanumeric-token - function or variable
	*/
	if (isalpha (c))
	{
		do
		{
			token += c;
			c = getChar();
		}
		while (isalnum(c) || (c == '_'));
		unGetChar();
		msg.print(Messenger::Verbose, "LEXER (%p): found an alpha token [%s]...\n", tree_, token.get());
		// Skip over keyword detection if we are expecting a path step
		if (!expectPathStep_)
		{
			// Is this a variable declaration statement?
			VTypes::DataType dt = VTypes::dataType(token);
			if (dt != VTypes::nDataTypes)
			{
				msg.print(Messenger::Verbose, "LEXER (%p): ...which is a variable type name (->VTYPE)\n",tree_);
				CommandParser_lval.vtype = dt;
				return VTYPE;
			}

			// Built-in numeric constants
			// true, false, or NULL token?
			if (token == "true")
			{
				CommandParser_lval.intconst = 1;
				return INTCONST;
			}
			else if ((token == "false") || (token == "NULL"))
			{
				CommandParser_lval.intconst = 0;
				return INTCONST;
			}
			else if (token == "Pi")
			{
				CommandParser_lval.doubleconst = PI;
				return DOUBLECONST;
			}

			// Additional numeric constants
			if (useAdditionalConstants_)
			{
				if (token == "DEGRAD")
				{
					CommandParser_lval.doubleconst = DEGRAD;
					return DOUBLECONST;
				}
				else if (token == "Bohr")
				{
					CommandParser_lval.doubleconst = BOHRRADIUS;
					return DOUBLECONST;
				}
				else if (token == "NA")
				{
					CommandParser_lval.doubleconst = AVOGADRO;
					return DOUBLECONST;
				}
				else if (token == "c")
				{
					CommandParser_lval.doubleconst = SPEEDOFLIGHT;
					return DOUBLECONST;
				}
				else if (token == "kb")
				{
					CommandParser_lval.doubleconst = BOLTZMANN;
					return DOUBLECONST;
				}
				else if (token == "h")
				{
					CommandParser_lval.doubleconst = PLANCK;
					return DOUBLECONST;
				}
				else if (token == "hbar")
				{
					CommandParser_lval.doubleconst = HBAR;
					return DOUBLECONST;
				}
			}

			// Is this a recognised high-level keyword?
			n = 0;
			if (token == "if") n = UCHROMA_IF;
			else if (token == "else") n = UCHROMA_ELSE;
			else if (token == "for") n = UCHROMA_FOR;
			else if (token == "do") n = UCHROMA_DO;
			else if (token == "continue") n = UCHROMA_CONTINUE;
			else if (token == "break") n = UCHROMA_BREAK;
			else if (token == "while") n = UCHROMA_WHILE;
			else if (token == "return") n = UCHROMA_RETURN;
			if (n != 0)
			{
				msg.print(Messenger::Verbose, "LEXER (%p): ...which is a high-level keyword (%i)\n",tree_,n);
				return n;
			}

			// Is it an existing variable in scope?
			if (tree_ != NULL)
			{
				// Search the variable lists currently in scope...
				int scopelevel;
				Variable *v = tree_->findVariableInScope(token, scopelevel);
				if (v != NULL)
				{
					if (scopelevel == 0)
					{
						msg.print(Messenger::Verbose, "LEXER (%p): ...which is an existing local variable (->LOCALVAR)\n", tree_);
						CommandParser_lval.variable = v;
						return LOCALVAR;
					}
					else
					{
						msg.print(Messenger::Verbose, "LEXER (%p): ...which is an existing variable (->VAR)\n", tree_);
						CommandParser_lval.variable = v;
						return VAR;
					}
				}
			}

			// Is it one of uChroma's function keywords?
			for (n=0; n<Command::nCommands; n++) if (strcmp(token,Command::data[n].keyword) == 0) break;
			if (n != Command::nCommands)
			{
				msg.print(Messenger::Verbose, "LEXER (%p): ... which is a function (->FUNCCALL).\n", tree_);
				CommandParser_lval.functionId = n;
				functionStart_ = tokenStart_;
				return FUNCCALL;
			}

		}

		// The token isn't a high- or low-level function. It's either a path step or a normal variable
		if (expectPathStep_)
		{
			expectPathStep_ = false;
			msg.print(Messenger::Verbose, "LEXER (%p): ...which we assume is a path step (->STEPTOKEN)\n", tree_);
			name = token;
			CommandParser_lval.name = &name;
			return STEPTOKEN;
		}

		// If we get to here then we have found an unrecognised alphanumeric token
		// If the 'generateMissingVariables_' flag is set, generate a new variable with the current token name
		if (tree_->generateMissingVariables())
		{
			msg.print(Messenger::Verbose, "LEXER (%p): ...which has been autogenerated as a variable (->VAR)\n", tree_);
			CommandParser_lval.variable = tree_->addVariable(VTypes::DoubleData, &token);
			return VAR;
		}
		else
		{
			msg.print(Messenger::Verbose, "LEXER (%p): ...which is unrecognised (->NEWTOKEN)\n", tree_);
			name = token;
			CommandParser_lval.name = &name;
			return NEWTOKEN;
		}
	}

	/* We have found a symbolic character (or a pair) that corresponds to an operator */
	// Return immediately in the case of brackets, comma, and semicolon
	if ((c == '(') || (c == ')') || (c == ';') || (c == ',') || (c == '{') || (c == '}') || (c == '[') || (c == ']') || (c == '%') || (c == ':'))
	{
		msg.print(Messenger::Verbose, "LEXER (%p): found symbol [%c]\n",tree_,c);
		return c;
	}
	token += c;
	// Similarly, if the next character is a period, bracket or double quotes, return immediately
	char c2 = peekChar();
	if ((c2 == '.') || (c2 == '(') || (c2 == ')') || (c2 == ';') || (c2 == '{') || (c2 == '}') || (c2 == '"')) return c;
	// If next character is '-', return now if previous char was *not* another '-'
	if ((c2 == '-') && (c != '-')) return c;
	// If it is 'punctuation', add this second character to our operator and search for it
	if (ispunct(c2))
	{
		c = getChar();
		token += c;
		msg.print(Messenger::Verbose, "LEXER (%p): found symbol [%s]\n",tree_,token.get());
		SymbolToken st = (SymbolToken) enumSearch("", nSymbolTokens, SymbolTokenKeywords, token.get());
		if (st != nSymbolTokens) return SymbolTokenValues[st];
		else printf("Error: Unrecognised symbol found in input (%s).\n", token.get());
 	}
	else
	{
		// Make sure that this is a known symbol
		if ((c == '$') || (c == '%') || (c == '&') || (c == '@') || (c == '?') || (c == ':'))
		{
			printf("Error: Unrecognised symbol found in input (%c).\n", c);
		}
		else return c;
	}
	return 0;
}
