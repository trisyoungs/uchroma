/*
	*** Math Commands
	*** src/command/math.cpp
	Copyright T. Youngs 2010-2014

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

#include "command/commands.h"
#include "parser/commandnode.h"
#include "math/constants.h"
#include <math.h>

// Return absolute of argument
bool Command::function_Abs(CommandNode *c, ReturnValue &rv)
{
	rv.set( fabs(c->argd(0)) );
	return true;
}

// Return invserse cosine of argument
bool Command::function_ACos(CommandNode *c, ReturnValue &rv)
{
	rv.set( (assumeDegrees_ ? acos(c->argd(0)) * DEGRAD : acos(c->argd(0)) ) );
	return true;
}

// Return invserse sine of argument
bool Command::function_ASin(CommandNode *c, ReturnValue &rv)
{
	rv.set( (assumeDegrees_ ? asin(c->argd(0)) * DEGRAD : asin(c->argd(0)) ) );
	return true;
}

// Return invserse tangent of argument
bool Command::function_ATan(CommandNode *c, ReturnValue &rv)
{
	rv.set( (assumeDegrees_ ? atan(c->argd(0)) * DEGRAD : atan(c->argd(0)) ) );
	return true;
}

// Return cosine of argument (supplied in degrees)
bool Command::function_Cos(CommandNode *c, ReturnValue &rv)
{
	rv.set( (assumeDegrees_ ? cos(c->argd(0)) * DEGRAD : cos(c->argd(0)) ) );
	return true;
}

// Return exponential of of argument
bool Command::function_Exp(CommandNode *c, ReturnValue &rv)
{
	rv.set( exp(c->argd(0)) );
	return true;
}

// Return natural logarithm of argument
bool Command::function_Ln(CommandNode *c, ReturnValue &rv)
{
	rv.set( log(c->argd(0)) );
	return true;
}

// Return base-10 logarithm of argument
bool Command::function_Log(CommandNode *c, ReturnValue &rv)
{
	rv.set( log10(c->argd(0)) );
	return true;
}

// Round real value to nearest integer
bool Command::function_Nint(CommandNode *c, ReturnValue &rv)
{
	rv.set( floor(c->argd(0) + 0.5) );
	return true;
}

// Return sine of argument (supplied in degrees)
bool Command::function_Sin(CommandNode *c, ReturnValue &rv)
{
	rv.set( (assumeDegrees_ ? sin(c->argd(0)) * DEGRAD : sin(c->argd(0)) ) );
	return true;
}

// Return square root of argument
bool Command::function_Sqrt(CommandNode *c, ReturnValue &rv)
{
	rv.set( sqrt(c->argd(0)) );
	return true;
}

// Return tangent of argument (supplied in degrees)
bool Command::function_Tan(CommandNode *c, ReturnValue &rv)
{
	rv.set( (assumeDegrees_ ? tan(c->argd(0)) * DEGRAD : tan(c->argd(0)) ) );
	rv.set( tan(c->argd(0) / DEGRAD) );
	return true;
}
