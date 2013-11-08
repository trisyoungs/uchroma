/*
	*** Transformer
	*** src/base/transformer.cpp
	Copyright T. Youngs 2013

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

#include "base/transformer.h"
#include "templates/array.h"

// Constructor
Transformer::Transformer()
{
	// Add variable trio to Tree's root ScopeNode
	x_ = equation_.addGlobalVariable("x");
	y_ = equation_.addGlobalVariable("y");
	z_ = equation_.addGlobalVariable("z");
}

// Destructor
Transformer::~Transformer()
{
}

// Set equation, returning if Tree construction was successful
bool Transformer::setEquation(QString equation)
{
	text_ = equation;
	valid_ = equation_.setCommands(equation);
}

// Return text used to generate last equation_
QString Transformer::text()
{
	return text_;
}

// Return whether current equation is valid
bool Transformer::valid()
{
	return valid_;
}

// Transform single value
double Transformer::transform(double x, double y, double z)
{
	x_->set(ReturnValue(x));
	y_->set(ReturnValue(y));
	z_->set(ReturnValue(z));
}

// Transform whole array, including application of pre/post transform shift
Array<double> Transformer::transformArray(Array<double> sourceX, Array<double> sourceY, double z, int target, double preShift, double postShift)
{
	if (sourceX.nItems() != sourceY.nItems())
	{
		msg.print("Error in Transformer::transformArray() - x and y array sizes do not match.\n");
		return Array<double>();
	}

	// If equation is not valid, just return original array
	if (!valid_)
	{
		msg.print("Equation is not valid, so returning original array.\n");
		return (target == 0 ? sourceX : sourceY);
	}

	// Create new array, and create reference to target array
	Array<double> newArray(sourceX.nItems());

	// Reusable ReturnValues
	ReturnValue rv[3];
	rv[2] = z;
	for (int n=0; n<sourceX.nItems(); ++n)
	{
		// Set values in equation
		rv[0] = sourceX[n];
		rv[1] = sourceY[n];
		newArray[n] = equation_.execute();
	}

	return newArray;
}
