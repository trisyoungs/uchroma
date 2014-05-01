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
#include "templates/reflist.h"
#include "version.h"

// Window close event
void UChromaWindow::closeEvent(QCloseEvent *event)
{
	if (checkBeforeClose()) event->accept();
	else event->setAccepted(false);
}

/*
// File Menu
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
	clearData();
	currentCollection_ = addCollection();

	updateAfterLoad();
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
	clearData();
	loadInputFile(fileName);
	updateAfterLoad();
}

void UChromaWindow::on_actionFileSave_triggered(bool checked)
{
	// Has an input filename already been chosen?
	if (inputFile_.isEmpty())
	{
		QString fileName = QFileDialog::getSaveFileName(this, "Choose save file name", inputFileDirectory_.absolutePath(), "uChroma files (*.ucr);;All files (*.*)");
		if (fileName.isEmpty()) return;
		inputFile_ = fileName;
	}

	if (saveInputFile(inputFile_)) modified_ = false;
	updateTitleBar();
}

void UChromaWindow::on_actionFileSaveAs_triggered(bool checked)
{
	// Has an input filename already been chosen?
	QString fileName = QFileDialog::getSaveFileName(this, "Choose save file name", inputFileDirectory_.absolutePath(), "uChroma files (*.ucr);;All files (*.*)");
	if (fileName.isEmpty()) return;
	
	inputFile_ = fileName;
	if (saveInputFile(inputFile_)) modified_ = false;
	updateTitleBar();
}

void UChromaWindow::on_actionFileImportData_triggered(bool checked)
{
	// Check for valid current collection
	if (!currentCollection_) return;

	// Raise the Data Import dialog
	bool fitData = currentCollection_->nSlices() == 0;
	bool result = dataImportDialog_.import();
	if (!result) return;

	// Loop over list of imported slices and copy them to our local list
	for (Slice* slice = dataImportDialog_.importedSlices(); slice != NULL; slice = slice->next) currentCollection_->addSlice(slice);

	// Update axis limits
	updateAxisLimits();

	axesWindow_.updateControls();
	updateCollectionDataTab();
	updateCollectionTransformTab();
	setAsModified();
	
	// Need to update display
	updateDisplay();
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
 * Tools Menu
 */
void UChromaWindow::on_actionToolsSliceMonitor_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked)
	{
		updateSliceMonitor();
		sliceMonitorDialog_.show();
	}
	else sliceMonitorDialog_.hide();
}

void UChromaWindow::on_actionToolsFitWindow_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked)
	{
		fitDialog_.updateAll();
		fitDialog_.show();
	}
	else fitDialog_.hide();
}

void UChromaWindow::on_actionToolsCreateData_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) createDialog_.updateAndShow();
	else createDialog_.hide();
}

void UChromaWindow::on_actionToolsAxesWindow_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) axesWindow_.updateAndShow();
	else axesWindow_.hide();
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
