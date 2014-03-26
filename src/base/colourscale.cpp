/*
	*** ColourScale
	*** src/base/colourscale.cpp
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

#include "base/colourscale.h"

/*
 * ColourScalePoint
 */

// Constructor
ColourScalePoint::ColourScalePoint() : ListItem<ColourScalePoint>()
{
	// Private variables
	value_ = 0.0;
	setColour(QColor(0,0,0,255));
}

// Return value of point
double ColourScalePoint::value() const
{
	return value_;
}

// Set value and colour for point
void ColourScalePoint::set(double value, QColor colour)
{
	value_ = value;
	colour_ = colour;
}

// Set value of point
void ColourScalePoint::setValue(double d)
{
	value_ = d;
}

// Set colour
void ColourScalePoint::setColour(QColor colour)
{
	colour_ = colour;
}

// Return colour
QColor ColourScalePoint::colour() const
{
	return colour_;
}

/*
// ColourScaleDelta
*/

// Constructor
ColourScaleDelta::ColourScaleDelta() : ListItem<ColourScaleDelta>()
{
	// Private variables
	start_ = 0.0;
	delta_ = 0.0;
}

// Check whether the delta 'contains' the supplied value
bool ColourScaleDelta::containsValue(double d) const
{
	if (d < start_) return false;
	if (d > (start_ + delta_)) return false;
	return true;
}

// Create delta from two existing colours
void ColourScaleDelta::set(ColourScalePoint* point1, ColourScalePoint* point2, bool useHSV)
{
	// Copy first colour point
	start_ = point1->value_;
	startColour_ = point1->colour();
	if (useHSV)
	{
		deltaColour_[0] = point2->colour_.hue() - startColour_.hue();
		deltaColour_[1] = point2->colour_.saturation() - startColour_.saturation();
		deltaColour_[2] = point2->colour_.value() - startColour_.value();
	}
	else
	{
		deltaColour_[0] = point2->colour_.red() - startColour_.red();
		deltaColour_[1] = point2->colour_.green() - startColour_.green();
		deltaColour_[2] = point2->colour_.blue() - startColour_.blue();
	}
	deltaColour_[3] = point2->colour_.alpha() - startColour_.alpha();
	delta_ = point2->value_ - start_;
}

// Get colour for value v
QColor ColourScaleDelta::colour(double value, bool useHSV) const
{
	// Clamp 'v' to range 0.0 - 1.0 to span range of delta
	double clampv = (value - start_) / delta_;
	if (clampv < 0.0) clampv = 0.0;
	else if (clampv > 1.0) clampv = 1.0;
	QColor col;
	if (useHSV)
	{
		col.setHsv(startColour_.hue() + deltaColour_[0] * clampv, startColour_.saturation() + deltaColour_[1] * clampv, startColour_.value() + deltaColour_[2] * clampv);
	}
	else
	{
		col.setRed(startColour_.red() + deltaColour_[0] * clampv);
		col.setGreen(startColour_.green() + deltaColour_[1] * clampv);
		col.setBlue(startColour_.blue() + deltaColour_[2]* clampv);
	}
	col.setAlpha(startColour_.alpha() + deltaColour_[3] * clampv);
	return col;
}

// Return the starting value of the range
double ColourScaleDelta::start() const
{
	return start_;
}

// Return the range of the delta
double ColourScaleDelta::delta() const
{
	return delta_;
}

/*
 * ColourScale
 */

// Constructor
ColourScale::ColourScale()
{
	// Private variables
	interpolated_ = true;
	useHSV_ = false;
}

// Copy Constructor
ColourScale::ColourScale(const ColourScale& source)
{
	(*this) = source;
}

// Assignment operator
void ColourScale::operator=(const ColourScale& source)
{
	clear();
	useHSV_ = source.useHSV_;
	for (ColourScalePoint* csp = source.points_.first(); csp != NULL; csp = csp->next) addPoint( csp->value(), csp->colour() );
	interpolated_ = source.interpolated_;
}

// Set whether the colourscale is interpolated
void ColourScale::setInterpolated(bool b)
{
	interpolated_ = b;
}

// Return whether the colourscale is interpolated
bool ColourScale::interpolated() const
{
	return interpolated_;
}

// Set whether or not to use HSV interpolation
bool ColourScale::setUseHSV(bool b)
{
	useHSV_ = b;
	calculateDeltas();
}

// Return whether or not to use HSV interpolation
bool ColourScale::useHSV() const
{
	return useHSV_;
}

// Recalculate colour deltas between points
void ColourScale::calculateDeltas()
{
	// Clear old list of deltas
	deltas_.clear();
	ColourScaleDelta *delta;
	for (ColourScalePoint *csp = points_.first(); csp != points_.last(); csp = csp->next)
	{
		delta = deltas_.add();
		delta->set(csp, csp->next, useHSV_);
	}
}

