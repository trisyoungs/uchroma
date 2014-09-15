/*
	*** View Layout
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
#include "base/currentproject.h"
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
	name_ = source.name_;
	nColumns_ = source.nColumns_;
	nRows_ = source.nRows_;
	panes_ = source.panes_;
}

/*
 * Parent
 */

// Pane has changed
void ViewLayout::paneChanged(ViewPane* caller)
{
	if (caller) caller->recalculateViewport(pixelWidth_, pixelHeight_, nColumns_, nRows_, remainingWidth_, remainingHeight_);
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
	nColumns_ = 4;
	nRows_ = 4;
	panes_.clear();
}

// Set default layout
ViewPane* ViewLayout::setDefault()
{
	clear();

	setName("Default");
	setGrid(4, 4);
	ViewPane* pane = addPane("Main view", 0, 0, 4, 4);
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

	return pane;
}

// Remove pane from layout
void ViewLayout::removePane(ViewPane* pane)
{
	if (!panes_.contains(pane))
	{
		msg.print("Internal Error: Tried to remove a pane from a ViewLayout that doesn't own it.\n");
		return;
	}

	// Notify all collections associated to the pane that it longer exists
	for (RefListItem<Collection,bool>* ri = pane->collections(); ri != NULL; ri = ri->next) ri->item->setDisplayPane(NULL);

	panes_.remove(pane);
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
	// Search in reverse order, since this reflects the 'stack' when drawn
	for (ViewPane* pane = panes_.last(); pane != NULL; pane = pane->prev) if (pane->containsCoordinate(layoutX, layoutY)) return pane;
	return NULL;
}

// Return pane containing specified grid reference
ViewPane* ViewLayout::paneAtGrid(int gridX, int gridY)
{
	// Search in reverse order, since this reflects the 'stack' when drawn
	for (ViewPane* pane = panes_.last(); pane != NULL; pane = pane->prev) if (pane->containsGridReference(gridX, gridY)) return pane;
	return NULL;
}

// Translate pane by the amount specified
void ViewLayout::translatePane(ViewPane* pane, int deltaX, int deltaY)
{
	// Check that a meaningful delta was supplied
	if ((deltaX == 0) && (deltaY == 0)) return;

	pane->setBottomLeft(pane->bottomEdge()+deltaY, pane->leftEdge()+deltaX);
	pane->recalculateViewport(pixelWidth_, pixelHeight_, nColumns_, nRows_, remainingWidth_, remainingHeight_);
}

// Reset view of all panes
void ViewLayout::resetViewMatrix()
{
	// Loop over view panes
	for (ViewPane* pane = panes_.first(); pane != NULL; pane = pane->next) pane->resetViewMatrix();
}

// Update interaction primitives for all panes
void ViewLayout::updateInteractionPrimitives(int axis)
{
	// Loop over view panes
	for (ViewPane* pane = panes_.first(); pane != NULL; pane = pane->next) pane->updateInteractionPrimitive(axis);
}

/*
 * Signal / Update
 */

// Process supplied Collection changed/update signal
bool ViewLayout::processUpdate(Collection* source, Collection::CollectionSignal signal)
{
	// Search for a pane that is interested in this signal...
	for (ViewPane* pane = panes_.first(); pane != NULL; pane = pane->next) if (pane->processUpdate(source, signal)) return true;
	return false;
}
