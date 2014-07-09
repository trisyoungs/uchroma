/*
	*** Main Window - Slots
	*** src/gui/uchroma_slots.cpp
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

#include "gui/uchroma.h"
#include "render/fontinstance.h"
#include "templates/reflist.h"
#include "version.h"

// Window close event
void UChromaWindow::closeEvent(QCloseEvent *event)
{
	if (checkBeforeClose()) event->accept();
	else event->setAccepted(false);
}

// Return centre coordinate of main window
QPoint UChromaWindow::centrePos()
{
	QPoint centre = pos();
	centre += QPoint(width()/2, height()/2);
	return centre;
}

/*
 * File Actions
 */

void UChromaWindow::on_actionFileNew_triggered(bool checked)
{
	if (modified_)
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning", "The current file has been modified.\nDo you want to save this data first?", QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No, QMessageBox::Cancel);
		if (button == QMessageBox::Cancel) return;
		else if (button == QMessageBox::Yes)
		{
			// Save file, and check modified_ status to make sure it wasn't cancelled.
			on_actionFileSave_triggered(false);
			if (modified_) return;
		}
	}

	// Clear data and create new, empty collection ready for use
	clearData(false);
	currentCollection_ = addCollection();

	// Update the GUI
	updateGUI();
}

void UChromaWindow::on_actionFileLoad_triggered(bool checked)
{
	if (modified_)
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning", "The current file has been modified.\nDo you want to save this data first?", QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No, QMessageBox::Cancel);
		if (button == QMessageBox::Cancel) return;
		else if (button == QMessageBox::Yes)
		{
			// Save file, and check modified_ status to make sure it wasn't cancelled.
			on_actionFileSave_triggered(false);
			if (modified_) return;
		}
	}

	QString fileName = QFileDialog::getOpenFileName(this, "Choose file to load", inputFileDirectory_.absolutePath(), "uChroma files (*.ucr);;All files (*.*)");
	if (fileName.isEmpty()) return;
	inputFileDirectory_ = fileName;

	// Clear existing data and load input file
	clearData(true);
	loadInputFile(fileName);

	// Update the GUI
	updateGUI();
}

void UChromaWindow::on_actionFileSave_triggered(bool checked)
{
	// Has an input filename already been chosen?
	if (inputFile_.isEmpty())
	{
		QString fileName = QFileDialog::getSaveFileName(this, "Choose save file name", inputFileDirectory_.absolutePath(), "uChroma files (*.ucr);;All files (*.*)");
		if (fileName.isEmpty()) return;
		inputFile_ = fileName;

		// Make sure the file has the right extension
		QFileInfo fileInfo(inputFile_);
		if (fileInfo.suffix() != "ucr") inputFile_ += ".ucr";
	}

	if (saveInputFile(inputFile_)) modified_ = false;

	// Update title bar
	updateTitleBar();
}

void UChromaWindow::on_actionFileSaveAs_triggered(bool checked)
{
	// Get a filename from the user
	QString fileName = QFileDialog::getSaveFileName(this, "Choose save file name", inputFileDirectory_.absolutePath(), "uChroma files (*.ucr);;All files (*.*)");
	if (fileName.isEmpty()) return;
	inputFile_ = fileName;

	// Make sure the file has the right extension
	QFileInfo fileInfo(inputFile_);
	if (fileInfo.suffix() != "ucr") inputFile_ += ".ucr";

	if (saveInputFile(inputFile_)) modified_ = false;

	// Update title bar
	updateTitleBar();
}


void UChromaWindow::on_actionFileExportImage_triggered(bool checked)
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
	if (checkBeforeClose()) QApplication::exit(0);
}

// Check for modified data before closing
bool UChromaWindow::checkBeforeClose()
{
	if (modified_)
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning", "The current file has been modified.\nDo you want to save this data first?", QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No, QMessageBox::Cancel);
		if (button == QMessageBox::Cancel) return false;
		else if (button == QMessageBox::Yes)
		{
			// Save file, and check modified_ status to make sure it wasn't cancelled.
			on_actionFileSave_triggered(false);
			if (modified_) return false;
		}
	}
	return true;
}

/*
 * View Actions
 */

void UChromaWindow::on_actionViewPerspective_triggered(bool checked)
{
	if (currentViewPane_) currentViewPane_->setHasPerspective(checked);

	ui.MainView->update();
}

void UChromaWindow::on_actionViewReset_triggered(bool checked)
{
	if (currentViewPane_) currentViewPane_->resetView();

	ui.MainView->update();
}

/*
 * Collections
 */

void UChromaWindow::on_actionCollectionsNew_triggered(bool checked)
{
	addCollection();

	updateGUI();
}

