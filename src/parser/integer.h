/*
	*** Integer Variable
	*** src/parser/integer.h
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

#ifndef UCHROMA_INTEGERVARIABLE_H
#define UCHROMA_INTEGERVARIABLE_H

#include "parser/variable.h"
#include "parser/accessor.h"

// Integer Variable
class IntegerVariable : public Variable
{
	public:
	// Constructor / Destructor
	IntegerVariable(int i = 0, bool constant = false);
	~IntegerVariable();

	/*
	// Set / Get
	*/
	public:
	// Return value of node
	bool execute(ReturnValue& rv);
	// Set from returnvalue node
	bool set(ReturnValue rv);
	// Reset variable
	void reset();

	/*
	// Variable Data
	*/
	private:
	// Integer data
	int integerData_;
	// Print node contents
	void nodePrint(int offset, const char *prefix);
};

#endif

