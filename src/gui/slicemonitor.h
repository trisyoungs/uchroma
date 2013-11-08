/*
	*** Slice Monitor Widget
	*** src/slicemonitor.h
	Copyright T. Youngs 2012-2013.

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

#ifndef UCHROMA_SLICEMONITOR_H
#define UCHROMA_SLICEMONITOR_H

#include <QtGui/QWidget>
#include <QtCore/QObject>
#include "gui/ui_slicemonitor.h"

// Forward Declarations
class UChromaWindow;

/*
 * SliceMonitor
 */
class SliceMonitorDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	SliceMonitorDialog(QWidget *parent);
	// Destructor
	~SliceMonitorDialog();
	// Main form declaration
	Ui::SliceMonitorDialog ui;


	/*
	 * Link to UChroma
	 */
	private:
	// UChromaWindow pointer
	static UChromaWindow* uChroma_;
	
	public:
	// Set UChromaWindow pointer
	static void setUChroma(UChromaWindow* ptr);


	/*
	 * Slots / Reimplementations
	 */
	private:

	public slots:

	public:
};

#endif
