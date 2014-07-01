/*
	*** uChroma Main Window
	*** src/gui/uchroma.h
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

#ifndef UCHROMA_MAINWINDOW_H
#define UCHROMA_MAINWINDOW_H

#include "gui/ui_uchroma.h"
#include "gui/axes.h"
#include "gui/data.h"
#include "gui/slices.h"
#include "gui/slicemonitor.h"
#include "gui/style.h"
#include "gui/transform.h"
#include "gui/view.h"
#include "gui/saveimage.h"
#include "gui/create.h"
#include "gui/dataimport.h"
#include "gui/fit.h"
#include "base/collection.h"
#include "base/transformer.h"
#include "base/lineparser.h"

// Forward Declarations
/* None */

class UChromaWindow : public QMainWindow
{
	// All Qt declarations must include this macro
	Q_OBJECT


	/*
	 * Window Functions
	 */
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;
	// Axes Window
	AxesWindow axesWindow_;
	// Data Window
	DataWindow dataWindow_;
	// Slice Monitor Window
	SliceMonitorWindow sliceMonitorWindow_;
	// Slices Window
	SlicesWindow slicesWindow_;
	// Style Window
	StyleWindow styleWindow_;
	// Transform Window
	TransformWindow transformWindow_;
	// View Window
	ViewWindow viewWindow_;
	// Save Image Dialog
	SaveImageDialog saveImageDialog_;
	// Data Import Dialog
	DataImportDialog dataImportDialog_;
	// Fit Window Dialog
	FitDialog fitDialog_;
	// Create Window Dialog
	CreateDialog createDialog_;

	public:
	// Constructor / Destructor
	UChromaWindow(QMainWindow *parent = 0);
	~UChromaWindow();
	// Main form declaration
	Ui::UChromaWindow ui;
	// Return centre coordinate of main window
	QPoint centrePos();

	protected:
	// Window close event
	void closeEvent(QCloseEvent *event);

	public:
	// Load program settings
	void loadSettings();
	// Save program settings
	void saveSettings();


	/*
	 * File Actions
	 */
	private slots:
	void on_actionFileNew_triggered(bool checked);
	void on_actionFileLoad_triggered(bool checked);
	void on_actionFileSave_triggered(bool checked);
	void on_actionFileSaveAs_triggered(bool checked);
	void on_actionFileSaveImage_triggered(bool checked);
	void on_actionFileQuit_triggered(bool checked);

	private:
	// Check for modified data before closing
	bool checkBeforeClose();


	/*
	 * View Actions
	 */
	private slots:
	void on_actionViewPerspective_triggered(bool checked);
	void on_actionViewReset_triggered(bool checked);
	void on_actionViewGraphMode_triggered(bool checked);


	/*
	 * Collections Actions
	 */
	private slots:
	void on_actionCollectionsNew_triggered(bool checked);
	void on_actionCollectionsCreate_triggered(bool checked);
	void on_actionCollectionsFocusNext_triggered(bool checked);
	void on_actionCollectionsFocusPrevious_triggered(bool checked);


	/*
	 * Data Actions
	 */
	private slots:
	void on_actionDataLoadXY_triggered(bool checked);
	void on_actionDataImport_triggered(bool checked);


	/*
	 * Tools Actions
	 */
	private slots:
	void on_actionToolsFitWindow_triggered(bool checked);


	/*
	 * Axes Actions
	 */
	private:
	// Change interaction axis target
	void interactionActionTriggered(int axis);

	private slots:
	void on_actionAxesShowAll_triggered(bool checked);
	void on_actionAxesInteractX_triggered(bool checked);
	void on_actionAxesInteractY_triggered(bool checked);
	void on_actionAxesInteractZ_triggered(bool checked);
	void on_actionAxesInteractNone_triggered(bool checked);


	/*
	 * Windows Actions
	 */
	private slots:
	void on_actionWindowsCollections_triggered(bool checked);
	void on_actionWindowsData_triggered(bool checked);
	void on_actionWindowsStyle_triggered(bool checked);
	void on_actionWindowsTransform_triggered(bool checked);
	void on_actionWindowsView_triggered(bool checked);
	void on_actionWindowsAxes_triggered(bool checked);
	void on_actionWindowsSlices_triggered(bool checked);
	void on_actionWindowsSliceMonitor_triggered(bool checked);


	/*
	 * Settings Menu
	 */
	private slots:
	void on_actionSettingsChooseFont_triggered(bool checked);


