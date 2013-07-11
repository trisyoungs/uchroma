/*
	*** PlotData
	*** src/plotwidget_data.cpp
	Copyright T. Youngs 2012-2013

	This file is part of JournalViewer.

	JournalViewer is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	JournalViewer is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with JournalViewer.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "plotwidget.uih"
#include <math.h>

/*
// Plot Data Group
*/

// Constructor
PlotDataGroup::PlotDataGroup(QString name) : ListItem<PlotDataGroup>()
{
	name_ = name;
	visible_ = false;
	nDataSetsVisible_ = 0;
}

// Return name of this period
QString PlotDataGroup::name()
{
	return name_;
}

// Set availability
void PlotDataGroup::setVisible(bool avail)
{
	visible_ = avail;
}

// Return availability
bool PlotDataGroup::visible()
{
	return visible_;
}

// Modify dataset visibility count
void PlotDataGroup::changeVisibleCount(bool newDataSetVisibility)
{
	if (newDataSetVisibility) ++nDataSetsVisible_;
	else --nDataSetsVisible_;
}

// Return number of dataSets_ which are currently visible
int PlotDataGroup::nDataSetsVisible()
{
	return nDataSetsVisible_;
}

// Set general colour for this group
void PlotDataGroup::setColour(QColor colour)
{
	colour_ = colour;
}

// Return general colour for this group
QColor PlotDataGroup::colour()
{
	return colour_;
}

/*
 * PlotDataBlock
 */

// Static member
QVector<qreal> PlotDataBlock::lineStyles_[] = {
	QVector<qreal>() << 10 << 1,
	QVector<qreal>() << 2 << 2,
	QVector<qreal>() << 5 << 2
};

// Constructor
PlotDataBlock::PlotDataBlock(QString blockName) : ListItem<PlotDataBlock>()
{
	blockName_ = blockName;
	lineStyle_ = PlotDataBlock::SolidStyle;
	visible_ = false;
}

// Destructor
PlotDataBlock::~PlotDataBlock()
{
}

// Return name of block
QString PlotDataBlock::blockName()
{
	return blockName_;
}

// Set associated line style
void PlotDataBlock::setLineStyle(PlotDataBlock::BlockLineStyle style)
{
	lineStyle_ = style;
}

// Return associated line dash pattern
const QVector<qreal>& PlotDataBlock::dashes()
{
	return lineStyles_[lineStyle_];
}

// Set visibility of block
void PlotDataBlock::setVisible(bool visible)
{
	visible_ = visible;
}

// Return visibility of block
bool PlotDataBlock::visible()
{
	return visible_;
}

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
	parent_ = NULL;
}

/*!
 * \brief Destructor
 * \details Destructor for PlotData
 */
PlotData::~PlotData()
{
}

// Set source data
void PlotData::setData(PlotDataGroup* parent, Data2D& source, QString runNumber)
{
	parent_ = parent;
	data_ = source;
	name_ = runNumber;
}

// Return reference to contained data
Data2D& PlotData::data()
{
	return data_;
}

// Determine data limits
void PlotData::determineLimits()
{
	const double* xarray = data_.arrayX().array();
	const double* yarray = data_.arrayY().array();
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
		for (int n=1; n<data_.arrayX().nItems(); ++n)
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
	// Generate QPainterPath, determining minimum / maximum values along the way
	linePath_ = QPainterPath();
	QRect symbolRect(0, 0, 7, 7);
	const double* xarray = data_.arrayX().array();
	const double* yarray = data_.arrayY().array();
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
		for (int n=1; n<data_.arrayX().nItems(); ++n)
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
 * \brief Set block
 */
void PlotData::setBlock(PlotDataBlock* block)
{
	block_ = block;
}

/*!
 * \brief Return block
 */
PlotDataBlock* PlotData::block()
{
	return block_;
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
	if (parent_)
	{
		if (block_) return (parent_->visible() && block_->visible());
		else return parent_->visible();
	}
	else if (block_) return block_->visible();
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

/*!
 * \brief Return name
 */
QString PlotData::name()
{
	return name_;
}

// Return parent
PlotDataGroup* PlotData::parent()
{
	return parent_;
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
	if (parent_) pen.setColor(parent_->colour());
	else pen.setColor(lineColour_);
	
	// Get line style from PlotDataBlock (if available)
	if (block_) pen.setDashPattern(block_->dashes());
}
