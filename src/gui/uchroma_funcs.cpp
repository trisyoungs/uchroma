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
#include "templates/variantpointer.h"
#include "version.h"

// Constructor
UChromaWindow::UChromaWindow(QMainWindow *parent) : QMainWindow(parent), axesWindow_(*this), dataWindow_(*this), slicesWindow_(*this), sliceMonitorWindow_(*this), styleWindow_(*this), transformWindow_(*this), viewWindow_(*this), saveImageDialog_(this), createDialog_(this), dataImportDialog_(this), fitDialog_(this)
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

	// Set UChroma pointers in widgets/dialogs where necessary
	ui.MainView->setUChroma(this);
	GraphWidget::setUChroma(this);
	FitDialog::setUChroma(this);
	CreateDialog::setUChroma(this);

	// Load font for viewer
	ui.MainView->setupFont(viewerFont_);

	// Connect signals / slots between the Viewer and uChroma
	connect(ui.MainView, SIGNAL(sliceAxisClicked()), this, SLOT(addSurfaceSlice()));
	connect(ui.MainView, SIGNAL(renderComplete(QString)), this, SLOT(updateRenderTimeLabel(QString)));
	connect(ui.MainView, SIGNAL(surfacePrimitivesUpdated(bool)), this, SLOT(updateCollections(bool)));

	// Connect signals / slots between SliceMonitor and main UI
	connect(this, SIGNAL(sliceDataChanged()), sliceMonitorWindow_.ui.MonitorGraph, SLOT(staticDataChanged()));

	// Connect sub-window closed signal to toggle buttons / menu items in uChroma's main window
	connect(&axesWindow_, SIGNAL(windowClosed(bool)), ui.actionWindowsAxes, SLOT(setChecked(bool)));
	connect(&dataWindow_, SIGNAL(windowClosed(bool)), ui.actionWindowsData, SLOT(setChecked(bool)));
	connect(&styleWindow_, SIGNAL(windowClosed(bool)), ui.actionWindowsStyle, SLOT(setChecked(bool)));
	connect(&transformWindow_, SIGNAL(windowClosed(bool)), ui.actionWindowsTransform, SLOT(setChecked(bool)));
	connect(&viewWindow_, SIGNAL(windowClosed(bool)), ui.actionWindowsView, SLOT(setChecked(bool)));	
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

// Update all aspects of GUI
void UChromaWindow::updateGUI()
{
	refreshing_ = true;

	refreshCollections();
	
	updateSubWindows();
	updateTitleBar();
	updateDisplay();

	refreshing_ = false;
}

