/*
	*** Slice Monitor Window
	*** src/slicemonitor.h
	Copyright T. Youngs 2012-2014.

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
 * Slice Monitor
 */
class SliceMonitorWindow : public QWidget
{
	Q_OBJECT

	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;

	public:
	// Constructor / Destructor
	SliceMonitorWindow(UChromaWindow& parent);
	~SliceMonitorWindow();
	// Main form declaration
	Ui::SliceMonitorWindow ui;
	// UChromaWindow reference
	UChromaWindow& uChroma_;

	protected:
	// Window close event
	void closeEvent(QCloseEvent *event);

	signals:
	// Window closed signal
	void windowClosed(bool);

	/*
	 * Slots
	 */
	public slots:


	/*
	 * Update Functions
	 */
	public:
	// Update controls and show window
	void updateAndShow();
	// Update controls
	void updateControls(bool force = false);
};

#endif
