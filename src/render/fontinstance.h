/*
	*** Font (from FTGL)
	*** src/render/fontinstance.h
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

#ifndef UCHROMA_FONTINSTANCE_H
#define UCHROMA_FONTINSTANCE_H

#include <FTGL/ftgl.h>
#include <QtCore/QString>

// Forward Declarations
/* none */

// Static Font Instance
class FontInstance
{
	private:
	// Font file last passed to setupFont()
	static QString fontFile_;
	// FTGL font for text
	static FTFont* font_;
	// Basic font height (from baseline to top of normal height letter)
	static double fontBaseHeight_;

	public:
	// Setup font specified
	static bool setupFont(QString fontName);
	// Return whether font exists and is ready for use
	static bool fontOK();
	// Return current font
	static FTFont* font();
	// Return base height of font
	static double fontBaseHeight();
	// Return bounding box for specified string
	static FTBBox boundingBox(QString text);
};

#endif
