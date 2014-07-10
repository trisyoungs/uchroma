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
#include <QMouseEvent>

// Constructor
PaneOrganiser::PaneOrganiser(QWidget* parent) : QWidget(parent)
{
	setAcceptDrops(true);
 
	// Layout
	viewLayout_ = NULL;
	layoutPixelHeight_ = 1;
	layoutPixelWidth_ = 1;
	dragPane_ = NULL;
	dragGridReference_ = QPoint();
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

// Return grid reference containing coordinates supplied
QPoint PaneOrganiser::gridReference(QPoint point)
{
	if ((point.x() < 0) || (point.x() > width())) return QPoint(-1,-1);
	else if ((point.y() < 0) || (point.y() > height())) return QPoint(-1,-1);

	// Divide by pixel width and return
	return QPoint(point.x() / layoutPixelWidth_, (height() - point.y()) / layoutPixelHeight_);
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
	// Is the correct data type being dragged over us?
	if (event->mimeData()->hasFormat("image/x-uchroma-pane")) event->accept();
	else event->ignore();
}

void PaneOrganiser::dragLeaveEvent(QDragLeaveEvent *event)
{
	// Object has been dragged outside the organiser
	update();
	event->accept();
}

void PaneOrganiser::dragMoveEvent(QDragMoveEvent *event)
{
	// Get grid reference currently under mouse
	dragGridReference_ = gridReference(event->pos());

	if (event->mimeData()->hasFormat("image/x-uchroma-pane"))
	{
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
	else event->ignore();

	update();
}

void PaneOrganiser::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasFormat("image/x-uchroma-pane"))
	{
		viewLayout_->translatePane(dragPane_, dragGridReference_.x()-clickedGridReference_.x(), dragGridReference_.y()-clickedGridReference_.y());

		dragPane_ = NULL;
		update();

		event->setDropAction(Qt::MoveAction);
		event->accept();

		// Need to update the main view now
		emit(updateMainDisplay());
	}
	else event->ignore();
}

void PaneOrganiser::mousePressEvent(QMouseEvent *event)
{
	// If the mouse press wasn't left button, ignore it
	if (! event->buttons().testFlag(Qt::LeftButton)) return;

	// First, get the local grid reference under the mouse pointer
	clickedGridReference_ = gridReference(event->pos());
	if (clickedGridReference_.x() == -1) return;

	// Is there a pane at this reference?
	dragPane_ = viewLayout_->paneAtGrid(clickedGridReference_.x(), clickedGridReference_.y());
	if (!dragPane_) return;

	// Update view immediately so the pane disappears
	update();

	// Draw image of the pane
	QPixmap paneImage(dragPane_->width()*layoutPixelWidth_, dragPane_->height()*layoutPixelHeight_);
	int paneIndex = viewLayout_->paneIndex(dragPane_);
	QPainter painter(&paneImage);
	painter.setBrush(QColor(paneIndex&1 ? 255 : 128, paneIndex&2 ? 255 : 128, paneIndex&4 ? 255 : 128, 255));
	painter.drawRect(QRect(0, 0, paneImage.width(), paneImage.height()));
	painter.end();

	// Construct mime data for drag event
	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);
	dataStream << clickedGridReference_;
	QMimeData *mimeData = new QMimeData;
	mimeData->setData("image/x-uchroma-pane", itemData);

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);
	drag->setHotSpot(event->pos() - QPoint(dragPane_->leftEdge()*layoutPixelWidth_, height()-(dragPane_->bottomEdge()+dragPane_->height())*layoutPixelHeight_));
	drag->setPixmap(paneImage);

	// Begin the drag event
	if (drag->exec(Qt::MoveAction) != Qt::MoveAction)
	{
		// Reset everything, and do not perform a move
		dragPane_ = NULL;
		dragGridReference_ = QPoint();
		update();
	}
}

void PaneOrganiser::resizeEvent(QResizeEvent* event)
{
	calculateLayoutPixelSize();
	update();
}

// Paint event
void PaneOrganiser::paintEvent(QPaintEvent *event)
{
	const int paneMargin = 4;
	QPainter painter(this);

	QBrush paneBrush(QColor(234,234,234));
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
			paneRect.moveTopLeft(QPoint(x*layoutPixelWidth_+paneMargin, height()-((y+1)*layoutPixelHeight_)+paneMargin));
			painter.drawRect(paneRect);
		}
	}

	// Draw panes
	int colourCount = 0;
	QColor paneColour;
	for (ViewPane* pane = viewLayout_->panes(); pane != NULL; pane = pane->next)
	{
		// Set pane brush here, so we always cycle through the colours
		paneColour.setRgb(colourCount&1 ? 255 : 0, colourCount&2 ? 255 : 0, colourCount&4 ? 255 : 0, 128);

		++colourCount;

		// If there is a current dragPane_, draw a drop indicator highlighting the new position instead of the actual pane
		if (pane == dragPane_)
		{
			paneColour.setAlpha(60);
			painter.setBrush(paneColour);
			painter.setPen(Qt::NoPen);
			QPoint newBottomLeft = QPoint(pane->leftEdge(), pane->bottomEdge()) + (dragGridReference_ - clickedGridReference_);
			paneRect.setCoords(newBottomLeft.x()*layoutPixelWidth_, height()-newBottomLeft.y()*layoutPixelHeight_, (newBottomLeft.x()+pane->width())*layoutPixelWidth_-2, height()-(newBottomLeft.y()+pane->height())*layoutPixelHeight_-2);
		}
		else
		{
			painter.setBrush(paneColour);
			painter.setPen(Qt::NoPen);
			// Construct a rectangle for this pane
			paneRect.setCoords(pane->leftEdge()*layoutPixelWidth_, height()-pane->bottomEdge()*layoutPixelHeight_, (pane->leftEdge()+pane->width())*layoutPixelWidth_, height()-(pane->bottomEdge()+pane->height())*layoutPixelHeight_);
		}
		painter.drawRect(paneRect);
	}

	painter.end();
}
