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

#include "base/session.h"
#include "gui/uchroma.h"
#include "gui/editviewlayout.h"
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
	if (Session::isModified())
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning", "The current file has been modified.\nDo you want to save this data first?", QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No, QMessageBox::Cancel);
		if (button == QMessageBox::Cancel) return;
		else if (button == QMessageBox::Yes)
		{
			// Save file, and check Session::isModified() status to make sure it wasn't cancelled.
			on_actionFileSaveSession_triggered(false);
			if (Session::isModified()) return;
		}
	}

	startNewSession(true);

	// Update the GUI
	updateGUI();
}

void UChromaWindow::on_actionFileLoadSession_triggered(bool checked)
{
	if (Session::isModified())
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning", "The current file has been modified.\nDo you want to save this data first?", QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No, QMessageBox::Cancel);
		if (button == QMessageBox::Cancel) return;
		else if (button == QMessageBox::Yes)
		{
			// Save file, and check Session::isModified() status to make sure it wasn't cancelled.
			on_actionFileSaveSession_triggered(false);
			if (Session::isModified()) return;
		}
	}

	QString fileName = QFileDialog::getOpenFileName(this, "Choose file to load", inputFileDirectory_.absolutePath(), "uChroma files (*.ucr);;All files (*.*)");
	if (fileName.isEmpty()) return;
	inputFileDirectory_ = fileName;

	// Load input file
	if (!loadInputFile(fileName))
	{
		QMessageBox::information(this, "Failed to load session", "Failed to load the session, so reverting to the default.\n");
		startNewSession(true);
	}

	// Update the GUI
	updateGUI();
}

void UChromaWindow::on_actionFileSaveSession_triggered(bool checked)
{
	// Has an input filename already been chosen?
	if (Session::inputFile().isEmpty())
	{
		QString fileName = QFileDialog::getSaveFileName(this, "Choose save file name", inputFileDirectory_.absolutePath(), "uChroma files (*.ucr);;All files (*.*)");
		if (fileName.isEmpty()) return;

		// Make sure the file has the right extension
		QFileInfo fileInfo(fileName);
		if (fileInfo.suffix() != "ucr") fileName += ".ucr";
		Session::setInputFile(fileName);
	}

	if (saveInputFile(Session::inputFile())) Session::setAsNotModified();
}

void UChromaWindow::on_actionFileSaveSessionAs_triggered(bool checked)
{
	// Get a filename from the user
	QString fileName = QFileDialog::getSaveFileName(this, "Choose save file name", inputFileDirectory_.absolutePath(), "uChroma files (*.ucr);;All files (*.*)");
	if (fileName.isEmpty()) return;

	// Make sure the file has the right extension
	QFileInfo fileInfo(fileName);
	if (fileInfo.suffix() != "ucr") fileName += ".ucr";
	Session::setInputFile(fileName);

	if (saveInputFile(Session::inputFile())) Session::setAsNotModified();
}

void UChromaWindow::on_actionFilePrint_triggered(bool checked)
{
	// TODO
}

void UChromaWindow::on_actionFileExportImage_triggered(bool checked)
{
	if (saveImageDialog_.getImageDetails(imageExportFile_, imageExportWidth_, imageExportHeight_, imageExportFormat_, imageExportMaintainAspect_, double(ui.MainView->width()) / double(ui.MainView->height())))
	{
		// Check to see if existing image file already exists
		if (QFile::exists(saveImageDialog_.imageFileName()))
		{
			if (QMessageBox::question(this, "File Exists", "The file '" + saveImageDialog_.imageFileName() + "' already exists.\nOverwrite it?", QMessageBox::No | QMessageBox::Yes, QMessageBox::No) == QMessageBox::No) return;
		}

		imageExportFile_ = saveImageDialog_.imageFileName();
		imageExportFormat_ = saveImageDialog_.imageFormat();
		imageExportHeight_ = saveImageDialog_.imageHeight();
		imageExportWidth_ = saveImageDialog_.imageWidth();
		imageExportMaintainAspect_ = saveImageDialog_.imageAspectRatioMaintained();
		QPixmap pixmap = ui.MainView->generateImage(imageExportWidth_, imageExportHeight_);
		pixmap.save(imageExportFile_, Viewer::imageFormatExtension(imageExportFormat_), -1);

		// The sizes of panes may now be incorrect, so trigger a resize event
		recalculateViewLayout(ui.MainView->contextWidth(), ui.MainView->contextHeight());
	}
}

