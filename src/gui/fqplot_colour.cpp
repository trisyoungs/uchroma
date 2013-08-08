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
	if (colourSource_ == FQPlotWindow::SingleColourSource) ui.ColourGradient->setColourScale(colourScale_[FQPlotWindow::SingleColourSource]);
	else if (colourSource_ == FQPlotWindow::RGBGradientSource) ui.ColourGradient->setColourScale(colourScale_[FQPlotWindow::RGBGradientSource]);
	else if (colourSource_ == FQPlotWindow::HSVGradientSource) ui.ColourGradient->setColourScale(colourScale_[FQPlotWindow::HSVGradientSource]);
	else if (colourSource_ == FQPlotWindow::CustomGradientSource) ui.ColourGradient->setColourScale(colourScale_[FQPlotWindow::CustomGradientSource]);
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
		colourScale_[FQPlotWindow::SingleColourSource].setPoint(0, 0.0, ui.ColourSingleColourButton->colour());
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
		colourScale_[FQPlotWindow::RGBGradientSource].setPoint(0, ui.ColourRGBGradientASpin->value(), ui.ColourRGBGradientAButton->colour());
		updateGradientBar();
		updateSurface(false);
	}
}

void FQPlotWindow::on_ColourRGBGradientASpin_valueChanged(double value)
{
	if (refreshing_) return;
	colourScale_[FQPlotWindow::RGBGradientSource].setPointValue(0, ui.ColourRGBGradientASpin->value());
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
		colourScale_[FQPlotWindow::RGBGradientSource].setPointColour(1, ui.ColourRGBGradientBButton->colour());
		updateGradientBar();
		updateSurface(false);
	}
}

void FQPlotWindow::on_ColourRGBGradientBSpin_valueChanged(double value)
{
	if (refreshing_) return;
	colourScale_[FQPlotWindow::RGBGradientSource].setPointValue(1, ui.ColourRGBGradientBSpin->value());
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
		colourScale_[FQPlotWindow::HSVGradientSource].setPoint(0, ui.ColourHSVGradientASpin->value(), ui.ColourHSVGradientAButton->colour());
		updateGradientBar();
		updateSurface(false);
	}
}

void FQPlotWindow::on_ColourHSVGradientASpin_valueChanged(double value)
{
	if (refreshing_) return;
	colourScale_[FQPlotWindow::HSVGradientSource].setPointValue(0, ui.ColourHSVGradientASpin->value());
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
		colourScale_[FQPlotWindow::HSVGradientSource].setPointColour(1, ui.ColourHSVGradientBButton->colour());
		updateGradientBar();
		updateSurface(false);
	}
}

void FQPlotWindow::on_ColourHSVGradientBSpin_valueChanged(double value)
{
	if (refreshing_) return;
	colourScale_[FQPlotWindow::HSVGradientSource].setPointValue(1, ui.ColourHSVGradientBSpin->value());
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
	
	colourScale_[CustomGradientSource].setPointColour(cspId, newcol);

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
	colourScale_[CustomGradientSource].setPointValue(cspId, item->text().toDouble());

	// Refresh table
	updateColourTab();
	updateSurface(false);
	setAsModified();
}

void FQPlotWindow::on_ColourCustomGradientAddButton_clicked(bool checked)
{
	colourScale_[CustomGradientSource].addPoint(colourScale_[CustomGradientSource].lastPoint() ? colourScale_[CustomGradientSource].lastPoint()->value() + 1.0 : 0.0, Qt::white);

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
		toDelete.addUnique(colourScale_[CustomGradientSource].point(cspId));
	}

	// Now delete the points
	for (RefListItem<ColourScalePoint,int>* ri = toDelete.first(); ri != NULL; ri = ri->next) colourScale_[CustomGradientSource].removePoint(ri->item);

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

	ColourScalePoint* csp;
	// Single Colour
	csp = colourScale_[FQPlotWindow::SingleColourSource].point(0);
	ui.ColourSingleColourButton->setColour(csp ? csp->colour() : QColor());

	// RGB Gradient
	csp = colourScale_[FQPlotWindow::RGBGradientSource].point(0);
	ui.ColourRGBGradientAButton->setColour(csp ? csp->colour() : QColor());
	ui.ColourRGBGradientASpin->setValue(csp->value());
	csp = colourScale_[FQPlotWindow::RGBGradientSource].point(1);
	ui.ColourRGBGradientBButton->setColour(csp ? csp->colour() : QColor());
	ui.ColourRGBGradientBSpin->setValue(csp->value());

	// HSV Gradient
	csp = colourScale_[FQPlotWindow::HSVGradientSource].point(0);
	ui.ColourHSVGradientAButton->setColour(csp ? csp->colour() : QColor());
	ui.ColourHSVGradientASpin->setValue(csp->value());
	csp = colourScale_[FQPlotWindow::HSVGradientSource].point(1);
	ui.ColourHSVGradientBButton->setColour(csp ? csp->colour() : QColor());
	ui.ColourHSVGradientBSpin->setValue(csp->value());

	// Custom Gradient - add points in reverse order so higher values appear at the top of the list
	ui.ColourCustomGradientTable->clearContents();
	ui.ColourCustomGradientTable->setRowCount(colourScale_[FQPlotWindow::CustomGradientSource].nPoints());
	QTableWidgetItem *item;
	int count = 0;
	for (ColourScalePoint *csp = colourScale_[FQPlotWindow::CustomGradientSource].lastPoint(); csp != NULL; csp = csp->prev)
	{
		item = new QTableWidgetItem(QString::number(csp->value()));
		item->setData(Qt::UserRole, colourScale_[FQPlotWindow::CustomGradientSource].nPoints() - (count+1));
		ui.ColourCustomGradientTable->setItem(count, 0, item);
		item = new QTableWidgetItem();
		item->setBackgroundColor(csp->colour());
		item->setData(Qt::UserRole, colourScale_[FQPlotWindow::CustomGradientSource].nPoints() - (count+1));
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
