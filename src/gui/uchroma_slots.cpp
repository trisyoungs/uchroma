/*
	*** Main Window - Slots
	*** src/gui/uchroma_slots.cpp
	Copyright T. Youngs 2013-2015

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
#include "gui/editviewlayout.h"
#include "gui/editfitresults.h"
#include "gui/operate_bgsub.h"
#include "gui/operate_setz.h"
#include "render/fontinstance.h"
#include "session/session.h"
#include "templates/reflist.h"
#include "version.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QProgressDialog>
#include <QPainter>

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
	if (UChromaSession::isModified())
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning", "The current file has been modified.\nDo you want to save this data first?", QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No, QMessageBox::Cancel);
		if (button == QMessageBox::Cancel) return;
		else if (button == QMessageBox::Yes)
		{
			// Save file, and check UChromaSession::isModified() status to make sure it wasn't cancelled.
			on_actionFileSaveSession_triggered(false);
			if (UChromaSession::isModified()) return;
		}
	}

	UChromaSession::startNewSession(true);

	// Update the GUI
	updateGUI();
}

void UChromaWindow::on_actionFileLoadSession_triggered(bool checked)
{
	if (UChromaSession::isModified())
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning", "The current file has been modified.\nDo you want to save this data first?", QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No, QMessageBox::Cancel);
		if (button == QMessageBox::Cancel) return;
		else if (button == QMessageBox::Yes)
		{
			// Save file, and check UChromaSession::isModified() status to make sure it wasn't cancelled.
			on_actionFileSaveSession_triggered(false);
			if (UChromaSession::isModified()) return;
		}
	}

	QString fileName = QFileDialog::getOpenFileName(this, "Choose file to load", UChromaSession::sessionFileDirectory().absolutePath(), "uChroma files (*.ucr);;All files (*.*)");
	if (fileName.isEmpty()) return;

	// Load input file
	if (!UChromaSession::loadSession(fileName))
	{
		QMessageBox::information(this, "Failed to load session", "Failed to load the session, so reverting to the default.\n");
		UChromaSession::startNewSession(true);
	}

	// Update the GUI
	updateGUI();
}

void UChromaWindow::on_actionFileSaveSession_triggered(bool checked)
{
	// Has an input filename already been chosen?
	if (UChromaSession::inputFile().isEmpty())
	{
		QString fileName = QFileDialog::getSaveFileName(this, "Choose save file name", UChromaSession::sessionFileDirectory().absolutePath(), "uChroma files (*.ucr);;All files (*.*)");
		if (fileName.isEmpty()) return;

		// Make sure the file has the right extension
		QFileInfo fileInfo(fileName);
		if (fileInfo.suffix() != "ucr") fileName += ".ucr";
		UChromaSession::setInputFile(fileName);
	}

	if (UChromaSession::saveSession(UChromaSession::inputFile())) UChromaSession::setAsNotModified();
}

void UChromaWindow::on_actionFileSaveSessionAs_triggered(bool checked)
{
	// Get a filename from the user
	QString fileName = QFileDialog::getSaveFileName(this, "Choose save file name", UChromaSession::sessionFileDirectory().absolutePath(), "uChroma files (*.ucr);;All files (*.*)");
	if (fileName.isEmpty()) return;

	// Make sure the file has the right extension
	QFileInfo fileInfo(fileName);
	if (fileInfo.suffix() != "ucr") fileName += ".ucr";
	UChromaSession::setInputFile(fileName);

	if (UChromaSession::saveSession(UChromaSession::inputFile())) UChromaSession::setAsNotModified();
}

void UChromaWindow::on_actionFilePrint_triggered(bool checked)
{
	// TODO
}

void UChromaWindow::on_actionFileExportImage_triggered(bool checked)
{
	static bool firstRun = true;
	if (saveImageDialog_.getImageDetails(double(ui.MainView->width()) / double(ui.MainView->height())))
	{
		// Check to see if existing image file already exists
		if (QFile::exists(UChromaSession::imageExportFileName()))
		{
			if (QMessageBox::question(this, "File Exists", "The file '" + UChromaSession::imageExportFileName() + "' already exists.\nOverwrite it?", QMessageBox::No | QMessageBox::Yes, QMessageBox::No) == QMessageBox::No) return;
		}

		int imageWidth = UChromaSession::imageExportWidth();
		int imageHeight = UChromaSession::imageExportHeight();

		// Generate and save pixmap
		QPixmap pixmap = ui.MainView->generateImage(imageWidth, imageHeight);
		pixmap.save(UChromaSession::imageExportFileName(), UChromaSession::imageFormatExtension(UChromaSession::imageExportFormat()), -1);
	}

	firstRun = false;
}

void UChromaWindow::on_actionFileQuit_triggered(bool checked)
{
	if (checkBeforeClose()) QApplication::exit(0);
}

// Check for modified data before closing
bool UChromaWindow::checkBeforeClose()
{
	if (UChromaSession::isModified())
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning", "The current file has been modified.\nDo you want to save this data first?", QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No, QMessageBox::Cancel);
		if (button == QMessageBox::Cancel) return false;
		else if (button == QMessageBox::Yes)
		{
			// Save file, and check UChromaSession::isModified() status to make sure it wasn't cancelled.
			on_actionFileSaveSession_triggered(false);
			if (UChromaSession::isModified()) return false;
		}
	}

	// Nullify pointer in UChromaSession, so it doesn't try and call anything to do with the main window
	UChromaSession::setMainWindow(NULL);

	return true;
}

/*
 * Edit Actions
 */