void UChromaWindow::on_actionFileQuit_triggered(bool checked)
{
	if (checkBeforeClose()) QApplication::exit(0);
}

// Check for modified data before closing
bool UChromaWindow::checkBeforeClose()
{
	if (Session::isModified())
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning", "The current file has been modified.\nDo you want to save this data first?", QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No, QMessageBox::Cancel);
		if (button == QMessageBox::Cancel) return false;
		else if (button == QMessageBox::Yes)
		{
			// Save file, and check Session::isModified() status to make sure it wasn't cancelled.
			on_actionFileSaveSession_triggered(false);
			if (Session::isModified()) return false;
		}
	}
	return true;
}

/*
 * View Actions
 */

bool UChromaWindow::viewTypeChanged(ViewPane::ViewType vt)
{
	currentViewPane_->setViewType(vt);
	currentViewPane_->resetViewMatrix();
	currentViewPane_->recalculateView();

	Session::setAsModified();

	axesWindow_.updateControls();
	ui.MainView->update();
}

void UChromaWindow::on_actionViewPerspective_triggered(bool checked)
{
	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane_, "view pane in UChromaWindow::on_actionViewPerspective_triggered()")) return;
	
	currentViewPane_->setHasPerspective(checked);

	Session::setAsModified();

	ui.MainView->update();
}

void UChromaWindow::on_actionViewReset_triggered(bool checked)
{
	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane_, "view pane in UChromaWindow::on_actionViewReset_triggered()")) return;

	currentViewPane_->resetViewMatrix();
	currentViewPane_->recalculateView();

	Session::setAsModified();

	ui.MainView->update();
}

void UChromaWindow::on_actionViewShowAll_triggered(bool checked)
{
	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane_, "view pane in UChromaWindow::on_actionViewShowAll_triggered()")) return;

	currentViewPane_->showAllData();
	currentViewPane_->recalculateView();

	Session::setAsModified();

	updateGUI();
}

void UChromaWindow::on_actionViewNormal_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane_, "view pane in UChromaWindow::on_actionViewNormal_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::NormalView);
}

void UChromaWindow::on_actionViewAutoStretched3D_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane_, "view pane in UChromaWindow::on_actionViewAutoStretched3D_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::AutoStretchedView);
}

void UChromaWindow::on_actionViewFlatXY_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane_, "view pane in UChromaWindow::on_actionViewFlatXY_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::FlatXYView);
}

void UChromaWindow::on_actionViewFlatXZ_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane_, "view pane in UChromaWindow::on_actionViewFlatXZ_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::FlatXZView);
}

void UChromaWindow::on_actionViewFlatYZ_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane_, "view pane in UChromaWindow::on_actionViewFlatYZ_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::FlatYZView);
}

void UChromaWindow::on_actionViewLinked_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane_, "view pane in UChromaWindow::on_actionViewLinked_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::LinkedView);
}

void UChromaWindow::on_actionViewAxes_triggered(bool checked)
{
	ui.actionWindowAxes->trigger();
}

void UChromaWindow::on_actionViewChangeLayout_triggered(bool checked)
{
	EditViewLayoutDialog layoutDialog(*this);

	// Call the dialog
	if (layoutDialog.call(viewLayout_))
	{
		viewLayout_ = layoutDialog.viewLayout();
		currentViewPane_ = viewLayout_.panes();
		recalculateViewLayout(ui.MainView->contextWidth(), ui.MainView->contextHeight());

		Session::setAsModified();

		updateGUI();
	}
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
	if (!Collection::objectValid(currentCollection_, "current collection in UChromaWindow::on_actionCollectionDuplicate_triggered")) return;

	Collection* newCollection;
	if (currentCollection_->parent())
	{
		if (currentCollection_->type() == Collection::FitCollection) newCollection = currentCollection_->parent()->addFit(currentCollection_->name());
		else if (currentCollection_->type() == Collection::ExtractedCollection) newCollection = currentCollection_->parent()->addSlice(currentCollection_->name());
		else return;
	}
	else newCollection = addCollection(currentCollection_->name());

	// Grab name before we copy, since this will be overwritten...
	QString name = newCollection->name();
	(*newCollection) = (*currentCollection_);
	newCollection->setName(name);

	updateGUI();
}

