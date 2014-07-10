/*
	*** uChroma Layout Window
	*** src/gui/layout.h
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

#ifndef UCHROMA_LAYOUTWINDOW_H
#define UCHROMA_LAYOUTWINDOW_H

#include "gui/ui_layout.h"

// Forward Declarations
class UChromaWindow;

class LayoutWindow : public QWidget
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
	LayoutWindow(UChromaWindow& parent);
	~LayoutWindow();
	// Main form declaration
	Ui::LayoutWindow ui;
	// UChromaWindow reference
	UChromaWindow& uChroma_;

	protected:
	// Window close event
	void closeEvent(QCloseEvent *event);

	signals:
	// Window closed signal
	void windowClosed(bool);


	/*
	 * Current Pane
	 */
	private:
	// Current pane selected in PaneOrganiser
	ViewPane* currentPane_;

	public:
	// Return current pane
	ViewPane* currentPane();


	/*
	 * Convenience Functions
	 */
	private:


	/*
	 * Slots
	 */
	private slots:
	void on_GridNColumnsSpin_valueChanged(int value);
	void on_GridNRowsSpin_valueChanged(int value);
	void on_PaneAddButton_clicked(bool checked);
	void on_PaneRemoveButton_clicked(bool checked);
	void on_PaneNextButton_clicked(bool checked);
	void on_PanePreviousButton_clicked(bool checked);
	void on_PaneNameEdit_textChanged(QString text);
	void on_PaneRoleCombo_currentIndexChanged(int index);
	void on_Organiser_currentPaneChanged(int gridX, int gridY);


	/*
	 * Signale
	 */
	signals:
	// Main display update required
	void updateMainDisplay();


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
