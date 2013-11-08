/*
	*** Tree Node
	*** src/parser/treenode.cpp
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

#include "parser/treenode.h"
#include "parser/tree.h"
#include "parser/variablenode.h"
#include "base/sysfunc.h"
#include "templates/reflist.h"
#include <stdarg.h>
#include <string.h>

// Local constant
const int MAXNODEARGS = 10;

// Constructors
TreeNode::TreeNode() : ListItem<TreeNode>()
{
	// Private variables
	returnType_ = VTypes::NoData;
	readOnly_ = true;
	parent_ = NULL;
	nextArgument = NULL;
	prevArgument = NULL;
	nodeType_ = TreeNode::BasicNode;
}

// Destructor
TreeNode::~TreeNode()
{
}

// Copy data
void TreeNode::copy(TreeNode *source)
{
	nodeType_ = source->nodeType_;
	parent_ = source->parent_;
	args_ = source->args_;
	returnType_ = source->returnType_;
	readOnly_ = source->readOnly_;
}

// Retrieve node type
TreeNode::NodeType TreeNode::nodeType() const
{
	return nodeType_;
}

// Set parent 
void TreeNode::setParent(Tree *parent)
{
	parent_ = parent;
}

// Retrieve parent
Tree *TreeNode::parent() const
{
	return parent_;
}

// Sets the content type of the variable
void TreeNode::setReturnType(VTypes::DataType dt)
{
	returnType_ = dt;
}

// Returns content type of the variable
VTypes::DataType TreeNode::returnType() const
{
	return returnType_;
}

// Return readonly status
bool TreeNode::readOnly() const
{
	return readOnly_;
}

// Set the readonly status of the variable to true
void TreeNode::setReadOnly()
{
	readOnly_ = true;
}

// Return number of arguments currently assigned to node
int TreeNode::nArgs() const
{
	return args_.nItems();
}

// Return datatype of nth argument
VTypes::DataType TreeNode::argType(int i)
{
	if ((i < 0) || (i >= args_.nItems()))
	{
		printf("TreeNode::argType : Argument index %i is out of range (node = %p).\n", i, this);
		return VTypes::NoData;
	}
	return args_[i]->item->returnType();
}


// Set argument specified
bool TreeNode::setArg(int i, ReturnValue &rv)
{
	if ((i < 0) || (i >= args_.nItems()))
	{
		printf("TreeNode::setArg : Argument index %i is out of range (node = %p).\n", i, this);
		return false;
	}
	// Check readonly attribute of argument
	if (args_[i]->item->readOnly())
	{
		args_[i]->item->nodePrint(0);
		//printf("Argument %i is read-only and can't be set.\n", i);
		return false;
	}
	return args_[i]->item->set(rv);
}

// Return whether argument i was given
bool TreeNode::hasArg(int i)
{
	return (i < args_.nItems());
}

// Add list of arguments formas as a plain List<TreeNode>, beginning from supplied list head
void TreeNode::addListArguments(TreeNode *leaf)
{
	for (TreeNode *node = leaf; node != NULL; node = node->next) args_.add(node);
}

// Add list of arguments formed as a linked TreeNode list
void TreeNode::addJoinedArguments(TreeNode *lastleaf)
{
	/*
	The supplied leaf may be a single node, or it may be a list of nodes beginning at the *last* node (this is the case if Joined by the parser)
	Therefore, must walk backwards through the list first to get to the head...
	*/
	TreeNode *first;
	for (first = lastleaf; first != NULL; first = first->prevArgument) if (first->prevArgument == NULL) break;
	for (TreeNode *node = first; node != NULL; node = node->nextArgument) args_.add(node);
}

// Add multiple arguments to node
void TreeNode::addArguments(int nargs, ...)
{
	// Create variable argument parser
	va_list vars;
	va_start(vars,nargs);
	// Add arguments in the order they were provided
	for (int n=0; n<nargs; n++) addArgument(va_arg(vars, TreeNode*));
	va_end(vars);
	//msg.print(Messenger::Parse,"Node %p now has %i arguments.\n", this, args_.nItems());
}

// Add multiple arguments to node
void TreeNode::addArgument(TreeNode *arg)
{
	args_.add(arg);
}

