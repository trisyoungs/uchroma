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
#include "gui/uchroma.h"
#include "gui/editdataset.h"
#include "base/currentproject.h"
#include "templates/reflist.h"
#include "templates/variantpointer.h"

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

// Reload datasets from their associated files
void DataWindow::reloadDataSets()
{
	// Check for valid collection
	Collection* currentCollection = uChroma_.currentCollection();
	if (!currentCollection) return;

	QString dir = QFileDialog::getExistingDirectory(this, "Select Data Directory", currentCollection->dataFileDirectory().path());
	if (dir.isEmpty()) return;
	ui.SourceDirEdit->setText(dir);
	currentCollection->setDataFileDirectory(dir);
	
	// Reload all data and update surface
	QProgressDialog progress("Loading data...", "Abort", 0, currentCollection->nDataSets(), this);
	progress.setWindowModality(Qt::WindowModal);
	int n=0;
	for (DataSet* dataSet = currentCollection->dataSets(); dataSet != NULL; dataSet = dataSet->next)
	{
		currentCollection->loadDataSet(dataSet);
		progress.setValue(n++);
		if (progress.wasCanceled()) break;
	}
	progress.setValue(currentCollection->nDataSets());

	// Need to update GUI
	CurrentProject::setAsModified();
	uChroma_.updateGUI();
}

/*
 * Slots
 */

// Select source directory
void DataWindow::on_SourceDirSelectButton_clicked(bool checked)
{
	reloadDataSets();
}

// Add files button clicked
void DataWindow::on_AddFilesButton_clicked(bool checked)
{
	// Check for valid collection
	Collection* currentCollection = uChroma_.currentCollection();
	if (!currentCollection) return;

	QStringList files = QFileDialog::getOpenFileNames(this, QString("Select XY datafiles (importing into '")+currentCollection->name()+"')", currentCollection->dataFileDirectory().path(), "Text files (*.txt);;MINT files (*.mint01);;MDCS files (*.mdcs01);;All files (*)");

	QProgressDialog progress("Loading data...", "Abort Loading", 0, files.count(), this);
	progress.setWindowModality(Qt::WindowModal);
	int count = 0;
	
	// Determine automatic Z placement for dataset
	double z = 0.0, delta = 1.0;
	if (currentCollection->nDataSets() == 1) z = currentCollection->lastDataSet()->data().z() + 1.0;
	else if (currentCollection->nDataSets() > 1)
	{
		delta = currentCollection->lastDataSet()->data().z() - currentCollection->lastDataSet()->prev->data().z();
		z = currentCollection->lastDataSet()->prev->data().z() + delta;
	}

	foreach (QString fileName, files)
	{
		progress.setValue(count);
		if (progress.wasCanceled()) break;

		// Get FileInfo for fileName
		QFileInfo fileInfo(fileName);

		DataSet* dataSet = currentCollection->addDataSet(z);
		dataSet->setName(fileInfo.fileName());
		dataSet->setDataSource(DataSet::FileSource);
		dataSet->setSourceFileName(currentCollection->dataFileDirectory().relativeFilePath(fileName));
		currentCollection->setDataSetZ(dataSet, z);

		if (dataSet && currentCollection->loadDataSet(dataSet)) ++count;
		
		z += delta;
	}
	progress.setValue(files.count());

	// Was any data loaded?
	if (count == 0) return;

	// Query whether limits should be updated to encompass all data
	QMessageBox::Button button = QMessageBox::question(this, "New Data Loaded", "New data has been loaded - set current data limts to encompass all data?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	if (button == QMessageBox::Yes)
	{
		// Check display pane for current collection
		if (currentCollection->displayPane()) currentCollection->displayPane()->showAllData();
	}

	// Need to update GUI
	CurrentProject::setAsModified();
	uChroma_.updateGUI();
}

// Remove files button clicked
void DataWindow::on_RemoveFilesButton_clicked(bool checked)
{
	// Check for valid collection
	Collection* currentCollection = uChroma_.currentCollection();
	if (!currentCollection) return;

	// From the selected items, construct a list of dataset to remove
	RefList<DataSet,int> dataSetsToRemove;
	foreach(QTableWidgetItem* item, ui.DataSetsTable->selectedItems()) dataSetsToRemove.addUnique(currentCollection->dataSet(item->row()));

	// Delete datasets....
	for (RefListItem<DataSet,int>* ri = dataSetsToRemove.first(); ri != NULL; ri = ri->next) currentCollection->removeDataSet(ri->item);

	// Need to update GUI
	CurrentProject::setAsModified();
	uChroma_.updateGUI();
}

// Source data item selection changed
void DataWindow::on_DataSetsTable_itemSelectionChanged()
{
	bool nSelected = ui.DataSetsTable->selectedItems().count();
	ui.RemoveFilesButton->setEnabled(nSelected > 0);
	ui.EditDataSetButton->setEnabled(nSelected > 0);
}

void DataWindow::on_DataSetsTable_itemDoubleClicked(QTableWidgetItem* item)
{
	if (item->column() == 0)
	{
		DataSet* dataSet = VariantPointer<DataSet>(item->data(Qt::UserRole));
		if (!dataSet) return;

		EditDataSetDialog dataSetDialog(this);
		if (dataSetDialog.call(dataSet)) uChroma_.currentCollection()->setDataSetData(dataSet, dataSetDialog.dataSet());

		updateControls();
	}
}

void DataWindow::on_DataSetsTable_cellChanged(int row, int column)
{
	// Check for window refreshing or invalid Collection
	Collection* currentCollection = uChroma_.currentCollection();
	if (refreshing_ || (!currentCollection)) return;

	// Z changed
	if (column == 1)
	{
		// Get slice and widget item
		DataSet* dataSet = currentCollection->dataSet(row);
		if (dataSet == NULL) return;

		QTableWidgetItem* item = ui.DataSetsTable->item(row, column);
		if (item == NULL) return;

		// Set new value of z (its position in the list will be adjusted if necessary)
		currentCollection->setDataSetZ(dataSet, item->text().toDouble());

		// Need to update now
		CurrentProject::setAsModified();
		uChroma_.updateGUI();
	}
}

// Retrieve relative Z values from timestamps
void DataWindow::on_GetZFromTimeStampButton_clicked(bool checked)
{
	// Check for window refreshing or invalid Collection
	Collection* currentCollection = uChroma_.currentCollection();
	if (refreshing_ || (!currentCollection)) return;

	// Check for no datasets
	if (currentCollection->nDataSets() == 0) return;

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
	for (DataSet* dataSet = currentCollection->dataSets(); dataSet != NULL; dataSet = dataSet->next)
	{
		// Construct filename to search for
		QFileInfo baseInfo(dataSet->sourceFileName());
		s = dir.absoluteFilePath(baseInfo.baseName()) + "." + extension;
		QFileInfo fileInfo(s);
		if (!fileInfo.exists())
		{
			QMessageBox::warning(this, "Failed to Open File", "The file '" + s + "' could not be found.");
			break;
		}
		dataSet->data().setZ(referenceTime.secsTo(fileInfo.lastModified()));
		
		if ((earliest == 0) || (dataSet->data().z() < earliest)) earliest = dataSet->data().z();
	}
	
	// Set correct offset
	for (DataSet* dataSet = currentCollection->dataSets(); dataSet != NULL; dataSet = dataSet->next) dataSet->data().setZ(dataSet->data().z() - earliest);
	currentCollection->setDisplayDataInvalid();

	// Need to update now
	CurrentProject::setAsModified();
	uChroma_.updateGUI();
}

void DataWindow::on_ReloadFilesButton_clicked(bool checked)
{
	// Check for invalid Collection
	Collection* currentCollection = uChroma_.currentCollection();
	if (!currentCollection) return;
	
	// Reload all data
	QProgressDialog progress("Reloading data...", "Abort", 0, currentCollection->nDataSets(), this);
	progress.setWindowModality(Qt::WindowModal);
	int n=0;
	int nFailed = 0;
	for (DataSet* dataSet = currentCollection->dataSets(); dataSet != NULL; dataSet = dataSet->next)
	{
		if (!currentCollection->loadDataSet(dataSet)) ++nFailed;
		progress.setValue(n++);
		if (progress.wasCanceled()) break;
	}
	progress.setValue(currentCollection->nDataSets());

	// Any failed to load?
	if (nFailed > 0)
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Failed to Load Data", QString("Failed to reload data for ") + QString::number(nFailed) + " defined datasets.\nWould you like to remove empty datasets from the list?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		if (button == QMessageBox::Yes)
		{
			DataSet* dataSet = currentCollection->dataSets(), *nextDataSet;
			while (dataSet != NULL)
			{
				nextDataSet = dataSet->next;
				if (dataSet->data().nPoints() == 0) currentCollection->removeDataSet(dataSet);
				dataSet = nextDataSet;
			}
		}
	}

	// Need to update GUI
	CurrentProject::setAsModified();
	uChroma_.updateGUI();
}