// Update all subwindows
void UChromaWindow::updateSubWindows()
{
	// Make sure limits are up to date
	updateAxisLimits();

	axesWindow_.updateControls();
	dataWindow_.updateControls();
	slicesWindow_.updateControls();
	sliceMonitorWindow_.updateControls();
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

// Update display data
void UChromaWindow::updateDisplayData()
{
	for (Collection* c = collections_.first(); c != NULL; c = c->next) c->updateDisplayData(axisMin_, axisMax_, axisInverted_, axisLogarithmic_, axisStretch_);
}

// Update display
void UChromaWindow::updateDisplay()
{
	ui.MainView->postRedisplay();
}

/*
 * Axes
 */

// Update axis limits, clamping if necessary
void UChromaWindow::updateAxisLimits()
{
	// Loop over axes
	for (int axis = 0; axis < 3; ++axis)
	{
		// Set allowable range to avoid negative numbers if axis is now logarithmic
		if (axisLogarithmic_[axis])
		{
			axisLimitMin_[axis] = transformedDataPositiveMinima()[axis];
			axisLimitMax_[axis] = transformedDataPositiveMaxima()[axis];
		}
		else
		{
			axisLimitMin_[axis] = transformedDataMinima()[axis];
			axisLimitMax_[axis] = transformedDataMaxima()[axis];
		}

		// Clamp current axis values if necessary
		if (axisMin_[axis] < axisLimitMin_[axis]) setAxisToLimit(axis, true);
		if (axisMax_[axis] > axisLimitMax_[axis]) setAxisToLimit(axis, false);

		// Clamp axis position point values if necessary
		for (int axis=0; axis < 3; ++axis)
		{
			if (axisPosition_[axis][(axis+1)%3] < axisLimitMin_[(axis+1)%3])
			{
				axisPosition_[axis].set((axis+1)%3, axisLimitMin_[(axis+1)%3]);
				regenerateAxes_ = true;
			}
			if (axisPosition_[axis][(axis+2)%3] < axisLimitMin_[(axis+2)%3])
			{
				axisPosition_[axis].set((axis+2)%3, axisLimitMin_[(axis+2)%3]);
				regenerateAxes_ = true;
			}
		}
	}
}

// Set minimum value for specified axis
void UChromaWindow::setAxisMin(int axis, double value)
{
	axisMin_[axis] = value;

	// Set modification flag, and indicate that all display primitives must be updated
	setAsModified();
	setRegeneratePrimitives();
}

// Return minimum value for specified axis
double UChromaWindow::axisMin(int axis)
{
	return axisMin_[axis];
}

// Set maximum value for specified axis
void UChromaWindow::setAxisMax(int axis, double value)
{
	axisMax_[axis] = value;

	// Set modification flag, and indicate that all display primitives must be updated
	setAsModified();
	setRegeneratePrimitives();
}

// Return maximum value for specified axis
double UChromaWindow::axisMax(int axis)
{
	return axisMax_[axis];
}

// Set axis to extreme limit
void UChromaWindow::setAxisToLimit(int axis, bool minLim)
{
	if (minLim) axisMin_[axis] = axisLimitMin_[axis];
	else axisMax_[axis] = axisLimitMax_[axis];

	// Set modification flag, and indicate that all display primitives must be updated
	setAsModified();
	setRegeneratePrimitives();
}

// Return axis minimum limit for specified axis
double UChromaWindow::axisLimitMin(int axis)
{
	return axisLimitMin_[axis];
}

// Return axis maximum limit for specified axis
double UChromaWindow::axisLimitMax(int axis)
{
	return axisLimitMax_[axis];
}

// Return coordinate at centre of current axes
Vec3<double> UChromaWindow::axesCoordCentre()
{
	return axesCentre_;
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

// Set whether axis is inverted
void UChromaWindow::setAxisInverted(int axis, bool b)
{
	axisInverted_[axis] = b;

	// Set modification flag, and indicate that all display primitives must be updated
	setAsModified();
	setRegeneratePrimitives();
}

// Return whether axis is inverted
bool UChromaWindow::axisInverted(int axis)
{
	return axisInverted_[axis];
}

// Set whether axis is logarithmic
void UChromaWindow::setAxisLogarithmic(int axis, bool b)
{
	axisLogarithmic_[axis] = b;

	// Update and clamp axis values according to data
	updateAxisLimits();

	// Set modification flag, and indicate that all display primitives must be updated
	setAsModified();
	setRegeneratePrimitives();
}

// Return whether axis is logarithmic
bool UChromaWindow::axisLogarithmic(int axis)
{
	return axisLogarithmic_[axis];
}

// Set whether axis is visible
void UChromaWindow::setAxisVisible(int axis, bool b)
{
	axisVisible_[axis] = b;

	// Set modification flag, and indicate that axes display primitives must be updated
	setAsModified();
	regenerateAxes_ = true;
}

// Return whether specified axis is visible
bool UChromaWindow::axisVisible(int axis)
{
	return axisVisible_[axis];
}

// Set stretch factor for axis
void UChromaWindow::setAxisStretch(int axis, double value)
{
	axisStretch_[axis] = value;

	// Set modification flag, and indicate that all display primitives must be updated
	setAsModified();
	setRegeneratePrimitives();
}

// Return stretch factor for axis
double UChromaWindow::axisStretch(int axis)
{
	return axisStretch_[axis];
}

// Set axis position (in real surface-space coordinates)
void UChromaWindow::setAxisPosition(int axis, int dir, double value)
{
	// Clamp range to limits
	if ((value >= axisLimitMin_[dir]) && (value <= axisLimitMax_[dir])) axisPosition_[axis].set(dir, value);

	// Set modification flag, and indicate that axes display primitives must be updated
	setAsModified();
	regenerateAxes_ = true;
}

// Set axis position to axis limit (in real surface-space coordinates)
void UChromaWindow::setAxisPositionToLimit(int axis, int dir, bool minLim)
{
	axisPosition_[axis].set(dir, minLim ? axisLimitMin_[dir] : axisLimitMax_[dir]);

	// Set modification flag, and indicate that axes display primitives must be updated
	setAsModified();
	regenerateAxes_ = true;
}

// Return axis position (in real surface-space coordinates)
Vec3<double> UChromaWindow::axisPosition(int axis)
{
	return axisPosition_[axis];
}

// Set axis tick direction
void UChromaWindow::setAxisTickDirection(int axis, int dir, double value)
{
	axisTickDirection_[axis].set(dir, value);

	// Set modification flag, and indicate that axes display primitives must be updated
	setAsModified();
	regenerateAxes_ = true;
}

// Return axis tick direction
Vec3<double> UChromaWindow::axisTickDirection(int axis)
{
	return axisTickDirection_[axis];
}

// Set position of first tick delta on axes
void UChromaWindow::setAxisFirstTick(int axis, double value)
{
	axisFirstTick_[axis] = value;

	// Set modification flag, and indicate that axes display primitives must be updated
	setAsModified();
	regenerateAxes_ = true;

}

// Return position of first tick delta on axes
double UChromaWindow::axisFirstTick(int axis)
{
	return axisFirstTick_[axis];
}

// Set tick delta for axes
void UChromaWindow::setAxisTickDelta(int axis, double value)
{
	axisTickDelta_[axis] = value;

	// Set modification flag, and indicate that axes display primitives must be updated
	setAsModified();
	regenerateAxes_ = true;
}

// Return tick delta for axes
double UChromaWindow::axisTickDelta(int axis)
{
	return axisTickDelta_[axis];
}

// Set whether to calculate ticks automatically
void UChromaWindow::setAxisAutoTicks(int axis, bool b)
{
	axisAutoTicks_[axis] = b;

	// Set modification flag, and indicate that axes display primitives must be updated
	setAsModified();
	regenerateAxes_ = true;
}

// Return whether to calculate ticks automatically
bool UChromaWindow::axisAutoTicks(int axis)
{
	return axisAutoTicks_[axis];
}

// Set number of minor ticks in major tick intervals
void UChromaWindow::setAxisMinorTicks(int axis, int value)
{
	axisMinorTicks_[axis] = value;

	// Set modification flag, and indicate that axes primitives must be updated
	setAsModified();
	regenerateAxes_ = true;
}

// Return number of minor ticks in major tick intervals
int UChromaWindow::axisMinorTicks(int axis)
{
	return axisMinorTicks_[axis];
}

// Set orientation of labels for specified axis
void UChromaWindow::setAxisLabelOrientation(int axis, int component, double value)
{
	axisLabelOrientation_[axis].set(component, value);

	// Set modification flag, and indicate that axes primitives must be updated
	setAsModified();
	regenerateAxes_ = true;
}

// Return orientation of labels for specified axis
Vec3<double> UChromaWindow::axisLabelOrientation(int axis)
{
	return axisLabelOrientation_[axis];
}

// Set title for specified axis
void UChromaWindow::setAxisTitle(int axis, QString title)
{
	axisTitle_[axis] = title;

	// Set modification flag, and indicate that axes primitives must be updated
	setAsModified();
	regenerateAxes_ = true;
}

// Return title for specified axis
QString UChromaWindow::axisTitle(int axis)
{
	return axisTitle_[axis];
}

// Set orientation of titles for specified axis
void UChromaWindow::setAxisTitleOrientation(int axis, int component, double value)
{
	axisTitleOrientation_[axis].set(component, value);

	// Set modification flag, and indicate that axes primitives must be updated
	setAsModified();
	regenerateAxes_ = true;
}

// Return orientation of titles for specified axis
Vec4<double> UChromaWindow::axisTitleOrientation(int axis)
{
	return axisTitleOrientation_[axis];
}

// Set axis title text anchor position for specified axis
void UChromaWindow::setAxisTitleAnchor(int axis, TextPrimitive::HorizontalAnchor anchor)
{
	axisTitleAnchor_[axis] = anchor;

	// Set modification flag, and indicate that axes primitives must be updated
	setAsModified();
	regenerateAxes_ = true;
}

// Return axis title text anchor position for specified axis
TextPrimitive::HorizontalAnchor UChromaWindow::axisTitleAnchor(int axis)
{
	return axisTitleAnchor_[axis];
}

/*
 * View
 */

// Set font scaling for axis value labels
void UChromaWindow::setLabelScale(double value)
{
	labelScale_ = value;

	regenerateAxes_ = true;
}

// Return font scaling for axis value labels
double UChromaWindow::labelScale()
{
	return labelScale_;
}

// Return font scaling for titles
void UChromaWindow::setTitleScale(double value)
{
	titleScale_ = value;

	regenerateAxes_ = true;
}

// Return font scaling for titles
double UChromaWindow::titleScale()
{
	return titleScale_;
}

// Set current bounding box type
void UChromaWindow::setBoundingBox(UChromaWindow::BoundingBox type)
{
	boundingBox_ = type;

	// Bounding boxes are generated as part of the axes primitives, so need to regenerate them
	regenerateAxes_ = true;
}

// Return current bounding box type
UChromaWindow::BoundingBox UChromaWindow::boundingBox()
{
	return boundingBox_;
}

// Set y intercept for plane bounding box
void UChromaWindow::setBoundingBoxPlaneY(double value)
{
	boundingBoxPlaneY_ = value;

	// Bounding boxes are generated as part of the axes primitives, so need to regenerate them
	regenerateAxes_ = true;
}

// Return y intercept for plane bounding box
double UChromaWindow::boundingBoxPlaneY()
{
	return boundingBoxPlaneY_;
}

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