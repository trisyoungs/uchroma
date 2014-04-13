/*
	*** Equation Variable
	*** src/base/equationvariable.h
	Copyright T. Youngs 2012-2013.

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

#ifndef UCHROMA_EQUATION_H
#define UCHROMA_EQUATION_H

#include <QtCore/QString>
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
	// Return variable target
	Variable* variable();
	// Set value
	void setValue(double value);
	// Return value
	double value();
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

// Equation
class Equation : public ListItem<Equation>
{
	public:
	// Constructor / Destructor
	Equation();
	~Equation();


	/*
	 * Equation Definition
	 */
	private:
	// Name of equation
	QString name_;
	// Description of equation
	QString description_;
	// Source Filename
	QString fileName_;
	// Equation text
	QString equationText_;
	// List of variable used in eq, and starting values
	List<EquationVariable> variables_;

	public:
	// Set name of equation
	void setName(QString name);
	// Return name of equation
	QString name();
	// Set description of equation
	void setDescription(QString description);
	// Return description of equation
	QString description();
	// Set filename of equation
	void setFileName(QString fileName);
	// Return filename of equation
	QString fileName();
	// Set equation text
	void setEquationText(QString text);
	// Return equation text
	QString equationText();


	/*
	 * Load / Save
	 */
	public:
	// Save contained data to file
	bool save();
	// Load data from file
	bool load(QString fileName = QString());
};


#endif


