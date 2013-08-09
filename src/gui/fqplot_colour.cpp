/*
	*** Main Window - Colour Functions 
	*** src/gui/fqplot_colour.cpp
	Copyright T. Youngs 2013

	This file is part of FQPlot.

	FQPlot is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	FQPlot is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FQPlot.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/fqplot.h"
#include "templates/reflist.h"

// Update Gradient Bar
void FQPlotWindow::updateGradientBar()
{
	updateColourScale();
	ui.ColourGradient->setColourScale(colourScale_);
}

/*
 * Single Colour
 */
void FQPlotWindow::on_ColourSingleColourRadio_clicked(bool checked)
{
	if (refreshing_) return;
	colourSource_ = FQPlotWindow::SingleColourSource;
	updateGradientBar();
	updateSurface(false);
}

void FQPlotWindow::on_ColourSingleColourButton_clicked(bool checked)
{
	if (refreshing_) return;
	if (ui.ColourSingleColourButton->selectColour())
	{
		colourSinglePoint_.setColour(ui.ColourSingleColourButton->colour());
		updateGradientBar();
		updateSurface(false);
	}
}

/*
 * RGB Gradient
 */

void FQPlotWindow::on_ColourRGBGradientRadio_clicked(bool checked)
{
	if (refreshing_) return;
	colourSource_ = FQPlotWindow::RGBGradientSource;
	updateGradientBar();
	updateSurface(false);
}

void FQPlotWindow::on_ColourRGBGradientAButton_clicked(bool checked)
{
	if (ui.ColourRGBGradientAButton->selectColour())
	{
		colourRGBGradientAPoint_.setColour(ui.ColourRGBGradientAButton->colour());
		updateGradientBar();
		updateSurface(false);
	}
}

void FQPlotWindow::on_ColourRGBGradientASpin_valueChanged(double value)
{
	if (refreshing_) return;
	colourRGBGradientAPoint_.setValue(ui.ColourRGBGradientASpin->value());
	updateGradientBar();
	updateSurface(false);
}

void FQPlotWindow::on_ColourRGBGradientASetMinimumButton_clicked(bool checked)
{
	ui.ColourRGBGradientASpin->setValue(limitMin_.y);
}

void FQPlotWindow::on_ColourRGBGradientASetMaximumButton_clicked(bool checked)
{
	ui.ColourRGBGradientASpin->setValue(limitMax_.y);
}

void FQPlotWindow::on_ColourRGBGradientBButton_clicked(bool checked)
{
	if (ui.ColourRGBGradientBButton->selectColour())
	{
		colourRGBGradientBPoint_.setColour(ui.ColourRGBGradientBButton->colour());
		updateGradientBar();
		updateSurface(false);
	}
}

void FQPlotWindow::on_ColourRGBGradientBSpin_valueChanged(double value)
{
	if (refreshing_) return;
	colourRGBGradientBPoint_.setValue(ui.ColourRGBGradientBSpin->value());
	updateGradientBar();
	updateSurface(false);
}

void FQPlotWindow::on_ColourRGBGradientBSetMinimumButton_clicked(bool checked)
{
	ui.ColourRGBGradientBSpin->setValue(limitMin_.y);
}

void FQPlotWindow::on_ColourRGBGradientBSetMaximumButton_clicked(bool checked)
{
	ui.ColourRGBGradientBSpin->setValue(limitMax_.y);
}

/*
 * HSV Gradient
 */

void FQPlotWindow::on_ColourHSVGradientRadio_clicked(bool checked)
{
	if (refreshing_) return;
	colourSource_ = FQPlotWindow::HSVGradientSource;
	updateGradientBar();
	updateSurface(false);
}

void FQPlotWindow::on_ColourHSVGradientAButton_clicked(bool checked)
{
	if (ui.ColourHSVGradientAButton->selectColour())
	{
		colourHSVGradientAPoint_.setColour(ui.ColourHSVGradientAButton->colour());
		updateGradientBar();
		updateSurface(false);
	}
}

void FQPlotWindow::on_ColourHSVGradientASpin_valueChanged(double value)
{
	if (refreshing_) return;
	colourHSVGradientAPoint_.setValue(ui.ColourHSVGradientASpin->value());
	updateGradientBar();
	updateSurface(false);
}

