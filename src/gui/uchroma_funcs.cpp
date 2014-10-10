/*
	*** Main Window - Functions 
	*** src/gui/uchroma_funcs.cpp
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
#include "base/session.h"
#include "templates/reflist.h"
#include "templates/variantpointer.h"
#include "version.h"

// Constructor
UChromaWindow::UChromaWindow(QMainWindow *parent) : QMainWindow(parent),
	axesWindow_(*this), dataWindow_(*this), logWindow_(*this), styleWindow_(*this), transformWindow_(*this), viewWindow_(*this),
	createCollectionDialog_(*this), dataImportDialog_(*this), editFitSetupDialog_(*this), saveImageDialog_(*this),
	viewLayout_(*this),
	viewTypeActionGroup_(this)
{
	// Initialise the icon resource
	Q_INIT_RESOURCE(icons);

	// Call the main creation function
	ui.setupUi(this);

	// Set pointer in CurrentProject
	Session::setMainWindow(this);

	// Set variable defaults
	hardIOFail_ = false;
	inputFileDirectory_ = getenv("PWD");
#ifdef WIN32
	viewerFont_ = QDir::current().absoluteFilePath("bin/wright.ttf");
#elif __APPLE__
	viewerFont_ = ui.application()->applicationDirPath() + "/../SharedSupport/resources/wright.ttf");
#else
	viewerFont_ = QDir::current().absoluteFilePath("wright.ttf");
#endif
	imageExportFile_ = "image.png";
	imageExportWidth_ = 800;
	imageExportHeight_ = 600;
	imageExportMaintainAspect_ = true;
	imageExportFormat_ = Viewer::PNGFormat;

	refreshing_ = false;

	// Load settings...
	loadSettings();

	// Set UChroma pointers in widgets/dialogs where necessary
	ui.MainView->setUChroma(this);

	// Load font for viewer
	if (!QFile::exists(viewerFont_)) QMessageBox::warning(this, "Font Error", "The specified font file '" + viewerFont_ + "' does not exist.");
	else if (!FontInstance::setupFont(viewerFont_)) QMessageBox::warning(this, "Font Error", "Failed to create a font from the specified font file '" + viewerFont_ +"'.");

	// Connect signals / slots between the Viewer and uChroma
	connect(ui.MainView, SIGNAL(renderComplete(QString)), this, SLOT(updateRenderTimeLabel(QString)));

	// Hide LeftWidgets (Collection list etc.) initially
	ui.LeftWidgetsWidget->setVisible(false);

	// Connect CollectionTree context menu signal
	connect(ui.CollectionTree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(collectionTreeContextMenuRequested(QPoint)));

	// Connect sub-window closed signal to toggle buttons / menu items in uChroma's main window
	connect(&axesWindow_, SIGNAL(windowClosed(bool)), ui.actionWindowAxes, SLOT(setChecked(bool)));
	connect(&dataWindow_, SIGNAL(windowClosed(bool)), ui.actionWindowData, SLOT(setChecked(bool)));
	connect(&logWindow_, SIGNAL(windowClosed(bool)), ui.actionWindowLog, SLOT(setChecked(bool)));
	connect(&styleWindow_, SIGNAL(windowClosed(bool)), ui.actionWindowStyle, SLOT(setChecked(bool)));
	connect(&transformWindow_, SIGNAL(windowClosed(bool)), ui.actionWindowTransform, SLOT(setChecked(bool)));
	connect(&viewWindow_, SIGNAL(windowClosed(bool)), ui.actionWindowView, SLOT(setChecked(bool)));

	// Create necessary action groups
	// -- ViewType actions
	viewTypeActionGroup_.addAction(ui.actionViewNormal);
	viewTypeActionGroup_.addAction(ui.actionViewAutoStretched3D);
	viewTypeActionGroup_.addAction(ui.actionViewFlatXY);
	viewTypeActionGroup_.addAction(ui.actionViewFlatXZ);
	viewTypeActionGroup_.addAction(ui.actionViewFlatYZ);
	viewTypeActionGroup_.addAction(ui.actionViewLinked);
	// -- Interaction mode actions
	QActionGroup* actionGroup = new QActionGroup(this);
	actionGroup->addAction(ui.actionInteractNone);
	actionGroup->addAction(ui.actionInteractX);
	actionGroup->addAction(ui.actionInteractY);
	actionGroup->addAction(ui.actionInteractZ);

	// Add QLabel as a normal widget to the status bar
	statusBarInfoLabel_ = new QLabel(this);
	ui.StatusBar->addWidget(statusBarInfoLabel_);
	statusBarRenderingTimeLabel_ = new QLabel(this);
	ui.StatusBar->addPermanentWidget(statusBarRenderingTimeLabel_);	

	// Set initial interaction mode
	setInteractionMode(InteractionMode::ViewInteraction, -1);
}

// Destructor
UChromaWindow::~UChromaWindow()
{
	// Delete all collections explicitly, before the ViewLayout (and ViewPanes) are deleted
	collections_.clear();
}

/*
 * Window Functions
 */

