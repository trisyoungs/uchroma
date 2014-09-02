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

#include "base/currentproject.h"
#include "gui/uchroma.h"
#include "render/fontinstance.h"
#include "templates/reflist.h"
#include "version.h"
#include <kernels/fit.h>

// Window close event
void UChromaWindow::closeEvent(QCloseEvent *event)
{
	if (checkBeforeClose()) event->accept();
	else event->setAccepted(false);
}

/*
 * File Actions
 */

void UChromaWindow::on_actionFileNewSession_triggered(bool checked)
{
	if (CurrentProject::isModified())
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning", "The current file has been modified.\nDo you want to save this data first?", QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No, QMessageBox::Cancel);
		if (button == QMessageBox::Cancel) return;
		else if (button == QMessageBox::Yes)
		{
			// Save file, and check CurrentProject::isModified() status to make sure it wasn't cancelled.
			on_actionFileSaveSession_triggered(false);
			if (CurrentProject::isModified()) return;
		}
	}

	// Clear data and create new, empty collection ready for use
	clearData(false);
	currentCollection_ = addCollection();

	// Update the GUI
	updateGUI();
}

void UChromaWindow::on_actionFileLoadSession_triggered(bool checked)
{
	if (CurrentProject::isModified())
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning", "The current file has been modified.\nDo you want to save this data first?", QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No, QMessageBox::Cancel);
		if (button == QMessageBox::Cancel) return;
		else if (button == QMessageBox::Yes)
		{
			// Save file, and check CurrentProject::isModified() status to make sure it wasn't cancelled.
			on_actionFileSaveSession_triggered(false);
			if (CurrentProject::isModified()) return;
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

void UChromaWindow::on_actionFileSaveSession_triggered(bool checked)
{
	// Has an input filename already been chosen?
	if (CurrentProject::inputFile().isEmpty())
	{
		QString fileName = QFileDialog::getSaveFileName(this, "Choose save file name", inputFileDirectory_.absolutePath(), "uChroma files (*.ucr);;All files (*.*)");
		if (fileName.isEmpty()) return;

		// Make sure the file has the right extension
		QFileInfo fileInfo(fileName);
		if (fileInfo.suffix() != "ucr") fileName += ".ucr";
		CurrentProject::setInputFile(fileName);
	}

	if (saveInputFile(CurrentProject::inputFile())) CurrentProject::setAsNotModified();
}

void UChromaWindow::on_actionFileSaveSessionAs_triggered(bool checked)
{
	// Get a filename from the user
	QString fileName = QFileDialog::getSaveFileName(this, "Choose save file name", inputFileDirectory_.absolutePath(), "uChroma files (*.ucr);;All files (*.*)");
	if (fileName.isEmpty()) return;

	// Make sure the file has the right extension
	QFileInfo fileInfo(fileName);
	if (fileInfo.suffix() != "ucr") fileName += ".ucr";
	CurrentProject::setInputFile(fileName);

	if (saveInputFile(CurrentProject::inputFile())) CurrentProject::setAsNotModified();
}

void UChromaWindow::on_actionFilePrint_triggered(bool checked)
{
	// TODO
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
	if (CurrentProject::isModified())
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning", "The current file has been modified.\nDo you want to save this data first?", QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No, QMessageBox::Cancel);
		if (button == QMessageBox::Cancel) return false;
		else if (button == QMessageBox::Yes)
		{
			// Save file, and check CurrentProject::isModified() status to make sure it wasn't cancelled.
			on_actionFileSaveSession_triggered(false);
			if (CurrentProject::isModified()) return false;
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
	if (!currentViewPane_) return;

	currentViewPane_->resetViewMatrix();
	currentViewPane_->recalculateView();

	ui.MainView->update();
}

void UChromaWindow::on_actionViewShowAll_triggered(bool checked)
{
	if (!currentViewPane_) return;

	currentViewPane_->showAllData();
	currentViewPane_->recalculateView();

	updateGUI();
}

void UChromaWindow::on_actionView2D_triggered(bool checked)
{
	if (refreshing_ || (!currentViewPane_)) return;

	currentViewPane_->setTwoDimensional(checked);
	currentViewPane_->recalculateView();

	axesWindow_.updateControls();
	ui.MainView->update();
}

void UChromaWindow::on_actionViewAutostretch3D_triggered(bool checked)
{
	if (refreshing_ || (!currentViewPane_)) return;

	currentViewPane_->setAutoStretch3D(checked);
	currentViewPane_->recalculateView();

	axesWindow_.updateControls();
	ui.MainView->update();
}

void UChromaWindow::on_actionViewAxes_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) axesWindow_.updateAndShow();
	else axesWindow_.hide();
}

void UChromaWindow::on_actionViewLayout_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) layoutWindow_.updateAndShow();
	else layoutWindow_.hide();
}

