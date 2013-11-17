/*
	*** Main Window - Source Data Functions 
	*** src/gui/uchroma_sourcedata.cpp
	Copyright T. Youngs 2013

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

#include "gui/uchroma.h"
#include "templates/reflist.h"

// Select source directory
void UChromaWindow::on_SourceDirSelectButton_clicked(bool checked)
{
	QString dir = QFileDialog::getExistingDirectory(this, "Data Directory", "Choose the directory containing the required files:");
	if (dir.isEmpty()) return;
	ui.SourceDirEdit->setText(dir);
	dataFileDirectory_ = dir;
	
	// Reload all data and update surface
	QProgressDialog progress("Loading data...", "Abort", 0, slices_.nItems(), this);
	progress.setWindowModality(Qt::WindowModal);
	int count = 0;
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next)
	{
		slice->loadData(dataFileDirectory_);
		progress.setValue(count);
		if (progress.wasCanceled()) break;
		++count;
	}
	progress.setValue(slices_.nItems());
	
	calculateDataLimits();
	updateDataTransforms();
	showAllData();
		
	setAsModified();
	updateSurface();
}

// Add files button clicked
void UChromaWindow::on_AddFilesButton_clicked(bool checked)
{
	QStringList files = QFileDialog::getOpenFileNames(this, "Select datafiles", dataFileDirectory_.path(), "MINT files (*.mint01);;MDCS files (*.mdcs01);;Text files (*.txt);;All files (*)");

	QProgressDialog progress("Loading data...", "Abort Loading", 0, files.count(), this);
	progress.setWindowModality(Qt::WindowModal);
	int count = 0;
	
	// Determine automatic Z placement for slice
	double z = 0.0, delta = 1.0;
	if (slices_.nItems() == 1) z = slices_.last()->data().z() + 1.0;
	else if (slices_.nItems() > 1)
	{
		delta = slices_.last()->data().z() - slices_.last()->prev->data().z();
		z = slices_.last()->prev->data().z() + delta;
	}

	foreach (QString fileName, files)
	{
		progress.setValue(count);
		if (progress.wasCanceled()) break;

		Slice *slice = addSlice(z, fileName, fileName);
		if (slice && slice->loadData(dataFileDirectory_)) ++count;
		
		z += delta;
	}
	progress.setValue(files.count());

	// Was any data loaded?
	if (count == 0) return;

	// Update data limits and file list
	calculateDataLimits();
	updateSourceDataTab();

	// Query whether limits should be updated to encompass all data
	QMessageBox::Button button = QMessageBox::question(this, "New Data Loaded", "New data has been loaded - set current data limts to encompass all data?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	if (button == QMessageBox::Yes) showAllData();

	// Update transform tab and set as modified
	updateTransformTab();
	setAsModified();

	// Need to update surface, and then we're done
	updateSurface();
}

// Remove files button clicked
void UChromaWindow::on_RemoveFilesButton_clicked(bool checked)
{
	// From the selected items, construct a row of Slices to remove
	RefList<Slice,int> slicesToRemove;
	foreach(QTableWidgetItem* item, ui.SourceFilesTable->selectedItems()) slicesToRemove.addUnique(slices_[item->row()]);

	// Delete slices....
	for (RefListItem<Slice,int>* ri = slicesToRemove.first(); ri != NULL; ri = ri->next) slices_.remove(ri->item);

	setAsModified();
	calculateDataLimits();
	updateTransformTab();
	updateSourceDataTab();
	updateSurface();
}

// Source data item selection changed
void UChromaWindow::on_SourceFilesTable_itemSelectionChanged()
{
	ui.RemoveFilesButton->setEnabled(ui.SourceFilesTable->selectedItems().count() != 0);
}

void UChromaWindow::on_SourceFilesTable_cellChanged(int row, int column)
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
		slice->data().setZ(item->text().toDouble());

		bool minBad = true, maxBad = true;
		int dummy = 0;
		do
		{
			// Shift item if necessary
			if (slice->prev && (slice->prev->data().z() > slice->data().z()))
			{
				slices_.shiftUp(slice);
				minBad = (slice->prev ? (slice->prev->data().z() > slice->data().z()) : false);
			}
			else minBad = false;
			if (slice->next && (slice->next->data().z() < slice->data().z()))
			{
				slices_.shiftDown(slice);
				maxBad = (slice->next ? (slice->next->data().z() < slice->data().z()) : false);
			}
			else maxBad = false;
			if (++dummy == 10) break;
			
		} while (minBad || maxBad);
		
		calculateDataLimits();
		updateDataTransforms();
		updateSourceDataTab();
		updateTransformTab();
		updateSurface();
	}
}

// Retrieve relative Z values from timestamps
void UChromaWindow::on_GetZFromTimeStampButton_clicked(bool checked)
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
		QFileInfo baseInfo(slice->sourceFileName());
		s = dir.absoluteFilePath(baseInfo.baseName()) + "." + extension;
		QFileInfo fileInfo(s);
		if (!fileInfo.exists())
		{
			QMessageBox::warning(this, "Failed to Open File", "The file '" + s + "' could not be found.");
			break;
		}
		slice->data().setZ(referenceTime.secsTo(fileInfo.lastModified()));
		
		if ((earliest == 0) || (slice->data().z() < earliest)) earliest = slice->data().z();
	}
	
	// Set correct offset
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next) slice->data().setZ(slice->data().z() - earliest);

	setAsModified();
	updateSourceDataTab();
	updateSurface();
}

void UChromaWindow::on_ReloadFilesButton_clicked(bool checked)
{
	// Reload all data and update surface
	int nFailed = 0;
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next)
	{
		if (!slice->loadData(dataFileDirectory_)) ++nFailed;
	}

	// Any failed to load?
	if (nFailed > 0)
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Failed to Load Data", QString("Failed to reload data for ") + QString::number(nFailed) + " defined slices.\nWould you like to remove empty slices from the list?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		if (button == QMessageBox::Yes)
		{
			Slice* slice = slices_.first(), *nextSlice;
			while (slice != NULL)
			{
				nextSlice = slice->next;
				if (slice->data().nPoints() == 0) slices_.remove(slice);
				slice = nextSlice;
			}
		}
	}

	setAsModified();
	updateAfterLoad();
}

// Update source data
void UChromaWindow::updateSourceDataTab()
{
	refreshing_ = true;
	ui.SourceDirEdit->setText(dataFileDirectory_.absolutePath());

	ui.SourceFilesTable->clearContents();
	ui.SourceFilesTable->setRowCount(slices_.nItems());
	int count = 0;
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next)
	{
		QTableWidgetItem* item = new QTableWidgetItem(slice->sourceFileName());
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ui.SourceFilesTable->setItem(count, 0, item);
		item = new QTableWidgetItem(QString::number(slice->data().z()));
		ui.SourceFilesTable->setItem(count, 1, item);
		++count;
	}
	ui.SourceFilesTable->resizeColumnsToContents();
	
	refreshing_ = false;
}
