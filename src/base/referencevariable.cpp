/*
	*** ReferenceVariable
	*** src/base/referencevariable.cpp
	Copyright T. Youngs 2012-2014

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

#include "base/referencevariable.h"
#include "base/messenger.h"
#include "parser/double.h"

// Constructor
ReferenceVariable::ReferenceVariable() : ListItem<ReferenceVariable>()
{
	variable_ = NULL;
	used_ = false;
}

// Destructor
ReferenceVariable::~ReferenceVariable()
{
}

// Set name
void ReferenceVariable::setName(QString name)
{
	name_ = name;
}

// Return name
QString ReferenceVariable::name()
{
	return name_;
}

// Set variable target
void ReferenceVariable::setVariable(DoubleVariable* variable)
{
	variable_ = variable;
}

// Return variable target
DoubleVariable* ReferenceVariable::variable()
{
	return variable_;
}

// Set whether this variable is used in the current equation
void ReferenceVariable::setUsed(bool used)
{
	used_ = used;
}

// Return whether this variable is used in the current equation
bool ReferenceVariable::used()
{
	return used_;
}
