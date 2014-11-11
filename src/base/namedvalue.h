/*
	*** Named Value
	*** src/base/namedvalue.h
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

#ifndef UCHROMA_NAMEDVALUE_H
#define UCHROMA_NAMEDVALUE_H

#include "base/namedvalue.h"
#include "templates/list.h"
#include <QtCore/QString>

// Forward Declarations
/* none */

// Named Value
class NamedValue : public ListItem<NamedValue>
{
	public:
	// Constructor / Destructor
	NamedValue();
	~NamedValue();
	// Copy constructor
	NamedValue(const NamedValue& source);
	// Assignment operator
	void operator=(const NamedValue& source);


	/*
	 * Data
	 */
	private:
	// Name of value
	QString name_;
	// Value
	double value_;

	public:
	// Set name
	void setName(QString name);
	// Return name
	QString name();
	// Return whether name matches
	bool isNamed(QString name);
	// Set value
	void setValue(double value);
	// Return value
	double value();
};

#endif
