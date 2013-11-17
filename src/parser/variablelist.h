/*
	*** Variable List
	*** src/parser/variablelist.h
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

#ifndef UCHROMA_VARIABLELIST_H
#define UCHROMA_VARIABLELIST_H

#include "parser/variable.h"
#include "math/constants.h"
#include "templates/list.h"

// Forward Declarations
class IntegerVariable;
class DoubleVariable;

// Variable list
class VariableList
{
	/*
	// Variable List
	*/
	public:
	// Constructor / Destructor
	VariableList();

	private:
	// List of variables
	List<Variable> variables_;
	// List of constants
	List<Variable> constants_;
	// Create variable of specified type
	static Variable *makeVariable(VTypes::DataType type, const char *name, TreeNode *initialValue = NULL);

	public:
	// Pass a newly-created variable / constant to the list for it to take ownership of
	void take(Variable *v, bool forcevariable = false);
	// Retrieve a named variable from the list
	Variable *find(const char *name) const;
	// Create a new variable in the list
	Variable *create(VTypes::DataType type, const char *name, TreeNode *initialValue = NULL);
	// Create a new variable (static function, so we don't take ownership of it).
	Variable *createFree(VTypes::DataType type, const char *name, TreeNode *initialValue = NULL);
	// Return the number of variables (not constants) contained in the list
	int nVariables() const;
	// Return first variable in the list
	Variable *variables() const;
	// Return specified variable in the list
	Variable *variable(int index);
	// Reset all variable values
	bool initialise();
	// Clear all variables and constants
	void clear();
	// Print list of variables and their values
	void print() const;
};

#endif