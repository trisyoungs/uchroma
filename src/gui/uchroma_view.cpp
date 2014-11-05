/*
	*** UChroma - View
	*** src/gui/uchroma_view.cpp
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

#include "gui/uchroma.h"

// Return curreent view layout
ViewLayout& UChromaWindow::viewLayout()
{
	return viewLayout_;
}

// Set view layout
void UChromaWindow::setViewLayout(ViewLayout& source)
{
	viewLayout_ = source;
}

// Recalculate layout pane positions / sizes (after context resize etc.)
void UChromaWindow::recalculateViewLayout(int contextWidth, int contextHeight)
{
	viewLayout_.recalculate(contextWidth, contextHeight);
	
	updateDisplay();
}

// Set current view pane to the one under the specified screen coordinates
bool UChromaWindow::setCurrentViewPane(int layoutX, int layoutY)
{
	ViewPane* newCurrentPane = viewLayout_.paneAt(layoutX, layoutY);
	if (newCurrentPane == NULL) return false;

	currentViewPane_ = newCurrentPane;

	// Toolbars and AxesWindow now need updating
	updateToolBars();
	axesWindow_.setWindowTitle("Axes (" + currentViewPane_->name() + ")");

	return true;
}

// Return current view pane
ViewPane* UChromaWindow::currentViewPane()
{
	return currentViewPane_;
}
