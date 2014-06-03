/*
	*** uChroma Style Window
	*** src/gui/style.h
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

#ifndef UCHROMA_STYLEWINDOW_H
#define UCHROMA_STYLEWINDOW_H

#include "gui/ui_style.h"

// Forward Declarations
class UChromaWindow;

class StyleWindow : public QWidget
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
	StyleWindow(UChromaWindow& parent);
	~StyleWindow();
	// Main form declaration
	Ui::StyleWindow ui;
	// UChromaWindow reference
	UChromaWindow& uChroma_;

	protected:
	// Window close event
	void closeEvent(QCloseEvent *event);

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
	// -- Style
	void on_StyleCombo_currentIndexChanged(int index);
	// -- Single Colour
	void on_ColourSingleColourRadio_clicked(bool checked);
	void on_ColourSingleColourButton_clicked(bool checked);
	// -- RGB Gradient
	void on_ColourRGBGradientRadio_clicked(bool checked);
	void on_ColourRGBGradientAButton_clicked(bool checked);
	void on_ColourRGBGradientASpin_valueChanged(double value);
	void on_ColourRGBGradientASetMinimumButton_clicked(bool checked);
	void on_ColourRGBGradientASetMaximumButton_clicked(bool checked);
	void on_ColourRGBGradientBButton_clicked(bool checked);
	void on_ColourRGBGradientBSpin_valueChanged(double value);
	void on_ColourRGBGradientBSetMinimumButton_clicked(bool checked);
	void on_ColourRGBGradientBSetMaximumButton_clicked(bool checked);
	// -- HSV Gradient
	void on_ColourHSVGradientRadio_clicked(bool checked);
	void on_ColourHSVGradientAButton_clicked(bool checked);
	void on_ColourHSVGradientASpin_valueChanged(double value);
	void on_ColourHSVGradientASetMinimumButton_clicked(bool checked);
	void on_ColourHSVGradientASetMaximumButton_clicked(bool checked);
	void on_ColourHSVGradientBButton_clicked(bool checked);
	void on_ColourHSVGradientBSpin_valueChanged(double value);
	void on_ColourHSVGradientBSetMinimumButton_clicked(bool checked);
	void on_ColourHSVGradientBSetMaximumButton_clicked(bool checked);
	// -- Custom Gradient
	void on_ColourCustomGradientRadio_clicked(bool checked);
	void on_ColourCustomGradientTable_itemSelectionChanged();
	void on_ColourCustomGradientTable_cellDoubleClicked(int row, int column);
	void on_ColourCustomGradientTable_cellChanged(int row, int column);
	void on_ColourCustomGradientAddButton_clicked(bool checked);
	void on_ColourCustomGradientRemoveButton_clicked(bool checked);
	// -- Alpha Control
	void on_ColourAlphaOwnAlphaRadio_clicked(bool checked);
	void on_ColourAlphaFixedAlphaRadio_clicked(bool checked);
	void on_ColourAlphaFixedAlphaSpin_valueChanged(int value);


	/*
	 * Update Functions
	 */
	private:
	// Update Gradient Bar
	void updateGradientBar();

	public:
	// Update controls and show window
	void updateAndShow();
	// Update controls
	void updateControls(bool force = false);
};

#endif
