/*
	*** ViewLayout
	*** src/base/viewlayout.h
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

#ifndef UCHROMA_VIEWLAYOUT_H
#define UCHROMA_VIEWLAYOUT_H

#include "base/viewpane.h"
#include "base/collection.h"
#include "templates/list.h"
#include <QtCore/QString>

// Forward Declarations
class UChromaWindow;

// ViewLayout
class ViewLayout : public ListItem<ViewLayout>
{
	public:
	// Constructor / Destructor
	ViewLayout(UChromaWindow& parent);
	~ViewLayout();
	// Copy constructor
	ViewLayout(const ViewLayout& source);
	// Assignment operator
	void operator=(const ViewLayout& source);


	/*
	 * Parent
	 */
	private:
	// Layout in which this pane exists
	UChromaWindow& parent_;

	public:
	// Set as modified (call parent routine)
	void setAsModified(ViewPane* caller = NULL);


	/*
	 * Layout
	 */
	private:
	// Layout name
	QString name_;
	// Number of columns in layout
	int nColumns_;
	// Number of rows in layout
	int nRows_;
	// Current layout width
	int layoutWidth_;
	// Current layout height
	int layoutHeight_;
	// Width of grid pixel
	int pixelWidth_;
	// Height of grid pixel
	int pixelHeight_;
	// Remaining width at right edge
	int remainingWidth_;
	// Remaining height at top edge
	int remainingHeight_;

	private:
	// Recalculate pixel dimensions and remainder
	void recalculatePixels();

	public:
	// Clear layout data
	void clear();
	// Set default layout data
	ViewPane* setDefault();
	// Set name
	void setName(QString name);
	// Set number of rows and columns
	void setGrid(int nColumns, int nRows);
	// Return number of columns in layout
	int nColumns() const;
	// Return number of rows in layout
	int nRows() const;
	// Set new layout size
	void resize(int contextWidth, int contextHeight);


	/*
	 * Pane Functions
	 */
	private:
	// List of panes in this layout
	ParentList<ViewPane,ViewLayout> panes_;

	public:
	// Add pane to layout
	ViewPane* addPane(QString name = QString(), int left = 0, int top = 0, int width = 1, int height = 1);
	// Return first pane in list
	ViewPane* panes();
	// Return last pane in list
	ViewPane* lastPane();
	// Return named pane (if it exists)
	ViewPane* pane(QString name);
	// Return index of specified pane in list
	int paneIndex(ViewPane* pane);
	// Return if pane is in the current list
	bool containsPane(ViewPane* pane);
	// Return pane under specified coordinate
	ViewPane* paneAt(int layoutX, int layoutY);
	// Return pane containing specified grid reference
	ViewPane* paneAtGrid(int gridX, int gridY);
	// Translate pane by the amount specified
	void translatePane(ViewPane* pane, int deltaX, int deltaY);
	// Reset view of all panes
	void resetView();
	// Update interaction primitives for all panes
	void updateInteractionPrimitives(int axis);


	/*
	 * Signal / Update
	 */
	public:
	// Process supplied Collection changed/update signal
	   bool processUpdate(Collection* source, Collection::CollectionSignal signal);
};

#endif
