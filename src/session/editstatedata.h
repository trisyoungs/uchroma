/*
	*** Edit State Data
	*** src/session/editstatedata.h
	Copyright T. Youngs 2013-2014

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

#ifndef UCHROMA_EDITSTATEDATA_H
#define UCHROMA_EDITSTATEDATA_H

#include "base/messenger.h"
#include "base/collection.h"
#include "render/linestyle.h"
#include "templates/list.h"

// Forward Declarations
/* none */

// Edit State Data
class EditStateData : public ListItem<EditStateData>
{
	public:
	// Constructor
	EditStateData();


	/*
	 * Data
	 */
	public:
	// Data Type
	enum EditStateDataType { IntegerData, DoubleData, StringData, Data2DData, LineStyleData, CollectionData, nDataTypes };

	private:
	// Name of contained data
	QString name_;
	// Type of contained data
	EditStateDataType type_;
	// Associated integer data
	int dataI_;
	// Associated double data
	double dataD_;
	// Associated string data
	QString dataS_;
	// Associated Collection data
	Collection dataCollection_;
	// Associated LineStyle data
	LineStyle dataLineStyle_;
	// Associated Data2D data
	Data2D dataData2D_;

	public:
	// Set data (int)
	void set(QString name, int value);
	// Set data (double)
	void set(QString name, double value);
	// Set data (QString)
	void set(QString name, QString value);
	// Set data (from Collection*)
	void set(QString name, Collection* value);
	// Set data (from Data2D*)
	void set(QString name, Data2D* value);
	// Set data (from LineStyle&)
	void set(QString name, LineStyle& value);
	// Return whether name of contained data matches that supplied
	bool isName(QString& name);
	// Return type of contained data
	EditStateDataType type();
	// Return integer value
	int asInt();
	// Return double value
	double asDouble();
	// Return string value
	QString asString();
	// Return reference to Data2D value
	Data2D& asData2D();
	// Return reference to Collection value
	Collection& asCollection();
	// Return LineStyle value
	LineStyle asLineStyle();
};

#endif
