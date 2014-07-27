/*
	*** ViewLayout
	*** src/base/viewlayout.cpp
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

#include "base/viewlayout.h"
#include "gui/uchroma.h"

// Constructor
ViewLayout::ViewLayout(UChromaWindow& parent) : ListItem<ViewLayout>(), parent_(parent)
{
	nColumns_ = 1;
	nRows_ = 1;
	pixelWidth_ = 0;
	pixelHeight_ = 0;
	remainingWidth_ = 0;
	remainingHeight_ = 0;
}

// Destructor
ViewLayout::~ViewLayout()
{
}

// Copy constructor
ViewLayout::ViewLayout(const ViewLayout& source) : parent_(parent_)
{
	(*this) = source;
}

// Assignment operator
void ViewLayout::operator=(const ViewLayout& source)
{
	// Clear current data
	panes_.clear();

	// Copy source data
	nColumns_ = source.nColumns_;
	nRows_ = source.nRows_;
	panes_ = source.panes_;
}

/*
 * Parent
 */

// Set as modified (call parent routine)
void ViewLayout::setAsModified(ViewPane* caller)
{
	if (caller) caller->recalculateViewport(pixelWidth_, pixelHeight_, nColumns_, nRows_, remainingWidth_, remainingHeight_);

	parent_.setAsModified();
}

/*
 * Layout
 */

// Recalculate pixel dimensions and remainder
void ViewLayout::recalculatePixels()
{
	// Set new pixel widths
	pixelWidth_ = layoutWidth_ / nColumns_;
	pixelHeight_ = layoutHeight_ / nRows_;

	// Recalculate pane sizes
	remainingWidth_ = layoutWidth_ - nColumns_*pixelWidth_;
	remainingHeight_ = layoutHeight_ - nRows_*pixelHeight_;
	for (ViewPane* pane = panes_.first(); pane != NULL; pane = pane->next) pane->recalculateViewport(pixelWidth_, pixelHeight_, nColumns_, nRows_, remainingWidth_, remainingHeight_);
}

// Clear layout data
void ViewLayout::clear()
{
	name_ = "<New Layout>";
	nColumns_ = 1;
	nRows_ = 1;
	panes_.clear();
}

// Set default layout
ViewPane* ViewLayout::setDefault()
{
	clear();

	setName("Default");
	setGrid(1, 1);
	ViewPane* pane = addPane("Main view", 0, 0, 1, 1);
	return pane;
}

// Set name
void ViewLayout::setName(QString name)
{
	name_ = name;
}

// Set number of rows and columns
void ViewLayout::setGrid(int nColumns, int nRows)
{
	nColumns_ = nColumns;
	nRows_ = nRows;

	recalculatePixels();
}

// Return number of columns in layout
int ViewLayout::nColumns() const
{
	return nColumns_;
}

// Return number of rows in layout
int ViewLayout::nRows() const
{
	return nRows_;
}

/*
 * Pane Functions
 */

// Add pane to layout
ViewPane* ViewLayout::addPane(QString name, int left, int top, int width, int height)
{
	ViewPane* pane = panes_.add(*this);
	pane->setName(name);
	pane->setSize(width, height);

	pane->recalculateViewport(pixelWidth_, pixelHeight_, nColumns_, nRows_, remainingWidth_, remainingHeight_);

	setAsModified();

	return pane;
}

// Return list of panes
ViewPane* ViewLayout::panes()
{
	return panes_.first();
}

// Return last pane in list
ViewPane* ViewLayout::lastPane()
{
	return panes_.last();
}

// Return named pane (if it exists)
ViewPane* ViewLayout::pane(QString name)
{
	for (ViewPane* pane = panes_.first(); pane != NULL; pane = pane->next) if (pane->name() == name) return pane;
	return NULL;
}

// Return index of specified pane in list
int ViewLayout::paneIndex(ViewPane* pane)
{
	return panes_.indexOf(pane);
}

// Return if pane is in the current list
bool ViewLayout::containsPane(ViewPane* pane)
{
	return panes_.contains(pane);
}

// Set new layout size
void ViewLayout::resize(int contextWidth, int contextHeight)
{
	layoutWidth_ = contextWidth;
	layoutHeight_ = contextHeight;

	recalculatePixels();
}

// Return pane under specified coordinate
ViewPane* ViewLayout::paneAt(int layoutX, int layoutY)
{
	for (ViewPane* pane = panes_.first(); pane != NULL; pane = pane->next) if (pane->containsCoordinate(layoutX, layoutY)) return pane;
	return NULL;
}

// Return pane containing specified grid reference
ViewPane* ViewLayout::paneAtGrid(int gridX, int gridY)
{
	for (ViewPane* pane = panes_.first(); pane != NULL; pane = pane->next) if (pane->containsGridReference(gridX, gridY)) return pane;
	return NULL;
}

// Translate pane by the amount specified
void ViewLayout::translatePane(ViewPane* pane, int deltaX, int deltaY)
{
	pane->setBottomLeft(pane->bottomEdge()+deltaY, pane->leftEdge()+deltaX);
	pane->recalculateViewport(pixelWidth_, pixelHeight_, nColumns_, nRows_, remainingWidth_, remainingHeight_);
}

// Reset view of all panes
void ViewLayout::resetView()
{
	// Loop over view panes
	for (ViewPane* pane = panes_.first(); pane != NULL; pane = pane->next) pane->resetView();
}

// Update interaction primitives for all panes
void ViewLayout::updateInteractionPrimitives(int axis)
{
	// Loop over view panes
	for (ViewPane* pane = panes_.first(); pane != NULL; pane = pane->next) pane->updateInteractionPrimitive(axis);
}
