/*
	*** Number Format
	*** src/base/numberformat.cpp
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

#include "base/numberformat.h"
#include <stdio.h>
#include <QtCore/QString>

// FormatType Keywords
const char* FormatTypeKeywords[] = { "Concise", "Decimal", "Integer", "Scientific" };

// Convert text string to FormatType
NumberFormat::FormatType NumberFormat::formatType(const char* s)
{
	for (int n=0; n<NumberFormat::nNumberFormats; ++n) if (strcmp(s,FormatTypeKeywords[n]) == 0) return (NumberFormat::FormatType) n;
	return NumberFormat::nNumberFormats;
}

// Convert FormatType to text string
const char* NumberFormat::formatType(NumberFormat::FormatType ft)
{
	return FormatTypeKeywords[ft];
}

// Constructor
NumberFormat::NumberFormat()
{
	type_ = NumberFormat::ConciseFormat;
	nDecimals_ = 4;
	forcePrecedingPlus_ = false;
	useUpperCaseExponent_ = true;
}

// Destructor
NumberFormat::~NumberFormat()
{
}

/*
 * Definition
 */

// Set ndex type
void NumberFormat::setType(NumberFormat::FormatType type)
{
	type_ = type;
}

// Return X index type
NumberFormat::FormatType NumberFormat::type()
{
	return type_;
}

// Set number of decimals to use
void NumberFormat::setNDecimals(int n)
{
	nDecimals_ = n;
}

// Return number of decimals to use
int NumberFormat::nDecimals()
{
	return nDecimals_;
}

// Set whether to force display of preceding '+' as well as '-'
void NumberFormat::setForcePrecedingPlus(bool b)
{
	forcePrecedingPlus_ = b;
}

// Return whether to force display of preceding '+' as well as '-'
bool NumberFormat::forcePrecedingPlus()
{
	return forcePrecedingPlus_;
}

// Set whether or not use use uppercase exponent character (for ScientificFormat)
void NumberFormat::setUseUpperCaseExponent(bool b)
{
	useUpperCaseExponent_ = b;
}

// Return whether or not use use uppercase exponent character (for ScientificFormat)
bool NumberFormat::useUpperCaseExponent()
{
	return useUpperCaseExponent_;
}

/*
 * Number Conversion
 */

// Return number formatted according to internal definition
QString NumberFormat::format(double number)
{
	static char result[128];

	switch (type_)
	{
		case (NumberFormat::IntegerFormat):
			if (forcePrecedingPlus_) sprintf(result, "%+i", int(number));
			else  sprintf(result, "%i", int(number));
			break;
		case (NumberFormat::DecimalFormat):
			if (forcePrecedingPlus_) sprintf(result, "%+1.*f", nDecimals_, number);
			else sprintf(result, "%1.*f", nDecimals_, number);
			break;
		case (NumberFormat::ScientificFormat):
			if (useUpperCaseExponent_)
			{
				if (forcePrecedingPlus_) sprintf(result, "%+1.*E", nDecimals_, number);
				else sprintf(result, "%1.*E", nDecimals_, number);
			}
			else
			{
				if (forcePrecedingPlus_) sprintf(result, "%+1.*e", nDecimals_, number);
				else sprintf(result, "%1.*e", nDecimals_, number);
			}
			break;
		case (NumberFormat::ConciseFormat):
			if (useUpperCaseExponent_)
			{
				if (forcePrecedingPlus_) sprintf(result, "%+1.*G", nDecimals_, number);
				else sprintf(result, "%1.*G", nDecimals_, number);
			}
			else
			{
				if (forcePrecedingPlus_) sprintf(result, "%+1.*g", nDecimals_, number);
				else sprintf(result, "%1.*g", nDecimals_, number);
			}
			break;
	}
	
	return QString(result);
}
