/*
	*** Double Variable
	*** src/parser/double.cpp
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

#include "parser/double.h"
#include "math/constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
// Variable
*/

// Constructor
DoubleVariable::DoubleVariable(double d, bool constant) : Variable(), doubleData_(d)
{
	// Private variables
	returnType_ = VTypes::DoubleData;
	readOnly_ = constant;
}

// Destructor
DoubleVariable::~DoubleVariable()
{
}

/*
// Set / Get
*/

// Set value of variable (real)
bool DoubleVariable::set(ReturnValue rv)
{
	if (readOnly_)
	{
		printf("A constant value (in this case a double) cannot be assigned to.\n");
		return false;
	}
	bool success;
	doubleData_ = rv.asDouble(success);
	return success;
}

// Reset variable
void DoubleVariable::reset()
{
	doubleData_ = 0.0;
}

// Return value of node
bool DoubleVariable::execute(ReturnValue &rv)
{
	rv.set(doubleData_);
	return true;
}

// Print node contents
void DoubleVariable::nodePrint(int offset, const char *prefix)
{
	// Construct tabbed offset
	Dnchar tab(offset+32);
	for (int n=0; n<offset-1; n++) tab += '\t';
	if (offset > 1) tab.strcat("   |--> ");
	tab.strcat(prefix);

	// Output node data
	if (readOnly_) printf("[C]%s%f (constant value)\n", tab.get(), doubleData_);
	else printf("[V]%s%f (variable, name=%s)\n", tab.get(), doubleData_, name_.get());
}

// Set value for return
void DoubleVariable::setValue(double d)
{
	doubleData_ = d;
}
