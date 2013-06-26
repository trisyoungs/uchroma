/*
	*** Main Window - FUnctions 
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
}

void FQPlotWindow::on_actionFileSave_triggered(bool checked)
{
	// Save current data in super-simple keyword format
	
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

	// Need to update stuff now...
	updateSurface();
}

// Remove files button clicked
void FQPlotWindow::on_RemoveFilesButton_clicked(bool checked)
{
}

// Source data item selection changed
void FQPlotWindow::on_SourceDataTable_itemSelectionChanged()
{
	ui.RemoveFilesButton->setEnabled(ui.SourceDataTable->selectedItems().count() != 0);
}

/*
 * Tabs - View
 */

void FQPlotWindow::on_ColourScaleTable_cellDoubleClicked(QTableWidgetItem* item, int column)
{
}
