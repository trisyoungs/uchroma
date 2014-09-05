/*
	*** Line Style
	*** src/base/linestyle.cpp
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

#include "render/linestyle.h"
#include <stdio.h>

// Constructor
LineStyle::LineStyle()
{
	width_ = 1.0;
	colour_.setRgb(0, 0, 0, 255);
	stipple_ = LineStipple::NoStipple;
}

// Destructor
LineStyle::~LineStyle()
{
}

// Copy constructor
LineStyle::LineStyle(const LineStyle& source)
{
	(*this) = source;
}

// Assignment operator
void LineStyle::operator=(const LineStyle& source)
{
	width_ = source.width_;
	stipple_ = source.stipple_;
	colour_ = source.colour_;
}

/*
 * Style
 */

// Set line style
void LineStyle::set(double width, LineStipple::StippleType stipple, QColor colour)
{
	width_ = width;
	stipple_ = stipple;
	colour_ = colour;
}

// Set line style
void LineStyle::set(double width, LineStipple::StippleType stipple, double r, double g, double b, double a)
{
	width_ = width;
	stipple_ = stipple;
	colour_.setRgbF(r, g, b, a);
}

// Set line width
void LineStyle::setWidth(double width)
{
	width_ = width;
}

// Return line width
double LineStyle::width()
{
	return width_;
}

// Set line stipple
void LineStyle::setStipple(LineStipple::StippleType stipple)
{
	stipple_ = stipple;
}

// Return line stipple
LineStipple::StippleType LineStyle::stipple()
{
	return stipple_;
}

// Set line colour
void LineStyle::setColour(QColor colour)
{
	colour_ = colour;
}

// Set line colour
void LineStyle::setColour(double r, double g, double b, double a)
{
	colour_.setRgbF(r, g, b, a);
}

// Return line colour
QColor LineStyle::colour()
{
	return colour_;
}

/*
 * GL
 */

// Apply line style
void LineStyle::apply()
{
	// -- Render axis (grid) lines
	glLineWidth(width_);
	LineStipple::stipple[stipple_].apply();
	GLfloat c[4] = { colour_.redF(), colour_.greenF(), colour_.blueF(), colour_.alphaF() };
	glColor4fv(c);
}

// Revert to normal line style (black, solid, 1.0px)
void LineStyle::revert()
{
	glLineWidth(1.0);
	LineStipple::stipple[LineStipple::NoStipple].apply();
	GLfloat c[4] = { 0.0, 0.0, 0.0, 1.0 };
	glColor4fv(c);
}
