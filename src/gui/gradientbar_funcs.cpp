/*
	*** Gradient Bar
	*** src/gui/gradientbar.cpp
	Copyright T. Youngs 2013

	This file is part of FQPlot.

	FQPlot is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	FQPlot is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FQPlot.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/gradientbar.uih"
#include <QtGui/QPainter>

/*
 * Gradient Bar
 */

// Constructor
GradientBar::GradientBar(QWidget* parent) : QWidget(parent)
{
}

// Set local colourscale
void GradientBar::setColourScale(ColourScale& colourScale)
{
	colourScale_ = colourScale;
	repaint();
}

// Paint event callback
void GradientBar::paintEvent(QPaintEvent *event)
{
	if (colourScale_.nPoints() == 0) return;

	QPainter painter(this);

	// Setup gradient - in ObjectBoundingMode, 0.0 = top of rectangle, and 1.0 is bottom
	QLinearGradient gradient(0.0, 1.0, 0.0, 0.0);
	gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
	double zero = colourScale_.firstPoint()->value();
	double span = colourScale_.lastPoint()->value() - zero;

	// -- Loop backwards through points
	for (ColourScalePoint* csp = colourScale_.firstPoint(); csp != NULL; csp = csp->next)
	{
		gradient.setColorAt((csp->value() - zero) / span, csp->colour());
	}
	
	// Draw single rectangle and we're done
	QBrush brush(gradient);
	QRectF rect(0.0, 0.0, width()-1.0, height()-1.0);
	painter.setBrush(brush);
	painter.drawRect(rect);
	painter.end();
}
