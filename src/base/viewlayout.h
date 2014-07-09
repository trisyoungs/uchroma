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
	void setAsModified();


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
	// Width of grid pixel
	int pixelWidth_;
	// Height of grid pixel
	int pixelHeight_;
	// List of panes in this layout
	ParentList<ViewPane,ViewLayout> panes_;

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
	// Add pane to layout
	ViewPane* addPane(QString name = QString(), int left = 0, int top = 0, int width = 1, int height = 1);
	// Return list of panes
	ViewPane* panes();


	/*
	 * Pane Functions
	 */
	public:
	// Recalculate pane sizes based on current context dimensions
	void resizePanes(int contextWidth, int contextHeight);
	// Return pane under specified point
	ViewPane* paneAt(int mouseX, int mouseY);
	// Reset view of all panes
	void resetView();
	// Update interaction primitives for all panes
	void updateInteractionPrimitives(int axis);
};

#endif