void UChromaWindow::on_actionCollectionsFocusNext_triggered(bool checked)
{
	focusNextCollection();
}

void UChromaWindow::on_actionCollectionsFocusPrevious_triggered(bool checked)
{
	focusPreviousCollection();
}

/*
 * Data Actions
 */

void UChromaWindow::on_actionDataLoadXY_triggered(bool checked)
{
	// Check for valid current collection
	if (!currentCollection_) return;

	dataWindow_.ui.AddFilesButton->click();
}

void UChromaWindow::on_actionDataImport_triggered(bool checked)
{
	// Check for valid current collection
	if (!currentCollection_) return;

	// Raise the Data Import dialog
	bool fitData = currentCollection_->nDataSets() == 0;
	bool result = dataImportDialog_.import();
	if (!result) return;

	// Loop over list of imported slices and copy them to our local list
	for (DataSet* dataSet = dataImportDialog_.importedSlices(); dataSet != NULL; dataSet = dataSet->next) currentCollection_->addDataSet(dataSet);

	// Update subwindows
	updateSubWindows();
	setAsModified();
	
	// Need to update display
	updateDisplay();
}

/*
 * Axes Actions
 */

// Change interaction axis target
void UChromaWindow::interactionActionTriggered(int axis)
{
	// Toggle interaction axis
	if ((interactionAxis_ == axis) || (axis == -1))
	{
		ui.actionAxesInteractNone->setChecked(true);
		setInteractionMode(InteractionMode::ViewInteraction, -1);
	}
	else
	{
		setInteractionMode(InteractionMode::ZoomInteraction, axis);
		updateInteractionPosition(ui.MainView->rMouseLast().x, ui.MainView->contextHeight() - ui.MainView->rMouseLast().y);
	}

	// Update GUI
	updateDisplay();
	updateCoordinateInfo();
}

void UChromaWindow::on_actionAxesShowAll_triggered(bool checked)
{
	currentViewPane_->showAllData();

	updateGUI();
}

void UChromaWindow::on_actionAxesInteractX_triggered(bool checked)
{
	if (refreshing_) return;

	interactionActionTriggered(0);
}

void UChromaWindow::on_actionAxesInteractY_triggered(bool checked)
{
	if (refreshing_) return;

	interactionActionTriggered(1);
}

void UChromaWindow::on_actionAxesInteractZ_triggered(bool checked)
{
	if (refreshing_) return;

	interactionActionTriggered(2);
}

void UChromaWindow::on_actionAxesInteractNone_triggered(bool checked)
{
	if (refreshing_) return;

	interactionActionTriggered(2);
}

/*
 * Windows Actions
 */

void UChromaWindow::on_actionWindowsCollections_triggered(bool checked)
{
	if (refreshing_) return;
	ui.LeftWidgetsWidget->setVisible(checked);
}

void UChromaWindow::on_actionWindowsData_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) dataWindow_.updateAndShow();
	else dataWindow_.hide();
}

void UChromaWindow::on_actionWindowsStyle_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) styleWindow_.updateAndShow();
	else styleWindow_.hide();
}

void UChromaWindow::on_actionWindowsTransform_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) transformWindow_.updateAndShow();
	else transformWindow_.hide();
}

void UChromaWindow::on_actionWindowsView_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) viewWindow_.updateAndShow();
	else viewWindow_.hide();
}

void UChromaWindow::on_actionWindowsAxes_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) axesWindow_.updateAndShow();
	else axesWindow_.hide();
}

void UChromaWindow::on_actionWindowsCreate_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) createWindow_.updateAndShow();
	else createWindow_.hide();
}

void UChromaWindow::on_actionWindowsFit_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) fitWindow_.updateAndShow();
	else fitWindow_.hide();
}

void UChromaWindow::on_actionWindowsSliceMonitor_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) sliceMonitorWindow_.updateAndShow();
	else sliceMonitorWindow_.hide();
}

/*
 * Settings Actions
 */

void UChromaWindow::on_actionSettingsChooseFont_triggered(bool checked)
{
	static QDir currentFontDirectory = viewerFont_;
	QString newFont = QFileDialog::getOpenFileName(this, "Choose truetype font", currentFontDirectory.path(), "TrueType font files (*.ttf);;All files (*.*)");
	if (!newFont.isEmpty())
	{
		viewerFont_ = newFont;
		if (!QFile::exists(viewerFont_)) QMessageBox::warning(this, "Font Error", "The specified font file '" + viewerFont_ + "' does not exist.");
		else if (!FontInstance::setupFont(viewerFont_)) QMessageBox::warning(this, "Font Error", "Failed to create a font from the specified font file '" + viewerFont_ +"'.");
		saveSettings();
	}
}
