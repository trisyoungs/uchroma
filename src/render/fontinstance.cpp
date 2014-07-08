/*
	*** Font Instance
	*** src/render/fontinstance.cpp
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

#include "render/fontinstance.h"

// Static Members
QString FontInstance::fontFile_ = "";
FTFont* FontInstance::font_ = NULL;
double FontInstance::fontBaseHeight_ = 0.0;

// Setup font specified
bool FontInstance::setupFont(QString fontName)
{
	// If the current font is valid, and matches the name of the new font supplied, do nothing
	if (font_ && (fontFile_ == fontName)) return true;

	if (font_) delete font_;
	font_ = NULL;
	fontFile_ = fontName;

	FTPolygonFont* newFont = new FTPolygonFont(qPrintable(fontName));
	if (newFont->Error())
	{
		delete newFont;
		fontBaseHeight_ = 1.0;
		return false;
	}
	else
	{
		font_ = newFont;
// 		font_->Depth(3.0);
// 		font_->Outset(-.5, 1.5);
		font_->FaceSize(1);
		FTBBox boundingBox = font_->BBox("ABCDEfghijKLMNOpqrstUVWXYz");
		fontBaseHeight_ = boundingBox.Upper().Yf() - boundingBox.Lower().Yf();
	}

	return (font_ != NULL);
}

// Return whether font exists and is ready for use
bool FontInstance::fontOK()
{
	return (font_ != NULL);
}

// Return current font
FTFont* FontInstance::font()
{
	return font_;
}

// Return base height of font
double FontInstance::fontBaseHeight()
{
	return fontBaseHeight_;
}

// Return bounding box for specified string
FTBBox FontInstance::boundingBox(QString text)
{
	if (font_) return font_->BBox(qPrintable(text));
	else return FTBBox();
}