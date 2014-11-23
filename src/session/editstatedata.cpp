/*
	*** EditStateDataData
	*** src/session/editstatedata.cpp
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

#include "session/editstatedata.h"
#include "base/collection.h"
#include "base/messenger.h"

// Constructor
EditStateData::EditStateData() : ListItem<EditStateData>()
{
	type_ = EditStateData::nDataTypes;
	name_ = "";
	dataI_ = 0;
	dataD_ = 0.0;
	dataS_ = "";
}

/*
 * Data
 */

// Set data (int)
void EditStateData::set(QString name, int value)
{
	name_ = name;
	type_ = EditStateData::IntegerData;
	dataI_ = value;
}

// Set data (double)
void EditStateData::set(QString name, double value)
{
	name_ = name;
	type_ = EditStateData::DoubleData;
	dataD_ = value;
}

// Set data (QString)
void EditStateData::set(QString name, QString value)
{
	name_ = name;
	type_ = EditStateData::StringData;
	dataS_ = value;
}

// Set data (from Collection*)
void EditStateData::set(QString name, Collection* value)
{
	name_ = name;
	type_ = EditStateData::CollectionData;
	dataCollection_ = (*value);
}

// Set data (from Data2D*)
void EditStateData::set(QString name, Data2D* value)
{
	name_ = name;
	type_ = EditStateData::Data2DData;
	dataData2D_ = (*value);
}

// Set data (from LineStyle&)
void EditStateData::set(QString name, LineStyle& value)
{
	name_ = name;
	type_ = EditStateData::LineStyleData;
	dataLineStyle_ = value;
}

// Return whether name of contained data matches that supplied
bool EditStateData::isName(QString& name)
{
	return (name_ == name);
}

// Return type of contained data
EditStateData::EditStateDataType EditStateData::type()
{
	return type_;
}

// Return integer value
int EditStateData::asInt()
{
	if (type_ == EditStateData::IntegerData) return dataI_;
	else printf("Internal Error - Can't convert type %i into an integer.\n", type_);
	return 0;
}

// Return double value
double EditStateData::asDouble()
{
	if (type_ == EditStateData::DoubleData) return dataD_;
	else printf("Internal Error - Can't convert type %i into a double.\n", type_);
	return 0.0;
}

// Return string value
QString EditStateData::asString()
{
	if (type_ == EditStateData::StringData) return dataS_;
	else printf("Internal Error - Can't convert type %i into a QString.\n", type_);
	return QString();
}

// Return Data2D value
Data2D& EditStateData::asData2D()
{
	if (type_ == EditStateData::Data2DData) return dataData2D_;
	else printf("Internal Error - Can't convert type %i into a Data2D.\n", type_);
	return dataData2D_;
}

// Return Collection value
Collection& EditStateData::asCollection()
{
	if (type_ == EditStateData::CollectionData) return dataCollection_;
	else printf("Internal Error - Can't convert type %i into a Collection.\n", type_);
	return dataCollection_;
}

// Return LineStyle value
LineStyle EditStateData::asLineStyle()
{
	if (type_ == EditStateData::IntegerData) return dataLineStyle_;
	else printf("Internal Error - Can't convert type %i into a LineStyle.\n", type_);
	return dataLineStyle_;
}
