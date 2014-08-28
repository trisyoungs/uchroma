/*
	*** Tree
	*** src/parser/tree.cpp
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
#include "parser/scopenode.h"
#include "parser/commandnode.h"
#include "parser/variablenode.h"
#include "parser/stepnode.h"
#include "parser/grammar.hh"
#include "parser/parser.h"
#include "parser/tree.h"
#include "parser/integer.h"
#include "parser/double.h"
#include "base/sysfunc.h"
#include "base/messenger.h"
#include <stdarg.h>
#include <string.h>

// Constructors
Tree::Tree()
{
	// Private variables
	acceptedFail_ = Command::NoFunction;
	rootNode_ = NULL;
	generateMissingVariables_ = false;

	// Initialise
	clear();
}

Tree::Tree(const char *commands)
{
	// Private variables
	acceptedFail_ = Command::NoFunction;
	rootNode_ = NULL;

	// Initialise
	clear();

	cmdparser.generateSingleTree(this, commands);
}

// Destructor
Tree::~Tree()
{
	clear();
}

// Set return type of tree
void Tree::setReturnType(VTypes::DataType dt)
{
	returnType_ = dt;
}

// Return return-type of tree
VTypes::DataType Tree::returnType() const
{
	return returnType_;
}

/*
// Create / Execute
*/

// Reset Tree, ready for new statement(s) to be added
void Tree::reset()
{
	msg.enter("Tree::reset");
	// Remove all nodes and statements except the first (which was the original root ScopeNode)
	nodes_.disown(rootNode_);
	nodes_.clear();
	scopeStack_.clear();
	statements_.clear();

	// Re-own the root node
	nodes_.own(rootNode_);
	scopeStack_.add(rootNode_);
	statements_.add(rootNode_);
	msg.exit("Tree::reset");
}

// Clear contents of tree
void Tree::clear(bool keepGlobalVariables)
{
	nodes_.clear();
	statements_.clear();
	scopeStack_.clear();
	rootNode_ = NULL;
	if (!keepGlobalVariables) globalScope_.variables.clear();

	// Create new root ScopNode
	rootNode_ = new ScopeNode(Command::NoFunction);
	rootNode_->setParent(this);
	nodes_.own(rootNode_);
	scopeStack_.add(rootNode_);
	statements_.add(rootNode_);
}

// Set flag to specify that missing variables should be generated
void Tree::setGenerateMissingVariables(bool generate)
{
	generateMissingVariables_ = generate;
}

// Return whether missing variables will be generated
bool Tree::generateMissingVariables()
{
	return generateMissingVariables_;
}

// Set commands in Tree
bool Tree::setCommands(QString commands)
{
	return cmdparser.generateSingleTree(this, qPrintable(commands));
}

// Add variable to global Tree scope
DoubleVariable* Tree::addGlobalVariable(const char* name)
{
	DoubleVariable* var = new DoubleVariable();
	var->setName(name);
	globalScope_.variables.take(var);
	return var;
}

// Remove variable from global Tree scope
void Tree::removeGlobalVariable(Variable* var)
{
	globalScope_.variables.remove(var);
}

// Set function for accepted fail
void Tree::setAcceptedFail(Command::Function func)
{
	if ((acceptedFail_ != Command::NoFunction) && (func != Command::NoFunction)) printf("Warning: An acceptedFail command is already set...\n");
	acceptedFail_ = func;
}

// Clear accepted fail bit
Command::Function Tree::acceptedFail() const
{
	return acceptedFail_;
}

// Execute tree
double Tree::execute()
{
	msg.enter("Tree::execute");
	int result = 0;
	ReturnValue rv;
	acceptedFail_ = Command::NoFunction;

	for (RefListItem<TreeNode,int> *ri = statements_.first(); ri != NULL; ri = ri->next)
	{
		msg.print(Messenger::Verbose, "Executing tree statement %p...\n", ri->item);
// 		ri->item->nodePrint(1);
		result = ri->item->execute(rv);
		// Catch failures arising from 'return' statements
		if (acceptedFail_ == Command::Return)
		{
			msg.print(Messenger::Verbose, "Execution of tree ended early because we returned.\n");
			result = 1;
			break;
		}
		if (result != 1) break;
	}

	// Print some final verbose output
	msg.print(Messenger::Verbose, "Final result from execution of tree %s\n", rv.info());
	if (result == 0) msg.print(Messenger::Verbose, "Execution FAILED.\n");
	msg.exit("Tree::execute");
	return rv.asDouble();
}

