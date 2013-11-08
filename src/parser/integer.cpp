/*
	*** Integer Variable and Array
	*** src/parser/integer.cpp
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

#include "parser/integer.h"
#include "math/constants.h"
#include <string.h>

/*
// Variable
*/

// Constructor
IntegerVariable::IntegerVariable(int i, bool constant) : integerData_(i)
{
	// Private variables
	returnType_ = VTypes::IntegerData;
	readOnly_ = constant;
}

// Destructor
IntegerVariable::~IntegerVariable()
{
}

/*
// Set / Get
*/

// Set from returnvalue node
bool IntegerVariable::set(ReturnValue rv)
{
	if (readOnly_)
	{
		printf("A constant value (in this case an integer) cannot be assigned to.\n");
		return false;
	}
	bool success;
	integerData_ = rv.asInteger(success);
	return success;
}

// Reset variable
void IntegerVariable::reset()
{
	integerData_ = 0;
}


// Return value of node
bool IntegerVariable::execute(ReturnValue& rv)
{
	rv.set(integerData_);
	return true;
}

// Print node contents
void IntegerVariable::nodePrint(int offset, const char *prefix)
{
	// Construct tabbed offset
	Dnchar tab(offset+32);
	for (int n=0; n<offset-1; n++) tab += '\t';
	if (offset > 1) tab.strcat("   |--> ");
	tab.strcat(prefix);

	// Output node data
	if (readOnly_) printf("[C]%s%i (constant value)\n", tab.get(), integerData_);
	else printf("[V]%s%i (variable, name=%s)\n", tab.get(), integerData_, name_.get());
}
