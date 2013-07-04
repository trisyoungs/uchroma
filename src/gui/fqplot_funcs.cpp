/*
	*** Main Window - Functions 
	*** src/gui/fqplot_funcs.cpp
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
#include "version.h"

// Constructor
FQPlotWindow::FQPlotWindow(QMainWindow *parent) : QMainWindow(parent)
{
	// Initialise the icon resource
	Q_INIT_RESOURCE(icons);

	// Call the main creation function
	ui.setupUi(this);

	// Set variable defaults
	dataFileDirectory_ = getenv("CWD");
	clearData();
	refreshing_ = false;
}

// Destructor
FQPlotWindow::~FQPlotWindow()
{
}

// Window close event
void FQPlotWindow::closeEvent(QCloseEvent *event)
{
}

// Update all tabs
void FQPlotWindow::updateAllTabs()
{
	updateSourceDataTab();
	updateTransformTab();
	updateColourTab();
	updateViewTab();
}

// Update title bar
void FQPlotWindow::updateTitleBar()
{
	if (modified_) setWindowTitle("FQPlot v" + QString(FQPLOTVERSION) + " - " + inputFile_ + " (modified) ");
	else setWindowTitle("FQPlot v" + QString(FQPLOTVERSION) + " - " + inputFile_);
}

/*
// File Menu
*/

void FQPlotWindow::on_actionFileLoad_triggered(bool checked)
{
	if (modified_)
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning", "The current file has been modified.\nDo you want to save this data first?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		if (button == QMessageBox::Yes)
		{
			// Save file, and check modified_ status to make sure it wasn't cancelled.
			on_actionFileSave_triggered(false);
			if (modified_) return;
		}
	}

	QString fileName = QFileDialog::getOpenFileName(this, "Choose file to load", dataFileDirectory_.absolutePath(), "FQPlot files (*.fqp);;All files (*.*)");
	if (fileName.isEmpty()) return;

	clearData();
	if (loadData(fileName))
	{
		// Update data and transform limits
		calculateDataLimits();

		// Update GUI
		updateAllTabs();
		updateTitleBar();
	}
}

void FQPlotWindow::on_actionFileSave_triggered(bool checked)
{
	// Has an input filename already been chosen?
	if (inputFile_.isEmpty())
	{
		inputFile_ = QFileDialog::getSaveFileName(this, "Choose save file name", dataFileDirectory_.absolutePath(), "FQPlot files (*.fqp);;All files (*.*)");
		if (inputFile_.isEmpty()) return;
	}

	if (saveData(inputFile_)) modified_ = false;
	updateTitleBar();
}

void FQPlotWindow::on_actionFileSaveAs_triggered(bool checked)
{
	// Has an input filename already been chosen?
	QString newFileName = QFileDialog::getSaveFileName(this, "Choose save file name", dataFileDirectory_.absolutePath(), "FQPlot files (*.fqp);;All files (*.*)");
	if (newFileName.isEmpty()) return;
	
	inputFile_ = newFileName;
	if (saveData(inputFile_)) modified_ = false;
	updateTitleBar();
}

void FQPlotWindow::on_actionFileQuit_triggered(bool checked)
{
	QApplication::exit(0);
}

/*
// View Menu
*/

void FQPlotWindow::on_actionViewReset_triggered(bool checked)
{
	Matrix A;
	A[14] = -5.0;
	ui.MainView->setViewMatrix(A);
	ui.MainView->postRedisplay();
}

/*
 * Tabs - Source Data
*/

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
void FQPlotWindow::on_SourceDataTable_itemSelectionChanged()
{
	ui.RemoveFilesButton->setEnabled(ui.SourceFilesTable->selectedItems().count() != 0);
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
		ui.SourceFilesTable->setItem(count, 0, item);
		item = new QTableWidgetItem(QString::number(slice->z()));
		ui.SourceFilesTable->setItem(count, 1, item);
		++count;
	}
	ui.SourceFilesTable->resizeColumnsToContents();
	
	refreshing_ = false;
}

/*
 * Tabs - Transform
 */

