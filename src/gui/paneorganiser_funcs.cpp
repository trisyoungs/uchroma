/*
	*** Pane Organiser
	*** src/gui/paneorganiser_funcs.cpp
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

#include "gui/paneorganiser.uih"
#include "base/viewlayout.h"
#include <QtGui/QPainter>

// Constructor
PaneOrganiser::PaneOrganiser(QWidget* parent) : QWidget(parent)
{
	// Layout
	viewLayout_ = NULL;
	layoutPixelHeight_ = 1;
	layoutPixelWidth_ = 1;
}

/*
 * Layout
 */

// Calculate pixel size based on current widget geometry
void PaneOrganiser::calculateLayoutPixelSize()
{
	if (viewLayout_ == NULL) return;

	layoutPixelWidth_ = width() / viewLayout_->nColumns();
	layoutPixelHeight_ = height() / viewLayout_->nRows();
}

// Set layout to display / organise
void PaneOrganiser::setViewLayout(ViewLayout* layout)
{
	viewLayout_ = layout;

	calculateLayoutPixelSize();

	update();
}

/*
 * Events
 */

void PaneOrganiser::dragEnterEvent(QDragEnterEvent *event)
{
}

void PaneOrganiser::dragLeaveEvent(QDragLeaveEvent *event)
{
}

void PaneOrganiser::dragMoveEvent(QDragMoveEvent *event)
{
}

void PaneOrganiser::dropEvent(QDropEvent *event)
{
}

void PaneOrganiser::mousePressEvent(QMouseEvent *event)
{
}

void PaneOrganiser::resizeEvent(QResizeEvent* event)
{
	calculateLayoutPixelSize();
	update();
}

// Paint event
void PaneOrganiser::paintEvent(QPaintEvent *event)
{
	const int paneMargin = 2;
	QPainter painter(this);

	QBrush paneBrush(QColor(222,222,222));
	QBrush whiteBrush(Qt::white);
	QPen pen(Qt::NoPen);
	painter.setPen(pen);

	// Set some rectangles
	QRect backgroundRect(0, 0, width(), height());
	QRect paneRect(0, 0, layoutPixelWidth_-2*paneMargin, layoutPixelHeight_-2*paneMargin);

	// Draw white background
	painter.setBrush(whiteBrush);
	painter.drawRect(backgroundRect);

	// Check for a valid layout - if there isn't one, we're done
	if (!viewLayout_)
	{
		painter.end();
		return;
	}

	// Draw pane grid
	painter.setBrush(paneBrush);
	for (int x = 0; x < viewLayout_->nColumns(); ++x)
	{
		for (int y = 0; y < viewLayout_->nRows(); ++y)
		{
			paneRect.moveTopLeft(QPoint(x*layoutPixelWidth_+paneMargin, height()-((y+1)*layoutPixelHeight_)));
			painter.drawRect(paneRect);
		}
	}
	painter.end();
}