// Check validity of supplied arguments
bool TreeNode::checkArguments(const char *arglist, const char *funcname)
{
	//msg.enter("TreeNode::checkArguments");
	//msg.print(Messenger::Parse, "Checking the %i argument(s) given to function '%s'...\n", args_.nItems(), funcname);
	const char *c = NULL, *altargs = arglist;
// 	msg.print(Messenger::Parse, "...argument list is [%s]\n", altargs);
	char upc;
	int count = 0, ngroup = -1, repeat = 0;
	bool optional = false, requirevar = false, result, cluster = false, reset = true;
	VTypes::DataType rtype;
	// If the argument list begins with '_', arguments will have already been checked and added elsewhere...
	if (*altargs == '_')
	{
// 		msg.exit("TreeNode::checkArguments");
		return true;
	}
	// Search for an alternative set of arguments
	result = true;
	do
	{
		if (reset)
		{
			c = altargs;
			if (*c == '|') ++c;
			altargs = strchr(c, '|');
			repeat = 0;
			cluster = false;
			count = 0;
			reset = false;
		}
		if (*c == '\0') break;
		upc = *c;
			if (*c == '|')
			{
				// This is the start of a new set of argument specifiers - does the current set of arguments 'fit'?
				if (args_.nItems() != count)
				{
// 					printf("Number of arguments (%i) doesn't match number in this set (%i) - next!\n", args_.nItems(), count);
					reset = true;
					continue;
				}
// 				msg.exit("TreeNode::checkArguments");
				return true;
			}
		// Retain previous information if this is a repeat, but make it an optional argument
		if (*c == '*') optional = true;
		else if (repeat == 0)
		{
			// Reset modifier values
			requirevar = false;
			repeat = 1;
			// Find next alpha character (and accompanying modifiers)
			while (!isalpha(*c) && (*c != '|') && (*c != '\0') )
			{
				switch (*c)
				{
					// Require variable
					case ('^'):	requirevar = true; break;
					// Clustering
					case ('['):	cluster = true; ngroup = 0; break;
					case (']'):	cluster = false; ngroup = -1; break;
					// Require array
					case ('2'):
					case ('3'):
					case ('4'):
					case ('5'):
					case ('6'):
					case ('7'):
					case ('8'):
					case ('9'):	repeat = *c - '0'; break;
					default:
						printf("BAD CHARACTER (%c) IN COMMAND ARGUMENTS\n", *c);
						break;
				}
				c++;
			}
			if (*c == '|')
			{
				// This is the start of a new set of argument specifiers - does the current set of arguments 'fit'?
				if (args_.nItems() != count)
				{
					printf("Number of arguments (%i) doesn't match number in this set (%i) - next!\n", args_.nItems(), count);
					reset = true;
					continue;
				}
// 				msg.exit("TreeNode::checkArguments");
				return true;
			}
			// Convert character to upper case if necessary
			if ((*c > 96) && (*c < 123))
			{
				upc = *c - 32;
				optional = true;
			}
			else
			{
				upc = *c;
				optional = false;
			}
		}
		if (*c == '\0') break;
// 		msg.print(Messenger::Parse,"...next/current argument token is '%c', opt=%s, reqvar=%s, repeat=%i, ngroup=%i\n", *c, optional ? "true" : "false", requirevar ? "true" : "false", repeat, ngroup);
		// If we have gone over the number of arguments provided, is this an optional argument?
		if (count >= args_.nItems())
		{
			if (!optional)
			{
				// If an alternative argument list is present, check this before we fail...
				if (altargs != NULL) { reset = true; continue; }
				printf("Error: The function '%s' requires argument %i.\n", funcname, count+1);
// 				printf("       Command syntax is '%s(%s)'.\n", funcname, Command::data[function_].argText);
// 				msg.exit("TreeNode::checkArguments");
				return false;
			}
			else if (cluster && (ngroup != 0))
			{
				printf("Error: The optional argument %i to function '%s' is part of a group and must be specified.\n", count+1, funcname);
// 				printf("       Command syntax is '%s(%s)'.\n", funcname, arglist);
// 				msg.exit("TreeNode::checkArguments");
				return false;
			}
			else
			{
// 				msg.exit("TreeNode::checkArguments");
				return true;
			}
		}
		// Check argument type
		rtype = argType(count);
		result = true;
		switch (upc)
		{
			// Number		(IntegerData, DoubleData)
			case ('N'):
				if ((rtype != VTypes::IntegerData) && (rtype != VTypes::DoubleData))
				{
					if (altargs != NULL) { reset = true; continue; }
					printf("Argument %i to function '%s' must be a number.\n", count+1, funcname);
					result = false;
				}
				break;
			// Integer		(IntegerData)
			case ('I'):
				if (rtype != VTypes::IntegerData)
				{
					if (altargs != NULL) { reset = true; continue; }
					printf("Argument %i to function '%s' must be an int.\n", count+1, funcname);
					result = false;
				}
				break;
			// Double		(DoubleData)
			case ('D'):
				if (rtype != VTypes::DoubleData)
				{
					if (altargs != NULL) { reset = true; continue; }
					printf("Argument %i to function '%s' must be a double.\n", count+1, funcname);
					result = false;
				}
				break;
			// Any Simple		(IntegerData, RealData, StringData)
			case ('S'):
				if ((rtype != VTypes::IntegerData) && (rtype != VTypes::DoubleData))
				{
					if (altargs != NULL) { reset = true; continue; }
					printf("Argument %i to function '%s' must be a number or a string.\n", count+1, funcname);
					result = false;
				}
				break;
			// Boolean		(Any Except NoData)
			case ('B'):
				if (rtype == VTypes::NoData)
				{
					if (altargs != NULL) { reset = true; continue; }
					printf("Argument %i to function '%s' must return something!\n", count+1, funcname);
					result = false;
				}
				break;
			// Variable of any type (but not a path)
			case ('V'):
				if (argNode(count)->nodeType() != TreeNode::VarWrapperNode)
				{
					if (altargs != NULL) { reset = true; continue; }
					printf("Argument %i to function '%s' must be a variable of some kind.\n", count+1, funcname);
					result = false;
				}
				break;
		}
		// Was this argument requested to be a modifiable variable value?
		if (requirevar && argNode(count)->readOnly())
		{
			printf("Argument %i to function '%s' must be a variable and not a constant.\n", count+1, funcname);
			result = false;
		}

		// Check for failure
		if (!result) break;
		if ((upc != '*') && (repeat == 1)) c++;
		if (cluster) ngroup++;
		repeat--;
		count++;
	} while (*c != '\0');
	// End of the argument specification - do we still have arguments left over in the command?
	if (result && (args_.nItems() > count))
	{
		printf("Error: %i extra arguments given to function '%s'.\n", args_.nItems()-count, funcname);
// 		msg.exit("TreeNode::checkArguments");
		return false;
	}
// 	msg.exit("TreeNode::checkArguments");
	return result;
}