void FQPlotWindow::on_TransformXTypeCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;
	transformType_[0] = (DataTransform) index;
	ui.TransformXValueSpin->setEnabled(index < 2);
	calculateTransformLimits();
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformYTypeCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;
	transformType_[1] = (DataTransform) index;
	ui.TransformYValueSpin->setEnabled(index < 2);
	calculateTransformLimits();
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformZTypeCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;
	transformType_[2] = (DataTransform) index;
	ui.TransformZValueSpin->setEnabled(index < 2);
	calculateTransformLimits();
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformXValueSpin_valueChanged(double value)
{
	if (refreshing_) return;
	transformValue_.x = value;
	calculateTransformLimits();
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformYValueSpin_valueChanged(double value)
{
	if (refreshing_) return;
	transformValue_.y = value;
	calculateTransformLimits();
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformZValueSpin_valueChanged(double value)
{
	if (refreshing_) return;
	transformValue_.z = value;
	calculateTransformLimits();
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformXPreShiftSpin_valueChanged(double value)
{
	if (refreshing_) return;
	preTransformShift_.x = value;
	calculateTransformLimits();
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformYPreShiftSpin_valueChanged(double value)
{
	if (refreshing_) return;
	preTransformShift_.y = value;
	calculateTransformLimits();
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformZPreShiftSpin_valueChanged(double value)
{
	if (refreshing_) return;
	preTransformShift_.z = value;
	calculateTransformLimits();
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformXPostShiftSpin_valueChanged(double value)
{
	if (refreshing_) return;
	postTransformShift_.x = value;
	calculateTransformLimits();
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformYPostShiftSpin_valueChanged(double value)
{
	if (refreshing_) return;
	postTransformShift_.y = value;
	calculateTransformLimits();
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformZPostShiftSpin_valueChanged(double value)
{
	if (refreshing_) return;
	postTransformShift_.z = value;
	calculateTransformLimits();
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_LimitXMinSpin_valueChanged(double value)
{
	if (refreshing_) return;
	limitMin_.x = value;
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_LimitYMinSpin_valueChanged(double value)
{
	if (refreshing_) return;
	limitMin_.y = value;
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_LimitZMinSpin_valueChanged(double value)
{
	if (refreshing_) return;
	limitMin_.z = value;
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_LimitXMaxSpin_valueChanged(double value)
{
	if (refreshing_) return;
	limitMax_.x = value;
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_LimitYMaxSpin_valueChanged(double value)
{
	if (refreshing_) return;
	limitMax_.y = value;
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_LimitZMaxSpin_valueChanged(double value)
{
	if (refreshing_) return;
	limitMax_.z = value;
	setAsModified();
	updateSurface();
}

// Update Transform tab
void FQPlotWindow::updateTransformTab()
{
	refreshing_ = true;

	// Transform type / value
	ui.TransformXTypeCombo->setCurrentIndex(transformType_[0]);
	ui.TransformYTypeCombo->setCurrentIndex(transformType_[1]);
	ui.TransformZTypeCombo->setCurrentIndex(transformType_[2]);
	ui.TransformXValueSpin->setEnabled(transformType_[0] < 2);
	ui.TransformYValueSpin->setEnabled(transformType_[1] < 2);
	ui.TransformZValueSpin->setEnabled(transformType_[2] < 2);
	ui.TransformXValueSpin->setValue(transformValue_.x);
	ui.TransformYValueSpin->setValue(transformValue_.y);
	ui.TransformZValueSpin->setValue(transformValue_.z);

	// Shifts
	ui.TransformXPreShiftSpin->setValue(preTransformShift_.x);
	ui.TransformYPreShiftSpin->setValue(preTransformShift_.y);
	ui.TransformZPreShiftSpin->setValue(preTransformShift_.z);
	ui.TransformXPostShiftSpin->setValue(postTransformShift_.x);
	ui.TransformYPostShiftSpin->setValue(postTransformShift_.y);
	ui.TransformZPostShiftSpin->setValue(postTransformShift_.z);

	// Limits
	ui.LimitXMinSpin->setRange(transformMin_.x, transformMax_.x);
	ui.LimitYMinSpin->setRange(transformMin_.y, transformMax_.y);
	ui.LimitZMinSpin->setRange(transformMin_.z, transformMax_.z);
	ui.LimitXMaxSpin->setRange(transformMin_.x, transformMax_.x);
	ui.LimitYMaxSpin->setRange(transformMin_.y, transformMax_.y);
	ui.LimitZMaxSpin->setRange(transformMin_.z, transformMax_.z);
	ui.LimitXMinSpin->setValue(limitMin_.x);
	ui.LimitYMinSpin->setValue(limitMin_.y);
	ui.LimitZMinSpin->setValue(limitMin_.z);
	ui.LimitXMaxSpin->setValue(limitMax_.x);
	ui.LimitYMaxSpin->setValue(limitMax_.y);
	ui.LimitZMaxSpin->setValue(limitMax_.z);

	refreshing_ = false;
}

/*
 * Tabs - Colour
 */

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

/*
 * Tabs - View
 */

void FQPlotWindow::on_ViewInvertZCheck_clicked(bool checked)
{
	if (refreshing_) return;
	invertZAxis_ = checked;
	setAsModified();
	ui.MainView->setInvertZ(invertZAxis_);
	ui.MainView->update();
}

// Update View tab
void FQPlotWindow::updateViewTab()
{
	refreshing_ = true;
	refreshing_ = false;
}
