/*
	*** Tree (Operator Checking)
	*** src/parser/tree_opcheck.cpp
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

#include "base/messenger.h"
#include "parser/tree.h"

// Check unary operator type compatibility
VTypes::DataType Tree::checkUnaryOperatorTypes(Command::Function func, VTypes::DataType type)
{
	msg.enter("Tree::checkUnaryOperatorTypes");
	int id = VTypes::dataSinglet(type);
	// Check for no data type
	if (id == VTypes::UntypedData)
	{
		msg.exit("Tree::checkUnaryOperatorTypes");
		return VTypes::NoData;
	}
	VTypes::DataType result = VTypes::NoData;
	switch (func)
	{
		// Postfix and prefix operators (must have integer or real types, or pointer class with list pointers)
		case (Command::OperatorPostfixIncrease):
		case (Command::OperatorPostfixDecrease):
		case (Command::OperatorPrefixIncrease):
		case (Command::OperatorPrefixDecrease):
			switch (id)
			{
				case (VTypes::Int):
				case (VTypes::Dbl):
					result = type;
					break;
			}
			break;
		case (Command::OperatorNegate):
			switch (id)
			{
				case (VTypes::Int):
				case (VTypes::Dbl):
					result = type;
					break;
			}
			break;
		case (Command::OperatorNot):
			// Always works, and always returns an integer
			result = VTypes::IntegerData;
			break;
		default:
			break;
	}
	// Print error message if necessary
	if (result == VTypes::NoData) printf("Error: Unary operator %s cannot act on %s.\n", Command::data[func].keyword, VTypes::aDataType(type));
	msg.exit("Tree::checkUnaryOperatorTypes");
	return result;
}

// Check binary operator type compatibility
VTypes::DataType Tree::checkBinaryOperatorTypes(Command::Function func, VTypes::DataType type1, VTypes::DataType type2)
{
	msg.enter("Tree::checkBinaryOperatorTypes");
	int id = VTypes::dataPair(type1, type2);
	// Check for no data type
	if (id == VTypes::UntypedData)
	{
		msg.exit("Tree::checkBinaryOperatorTypes");
		return VTypes::NoData;
	}
	printf("ID = %i, type1 = %i, type2 = %i\n", id, type1, type2);
	VTypes::DataType result = VTypes::NoData;
	switch (func)
	{
		case (Command::OperatorAdd):
		case (Command::OperatorSubtract):
		case (Command::OperatorAssignmentPlus):
		case (Command::OperatorAssignmentSubtract):
			switch (id)
			{
				case (VTypes::IntInt):
					result = type1;
					break;
				case (VTypes::IntDbl):
				case (VTypes::DblInt):
				case (VTypes::DblDbl):
					result = VTypes::DoubleData;
					break;
			}
			break;
		case (Command::OperatorAnd):
		case (Command::OperatorOr):
			result = VTypes::IntegerData;
			break;
		case (Command::OperatorDivide):
		case (Command::OperatorAssignmentDivide):
			switch (id)
			{
				case (VTypes::IntInt):
				case (VTypes::DblInt):
				case (VTypes::DblDbl):
					result = type1;
					break;
				case (VTypes::IntDbl):
					result = type2;
					break;
			}
			break;
		case (Command::OperatorAssignmentMultiply):
		case (Command::OperatorMultiply):
			switch (id)
			{
				case (VTypes::DblInt):
				case (VTypes::DblDbl):
				case (VTypes::IntInt):
					result = type1;
					break;
				case (VTypes::IntDbl):
					result = type2;
					break;
			}
			break;
		case (Command::OperatorEqualTo):
		case (Command::OperatorGreaterThan):
		case (Command::OperatorGreaterThanEqualTo):
		case (Command::OperatorLessThan):
		case (Command::OperatorLessThanEqualTo):
		case (Command::OperatorNotEqualTo):
			switch (id)
			{
				case (VTypes::IntInt):
				case (VTypes::IntDbl):
				case (VTypes::DblInt):
				case (VTypes::DblDbl):
					result = VTypes::IntegerData;
					break;
			}
			break;
		case (Command::OperatorPower):
			switch (id)
			{
				case (VTypes::IntInt):
				case (VTypes::DblInt):
				case (VTypes::DblDbl):
				case (VTypes::IntDbl):
					result = VTypes::DoubleData;
					break;
			}
			break;
		case (Command::OperatorModulus):
			switch (id)
			{
				case (VTypes::IntInt):
					result = type1;
					break;
			}
			break;
		case (Command::OperatorAssignment):
			switch (id)
			{
				case (VTypes::IntInt):
				case (VTypes::IntDbl):
				case (VTypes::DblInt):
				case (VTypes::DblDbl):
					result = type1;
					break;
			}
			break;
		default:
			break;
	}
	// Print error message
	if (result == VTypes::NoData) printf("Error: Operator %s cannot act between types %s and %s.\n", Command::data[func].keyword, VTypes::dataType(type1), VTypes::dataType(type2));
	msg.exit("Tree::checkBinaryOperatorTypes");
	return result;
}
