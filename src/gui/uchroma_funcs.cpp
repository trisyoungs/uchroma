/*
	*** Main Window - Functions 
	*** src/gui/uchroma_funcs.cpp
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
#include "version.h"

// Constructor
UChromaWindow::UChromaWindow(QMainWindow *parent) : QMainWindow(parent), saveImageDialog_(this), dataImportDialog_(this)
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
	connect(ui.MainView, SIGNAL(sliceAxisValueChanged(int,double)), this, SLOT(surfaceSliceAxisValueChanged(int,double)));
	connect(ui.MainView, SIGNAL(sliceAxisClicked(int,double)), this, SLOT(addSurfaceSlice(int,double)));
}

// Destructor
UChromaWindow::~UChromaWindow()
{
}

// Window close event
void UChromaWindow::closeEvent(QCloseEvent *event)
{
}

// Load settings
void UChromaWindow::loadSettings()
{
	QSettings settings;

	// Viewer font
	if (settings.contains("ViewerFont")) viewerFont_ = settings.value("ViewerFont").toString();
}

// Save settings
void UChromaWindow::saveSettings()
{
	QSettings settings;

	// Viewer font
	settings.setValue("ViewerFont", viewerFont_);
}

// Update all tabs
void UChromaWindow::updateAllTabs()
{
	updateSourceDataTab();
	updateTransformTab();
	updateColourTab();
	updateViewTab();
}

// Update title bar
void UChromaWindow::updateTitleBar()
{
	if (modified_) setWindowTitle("uChroma v" + QString(UCHROMAREVISION) + " - " + inputFile_ + " (modified) ");
	else setWindowTitle("uChroma v" + QString(UCHROMAREVISION) + " - " + inputFile_);
}

// Update GUI after loading data
void UChromaWindow::updateAfterLoad()
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

void UChromaWindow::on_actionFileNew_triggered(bool checked)
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

	clearData();
	updateSurface();
	ui.AnalyseSurfaceSliceGraph->removeAllDataSets();
	updateAfterLoad();
}

void UChromaWindow::on_actionFileLoad_triggered(bool checked)
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

	QString fileName = QFileDialog::getOpenFileName(this, "Choose file to load", dataFileDirectory_.absolutePath(), "uChroma files (*.fqp);;All files (*.*)");
	if (fileName.isEmpty()) return;

	clearData();
	loadData(fileName);
	updateAfterLoad();
}

void UChromaWindow::on_actionFileSave_triggered(bool checked)
{
	// Has an input filename already been chosen?
	if (inputFile_.isEmpty())
	{
		QString fileName = QFileDialog::getSaveFileName(this, "Choose save file name", dataFileDirectory_.absolutePath(), "uChroma files (*.fqp);;All files (*.*)");
		if (fileName.isEmpty()) return;
		inputFile_ = fileName;
	}

	if (saveData(inputFile_)) modified_ = false;
	updateTitleBar();
}

void UChromaWindow::on_actionFileSaveAs_triggered(bool checked)
{
	// Has an input filename already been chosen?
	QString fileName = QFileDialog::getSaveFileName(this, "Choose save file name", dataFileDirectory_.absolutePath(), "uChroma files (*.fqp);;All files (*.*)");
	if (fileName.isEmpty()) return;
	
	inputFile_ = fileName;
	if (saveData(inputFile_)) modified_ = false;
	updateTitleBar();
}

void UChromaWindow::on_actionFileImportData_triggered(bool checked)
{
	// Raise the Data Import dialog
	bool fitData = slices_.nItems() == 0;
	bool result = dataImportDialog_.import();
	if (!result) return;

	// Loop over list of imported slices and copy them to our local list
	for (Slice* slice = dataImportDialog_.importedSlices(); slice != NULL; slice = slice->next)
	{
		Slice* newSlice = slices_.add();
		(*newSlice) = (*slice);
	}

	// Update data limits and file list
	calculateDataLimits();
	if (fitData) showAll();
	updateSourceDataTab();
	updateTransformTab();
	setAsModified();
	
	// Need to update surface
	updateSurface();
}

void UChromaWindow::on_actionFileSaveImage_triggered(bool checked)
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

void UChromaWindow::on_actionFileQuit_triggered(bool checked)
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
 * View Menu
 */

void UChromaWindow::on_actionViewPerspective_triggered(bool checked)
{
	ui.MainView->setHasPerspective(checked);
	ui.MainView->update();
}

void UChromaWindow::on_actionViewReset_triggered(bool checked)
{
	Matrix A;
	A[14] = -5.0;
	ui.MainView->setViewMatrix(A);
	ui.MainView->update();
}

/*
 * Settings Menu
 */

void UChromaWindow::on_actionSettingsChooseFont_triggered(bool checked)
{
	static QDir currentFontDirectory = viewerFont_;
	QString newFont = QFileDialog::getOpenFileName(this, "Choose truetype font", currentFontDirectory.path(), "TrueType font files (*.ttf);;All files (*.*)");
	if (!newFont.isEmpty())
	{
		viewerFont_ = newFont;
		ui.MainView->setupFont(viewerFont_);
		saveSettings();
	}
}

/*
 * Colours
 */

// Update colour scale
void UChromaWindow::updateColourScale()
{
	colourScale_.clear();
	if (colourSource_ == UChromaWindow::SingleColourSource) colourScale_.addPoint(0.0, colourSinglePoint_.colour());
	else if (colourSource_ == UChromaWindow::RGBGradientSource)
	{
		colourScale_.addPoint(colourRGBGradientAPoint_.value(), colourRGBGradientAPoint_.colour());
		colourScale_.addPoint(colourRGBGradientBPoint_.value(), colourRGBGradientBPoint_.colour());
	}
	else if (colourSource_ == UChromaWindow::HSVGradientSource)
	{
		colourScale_.setUseHSV(true);
		colourScale_.addPoint(colourHSVGradientAPoint_.value(), colourHSVGradientAPoint_.colour());
		colourScale_.addPoint(colourHSVGradientBPoint_.value(), colourHSVGradientBPoint_.colour());
	}
	else if (colourSource_ == UChromaWindow::CustomGradientSource) colourScale_ = customColourScale_;
}
