/*
	*** Return Value
	*** src/parser/returnvalue.h
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

#ifndef UCHROMA_RETURNVALUE_H
#define UCHROMA_RETURNVALUE_H

#include "parser/returnvalue.h"
#include "parser/vtypes.h"
#include "base/dnchar.h"
#include "templates/vector3.h"

// Tree Return Value
class ReturnValue
{
	public:
	// Constructor
	ReturnValue();
	ReturnValue(int i);
	ReturnValue(double d);
	// Operator=
	void operator=(const ReturnValue &rv);
	void operator=(double d);
	void operator=(int i);


	/*
	// Data
	*/
	private:
	// Data type contained in class
	VTypes::DataType type_;
	// Variable members for returns
	int valueI_;
	double valueD_;

	public:
	// Return type of the stored data
	VTypes::DataType type();
	// Reset data
	void reset();
	// Return string of contained data
	const char *info();
	// Return unique 'pair' code based on return types
	int dataPair(ReturnValue& source);


	/*
	// Set
	*/
	public:
	// Set from integer value
	void set(int i);
	// Set from real value
	void set(double d);


	/*
	// Get (with type checking)
	*/
	public:
	// Return integer value
	int asInteger(bool &success);
	// Return real value
	double asDouble(bool &success);
	// Return character string
	const char *asString(bool &success);


	/*
	// Get (no type checking)
	*/
	public:
	// Return integer value
	int asInteger();
	// Return real value
	double asDouble();
	// Return character string
	const char *asString();
	// Return as boolean (guaranteed conversion)
	bool asBool();


	/*
	// In-place modify
	*/
	public:
	// Increase the contained variable
	bool increase();
	// Decrease the contained variable
	bool decrease();
};

#endif
