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
#include "version.h"

// Constructor
FQPlotWindow::FQPlotWindow(QMainWindow *parent) : QMainWindow(parent)
{
	// Initialise the icon resource
	Q_INIT_RESOURCE(icons);

	// Call the main creation function
	ui.setupUi(this);

	// Set variable defaults
// 	dataFileDirectory_ = getenv("CWD");
	dataFileDirectory_ = "/home/tris/work/RB1220486_Youngs/benzene_D_D2_250mBar";
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
	refreshing_ = true;
	updateSourceDataTab();
	updateTransformTab();
	updateViewTab();
	refreshing_ = false;
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
		// Update GUI
		updateAllTabs();
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
}

void FQPlotWindow::on_actionFileSaveAs_triggered(bool checked)
{
	// Has an input filename already been chosen?
	QString newFileName = QFileDialog::getSaveFileName(this, "Choose save file name", dataFileDirectory_.absolutePath(), "FQPlot files (*.fqp);;All files (*.*)");
	if (newFileName.isEmpty()) return;
	
	inputFile_ = newFileName;
	if (saveData(inputFile_)) modified_ = false;
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
	QStringList files = QFileDialog::getOpenFileNames(this, "Select datafiles", dataFileDirectory_.path(), "MINT files (*.mint01);;MDCS files (*.mdcs01);;All files (*)");

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

	// Update file list
	updateSourceDataTab();
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
}

/*
 * Tabs - Transform
 */

void FQPlotWindow::on_TransformXTypeCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;
	transformType_[0] = (DataTransform) index;
	ui.TransformXValueSpin->setEnabled(index < 2);
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformYTypeCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;
	transformType_[1] = (DataTransform) index;
	ui.TransformYValueSpin->setEnabled(index < 2);
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformZTypeCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;
	transformType_[2] = (DataTransform) index;
	ui.TransformZValueSpin->setEnabled(index < 2);
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformXValueSpin_valueChanged(double value)
{
	if (refreshing_) return;
	transformValue_.x = value;
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformYValueSpin_valueChanged(double value)
{
	if (refreshing_) return;
	transformValue_.y = value;
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformZValueSpin_valueChanged(double value)
{
	if (refreshing_) return;
	transformValue_.z = value;
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformXPreShiftSpin_valueChanged(double value)
{
	if (refreshing_) return;
	preTransformShift_.x = value;
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformYPreShiftSpin_valueChanged(double value)
{
	if (refreshing_) return;
	preTransformShift_.y = value;
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformZPreShiftSpin_valueChanged(double value)
{
	if (refreshing_) return;
	preTransformShift_.z = value;
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformXPostShiftSpin_valueChanged(double value)
{
	if (refreshing_) return;
	postTransformShift_.x = value;
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformYPostShiftSpin_valueChanged(double value)
{
	if (refreshing_) return;
	postTransformShift_.y = value;
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_TransformZPostShiftSpin_valueChanged(double value)
{
	if (refreshing_) return;
	postTransformShift_.z = value;
	setAsModified();
	updateSurface();
}

// Update Transform tab
void FQPlotWindow::updateTransformTab()
{
	ui.TransformXTypeCombo->setCurrentIndex(transformType_[0]);
	ui.TransformYTypeCombo->setCurrentIndex(transformType_[1]);
	ui.TransformZTypeCombo->setCurrentIndex(transformType_[2]);
	ui.TransformXValueSpin->setEnabled(transformType_[0] < 2);
	ui.TransformYValueSpin->setEnabled(transformType_[1] < 2);
	ui.TransformZValueSpin->setEnabled(transformType_[2] < 2);

	ui.TransformXValueSpin->setValue(transformValue_.x);
	ui.TransformYValueSpin->setValue(transformValue_.y);
	ui.TransformZValueSpin->setValue(transformValue_.z);

	ui.TransformXPreShiftSpin->setValue(preTransformShift_.x);
	ui.TransformYPreShiftSpin->setValue(preTransformShift_.y);
	ui.TransformZPreShiftSpin->setValue(preTransformShift_.z);
	ui.TransformXPostShiftSpin->setValue(postTransformShift_.x);
	ui.TransformYPostShiftSpin->setValue(postTransformShift_.y);
	ui.TransformZPostShiftSpin->setValue(postTransformShift_.z);
}

/*
 * Tabs - View
 */

void FQPlotWindow::on_ColourScaleTable_cellDoubleClicked(QTableWidgetItem* item, int column)
{
}

void FQPlotWindow::on_XMinSpin_valueChanged(double value)
{
	if (refreshing_) return;
	axisMin_.x = value;
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_YMinSpin_valueChanged(double value)
{
	if (refreshing_) return;
	axisMin_.y = value;
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_ZMinSpin_valueChanged(double value)
{
	if (refreshing_) return;
	axisMin_.z = value;
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_XMaxSpin_valueChanged(double value)
{
	if (refreshing_) return;
	axisMax_.x = value;
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_YMaxSpin_valueChanged(double value)
{
	if (refreshing_) return;
	axisMax_.y = value;
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_ZMaxSpin_valueChanged(double value)
{
	if (refreshing_) return;
	axisMax_.z = value;
	setAsModified();
	updateSurface();
}

void FQPlotWindow::on_XLogCheck_clicked(bool checked)
{
// 	axisLog_.x = checked;
	updateSurface();
}

void FQPlotWindow::on_YLogCheck_clicked(bool checked)
{
// 	axisLog_.y = checked;
	updateSurface();
}

void FQPlotWindow::on_ZLogCheck_clicked(bool checked)
{
// 	axisLog_.z = checked;
	updateSurface();
}

// Update source data tab
void FQPlotWindow::updateViewTab()
{
}