void DataWindow::on_EditDataSetButton_clicked(bool checked)
{
	// From the selected items, construct a list of dataset to remove
	RefList<DataSet,int> dataSetsToEdit;
	foreach(QTableWidgetItem* item, ui.DataSetsTable->selectedItems()) dataSetsToEdit.addUnique(VariantPointer<DataSet>(item->data(Qt::UserRole)));

	// Raise a dialog for each selected dataset in turn
	for (RefListItem<DataSet,int>* ri = dataSetsToEdit.first(); ri != NULL; ri = ri->next)
	{
		DataSet* dataSet = ri->item;
		if (!dataSet) continue;

		EditDataSetDialog dataSetDialog(this);
		if (dataSetDialog.call(dataSet)) uChroma_.currentCollection()->setDataSetData(dataSet, dataSetDialog.dataSet());
	}

	updateControls();
}

/*
 * Update
 */

// Update controls and show window
void DataWindow::updateAndShow()
{
	updateControls(true);
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

	ui.DataSetsTable->clearContents();
	ui.DataSetsTable->setRowCount(currentCollection->nDataSets());
	int count = 0;
	for (DataSet* dataSet = currentCollection->dataSets(); dataSet != NULL; dataSet = dataSet->next)
	{
		// Create filename item
		QTableWidgetItem* item = new QTableWidgetItem(dataSet->sourceFileName());
		item->setData(Qt::UserRole, VariantPointer<DataSet>(dataSet));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ui.DataSetsTable->setItem(count, 0, item);

		// Create nPoints item
		item = new QTableWidgetItem(QString::number(dataSet->data().nPoints()));
		item->setData(Qt::UserRole, VariantPointer<DataSet>(dataSet));
		item->setFlags(Qt::ItemIsSelectable);
		ui.DataSetsTable->setItem(count, 1, item);

		// Create Z value item
		item = new QTableWidgetItem(QString::number(dataSet->data().z()));
		item->setData(Qt::UserRole, VariantPointer<DataSet>(dataSet));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		ui.DataSetsTable->setItem(count, 2, item);

		// Create Source item
		item = new QTableWidgetItem(DataSet::dataSource(dataSet->dataSource()));
		item->setData(Qt::UserRole, VariantPointer<DataSet>(dataSet));
		item->setFlags(Qt::ItemIsSelectable);
		ui.DataSetsTable->setItem(count, 3, item);

		++count;
	}
	ui.DataSetsTable->resizeColumnsToContents();

	refreshing_ = false;
}