/*
 * Collection Actions
 */

void UChromaWindow::on_actionCollectionFocusNext_triggered(bool checked)
{
	focusNextCollection();
}

void UChromaWindow::on_actionCollectionFocusPrevious_triggered(bool checked)
{
	focusPreviousCollection();
}

void UChromaWindow::on_actionCollectionNew_triggered(bool checked)
{
	addCollection();

	updateGUI();
}

void UChromaWindow::on_actionCollectionCreate_triggered(bool checked)
{
	createCollectionDialog_.updateAndShow();
}

void UChromaWindow::on_actionCollectionDuplicate_triggered(bool checked)
{
	// TODO XXX
}

void UChromaWindow::on_actionCollectionStyle_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) styleWindow_.updateAndShow();
	else styleWindow_.hide();
}

void UChromaWindow::on_actionCollectionTransform_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) transformWindow_.updateAndShow();
	else transformWindow_.hide();
}

void UChromaWindow::on_actionCollectionDelete_triggered(bool checked)
{
	if (!currentCollection_) return;

	if (QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete collection '"+currentCollection_->title()+"' and all of its associated data?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
	{
		removeCollection(currentCollection_);
		
		updateGUI();
	}
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
	bool result = dataImportDialog_.import();
	if (!result) return;

	// Loop over list of imported slices and copy them to our local list
	for (DataSet* dataSet = dataImportDialog_.importedSlices(); dataSet != NULL; dataSet = dataSet->next) currentCollection_->addDataSet(dataSet);

	// Update subwindows
	updateSubWindows();
	
	// Need to update display
	updateDisplay();
}

void UChromaWindow::on_actionDataReload_triggered(bool checked)
{
	dataWindow_.reloadDataSets();
}

void UChromaWindow::on_actionDataView_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) dataWindow_.updateAndShow();
	else dataWindow_.hide();
}

/*
 * Analyse Actions
 */

void UChromaWindow::on_actionAnalyseNewFit_triggered(bool checked)
{
	if (!currentCollection_) return;

	// Add a new fit collection to the current collection
	Collection* newFit = currentCollection_->addFit();

	fitSetupDialog_.setFitKernel(newFit->fitKernel());
	if (fitSetupDialog_.updateAndExec()) newFit->fitKernel()->fit();
	else currentCollection_->removeFit(newFit);

	updateGUI();
}

void UChromaWindow::on_actionAnalyseEditFit_triggered(bool checked)
{
	if (!currentCollection_) return;

	if (currentCollection_->fitKernel())
	{
		fitSetupDialog_.setFitKernel(currentCollection_->fitKernel());
		if (fitSetupDialog_.updateAndExec())
		{
			if (currentCollection_->fitKernel()->fit()) updateGUI();
		}
	}
	else on_actionAnalyseNewFit_triggered(false);
}

void UChromaWindow::on_actionAnalyseUpdateFit_triggered(bool checked)
{
	if (!currentCollection_) return;

	if (currentCollection_->fitKernel())
	{
		if (currentCollection_->fitKernel()->fit()) updateGUI();
	}
	else
	{
		msg.print("Error: Current collection '%s' has no associated fit data.\n", qPrintable(currentCollection_->title()));
		ui.StatusBar->showMessage("Current collection '%s' has no associated fit data!", 3000);
	}
}

/*
 * Interact Actions
 */

// Change interaction axis target
void UChromaWindow::interactionActionTriggered(int axis)
{
	// Toggle interaction axis
	if ((interactionAxis_ == axis) || (axis == -1))
	{
		ui.actionInteractNone->setChecked(true);
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

void UChromaWindow::on_actionInteractX_triggered(bool checked)
{
	if (refreshing_) return;

	interactionActionTriggered(0);
}

void UChromaWindow::on_actionInteractY_triggered(bool checked)
{
	if (refreshing_) return;

	interactionActionTriggered(1);
}

void UChromaWindow::on_actionInteractZ_triggered(bool checked)
{
	if (refreshing_) return;

	interactionActionTriggered(2);
}

void UChromaWindow::on_actionInteractNone_triggered(bool checked)
{
	if (refreshing_) return;

	interactionActionTriggered(-1);
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

// TODO TEMPORARY
void UChromaWindow::on_actionWindowsView_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) viewWindow_.updateAndShow();
	else viewWindow_.hide();
}
