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

void FQPlotWindow::on_ColourScaleTable_itemSelectionChanged()
{
	if (refreshing_) return;
	bool selected = (ui.ColourScaleTable->selectedItems().count() != 0);
	ui.RemoveColourScalePointButton->setEnabled(selected);
}

void FQPlotWindow::on_ColourScaleTable_cellDoubleClicked(int row, int column)
{
	if (refreshing_ || (column == 0)) return;

	// Get item data (contains ColourScalePoint index)
	QTableWidgetItem* item = ui.ColourScaleTable->item(row, column);
	if (!item) return;
	int cspId = item->data(Qt::UserRole).toInt();
	
        // Request a colour dialog
        QColor newcol = QColorDialog::getColor(item->backgroundColor().rgba(), this, "Select new colour", QColorDialog::ShowAlphaChannel);
        if (!newcol.isValid()) return;
	
	ui.ColourScaleWidget->setPointColour(cspId, newcol);

	// Update table and refresh surface
	item->setBackgroundColor(newcol);
	updateSurface(false);
	setAsModified();
}

void FQPlotWindow::on_ColourScaleTable_cellChanged(int row, int column)
{
	if (refreshing_ || (column == 1)) return;
	
	// Get item data (contains ColourScalePoint index)
	QTableWidgetItem* item = ui.ColourScaleTable->item(row, column);
	if (!item) return;
	int cspId = item->data(Qt::UserRole).toInt();
	
	// Value of this point has changed, so must update the list and refresh the surface and necessary widgets
	ui.ColourScaleWidget->setPointValue(cspId, item->text().toDouble());

	// Refresh table
	updateColourTab();
	updateSurface(false);
	setAsModified();
}

void FQPlotWindow::on_AddColourScalePointButton_clicked(bool checked)
{
	ui.ColourScaleWidget->addPoint(ui.ColourScaleWidget->lastPoint() ? ui.ColourScaleWidget->lastPoint()->value() + 1.0 : 0.0, Qt::white);

	// Refresh table
	updateColourTab();
	updateSurface(false);
	setAsModified();
}

void FQPlotWindow::on_RemoveColourScalePointButton_clicked(bool checked)
{
	if (ui.ColourScaleTable->selectedItems().count() == 0) return;

	// Get list of items to delete
	int lastId = -1;
	RefList<ColourScalePoint,int> toDelete;
	foreach(QTableWidgetItem* item, ui.ColourScaleTable->selectedItems())
	{
		// Get point id from item
		int cspId = item->data(Qt::UserRole).toInt();
		if (lastId == cspId) continue;
		lastId = cspId;
		toDelete.addUnique(ui.ColourScaleWidget->point(cspId));
	}

	// Now delete the points
	for (RefListItem<ColourScalePoint,int>* ri = toDelete.first(); ri != NULL; ri = ri->next) ui.ColourScaleWidget->removePoint(ri->item);

	// Refresh table
	updateColourTab();
	updateSurface(false);
	setAsModified();
}

// Update Colour tab
void FQPlotWindow::updateColourTab()
{
	refreshing_ = true;

	// ColourScale - add points in reverse order so higher values appear at the top of the list
	ui.ColourScaleTable->clearContents();
	ui.ColourScaleTable->setRowCount(ui.ColourScaleWidget->nPoints());
	QTableWidgetItem *item;
	int count = 0;
	for (ColourScalePoint *csp = ui.ColourScaleWidget->lastPoint(); csp != NULL; csp = csp->prev)
	{
		item = new QTableWidgetItem(QString::number(csp->value()));
		item->setData(Qt::UserRole, ui.ColourScaleWidget->nPoints() - (count+1));
		ui.ColourScaleTable->setItem(count, 0, item);
		item = new QTableWidgetItem();
		item->setBackgroundColor(csp->colour());
		item->setData(Qt::UserRole, ui.ColourScaleWidget->nPoints() - (count+1));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable	);
		ui.ColourScaleTable->setItem(count, 1, item);
		++count;
	}
	// Select first item in list
	ui.ColourScaleTable->setCurrentItem(0);
	
	refreshing_ = false;
}
