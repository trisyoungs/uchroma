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
#include "templates/reflist.h"
#include "templates/variantpointer.h"
#include "version.h"

// Constructor
UChromaWindow::UChromaWindow(QMainWindow *parent) : QMainWindow(parent),
	axesWindow_(*this), createWindow_(*this), dataWindow_(*this), layoutWindow_(*this),
	logWindow_(*this), styleWindow_(*this), transformWindow_(*this), viewWindow_(*this),
	dataImportDialog_(*this), fitSetupDialog_(*this), saveImageDialog_(*this),
	viewLayout_(*this)
{
	// Initialise the icon resource
	Q_INIT_RESOURCE(icons);

	// Call the main creation function
	ui.setupUi(this);

	// Set variable defaults
	inputFileDirectory_ = getenv("PWD");
#ifdef WIN32
	viewerFont_ = QDir::current().absoluteFilePath("bin/wright.ttf");
#else
	viewerFont_ = QDir::current().absoluteFilePath("wright.ttf");
#endif
	imageExportFile_ = "image.png";
	imageExportWidth_ = 800;
	imageExportHeight_ = 600;
	imageExportMaintainAspect_ = true;
	imageExportFormat_ = Viewer::PNGFormat;

	// Clear data
	clearData(true);
	interacting_ = false;
	refreshing_ = false;

	// Set basic view layout, and give pointer to PaneOrganiser in LayoutWindow
	currentViewPane_ = viewLayout_.setDefault();
	layoutWindow_.ui.Organiser->setViewLayout(&viewLayout_);

	// Add an empty collection, and add it to the current view pane
	currentViewPane_->addCollection(addCollection());
	currentViewPane_->translateView(0.0, 0.0, -15.0);

	// Load settings...
	loadSettings();

	// Set UChroma pointers in widgets/dialogs where necessary
	ui.MainView->setUChroma(this);

	// Load font for viewer
	if (!QFile::exists(viewerFont_)) QMessageBox::warning(this, "Font Error", "The specified font file '" + viewerFont_ + "' does not exist.");
	else if (!FontInstance::setupFont(viewerFont_)) QMessageBox::warning(this, "Font Error", "Failed to create a font from the specified font file '" + viewerFont_ +"'.");

	// Connect signals / slots between the Viewer and uChroma
	connect(ui.MainView, SIGNAL(renderComplete(QString)), this, SLOT(updateRenderTimeLabel(QString)));
	connect(ui.MainView, SIGNAL(surfacePrimitivesUpdated()), this, SLOT(updateCollectionInfo()));

	// Set a static pointer to the main viewer in PrimitiveList
	PrimitiveList::setViewer(ui.MainView);

	// Connect CollectionTree context menu signal
	connect(ui.CollectionTree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(collectionTreeContextMenuRequested(QPoint)));

	// Connect LayoutWindow and PaneOrganiser update signals
	connect(layoutWindow_.ui.Organiser, SIGNAL(updateMainDisplay()), this, SLOT(updateDisplay()));
	connect(&layoutWindow_, SIGNAL(updateMainDisplay()), this, SLOT(updateDisplay()));

	// Connect sub-window closed signal to toggle buttons / menu items in uChroma's main window
	connect(&axesWindow_, SIGNAL(windowClosed(bool)), ui.actionViewAxes, SLOT(setChecked(bool)));
	connect(&createWindow_, SIGNAL(windowClosed(bool)), ui.actionCollectionCreate, SLOT(setChecked(bool)));
	connect(&dataWindow_, SIGNAL(windowClosed(bool)), ui.actionDataView, SLOT(setChecked(bool)));
	connect(&layoutWindow_, SIGNAL(windowClosed(bool)), ui.actionViewLayout, SLOT(setChecked(bool)));
	connect(&styleWindow_, SIGNAL(windowClosed(bool)), ui.actionCollectionStyle, SLOT(setChecked(bool)));
	connect(&transformWindow_, SIGNAL(windowClosed(bool)), ui.actionCollectionTransform, SLOT(setChecked(bool)));
	connect(&viewWindow_, SIGNAL(windowClosed(bool)), ui.actionWindowsView, SLOT(setChecked(bool)));

	// Create an action group for the axis interact buttons
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
	updateTitleBar();
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

	// Set titles of each subwindow to reflect current collection
	if (currentCollection_  != NULL)
	{
		dataWindow_.setWindowTitle("Data (" + currentCollection_->title() + ")");
		styleWindow_.setWindowTitle("Style (" + currentCollection_->title() + ")");
		transformWindow_.setWindowTitle("Transform (" + currentCollection_->title() + ")");
		viewWindow_.setWindowTitle("View (" + currentCollection_->title() + ")");
	}
}

// Update title bar
void UChromaWindow::updateTitleBar()
{
	if (modified_) setWindowTitle("uChroma v" + QString(UCHROMAREVISION) + " - " + inputFile_ + " (modified) ");
	else setWindowTitle("uChroma v" + QString(UCHROMAREVISION) + " - " + inputFile_);
}

// Update tool bars
void UChromaWindow::updateToolBars()
{
	// Toggle perspective and 2D based on current pane
	if (! currentViewPane_) return;

	ui.actionViewPerspective->setChecked(currentViewPane_->hasPerspective());
	ui.actionView2D->setChecked(currentViewPane_->twoDimensional());
}

// Update display
void UChromaWindow::updateDisplay()
{
	// Satisfy all registered collection changes first
	RefListItem<Collection,Collection::CollectionSignal>* ri = Collection::collectionSignals();
	while (ri)
	{
// 		printf("UChromaWindow::updateDisplay() : Collection %p (%s), signal = %i\n", ri->item, qPrintable(ri->item->title()), ri->data);

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