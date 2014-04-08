/*
	*** Parser
	*** src/parser/parser.cpp
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
#include "base/messenger.h"

// External Declarations
CommandParser cmdparser;
int CommandParser_parse();

// Constructor
CommandParser::CommandParser()
{
	// Private variables
	reset();
}

// Destructor
CommandParser::~CommandParser()
{
}

// Reset values in parser, ready for next source
void CommandParser::reset()
{
	source_ = StringSource;
	stringListSource_ = NULL;
	stringPos_ = -1;
	tokenStart_ = 0;
	functionStart_ = -1;
	stringSource_.clear();
	stringLength_ = 0;
	lineNumber_ = 0;
	expectPathStep_ = false;
	useAdditionalConstants_ = false;
	tree_ = NULL;
	failed_ = false;
	stack_.clear();
}

// Print error information and location
void CommandParser::printErrorInfo()
{
	printf("Error occurred here:\n");
	// QUICK'n'DIRTY!
	int i;
	char *temp = new char[stringLength_+32];
	for (i=0; i<stringLength_+32; ++i) temp[i] = '\0';
	for (i=0; i<tokenStart_; i++) temp[i] = (stringSource_[i] == '\t' ? '\t' : ' ');
	if (functionStart_ > -1) for (i=functionStart_; i<stringPos_; i++) if (temp[i] != '\t') temp[i] = '-';
	for (i=tokenStart_; i<stringPos_; i++) temp[i] = '^';
	temp[stringPos_] = '\0';
	// Print current string
	msg.print(" %s\n", stringSource_.get());
	msg.print(" %s^\n", temp);
	delete[] temp;
}

/*
// Character Stream Retrieval
*/

// Return whether the current input stream is a file
CommandParser::ParserSource CommandParser::source()
{
	return source_;
}

// Get next character from current input stream
char CommandParser::getChar()
{
	char c = 0;
	// Are we at the end of the current string?
	if (stringPos_ == stringLength_)
	{
		switch (source_)
		{
			case (CommandParser::StringListSource):
				// Are there any more strings to read in?
				if (stringListSource_ == NULL) return 0;
				stringSource_ = stringListSource_->get();
				stringListSource_ = stringListSource_->next;
				stringLength_ = stringSource_.length();
				stringPos_ = 0;
				break;
			case (CommandParser::StringSource):
				return 0;
				break;
			default:
				break;
		}
	}
	// Return current char
	c = stringSource_[stringPos_];
	stringPos_++;
	return c;
}

// Peek next character from current input stream
char CommandParser::peekChar()
{
	char c = 0;
	switch (source_)
	{
		case (CommandParser::StringListSource):
			if (stringPos_ == stringLength_)
			{
				if (stringListSource_ == NULL) c = 0;
				else c = '\n';
			}
			else c = stringSource_[stringPos_];
			break;
		case (CommandParser::StringSource):
			c = (stringPos_ == stringLength_ ? 0 : stringSource_[stringPos_]);
			break;
		default:
			break;
	}
	return c;
}

// 'Replace' last character read from current input stream
void CommandParser::unGetChar()
{
	switch (source_)
	{
		case (CommandParser::StringListSource):
			if (stringPos_ == 0) printf("Fix Required: Last character from previous string required for unGetChar...\n");
			else stringPos_ --;
			break;
		case (CommandParser::StringSource):
			stringPos_ --;
			break;
		default:
			break;
	}
}

/*
// Tree Generation
*/

// Perform tree generation (base function, called by generateFrom*)
bool CommandParser::generate()
{
	msg.enter("CommandParser::generate");
	expectPathStep_ = false;
	// Perform the parsing
	int result = CommandParser_parse();
	if (result != 0)
	{
		printErrorInfo();
	}
	if (failed_) result = -1;
	msg.exit("CommandParser::generate");
	return (result != 0 ? false : true);
}