// Return (execute) argument specified
bool TreeNode::arg(int i, ReturnValue &rv)
{
	if ((i < 0) || (i >= args_.nItems()))
	{
		printf("TreeNode::arg : Argument index %i is out of range (node = %p).\n", i, this);
		return false;
	}
	return args_[i]->item->execute(rv);
}

// Return (execute) argument specified as a bool
bool TreeNode::argb(int i)
{
	if ((i < 0) || (i >= args_.nItems()))
	{
		printf("TreeNode::argb : Argument index %i is out of range (node = %p).\n", i, this);
		return false;
	}
	static ReturnValue rv;
	bool success;
	bool result;
	if (!args_[i]->item->execute(rv)) printf("Couldn't retrieve argument %i.\n", i+1);
	result = (rv.asInteger(success) > 0);
	if (!success) printf("Couldn't cast argument %i into an integer.\n", i+1);
	return result;
}

// Return (execute) argument specified as an integer
int TreeNode::argi(int i)
{
	if ((i < 0) || (i >= args_.nItems()))
	{
		printf("TreeNode::argi : Argument index %i is out of range (node = %p).\n", i, this);
		return false;
	}
	static ReturnValue rv;
	bool success;
	int result = 0;
	if (!args_[i]->item->execute(rv)) printf("Couldn't retrieve argument %i.\n", i+1);
	result = rv.asInteger(success);
	if (!success) printf("Couldn't cast argument %i into an integer.\n", i+1);
	return result;
}

// Return (execute) argument specified as a double
double TreeNode::argd(int i)
{
	if ((i < 0) || (i >= args_.nItems()))
	{
		printf("TreeNode::argd : Argument index %i is out of range (node = %p).\n", i, this);
		return false;
	}
	static ReturnValue rv;
	bool success;
	double result = 0.0;
	if (!args_[i]->item->execute(rv)) printf("Couldn't retrieve argument %i.\n", i+1);
	result = rv.asDouble(success);
	if (!success) printf("Couldn't cast argument %i into a real.\n", i+1);
	return result;
}

// Return (execute) argument specified as a character
const char *TreeNode::argc(int i)
{
	if ((i < 0) || (i >= args_.nItems()))
	{
		printf("TreeNode::argc : Argument index %i is out of range (node = %p).\n", i, this);
		return "NULL";
	}
	static ReturnValue rv[MAXNODEARGS];
	bool success;
	const char *result = NULL;
	if (!args_[i]->item->execute(rv[i])) printf("Couldn't retrieve argument %i.\n", i+1);
	result = rv[i].asString(success);
	if (!success) printf("Couldn't cast argument %i into a character.\n", i+1);
	return result;
}

// Return the TreeNode corresponding to the argument, rather than executing it
TreeNode *TreeNode::argNode(int i)
{
	if ((i < 0) || (i > args_.nItems()))
	{
		printf("TreeNode::argNode : Argument index %i is out of range for returning the argument node (node = %p).\n", i, this);
		return NULL;
	}
	return args_[i]->item;
}

/*
// Virtuals
*/

// Search accessors (if any) available for node
StepNode *TreeNode::findAccessor(const char* s, TreeNode* arglist)
{
	// Default is to return NULL since there are no accessors available
	printf("Internal Error: This node type does not have any accessors.\n");
	return NULL;
}
