/*
	*** Command Node
	*** src/parser/commandnode.cpp
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

#include "parser/commandnode.h"
#include "parser/tree.h"
#include "parser/integer.h"
#include "parser/double.h"
#include "parser/variablenode.h"
#include "command/commands.h"
#include "base/sysfunc.h"
#include "base/messenger.h"
#include <string.h>
#include <stdarg.h>

// Constructor
CommandNode::CommandNode(Command::Function func) : function_(func)
{
	// Private variables
	nodeType_ = TreeNode::CmdNode;
}

CommandNode::CommandNode(TreeNode *source)
{
	copy(source);	
}

// Destructor
CommandNode::~CommandNode()
{
}

// Prepare function (if possible)
bool CommandNode::prepFunction()
{
	bool result = true;
	switch (function_)
	{
		// For the 'return' function, the return type must match the return type of the parent tree...
		case (Command::Return):
			if (parent_->returnType() == VTypes::NoData)
			{
				if (!hasArg(0)) break;
				msg.print("Error: Return value provided when none is required.\n");
				result = false;
			}
			else
			{
				if ((!hasArg(0)) && (parent_->returnType() != VTypes::NoData))
				{
					msg.print("Error: No return value provided.\n");
					result = false;
				}
				else if (argType(0) != parent_->returnType())
				{
					msg.print("Error: Return value of type '%s' provided for function that returns %s.\n", VTypes::dataType(argType(0)), VTypes::aDataType(parent_->returnType()));
					result = false;
				}
			}
			break;
		default:
			break;
	}
	return result;
}

// Get function
Command::Function CommandNode::function()
{
	return function_;
}

// Execute command
bool CommandNode::execute(ReturnValue& rv)
{
	// Execute the command
	//printf("Node function is %i (%s)\n", function_, commands.data[function_].keyword);
	return commands.call(function_, this, rv);
}

// Print node contents
void CommandNode::nodePrint(int offset, const char *prefix)
{
	// Construct tabbed offset
	Dnchar tab(offset+32);
	for (int n=0; n<offset-1; n++) tab += '\t';
	if (offset > 1) tab.strcat("   |--> ");
	tab.strcat(prefix);

	// Output node data
// 	printf("Function id = %p\n", function_);
	printf("[CN]%s%s (Command) (%i arguments)\n", tab.get(), Command::data[function_].keyword, args_.nItems());
	// Output Argument data
	for (RefListItem<TreeNode,int> *ri = args_.first(); ri != NULL; ri = ri->next) ri->item->nodePrint(offset+1);
}

// Set from returnvalue node
bool CommandNode::set(ReturnValue rv)
{
	printf("Internal Error: Trying to 'set' a CommandNode.\n");
	return false;
}

// Initialise node
bool CommandNode::initialise()
{
	printf("Internal Error: A CommandNode cannot be initialised.\n");
	return false;
}

// Create, run, and free a single command with simple arguments
bool CommandNode::run(Command::Function func, const char *arglist, ...)
{
	msg.enter("CommandNode::run");
	// Local tree to contain commandnode and its arguments
	Tree tree;

	// Create our temporary node
	CommandNode node(func);
	node.parent_ = &tree;

	// Set arguments from supplied list
	const char *c;
	va_list vars;
	va_start(vars, arglist);
	TreeNode *var = NULL;
	for (c = &arglist[0]; *c != '\0'; c++)
	{
		switch (*c)
		{
			case ('i'):
				var = tree.addConstant(va_arg(vars, int));
				break;
			case ('d'):
				var = tree.addConstant(va_arg(vars, double));
				break;
			default:
				printf("Invalid argument specifier '%c' in CommandNode::run.\n", *c);
				var = NULL;
				break;
		}
		node.addArgument(var);
	}
	va_end(vars);
	// Now, run the command...
	ReturnValue rv;
	bool result = node.execute(rv);
	msg.exit("CommandNode::run");
	return result;
}
