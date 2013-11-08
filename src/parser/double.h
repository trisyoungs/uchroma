/*
	*** Double Variable
	*** src/parser/double.h
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

#ifndef UCHROMA_DOUBLEVARIABLE_H
#define UCHROMA_DOUBLEVARIABLE_H

#include "parser/variable.h"
#include "parser/accessor.h"

// Double Variable
class DoubleVariable : public Variable
{
	public:
	// Constructor / Destructor
	DoubleVariable(double d = 0.0, bool constant = false);
	~DoubleVariable();

	/*
	// Set / Get
	*/
	public:
	// Return value of node
	bool execute(ReturnValue& rv);
	// Set from returnvalue node
	bool set(ReturnValue rv);
	// Reset node
	void reset();

	/*
	// Variable Data
	*/
	private:
	// Real data
	double doubleData_;
	// Print node contents
	void nodePrint(int offset, const char *prefix = "");
};

#endif