	/*
	 * Collections
	 */
	private slots:
	void on_CollectionTree_itemClicked(QTreeWidgetItem* item, int column);
	void on_CollectionTree_itemDoubleClicked(QTreeWidgetItem* item, int column);
	void on_CollectionTree_itemChanged(QTreeWidgetItem* item, int column);
	void on_CollectionAddButton_clicked(bool checked);
	void on_CollectionRemoveButton_clicked(bool checked);

	public slots:
	// Refresh collection list
	void refreshCollections();
	// Update displayed info for current collection
	void updateCollectionInfo();


	/*
	 * InfoBar
	 */
	public slots:
	// Update coordinate info
	void updateCoordinateInfo();
	// Update text of renderTimeLabel_ in statusbar
	void updateRenderTimeLabel(QString text);


	/*
	 * Update
	 */
	public slots:
	// Update all aspects of GUI
	void updateGUI();
	// Update all subwindows
	void updateSubWindows();
	// Update title bar
	void updateTitleBar();
	// Update display data (used for surfaces) in all collections
	void updateDisplayData();
	// Update main display
	void updateDisplay();


	/*
	 * Input File
	 */
	private:
	// Whether current data has been modified
	bool modified_;
	// Current input file directory
	QDir inputFileDirectory_;
	// Current input filename
	QString inputFile_;

	private:
	// Read AxisBlock keywords
	bool readAxisBlock(LineParser& parser, int axis);
	// Read CollectionBlock keywords
	bool readCollectionBlock(LineParser& parser, Collection* collection);
	// Read DataSetBlock keywords
	bool readDataSetBlock(LineParser& parser, DataSet* dataS, Collection* collection);
	// Read SettingsBlock keywords
	bool readSettingsBlock(LineParser& parser);
	// Read AxisBlock keywords
	bool readViewBlock(LineParser& parser);

	public:
	// Load input from file specified
	bool loadInputFile(QString fileName);
	// Save current input to file specified
	bool saveInputFile(QString fileName);
	// Flag data as (not) modified and update titlebar
	void setAsModified(bool isModified = true);


	/*
	 * Data
	 */
	private:
	// Current image export filename
	QString imageExportFile_;
	// Format for exported image
	Viewer::ImageFormat imageExportFormat_;
	// Size of exported image
	int imageExportWidth_, imageExportHeight_;
	// Whether to maintain current aspect ratio on image export
	bool imageExportMaintainAspect_;
	// Font file to use for viewer
	QString viewerFont_;
	// List of Collections
	List<Collection> collections_;
	// Currently-selected Collection
	Collection* currentCollection_;

	private:
	// Flag all primitive data for regeneration
	void setRegeneratePrimitives();
	// Recalculate tick deltas for specified axis
	void calculateTickDeltas(int axis);
	// Return absolute minimum transformed values over all collections
	Vec3<double> transformedDataMinima();
	// Return absolute maximum transformed values over all collections
	Vec3<double> transformedDataMaxima();
	// Return absolute minimum positive transformed values over all collections
	Vec3<double> transformedDataPositiveMinima();
	// Return absolute maximum positive transformed values over all collections
	Vec3<double> transformedDataPositiveMaxima();

	public:
	// Add new collection
	Collection* addCollection(QString title = QString());
	// Remove existing collection
	void removeCollection(Collection* collection);
	// Return first collection in list
	Collection* collections();
	// Return nth collection in list
	Collection* collection(int index);
	// Move collection focus to next in list
	void focusNextCollection();
	// Move collection focus to previous in list
	void focusPreviousCollection();
	// Return currently-selected Collection
	Collection* currentCollection();
	// Clear current data
	void clearData();
	// Set display limits to show all available data
	void showAllData();


	/*
	 * Axes
	 */
	private:
	// Data limits for surface generation
	Vec3<double> axisMin_, axisMax_;
	// Limiting values for axis limits (accounting for data, log scales etc.)
	Vec3<double> axisLimitMin_, axisLimitMax_;
	// Axis extreme coordinates
	Vec3<double> axisCoordMin_[3], axisCoordMax_[3];
	// Central coordinate of current axes
	Vec3<double> axesCentre_;
	// Whether to invert axes
	Vec3<bool> axisInverted_;
	// Whether axes should be plotted as logarithms
	Vec3<bool> axisLogarithmic_;
	// Axis visibility
	Vec3<bool> axisVisible_;
	// Stretch factors to apply to axes
	Vec3<double> axisStretch_;
	// Whether axis position is fractional or absolute
	Vec3<bool> axisPositionIsFractional_;
	// Axis position in real or surface-space coordinates
	Vec3<double> axisPositionReal_[3];
	// Axis position in fractional axis coordinates
	Vec3<double> axisPositionFractional_[3];
	// Axis tick direction
	Vec3<double> axisTickDirection_[3];
	// Position of first tick delta on axes
	Vec3<double> axisFirstTick_;
	// Tick delta for axes
	Vec3<double> axisTickDelta_;
	// Whether to calculate ticks automatically
	Vec3<bool> axisAutoTicks_;
	// Number of minor ticks in major tick intervals
	Vec3<int> axisMinorTicks_;
	// Orientation of axis labels (axial rot, in-plane rot, distance)
	Vec3<double> axisLabelOrientation_[3];
	// Axis titles
	QString axisTitle_[3];
	// Orientation of axis titles (axial rot, in-plane rot, distance, h-offset)
	Vec4<double> axisTitleOrientation_[3];
	// Axis title text anchor positions
	TextPrimitive::HorizontalAnchor axisTitleAnchor_[3];
	// Whether axes need to be regenerated
	bool regenerateAxes_;