void UChromaWindow::on_actionCollectionPromoteToMaster_triggered(bool checked)
{
	if (!Collection::objectValid(currentCollection_, "current collection in UChromaWindow::on_actionCollectionPromoteToMaster_triggered")) return;

	if (currentCollection_->type() == Collection::MasterCollection) return;

	Collection* newCollection = addCollection(currentCollection_->name());

	// Grab name before we copy, since this will be overwritten...
	QString name = newCollection->name();
	(*newCollection) = (*currentCollection_);
	newCollection->setName(name);

	updateGUI();
}

void UChromaWindow::on_actionCollectionStyle_triggered(bool checked)
{
	ui.actionWindowStyle->trigger();
}

void UChromaWindow::on_actionCollectionTransform_triggered(bool checked)
{
	ui.actionWindowTransform->trigger();
}

void UChromaWindow::on_actionCollectionDelete_triggered(bool checked)
{
	// Check current Collection
	if (!Collection::objectValid(currentCollection_, "collection in UChromaWindow::on_actionCollectionDelete_triggered()")) return;

	if (QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete collection '"+currentCollection_->name()+"' and all of its associated data?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
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
	// Check current Collection
	if (!Collection::objectValid(currentCollection_, "collection in UChromaWindow::on_actionDataLoadXY_triggered()")) return;

	dataWindow_.ui.AddFilesButton->click();
}

void UChromaWindow::on_actionDataImport_triggered(bool checked)
{
	// Check current Collection
	if (!Collection::objectValid(currentCollection_, "collection in UChromaWindow::on_actionDataImport_triggered()")) return;

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
	ui.actionWindowData->trigger();
}

/*
 * Analyse Actions
 */

void UChromaWindow::on_actionAnalyseNewFit_triggered(bool checked)
{
	// Check current Collection
	if (!Collection::objectValid(currentCollection_, "collection in UChromaWindow::on_actionAnalyseNewFit_triggered()")) return;

	// Add a new fit collection to the current collection, and add it to a suitable view pane
	Collection* newFit = currentCollection_->addFit("New Fit");
	RefList<ViewPane,bool> parentPanes = viewLayout_.panes(currentCollection_, ViewPane::StandardRole);
	if (parentPanes.contains(currentViewPane_)) currentViewPane_->addCollectionTarget(newFit);
	else if (parentPanes.nItems() != 0) parentPanes.first()->item->addCollectionTarget(newFit);

	editFitSetupDialog_.setFitKernel(newFit->fitKernel());
	if (editFitSetupDialog_.updateAndExec()) newFit->fitKernel()->fit();
	else currentCollection_->removeFit(newFit);

	updateGUI();
}

void UChromaWindow::on_actionAnalyseEditFit_triggered(bool checked)
{
	// Check current Collection
	if (!Collection::objectValid(currentCollection_, "collection in UChromaWindow::on_actionAnalyseEditFit_triggered()")) return;

	if (currentCollection_->fitKernel())
	{
		editFitSetupDialog_.setFitKernel(currentCollection_->fitKernel());
		if (editFitSetupDialog_.updateAndExec())
		{
			if (currentCollection_->fitKernel()->fit()) updateGUI();
		}
	}
	else
	{
		if (QMessageBox::question(this, "No Fit to Edit", "The current collection does not contain any fit parameters.\nWould you like to create a new fit?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) on_actionAnalyseNewFit_triggered(false);
	}
}

void UChromaWindow::on_actionAnalyseUpdateFit_triggered(bool checked)
{
	// Check current Collection
	if (!Collection::objectValid(currentCollection_, "collection in UChromaWindow::on_actionAnalyseUpdateFit_triggered()")) return;

	if (currentCollection_->fitKernel())
	{
		if (currentCollection_->fitKernel()->fit()) updateGUI();
	}
	else
	{
		msg.print("Error: Current collection '%s' has no associated fit data.\n", qPrintable(currentCollection_->name()));
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

/*
 * Window Actions
 */

void UChromaWindow::on_actionWindowAxes_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) axesWindow_.updateAndShow();
	else axesWindow_.hide();
}

void UChromaWindow::on_actionWindowData_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) dataWindow_.updateAndShow();
	else dataWindow_.hide();
}

void UChromaWindow::on_actionWindowLog_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) logWindow_.updateAndShow();
	else logWindow_.hide();
}

void UChromaWindow::on_actionWindowStyle_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) styleWindow_.updateAndShow();
	else styleWindow_.hide();
}

void UChromaWindow::on_actionWindowTransform_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) transformWindow_.updateAndShow();
	else transformWindow_.hide();
}

void UChromaWindow::on_actionWindowView_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) viewWindow_.updateAndShow();
	else viewWindow_.hide();
}
