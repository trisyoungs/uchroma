/*
	*** Command Parser
	*** src/parser/parser.h
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

#ifndef UCHROMA_COMMANDPARSER_H
#define UCHROMA_COMMANDPARSER_H

#include "parser/tree.h"
#include "templates/reflist.h"
#include "base/dnchar.h"
#include <fstream>

// Forward declarations
class TreeNode;

// Parser
class CommandParser
{
	public:
	// Constructor / Destructor
	CommandParser();
	~CommandParser();
	// Symbolic tokens - array of corresponding values refers to Bison's tokens
	enum SymbolToken { AssignSymbol, GEQSymbol, LEQSymbol, CNEQSymbol, FNEQSymbol, PlusEqSymbol, MinusEqSymbol, TimesEqSymbol, DivideEqSymbol, PlusPlusSymbol, MinusMinusSymbol, AndSymbol, OrSymbol, nSymbolTokens };
	// Source of parser input
	enum ParserSource { StringSource, StringListSource, nParserSources };
	// Friend declarations
	friend class Program;


	/*
	// Create / Execute
	*/
	private:
	// Character string source
	Dnchar stringSource_;
	// Character string list source
	Dnchar *stringListSource_;
	// Integer position in stringSource, total length of string, and starting position of current token/function
	int stringPos_, stringLength_, tokenStart_, functionStart_;
	// Line number in source file that we've just read
	int lineNumber_;
	// Current input type to parser
	ParserSource source_;
	// Whether the next token to expect is a path step
	bool expectPathStep_;

	public:
	// Reset structure ready for next source
	void reset();
	// Parser lexer, called by yylex()
	int lex();
	// Return current input source
	ParserSource source();
	// Get next character from current input stream
	char getChar();
	// Peek next character from current input stream
	char peekChar();
	// 'Replace' last character read from current input stream
	void unGetChar();
	// Clear all node data
	void clear();
	// Print layout of current tree
	void print();
	// Print error information and location
	void printErrorInfo();
	// Flag that the next token to expect is a path step
	bool setExpectPathStep(bool b);
	// Whether to treat the next alphanumeric token as a path step variable
	bool expectPathStep();

	/*
	// Tree Data
	*/
	private:
	// Current tree (target of node creation)
	Tree *tree_;
	// Stack of created trees
	RefList<Tree,bool> stack_;
	// Flag to indicate failure in tree generation (set by tree() function)
	bool failed_;
	// Perform tree generation (base function, called by generateFrom*)
	bool generate();

	public:
	// Return current tree target, raising warning and setting fail flag if no tree is defined...
	Tree *tree();
	// Pop tree (or function) from stack
	void popTree();
	// Populate supplied tree with commands
	bool generateSingleTree(Tree *t, const char *commands);


	/*
	// Pass-Throughs to Tree Functions
	*/
	public:	
	// Add integer constant
	TreeNode *addConstant(int i);
	// Add double constant
	TreeNode *addConstant(double d);
	// Create a new path on the stack with the specified base 'variable'
	TreeNode *createPath(TreeNode *var);
	// Expand topmost path
	bool expandPath(Dnchar* name, TreeNode* arglist = 0);
	// Finalise and remove the topmost path on the stack
	TreeNode *finalisePath();
	// Join two commands together
	TreeNode *joinCommands(TreeNode* node1, TreeNode* node2);
	// Add on a new scope to the stack
	TreeNode *pushScope(Command::Function func = Command::NoFunction);
	// Pop the topmost scope node
	bool popScope();
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
	// Wrap named variable (and array index)
	TreeNode *wrapVariable(Variable* var);
	// Add variable to topmost ScopeNode
	TreeNode *addVariable(VTypes::DataType type, Dnchar *name, TreeNode *initialValue = NULL);
	// Add array 'constant'
	TreeNode *addArrayConstant(TreeNode *values);
};

// External declaration
extern CommandParser cmdparser;

#endif
