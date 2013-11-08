/*
	*** Variable Types
	*** src/parser/types.h
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

#include "parser/vtypes.h"
#include "math/constants.h"
#include "base/sysfunc.h"
#include <string.h>
#include <stdio.h>

// Variable Types
const char *DataTypeNames[VTypes::nDataTypes] = { "no data", "int", "double" };
const char *DataTypePhrases[VTypes::nDataTypes] = { "no data", "an integer", "a double" };
const char *DataTypeKeywords[VTypes::nDataTypes] = { "_NODATA", "int", "double" };
VTypes::DataType VTypes::dataType(const char *s, bool reporterror)
{
	VTypes::DataType dt = (VTypes::DataType) enumSearch("", VTypes::nDataTypes, DataTypeKeywords, s);
	if ((dt == VTypes::nDataTypes) && reporterror) enumPrintValid(VTypes::nDataTypes,DataTypeKeywords);
	return dt;
}
const char *VTypes::dataType(VTypes::DataType dt)
{
	return DataTypeNames[dt];
}
const char *VTypes::aDataType(VTypes::DataType dt)
{
        return DataTypePhrases[dt];
}
VTypes::DataType VTypes::determineType(const char *s)
{
	// Try to determine type_ of the argument
	int ch, nn = 0, nch = 0, ndp = 0, npm = 0, ne = 0;
	unsigned int i;
	for (i = 0; i < strlen(s); i++)
	{
		ch = s[i];
		if ((ch > 47) && (ch < 58)) nn ++;
		else if (ch == '.') ndp ++;
		else if ((ch == '-') || (ch == '+')) npm ++;
		else if ((ch == 'e') || (ch == 'E')) ne ++;
		else nch ++;
	}
	// Based on the numbers we calculated, try to determine its type
	if ((nch != 0) || (ndp > 1) || (npm > 2) || (ne > 1) || (nn == 0)) return VTypes::NoData;
	else if (ndp == 1) return VTypes::DoubleData;
	else return VTypes::IntegerData;
}

int VTypes::dataPair(DataType type1, DataType type2)
{
	if ((type1 == VTypes::NoData) || (type2 == VTypes::NoData))
	{
		printf("One or both arguments have no data type.\n");
		return UntypedData;
	}
	int bit1 = (1 << (type1 < nDataTypes ? type1-1 : nDataTypes-1));
	int bit2 = (1 << (type2 < nDataTypes ? type2-1 : nDataTypes-1));
	printf("bit1/2 = %i/%i\n", bit2, bit2);
	return (bit1 + (bit2 << 2));
}

int VTypes::dataSinglet(DataType type1)
{
	if (type1 == VTypes::NoData)
	{
		printf("Argument has no data type.\n");
		return UntypedData;
	}
	return (1 << (type1 < nDataTypes ? type1-1 : nDataTypes-1));
}
