/*
	*** Flow Commands
	*** src/command/flow.cpp
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
#include "parser/tree.h"
#include "math/mathfunc.h"
#include <stdio.h>
#include <string.h>

// Dummy Node
bool Command::function_NoFunction(CommandNode *c, ReturnValue &rv)
{
	return true;
}

// Joiner
bool Command::function_Joiner(CommandNode *c, ReturnValue &rv)
{
	// Execute both commands
	bool result = true;
	if (c->hasArg(0)) result = c->arg(0, rv);
	if (result && c->hasArg(1)) result = c->arg(1, rv);
	return result;
}

// Declarations
bool Command::function_Declarations(CommandNode *c, ReturnValue &rv)
{
	// Reset each variable argument
	for (int n=0; n<c->nArgs(); ++n) if (!c->argNode(n)->initialise()) return false;
	return true;
}

// Break out of current 'for' loop or 'switch' structure
bool Command::function_Break(CommandNode *c, ReturnValue &rv)
{
	c->parent()->setAcceptedFail(Command::Break);
	return false;
}

// Continue for loop at next iteration
bool Command::function_Continue(CommandNode *c, ReturnValue &rv)
{
	c->parent()->setAcceptedFail(Command::Continue);
	return false;
}

// Do-While loop
bool Command::function_DoWhile(CommandNode *c, ReturnValue &rv)
{
	// Argument 0 - Blockment
	// Argument 1 - Test condition
	ReturnValue test;
	bool result;
	Command::Function af;
	do
	{
		// Run blockment- catch break and continue calls which return false
		result = c->arg(0, rv);
		if (!result)
		{
			// Check acceptedfail flag - if Break or Continue, reset flag and quit/continue
			af = c->parent()->acceptedFail();
			if (af == Command::Break)
			{
				c->parent()->setAcceptedFail(Command::NoFunction);
				return true;
			}
			else if (af == Command::Continue) c->parent()->setAcceptedFail(Command::NoFunction);
			else if (af != Command::NoFunction) return false;
		}
		// Perform test of condition
		if (!c->arg(1, test)) return false;
	} while (test.asBool());
	return true;
}

// For loop
bool Command::function_For(CommandNode *c, ReturnValue &rv)
{
	// Argument 0 - Initial value expression
	// Argument 1 - Loop condition
	// Argument 2 - Action on loop cycle
	// Argument 3 - Statementlist
	 // Get initial variable value
	if (!c->arg(0, rv)) return false;
	ReturnValue ifval;
	bool result;
	Command::Function af;
	while (true)
	{
		// Termination condition
		if (!c->arg(1, ifval)) return false;
		if (!ifval.asBool()) break;
		// Loop body - catch break and continue calls which return false
		result = c->arg(3, rv);
		if (!result)
		{
			// Check acceptedfail flag - if Break or Continue, reset flag and quit/continue
			af = c->parent()->acceptedFail();
			if (af == Command::Break)
			{
				c->parent()->setAcceptedFail(Command::NoFunction);
				return true;
			}
			else if (af == Command::Continue) c->parent()->setAcceptedFail(Command::NoFunction);
			else if (af != Command::NoFunction) return false;
		}
		// Loop 'increment' statement
		if (!c->arg(2, rv)) return false;
	}
	return true;
}

// If test
bool Command::function_If(CommandNode *c, ReturnValue &rv)
{
	ReturnValue ifval;
	if (!c->arg(0, ifval)) return false;
	if (ifval.asBool()) return (c->arg(1, rv));
	else if (c->hasArg(2)) return (c->arg(2, rv));
	return true;
}

// Return from function/filter/program
bool Command::function_Return(CommandNode *c, ReturnValue &rv)
{
	c->parent()->setAcceptedFail(Command::Return);
	if (c->hasArg(0)) c->arg(0, rv);
	return false;
}

// While loop
bool Command::function_While(CommandNode *c, ReturnValue &rv)
{
	// Argument 0 - Test condition
	// Argument 1 - Blockment
	ReturnValue test;
	bool result;
	Command::Function af;
	// Perform initial test of condition
	if (!c->arg(0, test)) return false;
	while (test.asBool())
	{
		// Run blockment- catch break and continue calls which return false
		result = c->arg(1, rv);
		if (!result)
		{
			// Check acceptedfail flag - if Break or Continue, reset flag and quit/continue
			af = c->parent()->acceptedFail();
			if (af == Command::Break)
			{
				c->parent()->setAcceptedFail(Command::NoFunction);
				return true;
			}
			else if (af == Command::Continue) c->parent()->setAcceptedFail(Command::NoFunction);
			else if (af != Command::NoFunction) return false;
		}
		// Perform test of condition
		if (!c->arg(0, test)) return false;
	}
	return true;
}