	private:
	// Update axis limits, clamping if necessary
	void updateAxisLimits();

	public:
	// Set minimum value for specified axis
	void setAxisMin(int axis, double value);
	// Return minimum value for specified axis
	double axisMin(int axis);
	// Set maximum value for specified axis
	void setAxisMax(int axis, double value);
	// Return maximum value for specified axis
	double axisMax(int axis);
	// Set axis to extreme limit
	void setAxisToLimit(int axis, bool minLim);
	// Return axis minimum limit for specified axis
	double axisLimitMin(int axis);
	// Return axis maximum limit for specified axis
	double axisLimitMax(int axis);
	// Return coordinate at centre of current axes
	Vec3<double> axesCoordCentre();
	// Return coordinate at minimum of specified axis
	Vec3<double> axisCoordMin(int axis);
	// Return coordinate at maximum of specified axis
	Vec3<double> axisCoordMax(int axis);
	// Set whether axis is inverted
	void setAxisInverted(int axis, bool b);
	// Return whether axis is inverted
	bool axisInverted(int axis);
	// Set whether axis is logarithmic
	void setAxisLogarithmic(int axis, bool b);
	// Return whether axis is logarithmic
	bool axisLogarithmic(int axis);
	// Set whether axis is visible
	void setAxisVisible(int axis, bool b);
	// Return whether specified axis is visible
	bool axisVisible(int axis);
	// Set stretch factor for axis
	void setAxisStretch(int axis, double value);
	// Return stretch factor for axis
	double axisStretch(int axis);
	// Set fractional position flag for axis
	void setAxisPositionIsFractional(int axis, bool value);
	// Return fractional position flag for axis
	double axisPositionIsFractional(int axis);
	// Set axis position (in real surface-space coordinates)
	void setAxisPositionReal(int axis, int dir, double value);
	// Set axis position to axis limit (in real surface-space coordinates)
	void setAxisPositionRealToLimit(int axis, int dir, bool minLim);
	// Return axis position (in real surface-space coordinates)
	Vec3<double> axisPositionReal(int axis);
	// Set axis position (in fractional axis coordinates)
	void setAxisPositionFractional(int axis, int dir, double value);
	// Return axis position (in fractional axis coordinates)
	Vec3<double> axisPositionFractional(int axis);
	// Set axis tick direction
	void setAxisTickDirection(int axis, int dir, double value);
	// Return axis tick direction for specified axis
	Vec3<double> axisTickDirection(int axis);
	// Set position of first tick delta on specified axis
	void setAxisFirstTick(int axis, double value);
	// Return position of first tick delta on specified axis
	double axisFirstTick(int axis);
	// Set tick delta for specified axis
	void setAxisTickDelta(int axis, double value);
	// Return tick delta for specified axis
	double axisTickDelta(int axis);
	// Set whether to calculate ticks automatically for specified axis
	void setAxisAutoTicks(int axis, bool b);
	// Return whether to calculate ticks automatically for specified axis
	bool axisAutoTicks(int axis);
	// Set number of minor ticks in major tick intervals for specified axis
	void setAxisMinorTicks(int axis, int value);
	// Return number of minor ticks in major tick intervals for specified axis
	int axisMinorTicks(int axis);
	// Set orientation of labels for specified axis
	void setAxisLabelOrientation(int axis, int component, double value);
	// Return orientation of labels for specified axis
	Vec3<double> axisLabelOrientation(int axis);
	// Set title for specified axis
	void setAxisTitle(int axis, QString title);
	// Return title for specified axis
	QString axisTitle(int axis);
	// Set orientation of titles for specified axis
	void setAxisTitleOrientation(int axis, int component, double value);
	// Return orientation of titles for specified axis
	Vec4<double> axisTitleOrientation(int axis);
	// Set axis title text anchor position for specified axis
	void setAxisTitleAnchor(int axis, TextPrimitive::HorizontalAnchor anchor);
	// Return axis title text anchor position for specified axis
	TextPrimitive::HorizontalAnchor axisTitleAnchor(int axis);
	// Set whether axis text labels face the viewer automatically
	void setLabelFaceViewer(bool b);
	// Return whether axis text labels face the viewer automatically
	bool labelFaceViewer();
	// Set whether axis text labels are corrected for left-right / up readability
	void setLabelCorrectOrientation(bool b);
	// Return whether axis text labels are corrected for left-right / up readability
	bool labelCorrectOrientation();
	// Update axes primitives if necessary
	void updateAxesPrimitives();


