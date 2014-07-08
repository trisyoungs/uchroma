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

// Recalculate layout pane positions / sizes (after context resize etc.)
void UChromaWindow::recalculateViewLayout(int contextWidth, int contextHeight)
{
	viewLayout_.resizePanes(contextWidth, contextHeight);
	
	updateDisplay();
}

// Set current view pane to the one under the specified screen coordinates
bool UChromaWindow::setCurrentViewPane(int mouseX, int mouseY)
{
	ViewPane* newCurrentPane = viewLayout_.paneAt(mouseX, mouseY);
	if (newCurrentPane == NULL) return false;

	currentViewPane_ = newCurrentPane;
	return true;
}

// Return current view pane
ViewPane* UChromaWindow::currentViewPane()
{
	return currentViewPane_;
}

// Return first ViewPane in current layout
ViewPane* UChromaWindow::viewPanes()
{
	return viewLayout_.panes();
}