// Print tree
void Tree::print()
{
	printf("Leaf Structure (%i statements):\n", statements_.nItems());
	int n=1;
	for (RefListItem<TreeNode,int> *ri = statements_.first(); ri != NULL; ri = ri->next)
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
// Statements / Commands / Operators
*/

// Add a node representing a whole statement to the execution list
bool Tree::addStatement(TreeNode *leaf)
{
	if (leaf == NULL)
	{
		printf("Internal Error: NULL TreeNode passed to Tree::addStatement().\n");
		return false;
	}
	msg.print(Messenger::Verbose, "Added statement node %p\n", leaf);
	leaf->setParent(this);
	statements_.add(leaf);
	return true;
}

// Add an operator to the Tree
TreeNode *Tree::addOperator(Command::Function func, TreeNode *arg1, TreeNode *arg2)
{
	msg.enter("Tree::addOperator");
	// Check compatibility between supplied nodes and the operator, since we didn't check the types in the lexer.
	VTypes::DataType rtype;
	if (arg2 == NULL) rtype = checkUnaryOperatorTypes(func, arg1->returnType());
	else rtype = checkBinaryOperatorTypes(func, arg1->returnType(), arg2->returnType());
	if (rtype == VTypes::NoData) return NULL;

	// Create new command node
	CommandNode *leaf = new CommandNode(func);
	nodes_.own(leaf);
	msg.print(Messenger::Verbose, "Added operator '%s' (%p)...\n", Command::data[func].keyword, leaf);
	// Add arguments and set parent
	leaf->addArguments(1,arg1);
	leaf->setParent(this);
	if (arg2 != NULL) leaf->addArguments(1,arg2);
	leaf->setReturnType(rtype);
	msg.exit("Tree::addOperator");
	return leaf;
}

// Add function-based leaf node to topmost branch on stack
TreeNode *Tree::addFunctionWithArglist(Command::Function func, TreeNode *arglist)
{
	msg.enter("Tree::addFunctionWithArglist");
	// Create new command node
	CommandNode *leaf = new CommandNode(func);
	nodes_.own(leaf);
	msg.print(Messenger::Verbose, "Added function '%s' (%p)...\n", Command::data[func].keyword, leaf);
	// Add argument list to node and set parent
	leaf->addJoinedArguments(arglist);
	leaf->setParent(this);
	// Store the function's return type
	leaf->setReturnType(Command::data[func].returnType);
	// Check that the correct arguments were given to the command and run any prep functions
	if (!leaf->checkArguments(Command::data[func].arguments, Command::data[func].keyword))
	{
		msg.print("Error: Function syntax is '%s(%s)'.\n", Command::data[func].keyword, Command::data[func].argText);
		leaf = NULL;
	}
	else if (!leaf->prepFunction()) leaf = NULL;
	msg.exit("Tree::addFunctionWithArglist");
	return leaf;
}

// Add a function node to the list (overloaded to accept simple arguments instead of a list)
TreeNode *Tree::addFunction(Command::Function func, TreeNode *a1, TreeNode *a2, TreeNode *a3, TreeNode *a4)
{
	msg.enter("Tree::addFunction");
	// Create new command node
	CommandNode *leaf = new CommandNode(func);
	nodes_.own(leaf);
	msg.print(Messenger::Verbose, "Added function '%s' (%p)...\n", Command::data[func].keyword, leaf);
	if (a1 != NULL) leaf->addArgument(a1);
	if (a2 != NULL) leaf->addArgument(a2);
	if (a3 != NULL) leaf->addArgument(a3);
	if (a4 != NULL) leaf->addArgument(a4);
	leaf->setParent(this);
	// Store the function's return type
	leaf->setReturnType(Command::data[func].returnType);
	// Check that the correct arguments were given to the command and run any prep functions
	if (!leaf->checkArguments(Command::data[func].arguments, Command::data[func].keyword))
	{
		msg.print("Error: Function syntax is '%s(%s)'.\n", Command::data[func].keyword, Command::data[func].argText);
		leaf = NULL;
	}
	else if (!leaf->prepFunction()) leaf = NULL;
	msg.exit("Tree::addFunction");
	return leaf;
}

// Add a declaration list
TreeNode *Tree::addDeclarations(TreeNode *declist)
{
	msg.enter("Tree::addDeclarations");
	// Create new command node
	CommandNode *leaf = new CommandNode(Command::Declarations);
	nodes_.own(leaf);
	msg.print(Messenger::Verbose, "Added declarations node (%p)...\n", leaf);
	// Add argument list to node and set parent
	leaf->addJoinedArguments(declist);
	leaf->setParent(this);
	// Check that the correct arguments were given to the command and run any prep functions
	if (!leaf->checkArguments(Command::data[Command::Declarations].arguments, Command::data[Command::Declarations].keyword)) leaf = NULL;
	msg.exit("Tree::addDeclarations");
	return leaf;
}

// Link two arguments together with their member pointers
TreeNode *Tree::joinArguments(TreeNode *arg1, TreeNode *arg2)
{
	arg1->prevArgument = arg2;
	arg2->nextArgument = arg1;
	msg.print(Messenger::Verbose, "Joining arguments %p and %p\n", arg1, arg2);
	return arg1;
}

// Join two commands together
TreeNode *Tree::joinCommands(TreeNode *node1, TreeNode *node2)
{
	CommandNode *leaf = new CommandNode(Command::Joiner);
	nodes_.own(leaf);
	leaf->setParent(this);
	if (node1 != NULL) leaf->addArgument(node1);
	if (node2 != NULL) leaf->addArgument(node2);
	msg.print(Messenger::Verbose, "Joined command nodes %p and %p (joiner node is %p)\n", node1, node2, leaf);
	return leaf;
}

// Add on a new scope to the stack
TreeNode *Tree::pushScope(Command::Function func)
{
	ScopeNode *node = new ScopeNode();
	nodes_.own(node);
	scopeStack_.add(node,func);
	msg.print(Messenger::Verbose, "ScopeNode %p is pushed.\n", node);
	return node;
}

// Pop the topmost scope node
bool Tree::popScope()
{
	RefListItem<ScopeNode,int> *ri = scopeStack_.last();
	if (ri == NULL)
	{
		printf("Internal Error: No scoped node to pop from stack.\n");
		return false;
	}
	ScopeNode *temp = ri->item;
	scopeStack_.remove(ri);
	msg.print(Messenger::Verbose, "ScopeNode %p is popped.\n", temp);
	return true;
}

/*
// Variables / Constants
*/

// Add constant value to tompost scope
TreeNode *Tree::addConstant(VTypes::DataType type, Dnchar *token)
{
	if (type == VTypes::IntegerData)
	{
		IntegerVariable *var = new IntegerVariable(atoi(token->get()), true);
		nodes_.own(var);
		return var;
	}
	else if (type == VTypes::DoubleData)
	{
		DoubleVariable *var = new DoubleVariable(atof(token->get()), true);
		nodes_.own(var);
		return var;
	}
	else printf("Internal Error: Don't know how to create a constant of type '%s' for Tree.\n", VTypes::dataType(type));
	return NULL;
}

// Add integer constant
TreeNode *Tree::addConstant(int i)
{
	IntegerVariable *var = new IntegerVariable(i, true);
	nodes_.own(var);
	return var;
}

// Add double constant
TreeNode *Tree::addConstant(double d)
{
	DoubleVariable *var = new DoubleVariable(d, true);
	nodes_.own(var);
	return var;
}

// Add variable to topmost scope
Variable *Tree::addVariable(VTypes::DataType type, Dnchar *name, TreeNode *initialValue)
{
	msg.print(Messenger::Verbose, "A new variable '%s' is being created with type %s.\n", name->get(), VTypes::dataType(type));
	// Get topmost scopenode
// 	printf("nscope = %i, %p  %p\n", scopeStack_.nItems(), scopeStack_.first(), scopeStack_.last());
	RefListItem<ScopeNode,int> *ri = scopeStack_.last();
	if (ri == NULL)
	{
		printf("Internal Error: No current scope in which to define variable '%s'.\n", name->get());
		return NULL;
	}

	// Create the supplied variable in the list of the topmost scope
	Variable *var = ri->item->variables.create(type, name->get(), initialValue);
	if (!var)
	{
// 		printf("Failed to create variable '%s' in local scope.\n", name->get());
		return NULL;
	}
	msg.print(Messenger::Verbose, "Created variable '%s' in scopenode %p\n", name->get(), ri->item);
	return var;
}

// Search for variable in current scope
Variable *Tree::findVariableInScope(const char *name, int &scopelevel)
{
	Variable *result = NULL;
	scopelevel = 0;
	msg.print(Messenger::Verbose, "Searching scope for variable '%s'...\n", name);

	// Search global scope first
	result = globalScope_.variables.find(name);

	// If not in global scope, search the current ScopeNode list for the variable name requested
	if (!result) for (RefListItem<ScopeNode,int> *ri = scopeStack_.last(); ri != NULL; ri = ri->prev)
	{
		msg.print(Messenger::Verbose, " ... scopenode %p...\n", ri->item);
		result = ri->item->variables.find(name);
		if (result != NULL) break;
		scopelevel --;
	}
	if (result == NULL) msg.print(Messenger::Verbose, "...variable '%s' not found in any scope.\n", name);
	else msg.print(Messenger::Verbose, "...variable '%s' found at a scope level of %i.\n", name, scopelevel);
	return result;
}

// Wrap named variable (and array index)
TreeNode *Tree::wrapVariable(Variable *var)
{
	VariableNode *vnode = new VariableNode(var);
	nodes_.own(vnode);
	vnode->setParent(this);
	return vnode;
}

/*
// Paths
*/

// Return root ScopeNode
ScopeNode* Tree::rootNode()
{
	return rootNode_;
}

// Create a new path on the stack
TreeNode *Tree::createPath(TreeNode *node)
{
	msg.enter("Tree::createPath");
	VariableNode *vnode = (VariableNode*) node;
	pathStack_.add(vnode, vnode);
// 	nodes_.own(vnode);	// Should not be called, since the passed *node is already owned by the tree
	msg.print(Messenger::Verbose, "A new path has been started, beginning from variable '%s'.\n", vnode->name());
	msg.exit("Tree::createPath");
	return vnode;
}

// Pop topmost path from stack
TreeNode *Tree::finalisePath()
{
	msg.enter("Tree::finalisePath");
	// Finalise the path before we remove it
	RefListItem<VariableNode,TreeNode*> *ri = pathStack_.last();
	if (ri == NULL)
	{
		msg.print("Internal Error: No path on stack to finalise.\n");
		return NULL;
	}
	ri->item->finalisePath();
	TreeNode *result = ri->item;
	msg.print(Messenger::Verbose, "Path beginning from variable '%s' has been finalised.\n", ri->item->name());
	pathStack_.remove(ri);
	msg.exit("Tree::finalisePath");
	return result;
}

// Expand the topmost path on the stack
bool Tree::expandPath(Dnchar *name, TreeNode *arglist)
{
	msg.enter("Tree::expandPath");
	// Get last item on path stack
	RefListItem<VariableNode,TreeNode*> *ri = pathStack_.last();
	if (ri == NULL)
	{
		printf("Internal Error: No path on stack to expand with accessor '%s'.\n", name->get());
		return false;
	}
	msg.print(Messenger::Verbose, "Tree is evaluating accessor '%s' as step %i from the basenode '%s'...\n", name->get(), ri->item->nArgs()+1, ri->item->name());
	// If the last step was an array and an array index was not give, we complain!
	if (ri->item != ri->data)
	{
		StepNode *laststep = (StepNode*) ri->data;
		if ((laststep->arraySize() > 0) && (laststep->arrayIndex() == NULL))
		{
			msg.print("Previous step in path requires an array index to be specified.\n");
			msg.exit("Tree::expandPath");
			return false;
		}
	}
	// Find next step accessor
	StepNode *result = ri->data->findAccessor(name->get(), arglist);
	// If we found a valid accessor, update the pathstack entry
	if (result)
	{
		msg.print(Messenger::Verbose, "...OK - matching accessor found: return type is %s\n", VTypes::dataType(result->returnType()));
		ri->data = (TreeNode*) result;
		nodes_.own(result);
		// Finalise the path before we remove it
		RefListItem<VariableNode,TreeNode*> *ri = pathStack_.last();
		if (ri == NULL)
		{
			msg.print("Internal Error: No path on stack to expand!\n");
			msg.exit("Tree::expandPath");
			return false;
		}
		ri->item->addArgument(result);
	}
// 	else msg.print("Error: Object of type '%s' has no matching accessor for '%s'.\n", VTypes::dataType(ri->data->returnType()), name->get());
	msg.exit("Tree::expandPath");
	return result;
}

// Return first in stack of scopenodes
RefListItem<ScopeNode,int> *Tree::scopeNodes()
{
	return scopeStack_.first();
}

// Return global scope
ScopeNode* Tree::globalScope()
{
	return &globalScope_;
}
