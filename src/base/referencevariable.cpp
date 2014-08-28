/*
	*** ReferenceVariable
	*** src/base/referencevariable.cpp
	Copyright T. Youngs 2012-2014

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

#include "base/referencevariable.h"
#include "base/messenger.h"
#include "parser/double.h"

// ReferenceType Keywords
const char* ReferenceTypeKeywords[] = { "Normal", "Fixed", "Relative" };

// Convert text string to ReferenceType
ReferenceVariable::ReferenceType ReferenceVariable::referenceType(const char* s)
{
	for (int n=0; n<ReferenceVariable::nReferenceTypes; ++n) if (strcmp(s,ReferenceTypeKeywords[n]) == 0) return (ReferenceVariable::ReferenceType) n;
	return ReferenceVariable::nReferenceTypes;
}

// Convert ReferenceType to text string
const char *ReferenceVariable::referenceType(ReferenceVariable::ReferenceType rt)
{
	return ReferenceTypeKeywords[rt];
}

// Constructor
ReferenceVariable::ReferenceVariable() : ListItem<ReferenceVariable>()
{
	// Variable Target
	variable_ = NULL;
	used_ = false;

	// Reference Definition
	sourceCollection_ = NULL;
	xType_ = ReferenceVariable::NormalReference;
	xIndex_ = 0;
	xOffset_ = 0;
	zType_ = ReferenceVariable::NormalReference;
	zIndex_ = 0;
	zOffset_ = 0;
}

// Destructor
ReferenceVariable::~ReferenceVariable()
{
}

// Copy constructor
ReferenceVariable::ReferenceVariable(const ReferenceVariable& source)
{
	(*this) = source;
}

// Assignment operator
void ReferenceVariable::operator=(const ReferenceVariable& source)
{
	// Variable Target
	variable_ = source.variable_;
	used_ = source.used_;
	name_ = source.name_;
	
	// Reference Definition
	sourceCollection_ = source.sourceCollection_;
	xType_ = source.xType_;
	xIndex_ = source.xIndex_;
	xOffset_ = source.xOffset_;
	zType_ = source.zType_;
	zIndex_ = source.zIndex_;
	zOffset_ = source.zOffset_;
	zDataSetName_ = source.zDataSetName_;
}

// Set name
void ReferenceVariable::setName(QString name)
{
	// We do not update the associated DoubleVariable's name here - it is up to the calling function to check this using updateVariable()
	name_ = name;
}

// Return name
QString ReferenceVariable::name()
{
	return name_;
}

// Set variable target
void ReferenceVariable::setVariable(DoubleVariable* variable)
{
	variable_ = variable;
}

// Reset variable target (to NULL) and usage status
void ReferenceVariable::resetVariable()
{
	variable_ = NULL;
	used_ = false;
}

// Return variable target
DoubleVariable* ReferenceVariable::variable()
{
	return variable_;
}

// Update associated variable to reflect name of ReferenceVariable
bool ReferenceVariable::updateVariable()
{
	if (!variable_) return false;
	if (name_ == variable_->name()) return false;
	variable_->setName(qPrintable(name_));
	return true;
}

// Set whether this variable is used in the current equation
void ReferenceVariable::setUsed(bool used)
{
	used_ = used;
}

// Return whether this variable is used in the current equation
bool ReferenceVariable::used()
{
	return used_;
}

/*
 * Reference Definition
 */

// Set source collection
void ReferenceVariable::setSourceCollection(Collection* source)
{
	sourceCollection_ = source;
}

// Return source collection
Collection* ReferenceVariable::sourceCollection()
{
	return sourceCollection_;
}

// Set X index type
void ReferenceVariable::setXType(ReferenceVariable::ReferenceType type)
{
	xType_ = type;
}

// Return X index type
ReferenceVariable::ReferenceType ReferenceVariable::xType()
{
	return xType_;
}

// Set X index
void ReferenceVariable::setXIndex(int index)
{
	xIndex_ = index;
}

// Return X index
int ReferenceVariable::xIndex()
{
	return xIndex_;
}

// Set X offset
void ReferenceVariable::setXOffset(int offset)
{
	xOffset_ = offset;
}

// Return X offset
int ReferenceVariable::xOffset()
{
	return xOffset_;
}

// Set Z index type
void ReferenceVariable::setZType(ReferenceVariable::ReferenceType type)
{
	zType_ = type;
}

// Return Z index type
ReferenceVariable::ReferenceType ReferenceVariable::zType()
{
	return zType_;
}

// Set Z index
void ReferenceVariable::setZIndex(int index)
{
	zIndex_ = index;
}

// Return Z index
int ReferenceVariable::zIndex()
{
	return zIndex_;
}

// Set Z offset
void ReferenceVariable::setZOffset(int offset)
{
	zOffset_ = offset;
}

// Return Z offset
int ReferenceVariable::zOffset()
{
	return zOffset_;
}

// Set Z DataSet name
void ReferenceVariable::setZDataSetName(QString name)
{
	zDataSetName_ = name;
}

// Return Z DataSet name
QString ReferenceVariable::zDataSetName()
{
	return zDataSetName_;
}
