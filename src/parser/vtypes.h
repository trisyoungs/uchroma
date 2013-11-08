/*
	*** Variable Types
	*** src/parser/vtypes.h
	Copyright T. Youngs 2010-2013

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

#ifndef UCHROMA_VTYPES_H
#define UCHROMA_VTYPES_H

// Variable description
class VTypes
{
	public:
	// Data Types
	enum DataType { NoData, IntegerData, DoubleData, nDataTypes };
	enum DataPair { ArrayMisMatch = -2, UntypedData = -1, Int = 1, Dbl = 2, IntInt = 5, IntDbl = 9, DblInt = 6, DblDbl = 10 };
	static VTypes::DataType dataType(const char *name, bool reporterror = 0);
	static const char *dataType(DataType dt);
	static const char *aDataType(DataType dt);
	static bool isPointer(DataType dt);
	static VTypes::DataType determineType(const char *s);
	static int dataPair(DataType type1, DataType type2);
	static int dataSinglet(DataType type1);
};

#endif
