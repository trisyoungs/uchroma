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

/*
 * Single Colour
 */
void FQPlotWindow::on_ColourSingleColourRadio_clicked(bool checked)
{
	colourSource_ = FQPlotWindow::SingleColourSource;
	updateSurface(false);
}

void FQPlotWindow::on_ColourSingleColourButton_clicked(bool checked)
{
	if (refreshing_) return;
	if (ui.ColourSingleColourButton->selectColour())
	{
		colourScale_[FQPlotWindow::SingleColourSource].setPoint(0, 0.0, ui.ColourSingleColourButton->colour());
		updateSurface(false);
	}
}

/*
 * Linear Gradient
 */

void FQPlotWindow::on_ColourLinearGradientRadio_clicked(bool checked)
{
	if (refreshing_) return;
	colourSource_ = FQPlotWindow::LinearGradientSource;
	updateSurface(false);
}

void FQPlotWindow::on_ColourLinearGradientAButton_clicked(bool checked)
{
	if (ui.ColourLinearGradientAButton->selectColour())
	{
		colourScale_[FQPlotWindow::LinearGradientSource].setPoint(0, ui.ColourLinearGradientASpin->value(), ui.ColourLinearGradientAButton->colour());
		updateSurface(false);
	}
}

void FQPlotWindow::on_ColourLinearGradientASpin_valueChanged(double value)
{
	if (refreshing_) return;
	colourScale_[FQPlotWindow::LinearGradientSource].setPointValue(0, ui.ColourLinearGradientASpin->value());
	updateSurface(false);
}

void FQPlotWindow::on_ColourLinearGradientASetMinimumButton_clicked(bool checked)
{
	ui.ColourLinearGradientASpin->setValue(limitMin_.y);
}

void FQPlotWindow::on_ColourLinearGradientASetMaximumButton_clicked(bool checked)
{
	ui.ColourLinearGradientASpin->setValue(limitMax_.y);
}

void FQPlotWindow::on_ColourLinearGradientBButton_clicked(bool checked)
{
	if (ui.ColourLinearGradientBButton->selectColour())
	{
		colourScale_[FQPlotWindow::LinearGradientSource].setPointColour(1, ui.ColourLinearGradientBButton->colour());
		updateSurface(false);
	}
}

void FQPlotWindow::on_ColourLinearGradientBSpin_valueChanged(double value)
{
	if (refreshing_) return;
	colourScale_[FQPlotWindow::LinearGradientSource].setPointValue(1, ui.ColourLinearGradientBSpin->value());
	updateSurface(false);
}

void FQPlotWindow::on_ColourLinearGradientBSetMinimumButton_clicked(bool checked)
{
	ui.ColourLinearGradientBSpin->setValue(limitMin_.y);
}

void FQPlotWindow::on_ColourLinearGradientBSetMaximumButton_clicked(bool checked)
{
	ui.ColourLinearGradientBSpin->setValue(limitMax_.y);
}

/*
 * Custom Gradient
 */

void FQPlotWindow::on_ColourCustomGradientRadio_clicked(bool checked)
{
	if (refreshing_) return;
	colourSource_ = FQPlotWindow::CustomGradientSource;
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

	// Linear Gradient
	csp = colourScale_[FQPlotWindow::LinearGradientSource].point(0);
	ui.ColourLinearGradientAButton->setColour(csp ? csp->colour() : QColor());
	ui.ColourLinearGradientASpin->setValue(csp->value());
	csp = colourScale_[FQPlotWindow::LinearGradientSource].point(1);
	ui.ColourLinearGradientBButton->setColour(csp ? csp->colour() : QColor());
	ui.ColourLinearGradientBSpin->setValue(csp->value());

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
	else if (colourSource_ == FQPlotWindow::LinearGradientSource) ui.ColourLinearGradientRadio->setChecked(true);
	else if (colourSource_ == FQPlotWindow::CustomGradientSource) ui.ColourCustomGradientRadio->setChecked(true);
	
	// Set current alpha control
	if (alphaControl_ == FQPlotWindow::OwnAlpha) ui.ColourAlphaOwnAlphaRadio->setChecked(true);
	else if (alphaControl_ == FQPlotWindow::FixedAlpha) ui.ColourAlphaFixedAlphaRadio->setChecked(true);
	ui.ColourAlphaFixedAlphaSpin->setValue(fixedAlpha_);

	refreshing_ = false;
}
