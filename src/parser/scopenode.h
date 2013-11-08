/*
	*** Scoped Command Node
	*** src/parser/scopenode.h
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

#ifndef UCHROMA_SCOPENODE_H
#define UCHROMA_SCOPENODE_H

#include "parser/commandnode.h"
#include "parser/variablelist.h"

// Scoped Command Node
class ScopeNode : public CommandNode
{
	public:
	// Constructor / Destructor
	ScopeNode(Command::Function func = Command::NoFunction);
	~ScopeNode();

	/*
	// Variables
	*/
	public:
	// List of variables (and constants) owned by this scope
	VariableList variables;

	/*
	// Set / Get / Execute
	*/
	public:
	// Execute command
	bool execute(ReturnValue &rv);
	// Set from returnvalue node
	bool set(ReturnValue &rv);
	// Initialise node
	bool initialise();
	// Print layout of current node
	void nodePrint(int offset, const char *prefix = "");

};

#endif
