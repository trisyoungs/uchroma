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
#include "base/indexdata.h"
#include <QString>

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
	// X index
	IndexData xIndex_;
	// Z index
	IndexData zIndex_;
	// Z DataSet name
	QString zDataSetName_;

	public:
	// Set source collection
	void setSourceCollection(Collection* source);
	// Return source collection
	Collection* sourceCollection();
	// Return x index data
	IndexData& xIndex();
	// Return Z index data
	IndexData& zIndex();
	// Set Z DataSet name
	void setZDataSetName(QString name);
	// Return Z DataSet name
	QString zDataSetName();


	/*
	 * Reference Data
	 */
	private:
	// Reference dataspace
	DataSpace referenceSpace_;
	// Current range used when updating variable values
	DataSpaceRange* currentReferenceRange_;

	public:
	// Generate reference data
	bool initialiseDataSpace(Collection* fitCollection, DataSpace& fitDataSpace);
	// Reset current reference DataSpaceRange to the first available
	void resetCurrentDataSpaceRange();
	// Set internal pointer to the next available DataSpaceRange
	void moveToNextDataSpaceRange();
	// Update value of target variable with DataSpaceRange indices provided
	bool updateValue(int rangeXIndex, int rangeZIndex);
};

#endif
