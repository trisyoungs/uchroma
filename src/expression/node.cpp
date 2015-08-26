/*
	*** General Node for Expression
	*** src/expression/node.cpp
	Copyright T. Youngs 2015

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

#include "expression/node.h"
#include "base/sysfunc.h"
#include "base/messenger.h"
#include "templates/reflist.h"
#include <stdarg.h>
#include <string.h>
#include <cctype>

// Local constant
const int MAXNODEARGS = 10;

// Constructors
Node::Node() : ListItem<Node>()
{
	// Private variables
	returnsNumber_ = false;
	readOnly_ = true;
	parent_ = NULL;
	nextArgument = NULL;
	prevArgument = NULL;
	nodeType_ = Node::BasicNode;
}

// Destructor
Node::~Node()
{
}

// Copy data
void Node::copy(Node* source)
{
	nodeType_ = source->nodeType_;
	parent_ = source->parent_;
	args_ = source->args_;
	returnsNumber_ = source->returnsNumber_;
	readOnly_ = source->readOnly_;
}

// Retrieve node type
Node::NodeType Node::nodeType() const
{
	return nodeType_;
}

// Set parent 
void Node::setParent(Expression* parent)
{
	parent_ = parent;
}

// Retrieve parent
Expression* Node::parent() const
{
	return parent_;
}

// Set whether node returns a number
void Node::setReturnsNumber(bool b)
{
	returnsNumber_ = b;
}

// Return whether node returns a number
bool Node::returnsNumber()
{
	return returnsNumber_;
}

// Set the readonly status of the variable to true
void Node::setReadOnly()
{
	readOnly_ = true;
}

// Return readonly status
bool Node::readOnly() const
{
	return readOnly_;
}

// Return number of arguments currently assigned to node
int Node::nArgs() const
{
	return args_.nItems();
}

// Return whether nth argument returns a number
bool Node::isArgNumeric(int i)
{
	if ((i < 0) || (i >= args_.nItems()))
	{
		msg.print(Messenger::Verbose, "Node::argType : Argument index %i is out of range (node = %p).\n", i, this);
		return false;
	}
	return args_[i]->item->returnsNumber();
}

// Set argument specified
bool Node::setArg(int i, double& rv)
{
	if ((i < 0) || (i >= args_.nItems()))
	{
		msg.print(Messenger::Verbose, "Node::setArg : Argument index %i is out of range (node = %p).\n", i, this);
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
bool Node::hasArg(int i)
{
	return (i < args_.nItems());
}

// Add list of arguments formas as a plain List<Node>, beginning from supplied list head
void Node::addListArguments(Node* leaf)
{
	for (Node* node = leaf; node != NULL; node = node->next) args_.add(node);
}

// Add list of arguments formed as a linked Node list
void Node::addJoinedArguments(Node* lastleaf)
{
	/*
	The supplied leaf may be a single node, or it may be a list of nodes beginning at the *last* node (this is the case if Joined by the parser)
	Therefore, must walk backwards through the list first to get to the head...
	*/
	Node* first;
	for (first = lastleaf; first != NULL; first = first->prevArgument) if (first->prevArgument == NULL) break;
	for (Node* node = first; node != NULL; node = node->nextArgument) args_.add(node);
}

// Add multiple arguments to node
void Node::addArguments(int nargs, ...)
{
	// Create variable argument parser
	va_list vars;
	va_start(vars,nargs);
	// Add arguments in the order they were provided
	for (int n=0; n<nargs; n++) addArgument(va_arg(vars, Node*));
	va_end(vars);
	//msg.print(Messenger::Parse,"Node %p now has %i arguments.\n", this, args_.nItems());
}

// Add multiple arguments to node
void Node::addArgument(Node* arg)
{
	args_.add(arg);
}

