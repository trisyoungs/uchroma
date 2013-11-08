/*
	*** Command Node
	*** src/parser/commandnode.h
	Copyright T. Youngs 2007-2011

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

#ifndef UCHROMA_COMMANDNODE_H
#define UCHROMA_COMMANDNODE_H

#include "templates/reflist.h"
#include "templates/list.h"
#include "templates/vector3.h"
#include "command/commands.h"
#include "parser/treenode.h"

// Forward Declarations
class Tree;

// Command Node
class CommandNode : public TreeNode
{
	public:
	// Constructors / Destructor
	CommandNode(Command::Function func = Command::NoFunction);
	CommandNode(TreeNode *source);
	~CommandNode();

	/*
	// Command Data
	*/
	protected:
	// Command that this node performs
	Command::Function function_;
	
	public:
	// Prepare the stored command function, initialising any data and running any commands
	bool prepFunction();
	// Get command function
	Command::Function function();
	// Execute command
	bool execute(ReturnValue& rv);
	// Print node contents
	void nodePrint(int offset, const char *prefix = "");
	// Set from returnvalue node
	bool set(ReturnValue rv);
	// Initialise node
	bool initialise();
	// Create, run, and free a single command with simple arguments
	static bool run(Command::Function func, const char *arglist, ...);
};

#endif
