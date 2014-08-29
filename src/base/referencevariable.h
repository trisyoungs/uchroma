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
#include <QtCore/QString>

// Forward Declarations
class Variable;

// Reference Variable
class ReferenceVariable : public ListItem<ReferenceVariable>
{
	public:
	// Constructor / Destructor
	ReferenceVariable();
	~ReferenceVariable();
	// Copy constructor
	ReferenceVariable(const ReferenceVariable& source);
	// Assignment operator
	void operator=(const ReferenceVariable& source);
	// Reference Type
	enum ReferenceType { NormalReference, FixedReference, RelativeReference, nReferenceTypes };
	// Convert text string to ReferenceType
	static ReferenceType referenceType(const char* s);
	// Convert ReferenceType to text string
	static const char* referenceType(ReferenceType id);


	/*
	 * Variable Target
	 */
	private:
	// Name of target variable
	QString name_;
	// Target Variable pointer
	Variable* variable_;
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
	// Update associated variable to reflect name of ReferenceVariable
	bool updateVariable();
	// Set whether this variable is used in the current equation
	void setUsed(bool used);
	// Return whether this variable is used in the current equation
	bool used();


	/*
	 * Reference Definition
	 */
	private:
	// Source collection
	Collection* sourceCollection_;
	// X index type
	ReferenceType xType_;
	// X index (for absolute type)
	int xIndex_;
	// X offset (for relative type)
	int xOffset_;
	// Z index type
	ReferenceType zType_;
	// Z index (for absolute type)
	int zIndex_;
	// Z offset (for relative type)
	int zOffset_;
	// Z DataSet name
	QString zDataSetName_;
	// Reference data
	DataSpace referenceSpace_;

	public:
	// Set source collection
	void setSourceCollection(Collection* source);
	// Return source collection
	Collection* sourceCollection();
	// Set X index type
	void setXType(ReferenceVariable::ReferenceType type);
	// Return X index type
	ReferenceVariable::ReferenceType xType();
	// Set X index
	void setXIndex(int index);
	// Return X index
	int xIndex();
	// Set X offset
	void setXOffset(int offset);
	// Return X offset
	int xOffset();
	// Set Z index type
	void setZType(ReferenceVariable::ReferenceType type);
	// Return Z index type
	ReferenceVariable::ReferenceType zType();
	// Set Z index
	void setZIndex(int index);
	// Return Z index
	int zIndex();
	// Set Z offset
	void setZOffset(int offset);
	// Return Z offset
	int zOffset();
	// Set Z DataSet name
	void setZDataSetName(QString name);
	// Return Z DataSet name
	QString zDataSetName();
};

#endif
