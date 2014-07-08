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

// Constructor
ViewLayout::ViewLayout() : ListItem<ViewLayout>()
{
	nColumns_ = 0;
	nRows_ = 0;
	pixelWidth_ = 0;
	pixelHeight_ = 0;
}

// Destructor
ViewLayout::~ViewLayout()
{
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
}

// Add pane to layout
ViewPane* ViewLayout::addPane(QString name, int left, int top, int width, int height)
{
	ViewPane* pane = panes_.add();
	pane->setName(name);
	pane->setSize(width, height);

	return pane;
}

// Return list of panes
ViewPane* ViewLayout::panes()
{
	return panes_.first();
}

/*
 * Pane Functions
 */

// Recalculate pane sizes based on current context dimensions
void ViewLayout::resizePanes(int contextWidth, int contextHeight)
{
	// Set new pixel widths
	pixelWidth_ = contextWidth / nColumns_;
	pixelHeight_ = contextHeight / nRows_;

	// Recalculate pane sizes
	int widthRemainder = contextWidth - nColumns_*pixelWidth_;
	int heightRemainder = contextHeight - nRows_*pixelHeight_;
	for (ViewPane* pane = panes_.first(); pane != NULL; pane = pane->next) pane->recalculateViewport(pixelWidth_, pixelHeight_, nColumns_, nRows_, widthRemainder, heightRemainder);
}

// Return pane under specified point
ViewPane* ViewLayout::paneAt(int mouseX, int mouseY)
{
	for (ViewPane* pane = panes_.first(); pane != NULL; pane = pane->next) if (pane->containsPoint(mouseX, mouseY)) return pane;
	return NULL;
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