void UChromaWindow::on_actionEditUndo_triggered(bool checked)
{
	UChromaSession::undo();

	updateGUI();
}

void UChromaWindow::on_actionEditRedo_triggered(bool checked)
{
	UChromaSession::redo();

	updateGUI();
}

/*
 * View Actions
 */

bool UChromaWindow::viewTypeChanged(ViewPane::ViewType vt)
{
	UChromaSession::currentViewPane()->setViewType(vt);
	UChromaSession::currentViewPane()->resetViewMatrix();
	UChromaSession::currentViewPane()->recalculateView(true);

	UChromaSession::setAsModified();

	axesWindow_.updateControls();
	updateToolBars();
	ui.MainView->update();

	return true;
}

void UChromaWindow::on_actionViewPerspective_triggered(bool checked)
{
	// Check current ViewPane
	if (!ViewPane::objectValid(UChromaSession::currentViewPane(), "view pane in UChromaWindow::on_actionViewPerspective_triggered()")) return;
	
	UChromaSession::currentViewPane()->setHasPerspective(checked);

	UChromaSession::setAsModified();

	ui.MainView->update();
}

void UChromaWindow::on_actionViewReset_triggered(bool checked)
{
	// Check current ViewPane
	if (!ViewPane::objectValid(UChromaSession::currentViewPane(), "view pane in UChromaWindow::on_actionViewReset_triggered()")) return;

	UChromaSession::currentViewPane()->resetViewMatrix();
	UChromaSession::currentViewPane()->recalculateView();

	UChromaSession::setAsModified();

	ui.MainView->update();
}

void UChromaWindow::on_actionViewShowAll_triggered(bool checked)
{
	// Check current ViewPane
	if (!ViewPane::objectValid(UChromaSession::currentViewPane(), "view pane in UChromaWindow::on_actionViewShowAll_triggered()")) return;

	UChromaSession::currentViewPane()->showAllData();

	UChromaSession::setAsModified();

	updateGUI();
}

void UChromaWindow::on_actionViewNormal_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(UChromaSession::currentViewPane(), "view pane in UChromaWindow::on_actionViewNormal_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::NormalView);
}

void UChromaWindow::on_actionViewAutoStretched3D_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(UChromaSession::currentViewPane(), "view pane in UChromaWindow::on_actionViewAutoStretched3D_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::AutoStretchedView);
}

void UChromaWindow::on_actionViewFlatXY_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(UChromaSession::currentViewPane(), "view pane in UChromaWindow::on_actionViewFlatXY_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::FlatXYView);
}

void UChromaWindow::on_actionViewFlatXZ_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(UChromaSession::currentViewPane(), "view pane in UChromaWindow::on_actionViewFlatXZ_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::FlatXZView);
}

void UChromaWindow::on_actionViewFlatZY_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(UChromaSession::currentViewPane(), "view pane in UChromaWindow::on_actionViewFlatZY_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::FlatZYView);
}

void UChromaWindow::on_actionViewLinked_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(UChromaSession::currentViewPane(), "view pane in UChromaWindow::on_actionViewLinked_triggered()")) return;

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
	if (layoutDialog.call(UChromaSession::viewLayout()))
	{
		UChromaSession::setViewLayout(layoutDialog.viewLayout());
		UChromaSession::viewLayout().recalculate(ui.MainView->contextWidth(), ui.MainView->contextHeight());

		UChromaSession::setAsModified();

		updateGUI();
	}
}

/*
 * Collection Actions
 */

void UChromaWindow::on_actionCollectionFocusNext_triggered(bool checked)
{
	UChromaSession::focusNextCollection();
}

void UChromaWindow::on_actionCollectionFocusPrevious_triggered(bool checked)
{
	UChromaSession::focusPreviousCollection();
}

void UChromaWindow::on_actionCollectionNew_triggered(bool checked)
{
	// Start edit state group
	UChromaSession::beginEditStateGroup("create new collection");

	UChromaSession::addCollection();

	// End edit state group
	UChromaSession::endEditStateGroup();

	updateGUI();
}

