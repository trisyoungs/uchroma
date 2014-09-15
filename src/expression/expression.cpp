/*
	*** Expression
	*** src/expression/expression.cpp
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
#include "expression/variable.h"
#include "expression/functionnode.h"
#include "expression/expression_grammar.hh"
#include "expression/variablenode.h"
#include "base/sysfunc.h"
#include "base/messenger.h"
#include <stdarg.h>
#include <string.h>

// Static members
Expression* Expression::target_ = NULL;

// Constructors
Expression::Expression()
{
	// Private variables
	generateMissingVariables_ = false;

	// Initialise
	clear();
}

Expression::Expression(const char* commands)
{
	// Initialise
	clear();

	// Generate expression
	generate(commands);
}

// Destructor
Expression::~Expression()
{
	clear();
}

/*
 * Creation
 */

// Reset values in parser, ready for next source
void Expression::resetParser()
{
	stringPos_ = -1;
	tokenStart_ = 0;
	functionStart_ = -1;
	stringSource_.clear();
	stringLength_ = 0;
	useAdditionalConstants_ = false;
	target_ = NULL;
}

// Clear contents of expression
void Expression::clear()
{
	nodes_.clear();
	statements_.clear();

	// Clear variables and constants, except those that are flagged as 'permanent'
	variables_.removeIfData(false);
	constants_.removeIfData(false);
}

// Set flag to specify that missing variables should be generated
void Expression::setGenerateMissingVariables(bool generate)
{
	generateMissingVariables_ = generate;
}

// Return whether missing variables will be generated
bool Expression::generateMissingVariables()
{
	return generateMissingVariables_;
}

// Print error information and location
void Expression::printErrorInfo()
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

// Get next character from current input stream
char Expression::getChar()
{
	char c = 0;

	// Are we at the end of the current string?
	if (stringPos_ == stringLength_) return 0;

	// Return current char
	c = stringSource_[stringPos_];
	stringPos_++;
	return c;
}

// Peek next character from current input stream
char Expression::peekChar()
{
	return (stringPos_ == stringLength_ ? 0 : stringSource_[stringPos_]);
}

// 'Replace' last character read from current input stream
void Expression::unGetChar()
{
	--stringPos_;
}

// Generate an expression
bool Expression::generate(const char* expressionText, bool quiet)
{
	msg.enter("Expression::generate");

	resetParser();
	target_ = this;

	stringSource_ = expressionText;
	stringSource_ += ';';
	stringPos_ = 0;
	stringLength_ = stringSource_.length();
	msg.print(Messenger::Verbose, "Parser source string is '%s', length is %i\n", stringSource_.get(), stringLength_);

	// Perform the parsing
	int result = ExpressionParser_parse();
	if ((result != 0) && (!quiet)) printErrorInfo();

	target_ = NULL;

	msg.exit("Expression::generate");
	return (result != 0 ? false : true);
}

// Return current expression target
Expression *Expression::target()
{
	return target_;
}

/*
 * Execution
 */

// Print expression
void Expression::print()
{
	printf("Leaf Structure (%i statements):\n", statements_.nItems());
	int n=1;
	for (RefListItem<Node,int> *ri = statements_.first(); ri != NULL; ri = ri->next)
	{
		printf("-------------------------------------------------------------\n");
		printf("Statement %i:\n", n);
		printf("item pointer is %p\n", ri->item);
		ri->item->nodePrint(1);
		n ++;
	}
	printf("-------------------------------------------------------------\n");
}

/*
 * Statement / Command Addition
 */

// Add a node representing a whole statement to the execution list
bool Expression::addStatement(Node* leaf)
{
	if (leaf == NULL)
	{
		printf("Internal Error: NULL Node passed to Expression::addStatement().\n");
		return false;
	}
	msg.print(Messenger::Verbose, "Added statement node %p\n", leaf);
	leaf->setParent(this);
	statements_.add(leaf);
	return true;
}

// Add an operator to the Expression
Node* Expression::addOperator(Functions::Function func, Node* arg1, Node* arg2)
{
	msg.enter("Expression::addOperator");
	if ((!arg1->returnsNumber()) || (!arg2->returnsNumber())) return NULL;

	// Create new command node
	FunctionNode* leaf = new FunctionNode(func);
	nodes_.own(leaf);
	msg.print(Messenger::Verbose, "Added operator '%s' (%p)...\n", Functions::data[func].keyword, leaf);
	// Add arguments and set parent
	leaf->addArguments(1,arg1);
	leaf->setParent(this);
	if (arg2 != NULL) leaf->addArguments(1,arg2);
	leaf->setReturnsNumber(Functions::data[func].returnsNumber);
	msg.exit("Expression::addOperator");
	return leaf;
}

