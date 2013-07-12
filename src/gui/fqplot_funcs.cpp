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
	if (modified_) setWindowTitle("FQPlot v" + QString(FQPLOTREVISION) + " - " + inputFile_ + " (modified) ");
	else setWindowTitle("FQPlot v" + QString(FQPLOTREVISION) + " - " + inputFile_);
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

void FQPlotWindow::on_actionViewPerspective_triggered(bool checked)
{
	ui.MainView->setHasPerspective(checked);
	ui.MainView->update();
}

void FQPlotWindow::on_actionViewReset_triggered(bool checked)
{
	Matrix A;
	A[14] = -5.0;
	ui.MainView->setViewMatrix(A);
	ui.MainView->update();
}
