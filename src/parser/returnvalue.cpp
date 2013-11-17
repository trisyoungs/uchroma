/*
	*** Return Value
	*** src/parser/treevalue.cpp
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

#include "parser/returnvalue.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "math/constants.h"
#include <stdio.h>
#include <stdlib.h>

// Constructors
ReturnValue::ReturnValue()
{
	// Private variables
	type_ = VTypes::NoData;
	valueI_ = 0;
	valueD_ = 0.0;
}
ReturnValue::ReturnValue(int i) : type_(VTypes::IntegerData), valueI_(i)
{
}
ReturnValue::ReturnValue(double d) : type_(VTypes::DoubleData), valueD_(d)
{
}

// Operator =
void ReturnValue::operator=(const ReturnValue &source)
{
	// Copy datatype of source
	type_ = source.type_;
	// To reduce unnecessary object creation/destruction, only copy the contents that corresponds to the type_
	switch (type_)
	{
		case (VTypes::NoData):
			break;
		case (VTypes::IntegerData):
			valueI_ = source.valueI_;
			break;
		case (VTypes::DoubleData):
			valueD_ = source.valueD_;
			break;
	}
}

// Operator =(double)
void ReturnValue::operator=(double d)
{
	type_ = VTypes::DoubleData;
	valueD_ = d;
}

// Operator =(int)
void ReturnValue::operator=(int i)
{
	type_ = VTypes::IntegerData;
	valueI_ = i;
}

// Return type of the stored data
VTypes::DataType ReturnValue::type()
{
	return type_;
}

// Reset data
void ReturnValue::reset()
{
	type_ = VTypes::NoData;
}

// Return string of contained data
const char *ReturnValue::info()
{
	static Dnchar result;
	switch (type_)
	{
		case (VTypes::NoData):
			result.sprintf("nothing (%s)", VTypes::dataType(type_));
			break;
		case (VTypes::IntegerData):
			result.sprintf("%i (%s)", valueI_, VTypes::dataType(type_));
			break;
		case (VTypes::DoubleData):
			result.sprintf("%f (%s)", valueD_, VTypes::dataType(type_));
			break;
	}
	return result;
}

// Return unique 'pair' code based on return types
int ReturnValue::dataPair(ReturnValue &source)
{
	return VTypes::dataPair(type_, source.type_);
}

/*
// Set
*/
// Set from integer value
void ReturnValue::set(int i)
{
	type_ = VTypes::IntegerData;
	valueI_ = i;
}

// Set from double value
void ReturnValue::set(double d)
{
	type_ = VTypes::DoubleData;
	valueD_ = d;
}

/*
// Get (with type checking)
*/

// Return as integer value
int ReturnValue::asInteger(bool &success)
{
	success = true;
	switch (type_)
	{
		case (VTypes::NoData):
			printf("Internal error: No data in ReturnValue to return as an integer!\n");
			success = false;
			return 0;
			break;
		case (VTypes::IntegerData):
			return valueI_;
			break;
		case (VTypes::DoubleData):
			return (signed long int) valueD_;
			break;
		default:
			printf("ReturnValue::asInteger() doesn't recognise this type (%s).\n", VTypes::dataType(type_));
			break;
	}
	success = false;
	return 0;
}

// Return as real value
double ReturnValue::asDouble(bool &success)
{
	success = true;
	switch (type_)
	{
		case (VTypes::NoData):
			printf("Internal error: No data in ReturnValue to return as a real!\n");
			success = false;
			return 0.0;
			break;
		case (VTypes::IntegerData):
			return (double)valueI_;
			break;
		case (VTypes::DoubleData):
			return valueD_;
			break;
		default:
			printf("ReturnValue::asDouble() doesn't recognise this type (%s).\n", VTypes::dataType(type_));
			break;
	}
	success = false;
	return 0;
}

// Return as character string
const char *ReturnValue::asString(bool &success)
{
	success = true;
	switch (type_)
	{
		case (VTypes::NoData):
			printf("Internal error: No data in ReturnValue to return as a string!\n");
			success = false;
			return "_NULL_";
			break;
		case (VTypes::IntegerData):
			return itoa(valueI_);
			break;
		case (VTypes::DoubleData):
			return ftoa(valueD_);
			break;
		default:
			// All pointer types
			msg.print("Cannot return a pointer as a string.\n");
			break;
	}
	success = false;
	return "NULL";
}

/*
// Get (no type checking)
*/

// Return as integer value
int ReturnValue::asInteger()
{
	static bool success;
	return asInteger(success);
}

// Return as real value
double ReturnValue::asDouble()
{
	static bool success;
	return asDouble(success);
}

// Return as character value
const char *ReturnValue::asString()
{
	static bool success;
	return asString(success);
}

// Return as boolean value
bool ReturnValue::asBool()
{
	static Dnchar booltest;
	switch (type_)
	{
		case (VTypes::NoData):
			return false;
			break;
		case (VTypes::IntegerData):
			return (valueI_ > 0);
			break;
		case (VTypes::DoubleData):
			return (valueD_ > 0.0);
			break;
	}
	return false;
}

/*
// In-place modify
*/

// Increase the contained variable
bool ReturnValue::increase()
{
	bool result = true;
	switch (type_)
	{
		case (VTypes::IntegerData):
			++valueI_;
			break;
		case (VTypes::DoubleData):
			++valueD_;
			break;
	}
	return result;
}

// Decrease the contained variable
bool ReturnValue::decrease()
{
	bool result = true;
	switch (type_)
	{
		case (VTypes::IntegerData):
			--valueI_;
			break;
		case (VTypes::DoubleData):
			--valueD_;
			break;
		default:
			printf("Internal Error: No 'decrease' has been defined for %s.\n", VTypes::aDataType(type_));
			break;
	}
	return result;
}