// Add function-based leaf node to topmost branch on stack
Node* Expression::addFunctionNodeWithArglist(Functions::Function func, Node* arglist)
{
	msg.enter("Expression::addFunctionWithArglist");
	// Create new command node
	FunctionNode* leaf = new FunctionNode(func);
	nodes_.own(leaf);
	msg.print(Messenger::Verbose, "Added function '%s' (%p)...\n", Functions::data[func].keyword, leaf);
	// Add argument list to node and set parent
	leaf->addJoinedArguments(arglist);
	leaf->setParent(this);
	// Store the function's return type
	leaf->setReturnsNumber(Functions::data[func].returnsNumber);
	// Check that the correct arguments were given to the command and run any prep functions
	if (!leaf->checkArguments(Functions::data[func].arguments, Functions::data[func].keyword))
	{
		msg.print(Messenger::Verbose, "Error: Function syntax is '%s(%s)'.\n", Functions::data[func].keyword, Functions::data[func].argText);
		leaf = NULL;
	}
	msg.exit("Expression::addFunctionWithArglist");
	return leaf;
}

// Add a function node to the list (overloaded to accept simple arguments instead of a list)
Node* Expression::addFunctionNode(Functions::Function func, Node* a1, Node* a2, Node* a3, Node* a4)
{
	msg.enter("Expression::addFunction");
	// Create new command node
	FunctionNode* leaf = new FunctionNode(func);
	nodes_.own(leaf);
	msg.print(Messenger::Verbose, "Added function '%s' (%p)...\n", Functions::data[func].keyword, leaf);
	if (a1 != NULL) leaf->addArgument(a1);
	if (a2 != NULL) leaf->addArgument(a2);
	if (a3 != NULL) leaf->addArgument(a3);
	if (a4 != NULL) leaf->addArgument(a4);
	leaf->setParent(this);

	// Store the function's return type
	leaf->setReturnsNumber(Functions::data[func].returnsNumber);

	// Check that the correct arguments were given to the command and run any prep functions
	if (!leaf->checkArguments(Functions::data[func].arguments, Functions::data[func].keyword))
	{
		msg.print(Messenger::Verbose, "Error: Function syntax is '%s(%s)'.\n", Functions::data[func].keyword, Functions::data[func].argText);
		leaf = NULL;
	}

	msg.exit("Expression::addFunction");
	return leaf;
}

// Add variable node targetting specified variable
Node* Expression::addVariableNode(Variable *var)
{
	VariableNode* vnode = new VariableNode(var);
	nodes_.own(vnode);
	vnode->setParent(this);
	return vnode;
}

// Link two arguments together with their member pointers
Node* Expression::joinArguments(Node* arg1, Node* arg2)
{
	arg1->prevArgument = arg2;
	arg2->nextArgument = arg1;
	msg.print(Messenger::Verbose, "Joining arguments %p and %p\n", arg1, arg2);
	return arg1;
}

// Join two commands together
Node* Expression::joinCommands(Node* node1, Node* node2)
{
	FunctionNode* leaf = new FunctionNode(Functions::Joiner);
	nodes_.own(leaf);
	leaf->setParent(this);
	if (node1 != NULL) leaf->addArgument(node1);
	if (node2 != NULL) leaf->addArgument(node2);
	msg.print(Messenger::Verbose, "Joined command nodes %p and %p (joiner node is %p)\n", node1, node2, leaf);
	return leaf;
}

/*
 * Variables / Constants
 */

// Add double constant
Variable* Expression::createConstant(double d, bool permanent)
{
	Variable *var = new Variable();
	var->set(d);
	var->setReadOnly();
	if (permanent) permanentNodes_.own(var);
	else nodes_.own(var);
	constants_.add(var, permanent);
	return var;
}

// Add variable to topmost scope
Variable* Expression::createVariable(const char* name, Node* initialValue, bool permanent)
{
	msg.print(Messenger::Verbose, "A new variable '%s' is being created.\n", name);

	Variable* var = new Variable;
	var->setName(name);
	if (!var->setInitialValue(initialValue))
	{
		delete var;
		msg.print("Failed to set initial value for variable.\n");
		return NULL;
	}

	if (permanent) permanentNodes_.own(var);
	else nodes_.own(var);
	variables_.add(var, permanent);
	msg.print(Messenger::Verbose, "Created variable '%s'.\n", name);

	return var;
}

// Search for variable in current scope
Variable *Expression::variable(const char* name)
{
	// Search global scope first
	Variable* result = NULL;

	for (RefListItem<Variable,bool>* ri = variables_.first(); ri != NULL; ri = ri->next)
	{
		if (strcmp(ri->item->name(),name) == 0)
		{
			result = ri->item;
			break;
		}
	}

	if (result == NULL) msg.print(Messenger::Verbose, "...variable '%s' not found.\n", name);
	else msg.print(Messenger::Verbose, "...variable '%s' found.\n", name);
	return result;
}

// Return variables
RefListItem<Variable,bool>* Expression::variables()
{
	return variables_.first();
}

// Return constants
RefListItem<Variable,bool>* Expression::constants()
{
	return constants_.first();
}

/*
 * Execution
 */

// Execute expression
double Expression::execute(bool& success)
{
	double expressionResult;

	for (RefListItem<Node,int> *ri = statements_.first(); ri != NULL; ri = ri->next)
	{
		msg.print(Messenger::Verbose, "Executing expression statement %p...\n", ri->item);
// 		ri->item->nodePrint(1);
		success = ri->item->execute(expressionResult);
		if (!success) break;
	}

	// Print some final verbose output
// 	msg.print("Final result from expression = %f\n", expressionResult);
	if (!success) msg.print("Warning: Execution FAILED.\n");

	return expressionResult;
}