// Check validity of supplied arguments
bool Node::checkArguments(const char* arglist, const char* funcname)
{
	//msg.enter("Node::checkArguments");
	//msg.print(Messenger::Parse, "Checking the %i argument(s) given to function '%s'...\n", args_.nItems(), funcname);
	const char* c = NULL, *altargs = arglist;
// 	msg.print(Messenger::Parse, "...argument list is [%s]\n", altargs);
	char upc;
	int count = 0, ngroup = -1, repeat = 0;
	bool optional = false, requireVar = false, result, cluster = false, reset = true;
	// If the argument list begins with '_', arguments will have already been checked and added elsewhere...
	if (*altargs == '_')
	{
// 		msg.exit("Node::checkArguments");
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
// 				msg.exit("Node::checkArguments");
				return true;
			}
		// Retain previous information if this is a repeat, but make it an optional argument
		if (*c == '*') optional = true;
		else if (repeat == 0)
		{
			// Reset modifier values
			requireVar = false;
			repeat = 1;
			// Find next alpha character (and accompanying modifiers)
			while (!isalpha(*c) && (*c != '|') && (*c != '\0') )
			{
				switch (*c)
				{
					// Require variable
					case ('^'):	
						requireVar = true; break;
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
						msg.print(Messenger::Verbose, "Error: Bad character (%c) in command arguments\n", *c);
						break;
				}
				c++;
			}
			if (*c == '|')
			{
				// This is the start of a new set of argument specifiers - does the current set of arguments 'fit'?
				if (args_.nItems() != count)
				{
					msg.print(Messenger::Verbose, "Error: Number of arguments (%i) doesn't match number in this set (%i) - next!\n", args_.nItems(), count);
					reset = true;
					continue;
				}
// 				msg.exit("Node::checkArguments");
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
				msg.print(Messenger::Verbose, "Error: The function '%s' requires argument %i.\n", funcname, count+1);
// 				printf("       Command syntax is '%s(%s)'.\n", funcname, Command::data[function_].argText);
// 				msg.exit("Node::checkArguments");
				return false;
			}
			else if (cluster && (ngroup != 0))
			{
				msg.print(Messenger::Verbose, "Error: The optional argument %i to function '%s' is part of a group and must be specified.\n", count+1, funcname);
// 				printf("       Command syntax is '%s(%s)'.\n", funcname, arglist);
// 				msg.exit("Node::checkArguments");
				return false;
			}
			else
			{
// 				msg.exit("Node::checkArguments");
				return true;
			}
		}

		// Check argument type
		result = true;
		switch (upc)
		{
			// Number
			case ('N'):
				if (!isArgNumeric(count))
				{
					if (altargs != NULL) { reset = true; continue; }
					msg.print(Messenger::Verbose, "Error: Argument %i to function '%s' must be a number.\n", count+1, funcname);
					result = false;
				}
				break;
		}

		// Was this argument requested to be a modifiable variable value?
		if (requireVar && argNode(count)->readOnly())
		{
			msg.print(Messenger::Verbose, "Error: Argument %i to function '%s' must be a variable and not a constant.\n", count+1, funcname);
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
		msg.print(Messenger::Verbose, "Error: %i extra arguments given to function '%s'.\n", args_.nItems()-count, funcname);
// 		msg.exit("Node::checkArguments");
		return false;
	}
// 	msg.exit("Node::checkArguments");
	return result;
}

// Return (execute) argument specified
bool Node::arg(int i, double& rv)
{
	if ((i < 0) || (i >= args_.nItems()))
	{
		msg.print(Messenger::Verbose, "Node::arg : Argument index %i is out of range (node = %p).\n", i, this);
		return false;
	}
	return args_[i]->item->execute(rv);
}

// Return (execute) argument specified as a bool
bool Node::argb(int i)
{
	if ((i < 0) || (i >= args_.nItems()))
	{
		msg.print(Messenger::Verbose, "Node::argb : Argument index %i is out of range (node = %p).\n", i, this);
		return false;
	}
	double rv;
	bool result;
	if (!args_[i]->item->execute(rv)) msg.print(Messenger::Verbose, "Couldn't retrieve argument %i.\n", i+1);
	result = (rv > 0);
	return result;
}

// Return (execute) argument specified as an integer
int Node::argi(int i)
{
	if ((i < 0) || (i >= args_.nItems()))
	{
		msg.print(Messenger::Verbose, "Node::argi : Argument index %i is out of range (node = %p).\n", i, this);
		return false;
	}
	double rv;
	int result = 0;
	if (!args_[i]->item->execute(rv)) msg.print(Messenger::Verbose, "Couldn't retrieve argument %i.\n", i+1);
	result = (int) rv;
	return result;
}

// Return (execute) argument specified as a double
double Node::argd(int i)
{
	if ((i < 0) || (i >= args_.nItems()))
	{
		msg.print(Messenger::Verbose, "Node::argd : Argument index %i is out of range (node = %p).\n", i, this);
		return false;
	}
	double result = 0.0;
	if (!args_[i]->item->execute(result)) msg.print(Messenger::Verbose, "Couldn't retrieve argument %i.\n", i+1);
	return result;
}

// Return the Node corresponding to the argument, rather than executing it
Node* Node::argNode(int i)
{
	if ((i < 0) || (i > args_.nItems()))
	{
		msg.print(Messenger::Verbose, "Node::argNode : Argument index %i is out of range for returning the argument node (node = %p).\n", i, this);
		return NULL;
	}
	return args_[i]->item;
}

