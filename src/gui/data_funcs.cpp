/*
	*** uChroma Data Window
	*** src/gui/data_funcs.cpp
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

#include "gui/data.h"
#include "uchroma.h"
#include "templates/reflist.h"

/*
 * Window Functions
 */

// Constructor
DataWindow::DataWindow(UChromaWindow& parent) : QWidget(&parent), uChroma_(parent)
{
	ui.setupUi(this);

	QWidget::setWindowFlags(Qt::Tool);

	refreshing_ = false;
}

// Destructor
DataWindow::~DataWindow()
{
}

// Window close event
void DataWindow::closeEvent(QCloseEvent* event)
{
	emit(windowClosed(false));
}

/*
 * Convenience Functions
 */

/*
 * Slots
 */

// Select source directory
void DataWindow::on_SourceDirSelectButton_clicked(bool checked)
{
	// Check for valid collection
	Collection* currentCollection = uChroma_.currentCollection();
	if (!currentCollection) return;

	QString dir = QFileDialog::getExistingDirectory(this, "Select Data Directory", currentCollection->dataFileDirectory().path());
	if (dir.isEmpty()) return;
	ui.SourceDirEdit->setText(dir);
	currentCollection->setDataFileDirectory(dir);
	
	// Reload all data and update surface
	QProgressDialog progress("Loading data...", "Abort", 0, currentCollection->nSlices(), this);
	progress.setWindowModality(Qt::WindowModal);
	int n=0;
	for (Slice* slice = currentCollection->slices(); slice != NULL; slice = slice->next)
	{
		currentCollection->loadSliceData(slice);
		progress.setValue(n++);
		if (progress.wasCanceled()) break;
	}
	progress.setValue(currentCollection->nSlices());

	// Need to update GUI
	uChroma_.setAsModified();
	uChroma_.updateGUI();
}