void UChromaWindow::on_actionCollectionCreate_triggered(bool checked)
{
	createCollectionDialog_.updateAndShow();
}

void UChromaWindow::on_actionCollectionDuplicate_triggered(bool checked)
{
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(currentCollection, "current collection in UChromaWindow::on_actionCollectionDuplicate_triggered")) return;

	Collection* newCollection;
	if (currentCollection->parent())
	{
		if (currentCollection->type() == Collection::FitCollection) newCollection = currentCollection->parent()->addFit(currentCollection->name());
		else if (currentCollection->type() == Collection::ExtractedCollection) newCollection = currentCollection->parent()->addSlice(currentCollection->name());
		else return;
	}
	else newCollection = UChromaSession::addCollection(currentCollection->name());

	// Grab name before we copy, since this will be overwritten...
	QString name = newCollection->name();
	(*newCollection) = (*currentCollection);
	newCollection->setName(name);

	// Create editstate
	UChromaSession::beginEditStateGroup("duplicate collection '%s'", qPrintable(currentCollection->name()));
	if (UChromaSession::addEditState(newCollection->objectInfo(), EditState::CollectionAddQuantity))
	{
		UChromaSession::addEditStateData(true, "collection", newCollection);
		UChromaSession::addEditStateData(true, "locator", newCollection->locator());
		UChromaSession::addEditStateData(true, "position", newCollection->listIndex());
	}
	UChromaSession::endEditStateGroup();

	updateGUI();
}

void UChromaWindow::on_actionCollectionPromoteToMaster_triggered(bool checked)
{
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(currentCollection, "current collection in UChromaWindow::on_actionCollectionPromoteToMaster_triggered")) return;

	if (currentCollection->type() == Collection::MasterCollection) return;

	Collection* newCollection = UChromaSession::addCollection(currentCollection->name());

	// Grab name before we copy, since this will be overwritten...
	QString name = newCollection->name();
	(*newCollection) = (*currentCollection);
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
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in UChromaWindow::on_actionCollectionDelete_triggered()")) return;

	// Start edit state group
	UChromaSession::beginEditStateGroup("delete collection '%s'", qPrintable(currentCollection->name()));

	// Remove specified collection
	UChromaSession::removeCollection(currentCollection);

	// Add new empty collection if there is now no current collection
	if (UChromaSession::currentCollection() == NULL) UChromaSession::addCollection();

	// End edit state group
	UChromaSession::endEditStateGroup();

	// Update GUI
	updateGUI();
}

void UChromaWindow::on_actionCollectionExport_triggered(bool checked)
{
	// Check current Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in UChromaWindow::on_actionCollectionDelete_triggered()")) return;

	static QDir currentDirectory = UChromaSession::sessionFileDirectory();

	// Get a filename from the user
	QString fileName = QFileDialog::getSaveFileName(this, "Choose export file name", currentDirectory.absolutePath(), "All files (*.*)");
	if (fileName.isEmpty()) return;

	currentCollection->exportData(fileName);

	currentDirectory = fileName;
}

/*
 * Data Actions
 */

void UChromaWindow::on_actionDataLoadXY_triggered(bool checked)
{
	// Check current Collection
	if (!Collection::objectValid(UChromaSession::currentCollection(), "collection in UChromaWindow::on_actionDataLoadXY_triggered()")) return;

	dataWindow_.ui.AddFilesButton->click();
}

