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
	paneMargin_= 4;
	viewLayout_ = NULL;
	layoutPixelHeight_ = 1;
	layoutPixelWidth_ = 1;
	paneUnderMouse_ = NULL;
	dragPane_ = NULL;
	dragGridReference_ = QPoint();
	stretchPane_ = NULL;
	stretchHandle_ = PaneOrganiser::nHandles;
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

	if (layout->panes()) emit(currentPaneChanged(layout->panes()->leftEdge(), layout->panes()->bottomEdge()));

	calculateLayoutPixelSize();

	update();
}

/*
 * Interaction
 */

// Return handle under mouse (if any)
PaneOrganiser::PaneHandle PaneOrganiser::handleUnderMouse(ViewPane* pane, int mouseX, int mouseY)
{
	// Handle size is twice the paneMargin_
	QRect handleRect(0, 0, paneMargin_*2, paneMargin_*2);
	QPoint paneBottomLeft = QPoint(pane->leftEdge()*layoutPixelWidth_, pane->bottomEdge()*layoutPixelHeight_);
	QPoint paneSize = QPoint(pane->width()*layoutPixelWidth_, pane->height()*layoutPixelHeight_);

	// -- Bottom left
	handleRect.moveBottomLeft(QPoint(paneBottomLeft.x(), paneBottomLeft.y()));
	printf("BL %i %i   MOUSE %i %i\n", handleRect.x(), handleRect.y(), mouseX, mouseY);
	if (handleRect.contains(mouseX, mouseY)) return PaneOrganiser::BottomLeftHandle;
	// -- Bottom middle
	handleRect.moveBottomLeft(QPoint(paneBottomLeft.x()+0.5*paneSize.x()-paneMargin_, height()-paneBottomLeft.y()));
	if (handleRect.contains(mouseX, mouseY)) return PaneOrganiser::BottomMiddleHandle;
	// -- Bottom right
	handleRect.moveBottomRight(QPoint(paneBottomLeft.x()+paneSize.x(), height()-pane->bottomEdge()*layoutPixelHeight_));
	if (handleRect.contains(mouseX, mouseY)) return PaneOrganiser::BottomRightHandle;
	// -- Middle left
	handleRect.moveBottomLeft(QPoint(paneBottomLeft.x(), height()-(paneBottomLeft.y()+0.5*paneSize.y()-paneMargin_)));
	if (handleRect.contains(mouseX, mouseY)) return PaneOrganiser::MiddleLeftHandle;
	// -- Middle right
	handleRect.moveBottomRight(QPoint(paneBottomLeft.x()+paneSize.x(), height()-(paneBottomLeft.y()+0.5*paneSize.y()-paneMargin_)));
	if (handleRect.contains(mouseX, mouseY)) return PaneOrganiser::MiddleRightHandle;
	// -- Top left
	handleRect.moveTopLeft(QPoint(paneBottomLeft.x(), height()-(paneBottomLeft.y()+paneSize.y())));
	if (handleRect.contains(mouseX, mouseY)) return PaneOrganiser::TopLeftHandle;
	// -- Top middle
	handleRect.moveTopLeft(QPoint(paneBottomLeft.x()+0.5*paneSize.x()-paneMargin_, height()-(paneBottomLeft.y()+paneSize.y())));
	if (handleRect.contains(mouseX, mouseY)) return PaneOrganiser::TopMiddleHandle;
	// -- Top right
	handleRect.moveTopRight(QPoint(paneBottomLeft.x()+paneSize.x(), height()-(paneBottomLeft.y()+paneSize.y())));
	if (handleRect.contains(mouseX, mouseY)) return PaneOrganiser::TopRightHandle;

	return PaneOrganiser::nHandles;
}