	/*
	 * Selection / Interaction
	 */
	public:
	// Interaction Modes
	enum InteractionMode
	{
		NoInteraction,
		ZoomInteraction
	};

	private:
	// Current interaction mode
	InteractionMode interactionMode_;
	// Whether the user is currently interacting with the display
	bool interacting_;
	// Current axis target for interaction
	int interactionAxis_;
	// Starting (clicked) value when interacting
	double clickedInteractionValue_;
	// Current interaction position on axis
	double currentInteractionValue_;

	private:
	// Calculate selection axis coordinate from supplied screen coordinates
	double screenToAxis(int axis, int mouseX, int mouseY);
	// Return axis bin value of closest point to supplied value
	int closestBin(int axis, double value);

	public:
	// Set interaction mode
	void setInteractionMode(InteractionMode mode);
	// Return interaction mode
	InteractionMode interactionMode();
	// Return whether the user is currently interacting with the display
	bool interacting();
	// Set interaction axis
	void setInteractionAxis(int axis);
	// Return current axis target for interaction
	int interactionAxis();
	// Start interaction at the specified screen coordinates
	void startInteraction(int mouseX, int mouseY, Qt::KeyboardModifiers modifiers);
	// Update current interaction value
	void updateInteractionPosition(int mouseX, int mouseY);
	// End interaction at the specified screen coordinates
	void endInteraction(int mouseX, int mouseY);
	// Return clicked interaction value on axis
	double clickedInteractionValue();
	// Return current interaction value on axis
	double currentInteractionValue();
	// Return clicked interaction coordinate on axis
	double clickedInteractionCoordinate();
	// Return current interaction coordinate on axis
	double currentInteractionCoordinate();


	/*
	 * Slices
	 */
	private:
	// List of user-defined groups containing extracted slices
	List<ExtractedSliceGroup> extractedSliceGroups_;
	// Current slice data
	ExtractedSlice currentSlice_;

	private:
	// Add / retrieve group
	ExtractedSliceGroup* addOrRetrieveGroup(QString name);
	// Extract slice based on specified axis and bin
	void extractSlice(int axis, int bin);

	public:
	// Return first extracted slice group
	ExtractedSliceGroup* extractedSliceGroups();
	// Return current slice data
	ExtractedSlice* currentSlice();

	signals:
	// Slice data has changed
	void sliceDataChanged();

	public slots:
	// Add current slice data to extracted slices list
	void addSurfaceSlice();


	/*
	 * View
	 */
	public:
	// Available Bounding Boxes
	enum BoundingBox { NoBox, PlaneBox, CubeBox, nBoundingBoxes };

	private:
	// Font scaling for axis value labels
	double labelScale_;
	// Font scaling for titles
	double titleScale_;
	// Current bounding box type
	BoundingBox boundingBox_;
	// Y-intercept of XZ plane
	double boundingBoxPlaneY_;
	// Whether axis text labels face the viewer automatically
	bool labelFaceViewer_;
	// Whether axis text labels are corrected for left-right / up readability
	bool labelCorrectOrientation_;
	// Whether graph (2D) mode is enabled
	bool graphMode_;

	public:
	// Set font scaling for axis value labels
	void setLabelScale(double value);
	// Return font scaling for axis value labels
	double labelScale();
	// Return font scaling for titles
	void setTitleScale(double value);
	// Return font scaling for titles
	double titleScale();
	// Set current bounding box type
	void setBoundingBox(UChromaWindow::BoundingBox type);
	// Return current bounding box type
	UChromaWindow::BoundingBox boundingBox();
	// Set y intercept for plane bounding box
	void setBoundingBoxPlaneY(double value);
	// Return y intercept for plane bounding box
	double boundingBoxPlaneY();
};

#endif
