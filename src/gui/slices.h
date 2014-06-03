/*
	*** uChroma Slices Window
	*** src/gui/slices.h
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

#ifndef UCHROMA_SLICESWINDOW_H
#define UCHROMA_SLICESWINDOW_H

#include "gui/ui_slices.h"

// Forward Declarations
class UChromaWindow;

class SlicesWindow : public QWidget
{
	// All Qt declarations must include this macro
	Q_OBJECT


	/*
	// Window Functions
	*/
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;

	public:
	// Constructor / Destructor
	SlicesWindow(UChromaWindow& parent);
	~SlicesWindow();
	// Main form declaration
	Ui::SlicesWindow ui;
	// UChromaWindow reference
	UChromaWindow& uChroma_;

	protected:
	// Window close event
	void closeEvent(QCloseEvent* event);

	signals:
	// Window closed signal
	void windowClosed(bool);


	/*
	 * Convenience Functions
	 */
	private:


	/*
	 * Slots
	 */
	private slots:
	// Slices list
	void on_SlicesClearButton_clicked(bool checked);
	void on_SlicesSaveButton_clicked(bool checked);
	void on_SlicesList_currentRowChanged(int index);


	/*
	 * Update Functions
	 */
	public:
	// Add slice data to graph
	void addSlice(ExtractedSlice* slice);
	// Update controls and show window
	void updateAndShow();
	// Update controls
	void updateControls(bool force = false);
};

#endif
