/*
	*** DataSpaceData
	*** src/base/dataspacedata.cpp
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

#include "base/dataspacedata.h"

// Add point to arrays
void DataSpaceData::addPoint(double x, double yRef, DisplayDataSet::DataPointType yType, double yCalc)
{
	x_.add(x);
	yReference_.add(yRef);
	yCalculated_.add(yCalc);
	yType_.add(yType);
}

// Add point to x and yReference only
void DataSpaceData::addPoint(double x, double yRef)
{
	x_.add(x);
	yReference_.add(yRef);
}

// Return X array
const Array<double> DataSpaceData::x()
{
	return x_;
}

// Return reference Y array
const Array<double> DataSpaceData::yReference()
{
	return yReference_;
}

// Set specified reference Y element
void DataSpaceData::setReferenceY(int index, double value)
{
	yReference_[index] = value;
}

// Zero reference Y array
void DataSpaceData::zeroYReference()
{
	yReference_ = 0.0;
}

// Set specified calcualted Y element
void DataSpaceData::setCalculatedY(int index, double value)
{
	yCalculated_[index] = value;
}

// Return caculated Y array
const Array<double> DataSpaceData::yCalculated()
{
	return yCalculated_;
}

// Return Y type array
const Array<DisplayDataSet::DataPointType> DataSpaceData::yType()
{
	return yType_;
}

// Set z value
void DataSpaceData::setZ(double z)
{
	z_ = z;
}

// Return z value
double DataSpaceData::z() const
{
	return z_;
}
