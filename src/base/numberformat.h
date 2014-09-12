/*
	*** Number Format
	*** src/base/numberformat.h
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

#ifndef UCHROMA_NUMBERFORMAT_H
#define UCHROMA_NUMBERFORMAT_H

#include <QtCore/QString>

// Forward Declarations
/* none */

// Number Format
class NumberFormat
{
	public:
	// Constructor / Destructor
	NumberFormat();
	~NumberFormat();
	// Basic Number Type
	enum FormatType { IntegerFormat, DecimalFormat, ScientificFormat, nNumberFormats };
	// Convert text string to FormatType
	static FormatType formatType(const char* s);
	// Convert FormatType to text string
	static const char* formatType(FormatType id);


	/*
	 * Definition
	 */
	private:
	// Format type
	FormatType type_;
	// Number of decimals in decimal format
	int nDecimals_;
	// Number of decimals in scientific format
	int nScientificDecimals_;
	// Whether to force display of preceding '+' as well as '-'
	bool forcePrecedingPlus_;
	// Whether or not use use uppercase exponent character (for ScientificFormat)
	bool useUpperCaseExponent_;

	public:
	// Set format type
	void setType(NumberFormat::FormatType type);
	// Return format type
	NumberFormat::FormatType type();
	// Set number of decimals in decimal format
	void setNDecimals(int n);
	// Return number of decimals in decimal format
	int nDecimals();
	// Set number of decimals in scientific format
	void setNScientificDecimals(int n);
	// Return number of decimals in scientific format
	int nScientificDecimals();
	// Set whether to force display of preceding '+' as well as '-'
	void setForcePrecedingPlus(bool b);
	// Return whether to force display of preceding '+' as well as '-'
	bool forcePrecedingPlus();
	// Set whether or not use use uppercase exponent character (for ScientificFormat)
	void setUseUpperCaseExponent(bool b);
	// Return whether or not use use uppercase exponent character (for ScientificFormat)
	bool useUpperCaseExponent();


	/*
	 * Number Conversion
	 */
	public:
	// Return number formatted according to internal definition
	QString format(double number);
};

#endif
