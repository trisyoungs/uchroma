/*
	*** Reference Variable
	*** src/base/referencevariable.h
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

#ifndef UCHROMA_REFERENCEVARIABLE_H
#define UCHROMA_REFERENCEVARIABLE_H

#include "base/dataspace.h"
#include "templates/list.h"
#include <QtCore/QString>

// Forward Declarations
class DoubleVariable;

// Reference Variable
class ReferenceVariable : public ListItem<ReferenceVariable>
{
	public:
	// Constructor / Destructor
	ReferenceVariable();
	~ReferenceVariable();


	/*
	 * Variable Target
	 */
	private:
	// Name of target variable
	QString name_;
	// Target Variable pointer
	DoubleVariable* variable_;
	// Whether the variable is used in the current equation
	bool used_;

	public:
	// Set name
	void setName(QString name);
	// Return name
	QString name();
	// Set variable target
	void setVariable(DoubleVariable* variable);
	// Return variable target
	DoubleVariable* variable();
	// Set whether this variable is used in the current equation
	void setUsed(bool used);
	// Return whether this variable is used in the current equation
	bool used();


	/*
	 * Reference Source
	 */
	private:
	// Source DataSet name
	// TODO
	// Reference data
	DataSpace referenceSpace_;

	public:
};

#endif
