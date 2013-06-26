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
}

// Destructor
FQPlotWindow::~FQPlotWindow()
{
}

// Window close event
void FQPlotWindow::closeEvent(QCloseEvent *event)
{
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
		inputFile_ = fileName;
		modified_ = false;
		
		// Update GUI
		updateSourceDataTab();
		updateViewTab();
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
}

// Source data item selection changed
void FQPlotWindow::on_SourceDataTable_itemSelectionChanged()
{
	ui.RemoveFilesButton->setEnabled(ui.SourceFilesTable->selectedItems().count() != 0);
}

// Update source data
void FQPlotWindow::updateSourceDataTab()
{
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
 * Tabs - View
 */

void FQPlotWindow::on_ColourScaleTable_cellDoubleClicked(QTableWidgetItem* item, int column)
{
}

void FQPlotWindow::on_XMinSpin_valueChanged(double value)
{
	axisMin_.x = value;
	updateSurface();
}

void FQPlotWindow::on_YMinSpin_valueChanged(double value)
{
	axisMin_.y = value;
	updateSurface();
}

void FQPlotWindow::on_ZMinSpin_valueChanged(double value)
{
	axisMin_.z = value;
	updateSurface();
}

void FQPlotWindow::on_XMaxSpin_valueChanged(double value)
{
	axisMax_.x = value;
	updateSurface();
}

void FQPlotWindow::on_YMaxSpin_valueChanged(double value)
{
	axisMax_.y = value;
	updateSurface();
}

void FQPlotWindow::on_ZMaxSpin_valueChanged(double value)
{
	axisMax_.z = value;
	updateSurface();
}

void FQPlotWindow::on_XLogCheck_clicked(bool checked)
{
	axisLog_.x = checked;
	updateSurface();
}

void FQPlotWindow::on_YLogCheck_clicked(bool checked)
{
	axisLog_.y = checked;
	updateSurface();
}

void FQPlotWindow::on_ZLogCheck_clicked(bool checked)
{
	axisLog_.z = checked;
	updateSurface();
}

void FQPlotWindow::on_XScaleSpin_valueChanged(double value)
{
	viewScales_.x = value;
	ui.MainView->update();
}

void FQPlotWindow::on_YScaleSpin_valueChanged(double value)
{
	viewScales_.y = value;
	ui.MainView->update();
}

void FQPlotWindow::on_ZScaleSpin_valueChanged(double value)
{
	viewScales_.z = value;
	ui.MainView->update();
}

// Update source data tab
void FQPlotWindow::updateViewTab()
{
}