/*
 * Qt
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

	// Emit currentPaneChanged()
	emit(currentPaneChanged(clickedGridReference_.x(), clickedGridReference_.y()));

	// Update view immediately so the pane being dragged disappears
	update();

	// Draw image of the pane
	QPixmap paneImage(dragPane_->width()*layoutPixelWidth_, dragPane_->height()*layoutPixelHeight_);
	int paneIndex = viewLayout_->paneIndex(dragPane_);
	QPainter painter(&paneImage);
	painter.setBrush(QColor(paneIndex&1 ? 255 : 128, paneIndex&2 ? 255 : 128, paneIndex&4 ? 255 : 128, 255));
	painter.setPen(Qt::NoPen);
	QRect paneRect(0, 0, paneImage.width(), paneImage.height());
	painter.drawRect(paneRect);
	painter.drawText(paneRect,  Qt::AlignHCenter | Qt::AlignVCenter, dragPane_->name());
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

void PaneOrganiser::mouseMoveEvent(QMouseEvent *event)
{
	// If we are currently in the middle of a drag, ignore this event
	if (dragPane_) event->ignore();

	// Check for a pane under the mouse and, if there is one, see if we're over a handle
	QPoint gridRef = gridReference(event->pos());
	ViewPane* hoverPane = viewLayout_->paneAtGrid(gridRef.x(), gridRef.y());
	PaneOrganiser::PaneHandle handle = PaneOrganiser::nHandles;
	if (hoverPane) handle = handleUnderMouse(hoverPane, event->x(), event->y());

	// Check values and update widget if necessary
	bool updateWidget = false;
	if (hoverPane != paneUnderMouse_) updateWidget = true;
	else if (handle != stretchHandle_) updateWidget = true;
	paneUnderMouse_ = hoverPane;
	stretchHandle_ = handle;

	if (updateWidget) update();

	event->accept();
}

void PaneOrganiser::mouseReleaseEvent(QMouseEvent *event)
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
	QBrush paneBrush(QColor(234,234,234));
	QBrush whiteBrush(Qt::white);
	QPen pen(Qt::NoPen);

	// Update pixel sizes
	calculateLayoutPixelSize();

	QPainter painter(this);
	painter.setPen(pen);

	// Set some rectangles
	QRect backgroundRect(0, 0, width(), height());
	QRect handleRect(0, 0, paneMargin_*2, paneMargin_*2);
	QRect paneRect(0, 0, layoutPixelWidth_-2*paneMargin_, layoutPixelHeight_-2*paneMargin_);

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
			paneRect.moveTopLeft(QPoint(x*layoutPixelWidth_+paneMargin_, height()-((y+1)*layoutPixelHeight_)+paneMargin_));
			painter.drawRect(paneRect);
		}
	}

	// Draw panes
	int colourCount = 0;
	QColor paneColour;
	QPoint paneBottomLeft, paneSize;
	for (ViewPane* pane = viewLayout_->panes(); pane != NULL; pane = pane->next)
	{
		// Set pane brush here, so we always cycle through the colours
		paneColour.setRgb(colourCount&1 ? 255 : 0, colourCount&2 ? 255 : 0, colourCount&4 ? 255 : 0, 128);
		painter.setPen(Qt::NoPen);

		++colourCount;

		// Calculate some useful points
		paneBottomLeft = QPoint(pane->leftEdge()*layoutPixelWidth_, pane->bottomEdge()*layoutPixelHeight_);
		paneSize = QPoint(pane->width()*layoutPixelWidth_, pane->height()*layoutPixelHeight_);

		// If there is a current dragPane_, draw a drop indicator highlighting the new position instead of the actual pane
		if (pane == dragPane_)
		{
			paneColour.setAlpha(60);
			painter.setBrush(paneColour);
			QPoint newBottomLeft = QPoint(pane->leftEdge(), pane->bottomEdge()) + (dragGridReference_ - clickedGridReference_);
			paneRect.setCoords(newBottomLeft.x()*layoutPixelWidth_, height()-newBottomLeft.y()*layoutPixelHeight_, (newBottomLeft.x()+pane->width())*layoutPixelWidth_-2, height()-(newBottomLeft.y()+pane->height())*layoutPixelHeight_-2);
		}
		else if (pane == stretchPane_)
		{
		}
		else
		{
			// Construct a rectangle for this pane
			painter.setBrush(paneColour);
			paneRect.setCoords(paneBottomLeft.x(), height()-paneBottomLeft.y(), paneBottomLeft.x()+paneSize.x(), height()-(paneBottomLeft.y()+paneSize.y()));
		}
		painter.drawRect(paneRect);

		// Draw on text
		painter.setPen(Qt::black);
		painter.drawText(paneRect, Qt::AlignHCenter | Qt::AlignVCenter, pane->name());

		// Draw on resizing handles
		if ((pane == paneUnderMouse_) && (dragPane_ == NULL))
		{
			paneColour.setAlpha(255);
			painter.setPen(Qt::NoPen);
			painter.setBrush(paneColour);
			// -- Bottom left
			handleRect.moveBottomLeft(QPoint(paneBottomLeft.x(), height()-paneBottomLeft.y()));
			painter.setBrush(stretchHandle_ == PaneOrganiser::BottomLeftHandle ? whiteBrush : paneColour);
			painter.drawRect(handleRect);
			// -- Bottom middle
			handleRect.moveBottomLeft(QPoint(paneBottomLeft.x()+0.5*paneSize.x()-paneMargin_, height()-paneBottomLeft.y()));
			painter.setBrush(stretchHandle_ == PaneOrganiser::BottomMiddleHandle ? whiteBrush : paneColour);
			painter.drawRect(handleRect);
			// -- Bottom right
			handleRect.moveBottomRight(QPoint(paneBottomLeft.x()+paneSize.x(), height()-pane->bottomEdge()*layoutPixelHeight_));
			painter.setBrush(stretchHandle_ == PaneOrganiser::BottomRightHandle ? whiteBrush : paneColour);
			painter.drawRect(handleRect);
			// -- Middle left
			handleRect.moveBottomLeft(QPoint(paneBottomLeft.x(), height()-(paneBottomLeft.y()+0.5*paneSize.y()-paneMargin_)));
			painter.setBrush(stretchHandle_ == PaneOrganiser::MiddleLeftHandle ? whiteBrush : paneColour);
			painter.drawRect(handleRect);
			// -- Middle right
			handleRect.moveBottomRight(QPoint(paneBottomLeft.x()+paneSize.x(), height()-(paneBottomLeft.y()+0.5*paneSize.y()-paneMargin_)));
			painter.setBrush(stretchHandle_ == PaneOrganiser::MiddleRightHandle ? whiteBrush : paneColour);
			painter.drawRect(handleRect);
			// -- Top left
			handleRect.moveTopLeft(QPoint(paneBottomLeft.x(), height()-(paneBottomLeft.y()+paneSize.y())));
			painter.setBrush(stretchHandle_ == PaneOrganiser::TopLeftHandle ? whiteBrush : paneColour);
			painter.drawRect(handleRect);
			// -- Top middle
			handleRect.moveTopLeft(QPoint(paneBottomLeft.x()+0.5*paneSize.x()-paneMargin_, height()-(paneBottomLeft.y()+paneSize.y())));
			painter.setBrush(stretchHandle_ == PaneOrganiser::TopMiddleHandle ? whiteBrush : paneColour);
			painter.drawRect(handleRect);
			// -- Top right
			handleRect.moveTopRight(QPoint(paneBottomLeft.x()+paneSize.x(), height()-(paneBottomLeft.y()+paneSize.y())));
			painter.setBrush(stretchHandle_ == PaneOrganiser::TopRightHandle ? whiteBrush : paneColour);
			painter.drawRect(handleRect);
		}
	}

	painter.end();
}