void UChromaWindow::on_actionDataImport_triggered(bool checked)
{
	// Raise the Data Import dialog
	bool result = importDialog_.import();
	if (!result) return;

	// Create new Collection if requested
	Collection* currentCollection = NULL;
	if (importDialog_.createNewCollection())
	{
		// Start edit state group
		UChromaSession::beginEditStateGroup("import data into new collection");

		currentCollection = UChromaSession::addCollection();

		// No need to capture additional undo information
		UChromaSession::endEditStateGroup();

		// Set name of collection
		currentCollection->setName(importDialog_.filename());
	}
	else currentCollection = UChromaSession::currentCollection();

	// Check current Collection
	
	if (!Collection::objectValid(currentCollection, "collection in UChromaWindow::on_actionDataImport_triggered()")) return;

	// Loop over list of imported slices and copy them to our local list
	for (DataSet* dataSet = importDialog_.importedSlices(); dataSet != NULL; dataSet = dataSet->next) currentCollection->addDataSet(dataSet);

	// Update everything
	updateGUI();
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
 * Operate Actions
 */

void UChromaWindow::on_actionOperateConstantBackgroundSubtraction_triggered(bool checked)
{
	// Check current Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in UChromaWindow::on_actionOperateConstantBackgroundSubtraction_triggered()")) return;

	OperateBGSubDialog bgSubDialog(*this);
	if (bgSubDialog.updateAndExec(currentCollection)) updateGUI();
}

void UChromaWindow::on_actionOperateSetZValues_triggered(bool checked)
{
	// Check current Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in UChromaWindow::on_actionOperateSetZValues_triggered()")) return;

	OperateSetZDialog setZDialog(this, currentCollection);
	if (setZDialog.exec()) updateGUI();
}

/*
 * Analyse Actions
 */

void UChromaWindow::on_actionAnalyseNewFit_triggered(bool checked)
{
	// Check current Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in UChromaWindow::on_actionAnalyseNewFit_triggered()")) return;

	// Add a new fit to the current collection
	Collection* newFit = currentCollection->addFit("New Fit");

	// Raise the EditFitSetup dialog
	editFitSetupDialog_.setFitKernel(newFit->fitKernel());
	if (editFitSetupDialog_.updateAndExec())
	{
		// Add new fit data to same pane as parent (if possible)
		RefList<ViewPane,bool> parentPanes = UChromaSession::viewLayout().panes(currentCollection, ViewPane::StandardRole);
		if (parentPanes.contains(UChromaSession::currentViewPane())) UChromaSession::currentViewPane()->addCollectionTarget(newFit);
		else if (parentPanes.nItems() != 0) parentPanes.first()->item->addCollectionTarget(newFit);

		// Set current collection to be the new fit
		UChromaSession::setCurrentCollection(newFit);
	}
	else currentCollection->removeFit(newFit);

	updateGUI();
}

void UChromaWindow::on_actionAnalyseEditFit_triggered(bool checked)
{
	// Check current Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in UChromaWindow::on_actionAnalyseEditFit_triggered()")) return;

	if (currentCollection->fitKernel())
	{
		editFitSetupDialog_.setFitKernel(currentCollection->fitKernel());
		if (editFitSetupDialog_.updateAndExec()) updateGUI();
	}
	else
	{
		if (QMessageBox::question(this, "No Fit to Edit", "The current collection does not contain any fit parameters.\nWould you like to create a new fit?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) on_actionAnalyseNewFit_triggered(false);
	}
}

void UChromaWindow::on_actionAnalyseUpdateFit_triggered(bool checked)
{
	// Check current Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in UChromaWindow::on_actionAnalyseUpdateFit_triggered()")) return;

	if (currentCollection->fitKernel())
	{
		if (currentCollection->fitKernel()->fit()) updateGUI();
	}
	else
	{
		QString message;
		message.sprintf("Error: Current collection '%s' has no associated fit data.\n", qPrintable(currentCollection->name()));
		msg.print(qPrintable(message));
		ui.StatusBar->showMessage(message, 3000);
	}
}

void UChromaWindow::on_actionAnalyseResetAndRestartFit_triggered(bool checked)
{
	// Check current Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in UChromaWindow::on_actionAnalyseResetAndRestartFit_triggered()")) return;

	if (currentCollection->fitKernel())
	{
		if (currentCollection->fitKernel()->fit(true)) updateGUI();
	}
	else
	{
		QString message;
		message.sprintf("Error: Current collection '%s' has no associated fit data.\n", qPrintable(currentCollection->name()));
		msg.print(qPrintable(message));
		ui.StatusBar->showMessage(message, 3000);
	}
}

void UChromaWindow::on_actionAnalyseViewFitResults_triggered(bool checked)
{
	// Check current Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in UChromaWindow::on_actionAnalyseResetAndRestartFit_triggered()")) return;

	if (currentCollection->fitKernel())
	{
		EditFitResultsDialog fitValuesDialog(this);
		fitValuesDialog.call(currentCollection);
	}
	else
	{
		QString message;
		message.sprintf("Error: Current collection '%s' has no associated fit data.\n", qPrintable(currentCollection->name()));
		msg.print(qPrintable(message));
		ui.StatusBar->showMessage(message, 3000);
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
	static QDir currentFontDirectory = UChromaSession::viewerFontFileName();
	QString newFont = QFileDialog::getOpenFileName(this, "Choose truetype font", currentFontDirectory.path(), "TrueType font files (*.ttf);;All files (*.*)");
	if (!newFont.isEmpty())
	{
		if (!QFile::exists(newFont)) QMessageBox::warning(this, "Font Error", "The specified font file '" + newFont + "' does not exist.");
		else if (!FontInstance::setup(newFont)) QMessageBox::warning(this, "Font Error", "Failed to create a font from the specified font file '" + newFont +"'.");
		UChromaSession::setViewerFontFileName(newFont);
		saveSettings();
	}
}

void UChromaWindow::on_actionSettingsResetFont_triggered(bool checked)
{
	UChromaSession::setViewerFontFileName(QString());
	FontInstance::setup(UChromaSession::viewerFontFileName());
	saveSettings();
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