void FQPlotWindow::on_ColourHSVGradientASetMinimumButton_clicked(bool checked)
{
	ui.ColourHSVGradientASpin->setValue(limitMin_.y);
}

void FQPlotWindow::on_ColourHSVGradientASetMaximumButton_clicked(bool checked)
{
	ui.ColourHSVGradientASpin->setValue(limitMax_.y);
}

void FQPlotWindow::on_ColourHSVGradientBButton_clicked(bool checked)
{
	if (ui.ColourHSVGradientBButton->selectColour())
	{
		colourHSVGradientBPoint_.setColour(ui.ColourHSVGradientBButton->colour());
		updateGradientBar();
		updateSurface(false);
	}
}

void FQPlotWindow::on_ColourHSVGradientBSpin_valueChanged(double value)
{
	if (refreshing_) return;
	colourHSVGradientBPoint_.setValue(ui.ColourHSVGradientBSpin->value());
	updateGradientBar();
	updateSurface(false);
}

void FQPlotWindow::on_ColourHSVGradientBSetMinimumButton_clicked(bool checked)
{
	ui.ColourHSVGradientBSpin->setValue(limitMin_.y);
}

void FQPlotWindow::on_ColourHSVGradientBSetMaximumButton_clicked(bool checked)
{
	ui.ColourHSVGradientBSpin->setValue(limitMax_.y);
}

/*
 * Custom Gradient
 */

void FQPlotWindow::on_ColourCustomGradientRadio_clicked(bool checked)
{
	if (refreshing_) return;
	colourSource_ = FQPlotWindow::CustomGradientSource;
	updateGradientBar();
	updateSurface(false);
}

void FQPlotWindow::on_ColourCustomGradientTable_itemSelectionChanged()
{
	if (refreshing_) return;
	bool selected = (ui.ColourCustomGradientTable->selectedItems().count() != 0);
	ui.ColourCustomGradientRemoveButton->setEnabled(selected);
}

void FQPlotWindow::on_ColourCustomGradientTable_cellDoubleClicked(int row, int column)
{
	if (refreshing_ || (column == 0)) return;

	// Get item data (contains ColourScalePoint index)
	QTableWidgetItem* item = ui.ColourCustomGradientTable->item(row, column);
	if (!item) return;
	int cspId = item->data(Qt::UserRole).toInt();
	
        // Request a colour dialog
        QColor newcol = QColorDialog::getColor(item->backgroundColor().rgba(), this, "Select new colour", QColorDialog::ShowAlphaChannel);
        if (!newcol.isValid()) return;
	
	customColourScale_.setPointColour(cspId, newcol);

	// Update table and refresh surface
	item->setBackgroundColor(newcol);
	updateGradientBar();
	updateSurface(false);
	setAsModified();
}

void FQPlotWindow::on_ColourCustomGradientTable_cellChanged(int row, int column)
{
	if (refreshing_ || (column == 1)) return;
	
	// Get item data (contains ColourScalePoint index)
	QTableWidgetItem* item = ui.ColourCustomGradientTable->item(row, column);
	if (!item) return;
	int cspId = item->data(Qt::UserRole).toInt();
	
	// Value of this point has changed, so must update the list and refresh the surface and necessary widgets
	customColourScale_.setPointValue(cspId, item->text().toDouble());

	// Refresh table
	updateColourTab();
	updateSurface(false);
	setAsModified();
}

void FQPlotWindow::on_ColourCustomGradientAddButton_clicked(bool checked)
{
	customColourScale_.addPoint(customColourScale_.lastPoint() ? customColourScale_.lastPoint()->value() + 1.0 : 0.0, Qt::white);

	// Refresh table
	updateColourTab();
	updateSurface(false);
	setAsModified();
}

void FQPlotWindow::on_ColourCustomGradientRemoveButton_clicked(bool checked)
{
	if (ui.ColourCustomGradientTable->selectedItems().count() == 0) return;

	// Get list of items to delete
	int lastId = -1;
	RefList<ColourScalePoint,int> toDelete;
	foreach(QTableWidgetItem* item, ui.ColourCustomGradientTable->selectedItems())
	{
		// Get point id from item
		int cspId = item->data(Qt::UserRole).toInt();
		if (lastId == cspId) continue;
		lastId = cspId;
		toDelete.addUnique(customColourScale_.point(cspId));
	}

	// Now delete the points
	for (RefListItem<ColourScalePoint,int>* ri = toDelete.first(); ri != NULL; ri = ri->next) customColourScale_.removePoint(ri->item);

	// Refresh table
	updateColourTab();
	updateSurface(false);
	setAsModified();
}