// Populate supplied tree with commands
bool CommandParser::generateSingleTree(Tree* t, const char* commands)
{
	msg.enter("CommandParser::generateSingleTree");
	if (t == NULL)
	{
		msg.print("NULL Tree passed to CommandParser::generateSingleTree.\n");
		return false;
	}

	// 'Push' supplied tree onto the stack
	tree_ = t;
	stack_.add(tree_, false);
	// Store the source string
	stringSource_ = commands;
	stringSource_ += ';';
	stringPos_ = 0;
	stringLength_ = stringSource_.length();
	msg.print(Messenger::Verbose, "Parser source string is '%s', length is %i\n", stringSource_.get(), stringLength_);
	source_ = CommandParser::StringSource;
	bool result = generate();
	reset();
	msg.exit("CommandParser::generateSingleTree");
	return result;
}

// Return current tree target, raising warning and setting fail flag if no tree is defined...
Tree *CommandParser::tree()
{
	static Tree dummyTree;
	if (tree_ == NULL)
	{
		failed_ = true;
		msg.print("Internal Error: Parser tried to do something to a non-existent tree.\n");
		return &dummyTree;
	}
	else return tree_;
}

// Pop tree
void CommandParser::popTree()
{
	msg.enter("CommandParser::popTree");
	// If the tree to be popped is a Filter, check that a filter type has been defined
	RefListItem<Tree,bool> *ri = stack_.last();
	msg.print(Messenger::Verbose, "Removing tree %p from stack (%i remain).\n", ri->item, stack_.nItems()-1);
	stack_.remove( stack_.last() );
	// Set current tree target to the top tree now on the stack
	ri = stack_.last();
	tree_ = ri == NULL ? NULL : ri->item;
	msg.exit("CommandParser::popTree");
}

/*
// Pass-Throughs to Tree Functions
*/

// Add integer constant
TreeNode *CommandParser::addConstant(int i)
{
	return tree()->addConstant(i);
}

// Add double constant
TreeNode *CommandParser::addConstant(double d)
{
	return tree()->addConstant(d);
}

// Create a new path on the stack with the specified base 'variable'
TreeNode *CommandParser::createPath(TreeNode *var)
{
	return tree()->createPath(var);
}

// Expand topmost path
bool CommandParser::expandPath(Dnchar *name, TreeNode *arglist)
{
	return tree()->expandPath(name, arglist);
}

// Finalise and remove the topmost path on the stack
TreeNode *CommandParser::finalisePath()
{
	return tree()->finalisePath();
}

// Join two commands together
TreeNode *CommandParser::joinCommands(TreeNode *node1, TreeNode *node2)
{
	return tree()->joinCommands(node1, node2);
}

// Add on a new scope to the stack
TreeNode *CommandParser::pushScope(Command::Function func)
{
	return tree()->pushScope(func);
}

// Pop the topmost scope node
bool CommandParser::popScope()
{
	return tree()->popScope();
}

// Add a node representing a whole statement to the execution list
bool CommandParser::addStatement(TreeNode *leaf)
{
	return tree()->addStatement(leaf);
}

// Add an operator to the Tree
TreeNode *CommandParser::addOperator(Command::Function func, TreeNode *arg1, TreeNode *arg2)
{
	return tree()->addOperator(func, arg1, arg2);
}

// Associate a command-based leaf node to the Tree
TreeNode *CommandParser::addFunctionWithArglist(Command::Function func, TreeNode *arglist)
{
	return tree()->addFunctionWithArglist(func, arglist);
}

// Add a function node to the list (overloaded to accept simple arguments instead of a list)
TreeNode *CommandParser::addFunction(Command::Function func, TreeNode *a1, TreeNode *a2, TreeNode *a3, TreeNode *a4)
{
	return tree()->addFunction(func, a1, a2, a3, a4);
}

// Add a declaration list
TreeNode *CommandParser::addDeclarations(TreeNode *declist)
{
	return tree()->addDeclarations(declist);
}

// Wrap named variable (and array index)
TreeNode *CommandParser::wrapVariable(Variable *var)
{
	return tree()->wrapVariable(var);
}

// Add variable to topmost ScopeNode
TreeNode *CommandParser::addVariable(VTypes::DataType type, Dnchar *name, TreeNode *initialValue)
{
	return tree()->addVariable(type, name, initialValue);
}
