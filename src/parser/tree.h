/*
	*** Tree
	*** src/parser/tree.h
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

#ifndef UCHROMA_TREE_H
#define UCHROMA_TREE_H

#include <iostream>
#include "parser/scopenode.h"
#include "parser/returnvalue.h"
#include "parser/variable.h"
#include "command/commands.h"
#include "templates/list.h"
#include "templates/reflist.h"
#include "base/dnchar.h"
#include <QtCore/QString>

// Forward declarations
class TreeNode;
class VariableNode;
class StepNode;
class DataSetReference;

// Tree
class Tree
{
	public:
	// Constructor / Destructor
	Tree();
	Tree(const char* commands);
	~Tree();


	/*
	 * Tree Character
	 */
	private:
	// Return type (used if defined as a function)
	VTypes::DataType returnType_;

	public:
	// Set return type of tree
	void setReturnType(VTypes::DataType dt);
	// Return return-type of tree
	VTypes::DataType returnType() const;


	/*
	 * Create / Execute
	 */
	private:
	// Flag to indicate that recent failure of this token is known and we should continue
	Command::Function acceptedFail_;
	// Flag to specify that missing variables should be generated
	bool generateMissingVariables_;
	
	public:
	// Reset Tree, ready for new statement(s) to be added
	void reset();
	// Clear all data contained in the Tree
	void clear(bool keepGlobalVariables = false);
	// Set whether missing variables should be generated
	void setGenerateMissingVariables(bool generate);
	// Return whether missing variables will be generated
	bool generateMissingVariables();
	// Set commands in Tree
	bool setCommands(QString commands);
	// Add variable to global Tree scope
	DoubleVariable* addGlobalVariable(const char* name);
	// Set function for accepted fail
	void setAcceptedFail(Command::Function func);
	// Return function for accepted fail
	Command::Function acceptedFail() const;
	// Execute
	double execute();


	/*
	 * Node Data
	 */
	private:
	// Permanent global ScopeNode
	ScopeNode globalScope_;
	// Root ScopeNode
	ScopeNode* rootNode_;
	// Node list - a disordered list of all nodes owned by the Tree
	List<TreeNode> nodes_;
	// Reflist of all statements in the Tree, to be executed sequentially
	RefList<TreeNode,int> statements_;
	// Stack of ScopeNodes
	RefList<ScopeNode,int> scopeStack_;
	// Reflist of DataSet References
	RefList<DataSetReference,int> referenceStack_;
	// Stack of variable paths (and last added stepnode)
	RefList<VariableNode,TreeNode*> pathStack_;
	// Number of syntactic errors encountered
	int nErrors_;
	// Check unary operator type compatibility
	VTypes::DataType checkUnaryOperatorTypes(Command::Function func, VTypes::DataType type);
	// Check binary operator type compatibility
	VTypes::DataType checkBinaryOperatorTypes(Command::Function func, VTypes::DataType type1, VTypes::DataType type2);
	
	public:
	// Return root ScopeNode
	ScopeNode* rootNode();
	// Create a new path on the stack with the specified base 'variable'
	TreeNode *createPath(TreeNode *var);
	// Expand topmost path
	bool expandPath(Dnchar* name, TreeNode* arglist = 0);
	// Finalise and remove the topmost path on the stack
	TreeNode *finalisePath();
	// Return number of arguments defined (for function)
	int nArgs() const;
	// Return first argument defined (for function)
	TreeNode *args() const;
	// Return first in stack of scopenodes
	RefListItem<ScopeNode,int> *scopeNodes();
	// Return global scope
	ScopeNode* globalScope();
	

	/*
	 * Statement / Command Addition
	 */
	public:
	// Add a node representing a whole statement to the execution list
	bool addStatement(TreeNode *leaf);
	// Add an operator to the Tree
	TreeNode *addOperator(Command::Function func, TreeNode *arg1, TreeNode *arg2 = NULL);
	// Associate a command-based leaf node to the Tree
	TreeNode *addFunctionWithArglist(Command::Function func, TreeNode *arglist);
	// Add a function node to the list (overloaded to accept simple arguments instead of a list)
	TreeNode *addFunction(Command::Function func, TreeNode *a1 = NULL, TreeNode *a2 = NULL, TreeNode *a3 = NULL, TreeNode *a4 = NULL);
	// Add a declaration list
	TreeNode *addDeclarations(TreeNode *declist);
	// Join two nodes together
	static TreeNode *joinArguments(TreeNode *arg1, TreeNode *arg2);
	// Join two commands together
	TreeNode *joinCommands(TreeNode *node1, TreeNode *node2);
	// Add on a new scope to the stack
	TreeNode *pushScope(Command::Function func = Command::NoFunction);
	// Pop the topmost scope node
	bool popScope();
	// Print statement info
	void print();


	/*
	 * Variables / Constants
	 */
	public:
	// Add constant value to tompost scope
	TreeNode *addConstant(VTypes::DataType type, Dnchar *token);
	// Add integer constant
	TreeNode *addConstant(int i);
	// Add double constant
	TreeNode *addConstant(double d);
	// Add variable to topmost ScopeNode
	Variable *addVariable(VTypes::DataType type, Dnchar* name, TreeNode* initialValue = 0);
	// Search for variable in current scope
	Variable *findVariableInScope(const char *name, int &scopelevel);
	// Wrap named variable (and array index)
	TreeNode *wrapVariable(Variable *var);
};

#endif