/*
 * Alpha Control
 */

void FQPlotWindow::on_ColourAlphaOwnAlphaRadio_clicked(bool checked)
{
	if (refreshing_) return;
	alphaControl_ = FQPlotWindow::OwnAlpha;
	updateSurface(false);
}

void FQPlotWindow::on_ColourAlphaFixedAlphaRadio_clicked(bool checked)
{
	if (refreshing_) return;
	alphaControl_ = FQPlotWindow::FixedAlpha;
	updateSurface(false);
}

void FQPlotWindow::on_ColourAlphaFixedAlphaSpin_valueChanged(int value)
{
	if (refreshing_) return;
	fixedAlpha_ = value;
	updateSurface(false);
}

/*
 * Update
 */
void FQPlotWindow::updateColourTab()
{
	refreshing_ = true;

	// Single Colour
	ui.ColourSingleColourButton->setColour(colourSinglePoint_.colour());

	// RGB Gradient
	ui.ColourRGBGradientAButton->setColour(colourRGBGradientAPoint_.colour());
	ui.ColourRGBGradientASpin->setValue(colourRGBGradientAPoint_.value());
	ui.ColourRGBGradientBButton->setColour(colourRGBGradientBPoint_.colour());
	ui.ColourRGBGradientBSpin->setValue(colourRGBGradientBPoint_.value());

	// HSV Gradient
	ui.ColourHSVGradientAButton->setColour(colourHSVGradientAPoint_.colour());
	ui.ColourHSVGradientASpin->setValue(colourHSVGradientAPoint_.value());
	ui.ColourHSVGradientBButton->setColour(colourHSVGradientBPoint_.colour());
	ui.ColourHSVGradientBSpin->setValue(colourHSVGradientBPoint_.value());

	// Custom Gradient - add points in reverse order so higher values appear at the top of the list
	ui.ColourCustomGradientTable->clearContents();
	ui.ColourCustomGradientTable->setRowCount(customColourScale_.nPoints());
	QTableWidgetItem *item;
	int count = 0;
	for (ColourScalePoint *csp = customColourScale_.lastPoint(); csp != NULL; csp = csp->prev)
	{
		item = new QTableWidgetItem(QString::number(csp->value()));
		item->setData(Qt::UserRole, customColourScale_.nPoints() - (count+1));
		ui.ColourCustomGradientTable->setItem(count, 0, item);
		item = new QTableWidgetItem();
		item->setBackgroundColor(csp->colour());
		item->setData(Qt::UserRole, customColourScale_.nPoints() - (count+1));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable	);
		ui.ColourCustomGradientTable->setItem(count, 1, item);
		++count;
	}
	// -- Select first item in list
	ui.ColourCustomGradientTable->setCurrentItem(0);
	
	// Set current colour source
	if (colourSource_ == FQPlotWindow::SingleColourSource) ui.ColourSingleColourRadio->setChecked(true);
	else if (colourSource_ == FQPlotWindow::RGBGradientSource) ui.ColourRGBGradientRadio->setChecked(true);
	else if (colourSource_ == FQPlotWindow::HSVGradientSource) ui.ColourHSVGradientRadio->setChecked(true);
	else if (colourSource_ == FQPlotWindow::CustomGradientSource) ui.ColourCustomGradientRadio->setChecked(true);

	// Update gradient bar
	updateGradientBar();

	// Set current alpha control
	if (alphaControl_ == FQPlotWindow::OwnAlpha) ui.ColourAlphaOwnAlphaRadio->setChecked(true);
	else if (alphaControl_ == FQPlotWindow::FixedAlpha) ui.ColourAlphaFixedAlphaRadio->setChecked(true);
	ui.ColourAlphaFixedAlphaSpin->setValue(fixedAlpha_);

	refreshing_ = false;
}