// Add files button clicked
void DataWindow::on_AddFilesButton_clicked(bool checked)
{
	// Check for valid collection
	Collection* currentCollection = uChroma_.currentCollection();
	if (!currentCollection) return;

	QStringList files = QFileDialog::getOpenFileNames(this, QString("Select XY datafiles (importing into '")+currentCollection->title()+"')", currentCollection->dataFileDirectory().path(), "MINT files (*.mint01);;MDCS files (*.mdcs01);;Text files (*.txt);;All files (*)");

	QProgressDialog progress("Loading data...", "Abort Loading", 0, files.count(), this);
	progress.setWindowModality(Qt::WindowModal);
	int count = 0;
	
	// Determine automatic Z placement for slice
	double z = 0.0, delta = 1.0;
	if (currentCollection->nSlices() == 1) z = currentCollection->lastSlice()->data().z() + 1.0;
	else if (currentCollection->nSlices() > 1)
	{
		delta = currentCollection->lastSlice()->data().z() - currentCollection->lastSlice()->prev->data().z();
		z = currentCollection->lastSlice()->prev->data().z() + delta;
	}

	foreach (QString fileName, files)
	{
		progress.setValue(count);
		if (progress.wasCanceled()) break;

		Slice *slice = currentCollection->addSlice(z);
		slice->setTitle(fileName);
		slice->setDataSource(Slice::FileSource);
		slice->setSourceFileName(currentCollection->dataFileDirectory().relativeFilePath(fileName));
		currentCollection->setSliceZ(slice, z);

		if (slice && currentCollection->loadSliceData(slice)) ++count;
		
		z += delta;
	}
	progress.setValue(files.count());

	// Was any data loaded?
	if (count == 0) return;

	// Query whether limits should be updated to encompass all data
	QMessageBox::Button button = QMessageBox::question(this, "New Data Loaded", "New data has been loaded - set current data limts to encompass all data?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	if (button == QMessageBox::Yes) uChroma_.showAllData();

	// Need to update GUI
	uChroma_.setAsModified();
	uChroma_.updateGUI();
}

// Remove files button clicked
void DataWindow::on_RemoveFilesButton_clicked(bool checked)
{
	// Check for valid collection
	Collection* currentCollection = uChroma_.currentCollection();
	if (!currentCollection) return;

	// From the selected items, construct a row of Slices to remove
	RefList<Slice,int> slicesToRemove;
	foreach(QTableWidgetItem* item, ui.SourceFilesTable->selectedItems()) slicesToRemove.addUnique(currentCollection->slice(item->row()));

	// Delete slices....
	for (RefListItem<Slice,int>* ri = slicesToRemove.first(); ri != NULL; ri = ri->next) currentCollection->removeSlice(ri->item);

	// Need to update GUI
	uChroma_.setAsModified();
	uChroma_.updateGUI();
}

// Source data item selection changed
void DataWindow::on_SourceFilesTable_itemSelectionChanged()
{
	ui.RemoveFilesButton->setEnabled(ui.SourceFilesTable->selectedItems().count() != 0);
}

void DataWindow::on_SourceFilesTable_cellChanged(int row, int column)
{
	// Check for window refreshing or invalid Collection
	Collection* currentCollection = uChroma_.currentCollection();
	if (refreshing_ || (!currentCollection)) return;

	// Z changed
	if (column == 1)
	{
		// Get slice and widget item
		Slice* slice = currentCollection->slice(row);
		if (slice == NULL) return;

		QTableWidgetItem* item = ui.SourceFilesTable->item(row, column);
		if (item == NULL) return;

		// Set new value of z (its position in the list will be adjusted if necessary)
		currentCollection->setSliceZ(slice, item->text().toDouble());

		// Need to update now
		uChroma_.setAsModified();
		uChroma_.updateGUI();
	}
}

// Retrieve relative Z values from timestamps
void DataWindow::on_GetZFromTimeStampButton_clicked(bool checked)
{
	// Check for window refreshing or invalid Collection
	Collection* currentCollection = uChroma_.currentCollection();
	if (refreshing_ || (!currentCollection)) return;

	// Check for no slices
	if (currentCollection->nSlices() == 0) return;

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
	for (Slice* slice = currentCollection->slices(); slice != NULL; slice = slice->next)
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
	for (Slice* slice = currentCollection->slices(); slice != NULL; slice = slice->next) slice->data().setZ(slice->data().z() - earliest);
	currentCollection->setDisplayDataInvalid();

	// Need to update now
	uChroma_.setAsModified();
	uChroma_.updateGUI();
}

void DataWindow::on_ReloadFilesButton_clicked(bool checked)
{
	// Check for invalid Collection
	Collection* currentCollection = uChroma_.currentCollection();
	if (!currentCollection) return;
	
	// Reload all data and update surface
	QProgressDialog progress("Reloading data...", "Abort", 0, currentCollection->nSlices(), this);
	progress.setWindowModality(Qt::WindowModal);
	int n=0;
	int nFailed = 0;
	for (Slice* slice = currentCollection->slices(); slice != NULL; slice = slice->next)
	{
		if (!currentCollection->loadSliceData(slice)) ++nFailed;
		progress.setValue(n++);
		if (progress.wasCanceled()) break;
	}
	progress.setValue(currentCollection->nSlices());

	// Any failed to load?
	if (nFailed > 0)
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Failed to Load Data", QString("Failed to reload data for ") + QString::number(nFailed) + " defined slices.\nWould you like to remove empty slices from the list?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		if (button == QMessageBox::Yes)
		{
			Slice* slice = currentCollection->slices(), *nextSlice;
			while (slice != NULL)
			{
				nextSlice = slice->next;
				if (slice->data().nPoints() == 0) currentCollection->removeSlice(slice);
				slice = nextSlice;
			}
		}
	}

	// Need to update GUI
	uChroma_.setAsModified();
	uChroma_.updateGUI();
}

/*
 * Update
 */

// Update controls and show window
void DataWindow::updateAndShow()
{
	updateControls();
	show();
	move(uChroma_.centrePos() - QPoint(width()/2, height()/2));
}

// Update controls
void DataWindow::updateControls(bool force)
{
	// If the window isn't visible, do nothing...
	if ((!isVisible()) && (!force) ) return;

	// Check for invalid Collection
	Collection* currentCollection = uChroma_.currentCollection();
	if (!currentCollection) return;

	refreshing_ = true;

	ui.SourceDirEdit->setText(currentCollection->dataFileDirectory().absolutePath());

	ui.SourceFilesTable->clearContents();
	ui.SourceFilesTable->setRowCount(currentCollection->nSlices());
	int count = 0;
	for (Slice* slice = currentCollection->slices(); slice != NULL; slice = slice->next)
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