// Add point to scale
ColourScalePoint *ColourScale::addPoint(double value, QColor colour)
{
	ColourScalePoint *csp = NULL;
	
	// If supplied value is less than that at the start of the list, add it at the beginning.
	// If larget than the one at the end, then append it to the end of the list.
	// If neither of these, find the first existing value which is larger, and add it before that one
	if (points_.nItems() == 0) csp = points_.add();
	else if (value > points_.last()->value()) csp = points_.add();
	else for (ColourScalePoint* oldPoint = points_.first(); oldPoint != NULL; oldPoint = oldPoint->next) if (oldPoint->value() > value)
	{
		csp = points_.insertBefore(oldPoint);
		break;
	}
	if (csp == NULL) return NULL;

	// Now, set data in new point
	csp->setColour(colour);
	csp->setValue(value);

	// Recalculate colour deltas
	calculateDeltas();
	
	return csp;
}

// Set colour and value data for point
void ColourScale::setPoint(int position, double value, QColor colour, bool setval, bool setcol)
{
	// Check position supplied
	if ((position < 0) || (position >= points_.nItems()))
	{
		printf("Scale point position to set (%i) is invalid - nItems = %i.\n", position, points_.nItems());
		return;
	}
	if (setcol) points_[position]->setColour(colour);
	if (setval)
	{
		points_[position]->setValue(value);
		// Position in list may have changed - check...
		bool minBad = true, maxBad = true;
		ColourScalePoint* csp = points_[position];
		int dummy = 0;
		do
		{
// 			printf("BEFORE SHIFT Prev = %f, current= %f, next = %f\n", csp->prev ? csp->prev->value() : -999, csp->value(), csp->next ? csp->next->value() : -999);
			// Shift item if necessary
			if (csp->prev && (csp->prev->value() > csp->value()))
			{
				points_.shiftUp(csp);
				minBad = (csp->prev ? (csp->prev->value() > csp->value()) : false);
			}
			else minBad = false;
			if (csp->next && (csp->next->value() < csp->value()))
			{
				points_.shiftDown(csp);
				maxBad = (csp->next ? (csp->next->value() < csp->value()) : false);
			}
			else maxBad = false;
// 			printf("AFTER SHIFT %i Prev = %f, current= %f, next = %f  B=%i %i\n", dummy, csp->prev ? csp->prev->value() : -999, csp->value(), csp->next ? csp->next->value() : -999, minBad, maxBad);
			if (++dummy == 10) break;
			
		} while (minBad || maxBad);
	}

	// Recalculate colour deltas
	calculateDeltas();
}

// Set only value for point
void ColourScale::setPointValue(int position, double value)
{
	setPoint(position, value, QColor(), true, false);
}

// Set only colour for point
void ColourScale::setPointColour(int position, QColor colour)
{
	setPoint(position, 0.0f, colour, false, true);
}

// Remove old point from colourscale
void ColourScale::removePoint(int position)
{
	// Check position supplied
	if ((position < 0) || (position >= points_.nItems()))
	{
		printf("Scale point position to set (%i) is invalid - nItems = %i.\n", position, points_.nItems());
		return;
	}
	points_.remove( points_[position] );

	// Recalculate colour deltas
	calculateDeltas();
}

// Remove specified point from colourscale
void ColourScale::removePoint(ColourScalePoint* point)
{
	points_.remove(point);

	// Recalculate colour deltas
	calculateDeltas();
}

// Return colour associated with value provided
QColor ColourScale::colour(double value) const
{
	// Step through points associated to scale and find the two that we are inbetween.
	// Check for no points being defined
	if (points_.nItems() == 0) return QColor(0,0,0);

	ColourScalePoint *csp = points_.first();	
	// Is supplied value less than the value at the first point?
	if (value < csp->value()) return csp->colour();
	
	// Find the correct delta to use
	for (ColourScaleDelta *delta = deltas_.first(); delta != NULL; delta = delta->next)
	{
		if (delta->containsValue(value)) return delta->colour(interpolated_ ? value : delta->start(), useHSV_);
	}

	// If we get to here then the supplied value is outside the range of all values, so take colour from the endpoint
	return points_.last()->colour();
}

// Return number of points in colourscale
int ColourScale::nPoints() const
{
	return points_.nItems();
}

// Return first point in colourscale
ColourScalePoint *ColourScale::firstPoint() const
{
	return points_.first();
}

// Return last point in colourscale
ColourScalePoint *ColourScale::lastPoint() const
{
	return points_.last();
}

// Return specific point in colourscale
ColourScalePoint *ColourScale::point(int id)
{
	return points_[id];
}

// Return first delta in colourscale
ColourScaleDelta *ColourScale::firstDelta() const
{
	return deltas_.first();
}

// Clear all points in colourscale
void ColourScale::clear()
{
	points_.clear();
	deltas_.clear();
}

// Set all alpha values to that specified
void ColourScale::setAllAlpha(int alpha)
{
	QColor color;
	for (ColourScalePoint *csp = points_.first(); csp != NULL; csp = csp->next)
	{
		color = csp->colour();
		color.setAlpha(alpha);
		csp->setColour(color);
	}
	calculateDeltas();
}

