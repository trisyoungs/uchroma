/*
	*** Operators
	*** src/command/operators.cpp
	Copyright T. Youngs 2010-2011

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

#include "command/commands.h"
#include "parser/commandnode.h"
#include "math/mathfunc.h"
#include <stdio.h>
#include <string.h>

Dnchar s;

// Add two quantities together
bool Command::function_OperatorAdd(CommandNode *c, ReturnValue &rv)
{
	ReturnValue lhs, rhs;
	bool b = true;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;
	int id = lhs.dataPair(rhs);
	if (id < 0) b = false;
	else switch (id)
	{
		case (VTypes::IntInt): rv.set(lhs.asInteger(b) + rhs.asInteger(b)); break;
		case (VTypes::IntDbl):
		case (VTypes::DblInt):
		case (VTypes::DblDbl): rv.set(lhs.asDouble(b) + rhs.asDouble(b)); break;
		default:
			printf("The operator '+' cannot act between %s and %s.\n", VTypes::aDataType(rv.type()), VTypes::aDataType(rhs.type()));
	}
	return b;
}

// Logical AND check on two operators
bool Command::function_OperatorAnd(CommandNode *c, ReturnValue &rv)
{
	ReturnValue v1, v2;
	if (!c->arg(0,v1)) return false;
	if (!c->arg(1,v2)) return false;
	rv.set(v1.asBool() && v2.asBool());
	return true;
}

// Assignment
bool Command::function_OperatorAssignment(CommandNode *c, ReturnValue &rv)
{
	// Grab the second argument result and assign it to the first
	if (!c->arg(1, rv)) return false;
	if (!c->setArg(0, rv)) return false;
	return true;
}

// Assignment Divide
bool Command::function_OperatorAssignmentDivide(CommandNode *c, ReturnValue &rv)
{
	if (!function_OperatorDivide(c,rv)) return false;
	// Now, set the first argument to our return value
	if (!c->setArg(0, rv)) return false;
	return true;
}

// Assignment Multiply
bool Command::function_OperatorAssignmentMultiply(CommandNode *c, ReturnValue &rv)
{
	if (!function_OperatorMultiply(c,rv)) return false;
	// Now, set the first argument to our return value
	if (!c->setArg(0, rv)) return false;
	return true;
}

// Assignment Plus
bool Command::function_OperatorAssignmentPlus(CommandNode *c, ReturnValue &rv)
{
	if (!function_OperatorAdd(c,rv)) return false;
	// Now, set the first argument to our return value
	if (!c->setArg(0, rv)) return false;
	return true;
}

// Assignment Subtract
bool Command::function_OperatorAssignmentSubtract(CommandNode *c, ReturnValue &rv)
{
	if (!function_OperatorSubtract(c,rv)) return false;
	// Now, set the first argument to our return value
	if (!c->setArg(0, rv)) return false;
	return true;
}

// Divide one quantity by another
bool Command::function_OperatorDivide(CommandNode *c, ReturnValue &rv)
{
	// Grab both argument (return) values and send them to be operated on
	ReturnValue lhs, rhs;
	bool b = true;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;
	int id = lhs.dataPair(rhs);
	if (id < 0) b = false;
	else switch (id)
	{
		case (VTypes::IntInt):
			rv.set(lhs.asInteger(b) / rhs.asInteger(b));
			break;
		case (VTypes::IntDbl):
		case (VTypes::DblInt):
		case (VTypes::DblDbl):
			rv.set(lhs.asDouble(b) / rhs.asDouble(b));
			break;
		default:
			printf("The operator '/' cannot act between %s and %s.\n", VTypes::aDataType(rv.type()), VTypes::aDataType(rhs.type()));
	}
	return b;
}

// Equal To
bool Command::function_OperatorEqualTo(CommandNode *c, ReturnValue &rv)
{
	// Grab both argument (return) values and send them to be operated on
	ReturnValue lhs, rhs;
	bool b = true;
	int result = 1;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;
	int id = lhs.dataPair(rhs);
	if (id < 0) b = false;
	else switch (id)
	{
		case (VTypes::IntInt): if (lhs.asInteger(b) != rhs.asInteger(b)) result = 0; break;
		case (VTypes::IntDbl):
		case (VTypes::DblInt):
		case (VTypes::DblDbl): if (lhs.asDouble(b) != rhs.asDouble(b)) result = 0; break;
		default:
			printf("The operator '==' cannot act between %s and %s.\n", VTypes::aDataType(rv.type()), VTypes::aDataType(rhs.type()));
	}
	rv.set(result);
	return b;
}

// Greater Than
bool Command::function_OperatorGreaterThan(CommandNode *c, ReturnValue &rv)
{
	// Grab both argument (return) values and send them to be operated on
	ReturnValue lhs, rhs;
	bool b = true;
	int result = 1;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;
	int id = lhs.dataPair(rhs);
	if (id < 0) b = false;
	else switch (id)
	{
		case (VTypes::IntInt): if (lhs.asInteger(b) <= rhs.asInteger(b)) result = 0; break;
		case (VTypes::IntDbl):
		case (VTypes::DblInt):
		case (VTypes::DblDbl): if (lhs.asDouble(b) <= rhs.asDouble(b)) result = 0; break;
		default:
			printf("The operator '>' cannot act between %s and %s.\n", VTypes::aDataType(rv.type()), VTypes::aDataType(rhs.type()));
	}
	rv.set(result);
	return b;
}

// Greater Than Equal To
bool Command::function_OperatorGreaterThanEqualTo(CommandNode *c, ReturnValue &rv)
{
	// Grab both argument (return) values and send them to be operated on
	ReturnValue lhs, rhs;
	bool b = true;
	int result = 1;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;
	int id = lhs.dataPair(rhs);
	if (id < 0) b = false;
	else switch (id)
	{
		case (VTypes::IntInt): if (lhs.asInteger(b) < rhs.asInteger(b)) result = 0; break;
		case (VTypes::IntDbl):
		case (VTypes::DblInt):
		case (VTypes::DblDbl): if (lhs.asDouble(b) < rhs.asDouble(b)) result = 0; break;
		default:
			printf("The operator '>=' cannot act between %s and %s.\n", VTypes::aDataType(rv.type()), VTypes::aDataType(rhs.type()));
	}
	rv.set(result);
	return b;
}

// Less Than
bool Command::function_OperatorLessThan(CommandNode *c, ReturnValue &rv)
{
	// Grab both argument (return) values and send them to be operated on
	ReturnValue lhs, rhs;
	bool b = true;
	int result = 1;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;
	int id = lhs.dataPair(rhs);
	if (id < 0) b = false;
	else switch (id)
	{
		case (VTypes::IntInt): if (lhs.asInteger(b) >= rhs.asInteger(b)) result = 0; break;
		case (VTypes::IntDbl):
		case (VTypes::DblInt):
		case (VTypes::DblDbl): if (lhs.asDouble(b) >= rhs.asDouble(b)) result = 0; break;
		default:
			printf("The operator '<' cannot act between %s and %s.\n", VTypes::aDataType(rv.type()), VTypes::aDataType(rhs.type()));
	}
	rv.set(result);
	return b;
}

// Less Than Equal To
bool Command::function_OperatorLessThanEqualTo(CommandNode *c, ReturnValue &rv)
{
	// Grab both argument (return) values and send them to be operated on
	ReturnValue lhs, rhs;
	bool b = true;
	int result = 1;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;
	int id = lhs.dataPair(rhs);
	if (id < 0) b = false;
	else switch (id)
	{
		case (VTypes::IntInt): if (lhs.asInteger(b) > rhs.asInteger(b)) result = 0; break;
		case (VTypes::IntDbl):
		case (VTypes::DblInt):
		case (VTypes::DblDbl): if (lhs.asDouble(b) > rhs.asDouble(b)) result = 0; break;
		default:
			printf("The operator '<=' cannot act between %s and %s.\n", VTypes::aDataType(rv.type()), VTypes::aDataType(rhs.type()));
	}
	rv.set(result);
	return b;
}

// Integer remainder of A/B
bool Command::function_OperatorModulus(CommandNode *c, ReturnValue &rv)
{
	// Grab both argument (return) values and send them to be operated on
	ReturnValue lhs, rhs;
	bool b = true;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;
	int id = lhs.dataPair(rhs);
	if (id < 0) b = false;
	else switch (id)
	{
		case (VTypes::IntInt): rv.set(lhs.asInteger(b) % rhs.asInteger(b)); break;
		default:
			printf("The operator '/' cannot act between %s and %s.\n", VTypes::aDataType(rv.type()), VTypes::aDataType(rhs.type()));
	}
	return b;
}

// Multiply one quantity by another
bool Command::function_OperatorMultiply(CommandNode *c, ReturnValue &rv)
{
	// Grab both argument (return) values and send them to be operated on
	ReturnValue lhs, rhs;
	bool b = true;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;
	int id = lhs.dataPair(rhs);
	if (id < 0) b = false;
	else switch (id)
	{
		case (VTypes::IntInt): rv.set(lhs.asInteger(b) * rhs.asInteger(b)); break;
		case (VTypes::IntDbl):
		case (VTypes::DblInt):
		case (VTypes::DblDbl): rv.set(lhs.asDouble(b) * rhs.asDouble(b)); break;
		default:
			printf("The operator '*' cannot act between %s and %s.\n", VTypes::aDataType(rv.type()), VTypes::aDataType(rhs.type()));
	}
	return b;
}

// Negate value
bool Command::function_OperatorNegate(CommandNode *c, ReturnValue &rv)
{
	if (!c->arg(0, rv)) return false;
	int i;
	switch (c->argType(0))
	{
		case (VTypes::IntegerData):
			rv.set(-rv.asInteger());
			break;
		case (VTypes::DoubleData):
			rv.set(-rv.asDouble());
			break;
		default:
			printf("Can't negate %s.\n", VTypes::aDataType(c->argType(0)));
			return false;
	}
	return true;
}

// Not (Reverse Logic)
bool Command::function_OperatorNot(CommandNode *c, ReturnValue &rv)
{
	// Grab argument and 'negate' it
	ReturnValue v1;
	if (!c->arg(0, v1)) return false;
	rv.set( !v1.asBool() );
	return true;
}

// Not Equal To
bool Command::function_OperatorNotEqualTo(CommandNode *c, ReturnValue &rv)
{
	ReturnValue lhs, rhs;
	bool b = true;
	int result = 1;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;
	int id = lhs.dataPair(rhs);
	if (id < 0) b = false;
	else switch (id)
	{
		case (VTypes::IntInt): if (lhs.asInteger(b) == rhs.asInteger(b)) result = 0; break;
		case (VTypes::IntDbl):
		case (VTypes::DblInt):
		case (VTypes::DblDbl): if (lhs.asDouble(b) == rhs.asDouble(b)) result = 0; break;
		default:
			printf("The operator '!=' cannot act between %s and %s.\n", VTypes::aDataType(rv.type()), VTypes::aDataType(rhs.type()));
	}
	rv.set(result);
	return b;
}

// Logical OR check on two operators
bool Command::function_OperatorOr(CommandNode *c, ReturnValue &rv)
{
	// Grab both argument (return) values and send them to be operated on
	ReturnValue v1, v2;
	if (!c->arg(0,v1)) return false;
	if (!c->arg(1,v2)) return false;
	rv.set(v1.asBool() || v2.asBool());
	return true;
}

// Postfix Decrease
bool Command::function_OperatorPostfixDecrease(CommandNode *c, ReturnValue &rv)
{
	// Get current value of argument
	if (!c->arg(0, rv)) return false;
	ReturnValue newvalue;
	newvalue = rv;
	newvalue.decrease();
	return c->setArg(0, newvalue);
}

// Postfix Increase
bool Command::function_OperatorPostfixIncrease(CommandNode *c, ReturnValue &rv)
{
	// Get current value of argument
	if (!c->arg(0, rv)) return false;
	ReturnValue newvalue;
	newvalue = rv;
	newvalue.increase();
	return c->setArg(0, newvalue);
}

// Prefix Decrease
bool Command::function_OperatorPrefixDecrease(CommandNode *c, ReturnValue &rv)
{
	// Get current value of argument
	if (!c->arg(0, rv)) return false;
	rv.decrease();
	return c->setArg(0, rv);
}

// Prefix Increase
bool Command::function_OperatorPrefixIncrease(CommandNode *c, ReturnValue &rv)
{
	// Get current value of argument
	if (!c->arg(0, rv)) return false;
	rv.increase();
	return c->setArg(0, rv);
}

// Raise one quantity to the power of another
bool Command::function_OperatorPower(CommandNode *c, ReturnValue &rv)
{
	ReturnValue lhs, rhs;
	bool b = true;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;
	int id = lhs.dataPair(rhs);
	if (id < 0) b = false;
	else switch (id)
	{
		case (VTypes::IntInt): rv.set(uChromaMath::power(lhs.asInteger(b),rhs.asInteger(b))); break;
		case (VTypes::IntDbl):
		case (VTypes::DblInt):
		case (VTypes::DblDbl): rv.set(pow(lhs.asDouble(b),rhs.asDouble(b))); break;
		default:
			printf("The operator '^' cannot act between %s and %s.\n", VTypes::aDataType(rv.type()), VTypes::aDataType(rhs.type()));
	}
	return b;
}

// Subtract one quantity from another
bool Command::function_OperatorSubtract(CommandNode *c, ReturnValue &rv)
{
	// Grab both argument (return) values and send them to be operated on
	ReturnValue lhs, rhs;
	bool b = true;
	if (!c->arg(0,lhs)) return false;
	if (!c->arg(1,rhs)) return false;
	int id = lhs.dataPair(rhs);
	if (id < 0) b = false;
	else switch (id)
	{
		case (VTypes::IntInt): rv.set(lhs.asInteger(b) - rhs.asInteger(b)); break;
		case (VTypes::IntDbl):
		case (VTypes::DblInt):
		case (VTypes::DblDbl): rv.set(lhs.asDouble(b) - rhs.asDouble(b)); break;
		default:
			printf("The operator '-' cannot act between %s and %s.\n", VTypes::aDataType(rv.type()), VTypes::aDataType(rhs.type()));
	}
	return b;
}
