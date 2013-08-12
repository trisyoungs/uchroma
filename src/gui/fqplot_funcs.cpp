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
FQPlotWindow::FQPlotWindow(QMainWindow *parent) : QMainWindow(parent), saveImageDialog_(this)
{
	// Initialise the icon resource
	Q_INIT_RESOURCE(icons);

	// Call the main creation function
	ui.setupUi(this);

	// Set variable defaults
	dataFileDirectory_ = getenv("CWD");
#ifdef WIN32
	viewerFont_ = QDir::current().absoluteFilePath("bin/wright.ttf");
#else
	viewerFont_ = QDir::current().absoluteFilePath("wright.ttf");
#endif
	clearData();
	sliceAxis_ = -1;
	sliceAxisValue_ = 0.0;
	refreshing_ = false;

	// Load settings...
	loadSettings();

	// Load font for viewer
	ui.MainView->setupFont(viewerFont_);

	// Connect signals / slots between Viewer and main UI
	connect(ui.MainView, SIGNAL(sliceAxisValueChanged(int,double)), this, SLOT(sliceAxisValueChanged(int,double)));
}

// Destructor
FQPlotWindow::~FQPlotWindow()
{
}

// Window close event
void FQPlotWindow::closeEvent(QCloseEvent *event)
{
}

// Load settings...
void FQPlotWindow::loadSettings()
{
	QSettings settings;

	// Viewer font
	if (settings.contains("ViewerFont")) viewerFont_ = settings.value("ViewerFont").toString();
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

// Update GUI after loading data
void FQPlotWindow::updateAfterLoad()
{
	// Update data and transform limits
	calculateDataLimits();

	// Update colour scale
	updateColourScale();
	
	// Update GUI
	updateAllTabs();
	updateTitleBar();

	// Update surface
	updateSurface();

	// Setup font
	ui.MainView->setupFont(viewerFont_);
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
	loadData(fileName);
	updateAfterLoad();
}

void FQPlotWindow::on_actionFileSave_triggered(bool checked)
{
	// Has an input filename already been chosen?
	if (inputFile_.isEmpty())
	{
		
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

void FQPlotWindow::on_actionFileSaveImage_triggered(bool checked)
{
	if (saveImageDialog_.getImageDetails(imageExportFile_, imageExportWidth_, imageExportHeight_, imageExportFormat_, imageExportMaintainAspect_, double(ui.MainView->width()) / double(ui.MainView->height())))
	{
		imageExportFile_ = saveImageDialog_.imageFileName();
		imageExportFormat_ = saveImageDialog_.imageFormat();
		imageExportHeight_ = saveImageDialog_.imageHeight();
		imageExportWidth_ = saveImageDialog_.imageWidth();
		imageExportMaintainAspect_ = saveImageDialog_.imageAspectRatioMaintained();
		QPixmap pixmap = ui.MainView->generateImage(imageExportWidth_, imageExportHeight_);
		pixmap.save(imageExportFile_, Viewer::imageFormatExtension(imageExportFormat_), -1);
	}
}

void FQPlotWindow::on_actionFileQuit_triggered(bool checked)
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

/*
 * Colours
 */

// Update colour scale
void FQPlotWindow::updateColourScale()
{
	colourScale_.clear();
	if (colourSource_ == FQPlotWindow::SingleColourSource) colourScale_.addPoint(0.0, colourSinglePoint_.colour());
	else if (colourSource_ == FQPlotWindow::RGBGradientSource)
	{
		colourScale_.addPoint(colourRGBGradientAPoint_.value(), colourRGBGradientAPoint_.colour());
		colourScale_.addPoint(colourRGBGradientBPoint_.value(), colourRGBGradientBPoint_.colour());
	}
	else if (colourSource_ == FQPlotWindow::HSVGradientSource)
	{
		colourScale_.setUseHSV(true);
		colourScale_.addPoint(colourHSVGradientAPoint_.value(), colourHSVGradientAPoint_.colour());
		colourScale_.addPoint(colourHSVGradientBPoint_.value(), colourHSVGradientBPoint_.colour());
	}
	else if (colourSource_ == FQPlotWindow::CustomGradientSource) colourScale_ = customColourScale_;
}
