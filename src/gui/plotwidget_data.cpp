/*
	*** PlotData
	*** src/plotwidget_data.cpp
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

#include "plotwidget.uih"
#include <math.h>

/*
// Plot Data
*/

/*!
 * \brief Constructor
 * \details Constructor for PlotData
 */
PlotData::PlotData() : ListItem<PlotData>()
{
	lineColour_ = Qt::black;
	lineStyle_ = Qt::SolidLine;
	xMin_ = 0.0;
	xMax_ = 0.0;
	yMin_ = 0.0;
	yMax_ = 0.0;
	data_ = NULL;
}

/*!
 * \brief Destructor
 * \details Destructor for PlotData
 */
PlotData::~PlotData()
{
}

// Set source data
void PlotData::setData(ExtractedSlice* source)
{
	data_ = source;
}

// Return reference to contained data
ExtractedSlice* PlotData::data()
{
	return data_;
}

// Determine data limits
void PlotData::determineLimits()
{
	// Check validity of data_ pointer
	if (data_ == NULL)
	{
// 		msg.print("Internal Error: PlotData has no valid data_ pointer.\n");
		return;
	}

	const double* xarray = data_->transformedData().arrayX().array();
	const double* yarray = data_->transformedData().arrayY().array();
	double x, y;
	if ((xarray != NULL) && (yarray != NULL))
	{
		// Get first point and use to set initial limits
		x = xarray[0];
		y = yarray[0];
		xMin_ = x;
		xMax_ = x;
		yMin_ = y;
		yMax_ = y;
		for (int n=1; n<data_->transformedData().arrayX().nItems(); ++n)
		{
			// Grab modified array values
			x = xarray[n];
			y = yarray[n];

			// Recalculate min/max values
			if (x > xMax_) xMax_ = x;
			if (x < xMin_) xMin_ = x;
			if (y > yMax_) yMax_ = y;
			if (y < yMin_) yMin_ = y;
		}
	}
}

// Regenerate painter path
void PlotData::generatePainterPaths(double xScale, double yScale)
{
	// Check the scale values at which the path was last created at - if it hasn't changed, don't bother recreating the path again...
	if ( (fabs(xScale-lastXScale_) < 1.0e-10) && (fabs(yScale-lastYScale_) < 1.0e-10)) return;

	// Reset painter path
	linePath_ = QPainterPath();
	
	// Check validity of data_ pointer
	if (data_ == NULL)
	{
// 		msg.print("Internal Error: PlotData has no valid data_ pointer.\n");
		return;
	}

	// Generate QPainterPath, determining minimum / maximum values along the way
	QRect symbolRect(0, 0, 7, 7);
	const double* xarray = data_->transformedData().arrayX().array();
	const double* yarray = data_->transformedData().arrayY().array();
	int enumY;
	double x, y, lastX, lastScaledY, scaledX, scaledY;
	if ((xarray != NULL) && (yarray != NULL))
	{
		// Get first point and use to set painter path origin
		x = xarray[0];
		y = yarray[0];
		linePath_.moveTo(x * xScale, y * yScale);
		lastX = x;
		lastScaledY = y * yScale;
		for (int n=1; n<data_->transformedData().arrayX().nItems(); ++n)
		{
			// Grab modified array values
			x = xarray[n];
			y = yarray[n];

			// Use scaled (screen) coordinates for point
			scaledX = x*xScale;
			scaledY = y*yScale;

			// Draw the next line
			linePath_.lineTo(scaledX, scaledY);

			// Store current X and scaled Y values
			lastX = x;
			lastScaledY = scaledY;
		}
	}
}

/*!
 * \brief Return line QPainterPath
 */
QPainterPath& PlotData::linePath()
{
	return linePath_;
}

/*!
 * \brief Return whether this data is visible
 */
bool PlotData::visible()
{
	if (data_ && data_->group()) return data_->group()->visible();
	else return true;
}

/*!
 * \brief Set vertical offset to apply to data
 */
void PlotData::setVerticalOffset(int offset)
{
	verticalOffset_ = offset;
}

/*!
 * \brief Return vertical offset to apply to data
 */
double PlotData::verticalOffset()
{
	return verticalOffset_;
}

/*!
 * \brief Return minimum x value for data
 */
double PlotData::xMin()
{
	return xMin_;
}

/*!
 * \brief Return maximum x value for data
 */
double PlotData::xMax()
{
	return xMax_;
}

/*!
 * \brief Return minimum y value for data
 */
double PlotData::yMin()
{
	return yMin_;
}

/*!
 * \brief Return maximum y value for data
 */
double PlotData::yMax()
{
	return yMax_;
}

/*
// Style
*/

/*!
 * \brief Set line colour
 */
void PlotData::setLineColour(QColor color)
{
	lineColour_ = color;
}

/*!
 * \brief Return line colour
 */
QColor PlotData::lineColour()
{
	return lineColour_;
}

/*!
 * \brief Set line style
 */
void PlotData::setLineStyle(Qt::PenStyle style)
{
	lineStyle_ = style;
}

/*!
 * \brief Return line style
 */
Qt::PenStyle PlotData::lineStyle()
{
	return lineStyle_;
}

// Set supplied pen colour and line style
void PlotData::stylePen(QPen& pen)
{
	// Get colour from PlotDataGroup parent (if available)
	pen.setColor(lineColour_);

	// Get line style from PlotDataGroup (if available)
	if (data_ && data_->group()) pen.setDashPattern(data_->group()->dashes());
}
