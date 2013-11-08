/*
	*** Accessor
	*** src/parser/accessor.h
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

#ifndef UCHROMA_ACCESSOR_H
#define UCHROMA_ACCESSOR_H

#include "parser/vtypes.h"

// Accessor
class Accessor
{
	public:
	// Accessor name
	const char *name;
	// Return type of accessor
	VTypes::DataType returnType;
	// Array data size (-1 = dynamic list, 0 = no array, N = size)
	int arraySize;
	// Whether data represented by accessor is read-only
	bool isReadOnly;
};

// Function Accessor
class FunctionAccessor
{
	public:
	// Function name
	const char *name;
	// Return type of function
	VTypes::DataType returnType;
	// Function argument specification
	const char *arguments;
	// Function argument text
	const char *argText;
};

#endif