// Return centre coordinate of main window
QPoint UChromaWindow::centrePos()
{
	QPoint centre = pos();
	centre += QPoint(width()/2, height()/2);
	return centre;
}

// Return QTextBrowser used in LogWindow
QTextBrowser* UChromaWindow::logWindowBrowser()
{
	return logWindow_.ui.LogBrowser;
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

/*
 * Update
 */

// Update all aspects of GUI
void UChromaWindow::updateGUI()
{
	refreshing_ = true;

	refreshCollections();
	
	updateToolBars();
	updateSubWindows();
	updateDisplay();

	refreshing_ = false;
}

// Update all subwindows
void UChromaWindow::updateSubWindows()
{
	// Make sure limits are up to date
	currentViewPane_->updateAxisLimits();

	axesWindow_.updateControls();
	dataWindow_.updateControls();
	styleWindow_.updateControls();
	transformWindow_.updateControls();
	viewWindow_.updateControls();

	// Set titles of each subwindow to reflect current collection and viewpane
	if (currentCollection_  != NULL)
	{
		dataWindow_.setWindowTitle("Data (" + currentCollection_->name() + ")");
		styleWindow_.setWindowTitle("Style (" + currentCollection_->name() + ")");
		transformWindow_.setWindowTitle("Transform (" + currentCollection_->name() + ")");
		viewWindow_.setWindowTitle("View (" + currentCollection_->name() + ")");
	}
	axesWindow_.setWindowTitle("Axes (" + currentViewPane_->name() + ")");
}

// Update tool bars
void UChromaWindow::updateToolBars()
{
	// Update controls related to current ViewPane
	if (ViewPane::objectValid(currentViewPane_, "view pane in UChromaWindow::updateToolBars()"))
	{
		ui.actionViewPerspective->setChecked(currentViewPane_->hasPerspective());
		QAction* action = viewTypeActionGroup_.actions().at(currentViewPane_->viewType());
		if (action) action->setChecked(true);
	}
}

// Update display
void UChromaWindow::updateDisplay()
{
	// Satisfy all registered collection changes first
	RefListItem<Collection,Collection::CollectionSignal>* ri = Collection::collectionSignals();
	while (ri)
	{
// 		printf("UChromaWindow::updateDisplay() : Collection %p (%s), signal = %i\n", ri->item, qPrintable(ri->item->name()), ri->data);

		// Pass this change to the viewLayout_...
		viewLayout_.processUpdate(ri->item, ri->data);

		// Have now dealt with this signal, so delete it and move on to the next
		ri = Collection::deleteCollectionSignal(ri);
	}

	ui.MainView->postRedisplay();
}

/*
 * View
 */

// Set whether axis text labels face the viewer automatically
void UChromaWindow::setLabelFaceViewer(bool b)
{
	labelFaceViewer_ = b;
}

// Return whether axis text labels face the viewer automatically
bool UChromaWindow::labelFaceViewer()
{
	return labelFaceViewer_;
}

// Set whether axis text labels are corrected for left-right / up readability
void UChromaWindow::setLabelCorrectOrientation(bool b)
{
	labelCorrectOrientation_ = b;
}

// Return whether axis text labels are corrected for left-right / up readability
bool UChromaWindow::labelCorrectOrientation()
{
	return labelCorrectOrientation_;
}