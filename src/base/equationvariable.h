/*
	*** Equation Variable
	*** src/base/equationvariable.h
	Copyright T. Youngs 2012-2014.

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

#ifndef UCHROMA_EQUATIONVARIABLE_H
#define UCHROMA_EQUATIONVARIABLE_H

#include <QString>
#include "templates/list.h"

// Forward Declarations
class Variable;

// Equation Variable
class EquationVariable : public ListItem<EquationVariable>
{
	public:
	// Constructor / Destructor
	EquationVariable();
	~EquationVariable();


	/*
	 * Variable Target
	 */
	private:
	// Name of target variable
	QString name_;
	// Target Variable pointer
	Variable* variable_;
	// Value
	double value_;
	// Whether minimum maximum limits are enabled
	bool minimumLimitEnabled_, maximumLimitEnabled_;
	// Values for minimum / maximum limits
	double minimumLimit_, maximumLimit_;
	// Whether the variable is to be fit
	bool fit_;
	// Whether the variable is used in the current equation
	bool used_;

	public:
	// Set name
	void setName(QString name);
	// Return name
	QString name();
	// Set variable target
	void setVariable(Variable* variable);
	// Reset variable target (to NULL) and usage status
	void resetVariable();
	// Return variable target
	Variable* variable();
	// Set value
	void setValue(double value);
	// Return value
	double value();
	// Poke value to variable target
	void pokeValueToVariable();
	// Set minimum limit
	void setMinimumLimit(bool enabled, double value);
	// Return whether minimum limit is enabled
	bool minimumLimitEnabled();
	// Return minimum limit value
	double minimumLimit();
	// Set maximum limit
	void setMaximumLimit(bool enabled, double value);
	// Return whether maximum limit is enabled
	bool maximumLimitEnabled();
	// Return maximum limit value
	double maximumLimit();
	// Set whether this variable should be fit
	void setFit(bool fit);
	// Return whether this variable should be fit
	bool fit();
	// Set whether this variable is used in the current equation
	void setUsed(bool used);
	// Return whether this variable is used in the current equation
	bool used();
};

#endif
