/*
	*** Expression Parser Lexer
	*** src/expression/expression_lexer.cpp
	Copyright T. Youngs 2014

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

#include "expression/expression.h"
#include "expression/functions.h"
#include "expression/expression_grammar.hh"
#include "expression/variable.h"
#include "math/constants.h"
#include "base/sysfunc.h"
#include "base/messenger.h"
#include <math.h>
#include <cctype>

// Symbols
const char* SymbolTokenKeywords[Expression::nSymbolTokens] = { "==", ">=", "<=", "!=", "<>", "&&", "||" };
int SymbolTokenValues[Expression::nSymbolTokens] = { UCR_EP_EQ, UCR_EP_GEQ, UCR_EP_LEQ, UCR_EP_NEQ, UCR_EP_NEQ, UCR_EP_AND, UCR_EP_OR };

/*
 * Bison Functions / Variables
 */

// Bison-generated ExpressionParser_lex()
int ExpressionParser_lex()
{
	if (!Expression::target()) return 0;
	return Expression::target()->lex();
}

// Parser lexer, called by yylex()
int Expression::lex()
{
	int n;
	bool done, hasExp;
	static QString token, name;
	char c;
	token.clear();

	// Skip over whitespace
	while ((c = getChar()) == ' ' || c == '\t' || c == '\r' || c == '\n' );

	if (c == 0) return 0;

	// Set this point as the start of our new token (for error reporting)
	tokenStart_ = stringPos_-1;

	/*
	// Number Detection - Either '.' or  a digit begins a number
	*/
	if (c == '.' || isdigit(c))
	{
		msg.print(Messenger::Verbose, "LEXER (%p): found the start of a number...\n", this);
		// Default to integer, unless first char is '.'
		hasExp = false;
		token += c;
		done = false;
		do
		{
			c = getChar();
			if (isdigit(c)) token += c;
			else if (c == '.') token += '.';
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
				if ((!token.isEmpty()) && (!token.endsWith("E")))
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
		ExpressionParser_lval.doubleConst = token.toDouble();
		msg.print(Messenger::Verbose, "LEXER (%p): found a numeric constant [%s] [%e]\n", this, qPrintable(token), ExpressionParser_lval.doubleConst);
		return UCR_EP_CONSTANT;
	}

	/*
	// Alpha-token - function or variable
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
		msg.print(Messenger::Verbose, "LEXER (%p): found an alpha token [%s]...\n", this, qPrintable(token));

		// Built-in numeric constants
		if (token == "Pi")
		{
			ExpressionParser_lval.doubleConst = PI;
			return UCR_EP_CONSTANT;
		}

		// Additional numeric constants
		if (useAdditionalConstants_)
		{
			if (token == "DEGRAD")
			{
				ExpressionParser_lval.doubleConst = DEGRAD;
				return UCR_EP_CONSTANT;
			}
			else if (token == "Bohr")
			{
				ExpressionParser_lval.doubleConst = BOHRRADIUS;
				return UCR_EP_CONSTANT;
			}
			else if (token == "NA")
			{
				ExpressionParser_lval.doubleConst = AVOGADRO;
				return UCR_EP_CONSTANT;
			}
			else if (token == "c")
			{
				ExpressionParser_lval.doubleConst = SPEEDOFLIGHT;
				return UCR_EP_CONSTANT;
			}
			else if (token == "kb")
			{
				ExpressionParser_lval.doubleConst = BOLTZMANN;
				return UCR_EP_CONSTANT;
			}
			else if (token == "h")
			{
				ExpressionParser_lval.doubleConst = PLANCK;
				return UCR_EP_CONSTANT;
			}
			else if (token == "hbar")
			{
				ExpressionParser_lval.doubleConst = HBAR;
				return UCR_EP_CONSTANT;
			}
		}

		// Is this a recognised high-level keyword?
		n = 0;
		if (token == "if") n = UCR_EP_IF;
		else if (token == "else") n = UCR_EP_ELSE;
		if (n != 0)
		{
			msg.print(Messenger::Verbose, "LEXER (%p): ...which is a high-level keyword (%i)\n",this,n);
			return n;
		}

		// Is it an existing variable?
		Variable *v = variable(token);
		if (v != NULL)
		{
			msg.print(Messenger::Verbose, "LEXER (%p): ...which is an existing variable (->VAR)\n", this);
			ExpressionParser_lval.variable = v;
			return UCR_EP_VAR;
		}

		// Is it one of uChroma's function keywords?
		n = Functions::function(qPrintable(token));
		if (n != Functions::nFunctions)
		{
			msg.print(Messenger::Verbose, "LEXER (%p): ... which is a function (->FUNCCALL).\n", this);
			ExpressionParser_lval.functionId = n;
			functionStart_ = tokenStart_;
			return UCR_EP_FUNCCALL;
		}

		// If we get to here then we have found an unrecognised alphanumeric token
		// If the 'generateMissingVariables_' flag is set, generate a new variable with the current token name
		if (generateMissingVariables_)
		{
			msg.print(Messenger::Verbose, "LEXER (%p): ...which has been autogenerated as a variable (->VAR)\n", this);
			ExpressionParser_lval.variable = createVariable(token);
			return UCR_EP_VAR;
		}
		else
		{
			msg.print(Messenger::Verbose, "LEXER (%p): ...which is unrecognised (->NEWTOKEN)\n", this);
			name = token;
			ExpressionParser_lval.name = &name;
			return UCR_EP_NEWTOKEN;
		}
	}

	/* We have found a symbolic character (or a pair) that corresponds to an operator */
	// Return immediately in the case of brackets, comma, and semicolon
	if ((c == '(') || (c == ')') || (c == ';') || (c == ',') || (c == '{') || (c == '}') || (c == '[') || (c == ']') || (c == '%') || (c == ':'))
	{
		msg.print(Messenger::Verbose, "LEXER (%p): found symbol [%c]\n",this,c);
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
		msg.print(Messenger::Verbose, "LEXER (%p): found symbol [%s]\n", this, qPrintable(token));
		SymbolToken st = (SymbolToken) enumSearch("", nSymbolTokens, SymbolTokenKeywords, qPrintable(token));
		if (st != nSymbolTokens) return SymbolTokenValues[st];
		else printf("Error: Unrecognised symbol found in input (%s).\n", qPrintable(token));
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
