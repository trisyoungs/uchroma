/*
	*** Main Window - Source Data Functions 
	*** src/gui/fqplot_sourcedata.cpp
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

// Select source directory
void FQPlotWindow::on_SourceDirSelectButton_clicked(bool checked)
{
	QString dir = QFileDialog::getExistingDirectory(this, "TimeStamp Extraction", "Choose the directory containing the required files:");
	if (dir.isEmpty()) return;
	ui.SourceDirEdit->setText(dir);
	dataFileDirectory_ = dir;
	
	// Reload all data and update surface
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next) slice->loadData(dataFileDirectory_);
	setAsModified();
	updateSurface();
}

// Add files button clicked
void FQPlotWindow::on_AddFilesButton_clicked(bool checked)
{
	QStringList files = QFileDialog::getOpenFileNames(this, "Select datafiles", dataFileDirectory_.path(), "MINT files (*.mint01);;MDCS files (*.mdcs01);;Text files (*.txt);;All files (*)");

	QProgressDialog progress("Loading Files...", "Abort Loading", 0, files.count(), this);
	progress.setWindowModality(Qt::WindowModal);

	int count = 0;
	foreach (QString fileName, files)
	{
		progress.setValue(count);
		if (progress.wasCanceled()) break;

		if (loadSlice(fileName)) ++count;
	}
	progress.setValue(files.count());

	// Update data limits and file list
	calculateDataLimits();
	updateSourceDataTab();
	updateTransformTab();
	setAsModified();
	
	// Need to update surface
	updateSurface();
}

// Remove files button clicked
void FQPlotWindow::on_RemoveFilesButton_clicked(bool checked)
{
	foreach(QTableWidgetItem* item, ui.SourceFilesTable->selectedItems())
	{
		printf("%i\n", item->row());
	}
	setAsModified();
}

// Source data item selection changed
void FQPlotWindow::on_SourceFilesTable_itemSelectionChanged()
{
	ui.RemoveFilesButton->setEnabled(ui.SourceFilesTable->selectedItems().count() != 0);
}

void FQPlotWindow::on_SourceFilesTable_cellChanged(int row, int column)
{
	if (refreshing_) return;

	// Z changed
	if (column == 1)
	{
		// Get slice and widget item
		Slice* slice = slices_[row];
		if (slice == NULL) return;

		QTableWidgetItem* item = ui.SourceFilesTable->item(row, column);
		if (item == NULL) return;

		// Set new value of z and check its position in the list
		slice->setZ(item->text().toDouble());

		bool minBad = true, maxBad = true;
		int dummy = 0;
		do
		{
			// Shift item if necessary
			if (slice->prev && (slice->prev->z() > slice->z()))
			{
				slices_.shiftUp(slice);
				minBad = (slice->prev ? (slice->prev->z() > slice->z()) : false);
			}
			else minBad = false;
			if (slice->next && (slice->next->z() < slice->z()))
			{
				slices_.shiftDown(slice);
				maxBad = (slice->next ? (slice->next->z() < slice->z()) : false);
			}
			else maxBad = false;
			if (++dummy == 10) break;
			
		} while (minBad || maxBad);
		
		calculateDataLimits();
		calculateTransformLimits();
		updateSourceDataTab();
		updateTransformTab();
		updateSurface();
	}
}

// Retrieve relative Z values from timestamps
void FQPlotWindow::on_GetZFromTimeStampButton_clicked(bool checked)
{
	if (slices_.nItems() == 0) return;

// 	QString dir = QInputDialog::getText(this, "Choose File Location", "Select the location of the files that will be interrogated:", QLineEdit::Normal, 
	bool ok;
	QString extension = QInputDialog::getItem(this, "TimeStamp Extraction", "Select the type of file to look for:", QStringList() << "log" << "raw" << "nxs", 0, false, &ok);
	if (!ok) return;

	QString dirString = QFileDialog::getExistingDirectory(this, "TimeStamp Extraction", "Choose the directory containing the required files:");
	if (dirString.isEmpty()) return;
	QDir dir(dirString);
	
	// Load timestamp data from files - set offset in seconds from an arbitrary point to start with, then adjust afterwards
	QString s;
	double earliest = 0.0;
	QDateTime referenceTime(QDate(1970,1,1));
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next)
	{
		// Construct filename to search for
		s = dir.absoluteFilePath(slice->baseFileName()) + "." + extension;
		QFileInfo fileInfo(s);
		if (!fileInfo.exists())
		{
			QMessageBox::warning(this, "Failed to Open File", "The file '" + s + "' could not be found.");
			break;
		}
		slice->setZ(referenceTime.secsTo(fileInfo.lastModified()));
		
		if ((earliest == 0) || (slice->z() < earliest)) earliest = slice->z();
	}
	
	// Set correct offset
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next) slice->setZ(slice->z() - earliest);

	setAsModified();
	updateSourceDataTab();
	updateSurface();
}

void FQPlotWindow::on_ReloadAllDataButton_clicked(bool checked)
{
	// Reload all data and update surface
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next) slice->loadData(dataFileDirectory_);
	setAsModified();
	updateSurface();
}

// Update source data
void FQPlotWindow::updateSourceDataTab()
{
	refreshing_ = true;
	ui.SourceDirEdit->setText(dataFileDirectory_.absolutePath());

	ui.SourceFilesTable->clearContents();
	ui.SourceFilesTable->setRowCount(slices_.nItems());
	int count = 0;
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next)
	{
		QTableWidgetItem* item = new QTableWidgetItem(slice->fileName());
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ui.SourceFilesTable->setItem(count, 0, item);
		item = new QTableWidgetItem(QString::number(slice->z()));
		ui.SourceFilesTable->setItem(count, 1, item);
		++count;
	}
	ui.SourceFilesTable->resizeColumnsToContents();
	
	refreshing_ = false;
}
