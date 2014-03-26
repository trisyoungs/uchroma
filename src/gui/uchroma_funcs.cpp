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
#include "templates/reflist.h"
#include "version.h"

// Constructor
UChromaWindow::UChromaWindow(QMainWindow *parent) : QMainWindow(parent), saveImageDialog_(this), dataImportDialog_(this), fitDialog_(this), sliceMonitorDialog_(this)
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
	clearData();
	sliceAxis_ = -1;
	refreshing_ = false;
	addCollection();

	// Load settings...
	loadSettings();

	// Set UChroma pointers in widgets where necessary
	ui.MainView->setUChroma(this);
	GraphWidget::setUChroma(this);

	// Load font for viewer
	ui.MainView->setupFont(viewerFont_);

	// Connect signals / slots between Viewer and main UI
	connect(ui.MainView, SIGNAL(sliceAxisClicked()), this, SLOT(addSurfaceSlice()));

	// Connect signals / slots between SliceMonitor and main UI
	connect(this, SIGNAL(sliceDataChanged()), sliceMonitorDialog_.ui.MonitorGraph, SLOT(staticDataChanged()));
}

// Destructor
UChromaWindow::~UChromaWindow()
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

/*
 * Update
 */

// Update all tabs
void UChromaWindow::updateAllTabs()
{
	updateCollectionTab();
	updateAxesTab();
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
	for (Collection* c = collections_.first(); c != NULL; c = c->next) c->calculateDataLimits();

	// Update GUI
	updateAxes();
	updateAllTabs();
	updateTitleBar();

	// Setup font
	ui.MainView->setupFont(viewerFont_);
}

// Update display data
void UChromaWindow::updateDisplayData()
{
	for (Collection* c = collections_.first(); c != NULL; c = c->next) c->updateSurfaceData(axisMin_, axisMax_, axisInverted_, axisLogarithmic_, axisStretch_);
}

// Update display
void UChromaWindow::updateDisplay()
{
	ui.MainView->postRedisplay();
}

/*
 * Axes
 */

// Return minimum limit for specified axis
double UChromaWindow::axisMin(int axis)
{
	return axisMin_[axis];
}

// Return maximum limit for specified axis
double UChromaWindow::axisMax(int axis)
{
	return axisMax_[axis];
}

// Return whether axis is logarithmic
bool UChromaWindow::axisLogarithmic(int axis)
{
	return axisLogarithmic_[axis];
}

// Return whether axis is inverted
bool UChromaWindow::axisInverted(int axis)
{
	return axisInverted_[axis];
}

// Return stretch factor for axis
double UChromaWindow::axisStretch(int axis)
{
	return axisStretch_[axis];
}

// Return whether specified axis is visible
bool UChromaWindow::axisVisible(int axis)
{
	return axisVisible_[axis];
}

// Return coordinate at minimum of specified axis
Vec3<double> UChromaWindow::axisCoordMin(int axis)
{
	return axisCoordMin_[axis];
}

// Return coordinate at maximum of specified axis
Vec3<double> UChromaWindow::axisCoordMax(int axis)
{
	return axisCoordMax_[axis];
}

// Return whether axis text labels face the viewer automatically
bool UChromaWindow::labelFaceViewer()
{
	return labelFaceViewer_;
}

// Return whether axis text labels are corrected for left-right / up readability
bool UChromaWindow::labelCorrectOrientation()
{
	return labelCorrectOrientation_;
}

// Return font scaling for axis value labels
double UChromaWindow::labelScale()
{
	return labelScale_;
}

// Return font scaling for titles
double UChromaWindow::titleScale()
{
	return titleScale_;
}

