/*
	*** Parser Tree Interface
	*** src/parser/parser_tree.cpp
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

#include "parser/parser.h"
#include "command/commands.h"

// Create a new path on the stack with the specified base 'variable'
TreeNode *CommandParser::createPath(TreeNode *var)
{
	if (tree_ == NULL)
	{
		printf("Internal Error: No current Tree target for Parser (createPath).\n");
		return NULL;
	}
	TreeNode *result = tree_->createPath(var);
	return result;
}

// Expand topmost path
bool CommandParser::expandPath(Dnchar *name, TreeNode *arglist)
{
	if (tree_ == NULL)
	{
		printf("Internal Error: No current Tree target for Parser (expandPath).\n");
		return false;
	}
	bool result = tree_->expandPath(name, arglist);
	return result;
}

// Finalise and remove the topmost path on the stack
TreeNode *CommandParser::finalisePath()
{
	if (tree_ == NULL)
	{
		printf("Internal Error: No current Tree target for Parser (finalisePath).\n");
		return NULL;
	}
	TreeNode *result = tree_->finalisePath();
	return result;
}

// Add a node representing a whole statement to the execution list
bool CommandParser::addStatement(TreeNode *leaf)
{
	if (tree_ == NULL)
	{
		printf("Internal Error: No current Tree target for Parser (addStatement).\n");
		return false;
	}
	else tree_->addStatement(leaf);
	return true;
}

// Add an operator to the Tree
TreeNode *CommandParser::addOperator(Command::Function func, TreeNode *arg1, TreeNode *arg2)
{
	if (tree_ == NULL)
	{
		printf("Internal Error: No current Tree target for Parser (addOperator).\n");
		return NULL;
	}
	TreeNode *result = tree_->addOperator(func, arg1, arg2);
	return result;
}

// Add a function node to the list (overloaded to accept simple arguments instead of a list)
TreeNode *CommandParser::addFunction(Command::Function func, TreeNode *a1, TreeNode *a2, TreeNode *a3, TreeNode *a4)
{
	if (tree_ == NULL)
	{
		printf("Internal Error: No current Tree target for Parser (addFunction).\n");
		return NULL;
	}
	TreeNode *result = tree_->addFunction(func, a1, a2, a3, a4);
	return result;
}

// Associate a command-based leaf node to the Tree
TreeNode *CommandParser::addFunctionWithArglist(Command::Function func, TreeNode *arglist)
{
	if (tree_ == NULL)
	{
		printf("Internal Error: No current Tree target for Parser (addFunctionWithArglist).\n");
		return NULL;
	}
	TreeNode *result = tree_->addFunctionWithArglist(func, arglist);
	return result;
}

// Add a declaration list
TreeNode *CommandParser::addDeclarations(TreeNode *declist)
{
	if (tree_ == NULL)
	{
		printf("Internal Error: No current Tree target for Parser (addDeclarations).\n");
		return NULL;
	}
	TreeNode *result = tree_->addDeclarations(declist);
	return result;
}

// Join two commands together
TreeNode *CommandParser::joinCommands(TreeNode *node1, TreeNode *node2)
{
	if (tree_ == NULL)
	{
		printf("Internal Error: No current Tree target for Parser (joinCommands).\n");
		return NULL;
	}
	TreeNode *result = tree_->joinCommands(node1, node2);
	return result;
}

// Add on a new scope to the stack
TreeNode *CommandParser::pushScope(Command::Function func)
{
	if (tree_ == NULL)
	{
		printf("Internal Error: No current Tree target for Parser (pushScope).\n");
		return NULL;
	}
	TreeNode *result = tree_->pushScope(func);
	return result;
}

// Pop the topmost scope node
bool CommandParser::popScope()
{
	if (tree_ == NULL)
	{
		printf("Internal Error: No current Tree target for Parser (popScope).\n");
		return false;
	}
	else tree_->popScope();
	return true;
}

// Add integer constant
TreeNode *CommandParser::addConstant(int i)
{
	if (tree_ == NULL)
	{
		printf("Internal Error: No current Tree target for Parser (addConstant(int)).\n");
		return NULL;
	}
	TreeNode *result = tree_->addConstant(i);
	return result;
}

// Add double constant
TreeNode *CommandParser::addConstant(double d)
{
	if (tree_ == NULL)
	{
		printf("Internal Error: No current Tree target for Parser (addConstant(double)).\n");
		return NULL;
	}
	TreeNode *result = tree_->addConstant(d);
	return result;
}

// Add variable to topmost ScopeNode
TreeNode *CommandParser::addVariable(VTypes::DataType type, Dnchar *name, TreeNode *initialValue)
{
	if (tree_ == NULL)
	{
		printf("Internal Error: No current Tree target for Parser (addVariable).\n");
		return NULL;
	}
	TreeNode *result = tree_->addVariable(type, name, initialValue);
	return result;
}

// Wrap named variable (and array index)
TreeNode *CommandParser::wrapVariable(Variable *var)
{
	if (tree_ == NULL)
	{
		printf("Internal Error: No current Tree target for Parser (wrapVariable).\n");
		return NULL;
	}
	TreeNode *result = tree_->wrapVariable(var);
	return result;
}
