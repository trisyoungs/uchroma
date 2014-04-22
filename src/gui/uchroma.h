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
#include "gui/saveimage.h"
#include "gui/create.h"
#include "gui/dataimport.h"
#include "gui/fit.h"
#include "gui/slicemonitor.h"
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
	// Window Functions
	*/
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;
	// Save Image Dialog
	SaveImageDialog saveImageDialog_;
	// Data Import Dialog
	DataImportDialog dataImportDialog_;
	// Slice Monitor Dialog
	SliceMonitorDialog sliceMonitorDialog_;
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
	// Finalise widgets (things that we couldn't do in Qt Designer)
	void finaliseUi();

	protected:
	// Window close event
	void closeEvent(QCloseEvent *event);

	public:
	// Load program settings
	void loadSettings();
	// Save program settings
	void saveSettings();


	/*
	// File Menu
	*/
	private slots:
	void on_actionFileNew_triggered(bool checked);
	void on_actionFileLoad_triggered(bool checked);
	void on_actionFileSave_triggered(bool checked);
	void on_actionFileSaveAs_triggered(bool checked);
	void on_actionFileImportData_triggered(bool checked);
	void on_actionFileSaveImage_triggered(bool checked);
	void on_actionFileQuit_triggered(bool checked);

	private:
	// Check for modified data before closing
	bool checkBeforeClose();


	/*
	// View Menu
	*/
	private slots:
	void on_actionViewPerspective_triggered(bool checked);
	void on_actionViewReset_triggered(bool checked);


	/*
	// Tools Menu
	*/
	private slots:
	void on_actionToolsSliceMonitor_triggered(bool checked);
	void on_actionToolsFitWindow_triggered(bool checked);
	void on_actionToolsCreateData_triggered(bool checked);


	/*
	// Settings Menu
	*/
	private slots:
	void on_actionSettingsChooseFont_triggered(bool checked);


	/*
	 * Left Tabs -- Collections
	 */
	private slots:
	void on_CollectionList_currentRowChanged(int index);
	void on_CollectionList_itemClicked(QListWidgetItem* item);
	void on_CollectionList_itemChanged(QListWidgetItem* item);
	void on_CollectionAddButton_clicked(bool checked);
	void on_CollectionRemoveButton_clicked(bool checked);

	public:
	// Update collection tab
	void updateCollectionTab();


	/*
	 * Left Tabs -- Collection -- Source Data
	 */
	private slots:
	void on_SourceDirSelectButton_clicked(bool checked);
	void on_AddFilesButton_clicked(bool checked);
	void on_RemoveFilesButton_clicked(bool checked);
	void on_SourceFilesTable_itemSelectionChanged();
	void on_SourceFilesTable_cellChanged(int row, int column);
	void on_GetZFromTimeStampButton_clicked(bool checked);
	void on_ReloadFilesButton_clicked(bool checked);

	public:
	// Update source data tab
	void updateCollectionDataTab();


	/*
	 * Left Tabs -- Collection -- Transform
	 */
	private:
	// Change functions
	bool transformEnabledChanged(int axis, bool enabled);
	bool transformEquationChanged(int axis, QString equation);
	bool transformInterpolateChanged(int axis, bool checked);
	bool transformInterpolateStepChanged(int axis, double step);
	bool transformInterpolateConstrainChanged(int axis, bool checked);
	private slots:
	void on_TransformXCheck_clicked(bool checked);
	void on_TransformYCheck_clicked(bool checked);
	void on_TransformZCheck_clicked(bool checked);
	void on_TransformXEquationEdit_textEdited(QString text);
	void on_TransformYEquationEdit_textEdited(QString text);
	void on_TransformZEquationEdit_textEdited(QString text);
	void on_TransformXInterpolateCheck_clicked(bool checked);
	void on_TransformXInterpolateStepSpin_valueChanged(double value);
	void on_TransformXInterpolateConstrainCheck_clicked(bool checked);
	void on_TransformZInterpolateCheck_clicked(bool checked);
	void on_TransformZInterpolateStepSpin_valueChanged(double value);
	void on_TransformZInterpolateConstrainCheck_clicked(bool checked);

	public:
	// Update Transform tab
	void updateCollectionTransformTab();


	/*
	 * Left Tabs -- Collection -- Style
	 */
	private:
	// Update Gradient Bar
	void updateGradientBar();

	private slots:
	// -- Style
	void on_StyleCombo_currentIndexChanged(int index);
	// -- Single Colour
	void on_ColourSingleColourRadio_clicked(bool checked);
	void on_ColourSingleColourButton_clicked(bool checked);
	// -- RGB Gradient
	void on_ColourRGBGradientRadio_clicked(bool checked);
	void on_ColourRGBGradientAButton_clicked(bool checked);
	void on_ColourRGBGradientASpin_valueChanged(double value);
	void on_ColourRGBGradientASetMinimumButton_clicked(bool checked);
	void on_ColourRGBGradientASetMaximumButton_clicked(bool checked);
	void on_ColourRGBGradientBButton_clicked(bool checked);
	void on_ColourRGBGradientBSpin_valueChanged(double value);
	void on_ColourRGBGradientBSetMinimumButton_clicked(bool checked);
	void on_ColourRGBGradientBSetMaximumButton_clicked(bool checked);
	// -- HSV Gradient
	void on_ColourHSVGradientRadio_clicked(bool checked);
	void on_ColourHSVGradientAButton_clicked(bool checked);
	void on_ColourHSVGradientASpin_valueChanged(double value);
	void on_ColourHSVGradientASetMinimumButton_clicked(bool checked);
	void on_ColourHSVGradientASetMaximumButton_clicked(bool checked);
	void on_ColourHSVGradientBButton_clicked(bool checked);
	void on_ColourHSVGradientBSpin_valueChanged(double value);
	void on_ColourHSVGradientBSetMinimumButton_clicked(bool checked);
	void on_ColourHSVGradientBSetMaximumButton_clicked(bool checked);
	// -- Custom Gradient
	void on_ColourCustomGradientRadio_clicked(bool checked);
	void on_ColourCustomGradientTable_itemSelectionChanged();
	void on_ColourCustomGradientTable_cellDoubleClicked(int row, int column);
	void on_ColourCustomGradientTable_cellChanged(int row, int column);
	void on_ColourCustomGradientAddButton_clicked(bool checked);
	void on_ColourCustomGradientRemoveButton_clicked(bool checked);
	// -- Alpha Control
	void on_ColourAlphaOwnAlphaRadio_clicked(bool checked);
	void on_ColourAlphaFixedAlphaRadio_clicked(bool checked);
	void on_ColourAlphaFixedAlphaSpin_valueChanged(int value);

	public:
	// Update
	void updateCollectionColourTab();


	/*
	 * Left Tabs -- Axes
	 */
	private:
	bool axisInvertChanged(int axis, bool checked);
	bool axisLogarithmicChanged(int axis, bool checked);
	bool axisVisibleChanged(int axis, bool checked);
	bool axisStretchChanged(int axis, double value);
	bool axisLimitChanged(int axis, bool minLim, double value);
	bool axisLimitSetExtreme(int axis, bool minLim);
	bool axisCrossChanged(int axis, int dir, double value);
	bool axisCrossSet(int axis, int dir, int type);
	bool axisAutoTicksChanged(int axis, bool enabled);
	bool axisTicksChanged(int axis, bool start, double value);
	bool axisTickOrientationChanged(int axis, int dir, double value);
	bool axisLabelOrientationChanged(int axis, int component, double value);
	bool axisMinorTicksChanged(int axis, int value);
	bool axisTitleChanged(int axis, QString& title);
	bool axisTitleOrientationChanged(int axis, int component, double value);
	bool axisTitleAlignmentChanged(int axis, TextPrimitive::HorizontalAnchor anchor);

	private slots:
	// -- X Axis Tab
	void on_AxisXInvertCheck_clicked(bool checked);
	void on_AxisXLogarithmicCheck_clicked(bool checked);
	void on_AxisXVisibleCheck_clicked(bool checked);
	void on_AxisXTitleEdit_textChanged(QString text);
	void on_AxisXStretchSpin_valueChanged(double value);
	void on_AxisXMinSpin_valueChanged(double value);
	void on_AxisYMinSpin_valueChanged(double value);
	void on_AxisZMinSpin_valueChanged(double value);
	void on_AxisXMinSetMinimumButton_clicked(bool checked);
	void on_AxisYMinSetMinimumButton_clicked(bool checked);
	void on_AxisZMinSetMinimumButton_clicked(bool checked);
	void on_AxisXMaxSpin_valueChanged(double value);
	void on_AxisYMaxSpin_valueChanged(double value);
	void on_AxisZMaxSpin_valueChanged(double value);
	void on_AxisXMaxSetMaximumButton_clicked(bool checked);
	void on_AxisYMaxSetMaximumButton_clicked(bool checked);
	void on_AxisZMaxSetMaximumButton_clicked(bool checked);
	void on_AxisXCrossAtYSpin_valueChanged(double value);
	void on_AxisXCrossAtZSpin_valueChanged(double value);
	void on_AxisXCrossAtYSetMinimumButton_clicked(bool checked);
	void on_AxisXCrossAtYSetZeroButton_clicked(bool checked);
	void on_AxisXCrossAtYSetMaximumButton_clicked(bool checked);
	void on_AxisXCrossAtZSetMinimumButton_clicked(bool checked);
	void on_AxisXCrossAtZSetZeroButton_clicked(bool checked);
	void on_AxisXCrossAtZSetMaximumButton_clicked(bool checked);
	void on_AxisXAutoTicksCheck_clicked(bool checked);
	void on_AxisXTicksStartSpin_valueChanged(double value);
	void on_AxisXTicksDeltaSpin_valueChanged(double value);
	void on_AxisXMinorTicksSpin_valueChanged(int value);
	void on_AxisXTickDirectionXSpin_valueChanged(double value);
	void on_AxisXTickDirectionYSpin_valueChanged(double value);
	void on_AxisXTickDirectionZSpin_valueChanged(double value);
	void on_AxisXLabelAxialRotationSlider_valueChanged(int value);
	void on_AxisXLabelAxialRotationSpin_valueChanged(int value);
	void on_AxisXLabelInPlaneRotationSlider_valueChanged(int value);
	void on_AxisXLabelInPlaneRotationSpin_valueChanged(int value);
	void on_AxisXLabelDistanceSpin_valueChanged(double value);
	void on_AxisXTitleHOffsetSlider_valueChanged(int value);
	void on_AxisXTitleHOffsetLeftButton_clicked(bool checked);
	void on_AxisXTitleHOffsetCentreButton_clicked(bool checked);
	void on_AxisXTitleHOffsetRightButton_clicked(bool checked);
	void on_AxisXTitleAnchorCombo_currentIndexChanged(int index);
	void on_AxisXTitleAxialRotationSlider_valueChanged(int value);
	void on_AxisXTitleAxialRotationSpin_valueChanged(int value);
	void on_AxisXTitleInPlaneRotationSlider_valueChanged(int value);
	void on_AxisXTitleInPlaneRotationSpin_valueChanged(int value);
	void on_AxisXTitleDistanceSpin_valueChanged(double value);
	// -- Y Axis Tab
	void on_AxisYInvertCheck_clicked(bool checked);
	void on_AxisYLogarithmicCheck_clicked(bool checked);
	void on_AxisYVisibleCheck_clicked(bool checked);
	void on_AxisYTitleEdit_textChanged(QString text);
	void on_AxisYStretchSpin_valueChanged(double value);
	void on_AxisYCrossAtXSpin_valueChanged(double value);
	void on_AxisYCrossAtZSpin_valueChanged(double value);
	void on_AxisYCrossAtXSetMinimumButton_clicked(bool checked);
	void on_AxisYCrossAtXSetZeroButton_clicked(bool checked);
	void on_AxisYCrossAtXSetMaximumButton_clicked(bool checked);
	void on_AxisYCrossAtZSetMinimumButton_clicked(bool checked);
	void on_AxisYCrossAtZSetZeroButton_clicked(bool checked);
	void on_AxisYCrossAtZSetMaximumButton_clicked(bool checked);
	void on_AxisYAutoTicksCheck_clicked(bool checked);
	void on_AxisYTicksStartSpin_valueChanged(double value);
	void on_AxisYTicksDeltaSpin_valueChanged(double value);
	void on_AxisYMinorTicksSpin_valueChanged(int value);
	void on_AxisYTickDirectionXSpin_valueChanged(double value);
	void on_AxisYTickDirectionYSpin_valueChanged(double value);
	void on_AxisYTickDirectionZSpin_valueChanged(double value);
	void on_AxisYLabelAxialRotationSlider_valueChanged(int value);
	void on_AxisYLabelAxialRotationSpin_valueChanged(int value);
	void on_AxisYLabelInPlaneRotationSlider_valueChanged(int value);
	void on_AxisYLabelInPlaneRotationSpin_valueChanged(int value);
	void on_AxisYLabelDistanceSpin_valueChanged(double value);
	void on_AxisYTitleHOffsetSlider_valueChanged(int value);
	void on_AxisYTitleHOffsetLeftButton_clicked(bool checked);
	void on_AxisYTitleHOffsetCentreButton_clicked(bool checked);
	void on_AxisYTitleHOffsetRightButton_clicked(bool checked);
	void on_AxisYTitleAnchorCombo_currentIndexChanged(int index);
	void on_AxisYTitleAxialRotationSlider_valueChanged(int value);
	void on_AxisYTitleAxialRotationSpin_valueChanged(int value);
	void on_AxisYTitleInPlaneRotationSlider_valueChanged(int value);
	void on_AxisYTitleInPlaneRotationSpin_valueChanged(int value);
	void on_AxisYTitleDistanceSpin_valueChanged(double value);
	// -- Z Axis Tab
	void on_AxisZInvertCheck_clicked(bool checked);
	void on_AxisZLogarithmicCheck_clicked(bool checked);
	void on_AxisZVisibleCheck_clicked(bool checked);
	void on_AxisZTitleEdit_textChanged(QString text);
	void on_AxisZStretchSpin_valueChanged(double value);
	void on_AxisZCrossAtXSpin_valueChanged(double value);
	void on_AxisZCrossAtYSpin_valueChanged(double value);
	void on_AxisZCrossAtXSetMinimumButton_clicked(bool checked);
	void on_AxisZCrossAtXSetZeroButton_clicked(bool checked);
	void on_AxisZCrossAtXSetMaximumButton_clicked(bool checked);
	void on_AxisZCrossAtYSetMinimumButton_clicked(bool checked);
	void on_AxisZCrossAtYSetZeroButton_clicked(bool checked);
	void on_AxisZCrossAtYSetMaximumButton_clicked(bool checked);
	void on_AxisZAutoTicksCheck_clicked(bool checked);
	void on_AxisZTicksStartSpin_valueChanged(double value);
	void on_AxisZTicksDeltaSpin_valueChanged(double value);
	void on_AxisZMinorTicksSpin_valueChanged(int value);
	void on_AxisZTickDirectionXSpin_valueChanged(double value);
	void on_AxisZTickDirectionYSpin_valueChanged(double value);
	void on_AxisZTickDirectionZSpin_valueChanged(double value);
	void on_AxisZLabelAxialRotationSlider_valueChanged(int value);
	void on_AxisZLabelAxialRotationSpin_valueChanged(int value);
	void on_AxisZLabelInPlaneRotationSlider_valueChanged(int value);
	void on_AxisZLabelInPlaneRotationSpin_valueChanged(int value);
	void on_AxisZLabelDistanceSpin_valueChanged(double value);
	void on_AxisZTitleHOffsetSlider_valueChanged(int value);
	void on_AxisZTitleHOffsetLeftButton_clicked(bool checked);
	void on_AxisZTitleHOffsetCentreButton_clicked(bool checked);
	void on_AxisZTitleHOffsetRightButton_clicked(bool checked);
	void on_AxisZTitleAnchorCombo_currentIndexChanged(int index);
	void on_AxisZTitleAxialRotationSlider_valueChanged(int value);
	void on_AxisZTitleAxialRotationSpin_valueChanged(int value);
	void on_AxisZTitleInPlaneRotationSlider_valueChanged(int value);
	void on_AxisZTitleInPlaneRotationSpin_valueChanged(int value);
	void on_AxisZTitleDistanceSpin_valueChanged(double value);
	// -- Extras Tab
	void on_ViewBoundingBoxNoneRadio_clicked(bool checked);
	void on_ViewBoundingBoxPlaneRadio_clicked(bool checked);
	void on_ViewBoundingBoxCubeRadio_clicked(bool checked);
	void on_ViewBoundingBoxPlaneYSpin_valueChanged(double value);
	void on_ViewBoundingBoxPlaneYSetMinimumButton_clicked(bool checked);
	void on_ViewBoundingBoxPlaneYSetZeroButton_clicked(bool checked);
	void on_ViewBoundingBoxPlaneYSetMaximumButton_clicked(bool checked);
	// -- General Options Group
	void on_ViewLabelsFaceViewerCheck_clicked(bool checked);
	void on_ViewLabelScaleSpin_valueChanged(double value);
	void on_ViewTitleScaleSpin_valueChanged(double value);

	public:
	// Update Axes tab
	void updateAxesTab();


	/*
	 * Right Tabs -- Surface
	 */
	private slots:
	void on_SurfaceSliceNoneRadio_clicked(bool checked);
	void on_SurfaceSliceXRadio_clicked(bool checked);
	void on_SurfaceSliceYRadio_clicked(bool checked);
	void on_SurfaceSliceZRadio_clicked(bool checked);

	public:
	// Update Surface tab (except main view)
	void updateSurfaceTab();


	/*
	 * Right Tabs -- Slices
	 */
	private slots:
	void on_SlicesClearButton_clicked(bool checked);
	void on_SlicesSaveButton_clicked(bool checked);
	void on_SlicesList_currentRowChanged(int index);

	public:
	// Update the Slices tab
	void updateSlicesTab();
	// Update the slice monitor widget
	void updateSliceMonitor();


	/*
	 * Update
	 */
	public:
	// Update all tabs
	void updateAllTabs();
	// Update title bar
	void updateTitleBar();
	// Update GUI after loading data
	void updateAfterLoad();
	// Update display data
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
	// Read SettingsBlock keywords
	bool readSettingsBlock(LineParser& parser);
	// Read SliceBlock keywords
	bool readSliceBlock(LineParser& parser, Slice* slice, Collection* collection);
	// Read AxisBlock keywords
	bool readViewBlock(LineParser& parser);

	public:
	// Load input from file specified
	bool loadInputFile(QString fileName);
	// Save current input to file specified
	bool saveInputFile(QString fileName);
	// Flag data as modified and update titlebar
	void setAsModified();


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
	// Flag all surface data for regeneration
	void regenerateAll();
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
	// Central coordinate of current axes
	Vec3<double> axesCentre_;
	// Whether to invert axes
	Vec3<bool> axisInverted_;
	// Whether axes should be plotted as logarithms
	Vec3<bool> axisLogarithmic_;
	// Axis visibility
	Vec3<bool> axisVisible_;
	// Axis position (in real surface-space coordinates)
	Vec3<double> axisPosition_[3];
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
	// Stretch factors to apply to axes
	Vec3<double> axisStretch_;
	// Axis extreme coordinates
	Vec3<double> axisCoordMin_[3], axisCoordMax_[3];
	// Whether axis text labels face the viewer automatically
	bool labelFaceViewer_;
	// Whether axis text labels are corrected for left-right / up readability
	bool labelCorrectOrientation_;
	// Font scaling for axis value labels
	double labelScale_;
	// Font scaling for titles
	double titleScale_;
	// Whether axes need to be regenerated
	bool regenerateAxes_;

	public:
	// Return minimum limit for specified axis
	double axisMin(int axis);
	// Return maximum limit for specified axis
	double axisMax(int axis);
	// Return central coordinate of current axes
	Vec3<double> axesCentre();
	// Return whether axis is inverted
	bool axisInverted(int axis);
	// Return whether axis is logarithmic
	bool axisLogarithmic(int axis);
	// Return whether specified axis is visible
	bool axisVisible(int axis);
	// Return stretch factor for axis
	double axisStretch(int axis);
	// Return coordinate at minimum of specified axis
	Vec3<double> axisCoordMin(int axis);
	// Return coordinate at maximum of specified axis
	Vec3<double> axisCoordMax(int axis);
	// Return whether axis text labels face the viewer automatically
	bool labelFaceViewer();
	// Return whether axis text labels are corrected for left-right / up readability
	bool labelCorrectOrientation();
	// Return font scaling for axis value labels
	double labelScale();
	// Return font scaling for titles
	double titleScale();
	// Update axes primitives if necessary
	void updateAxesPrimitives();


	/*
	 * Slices
	 */
	private:
	// List of user-defined groups containing extracted slices
	List<ExtractedSliceGroup> extractedSliceGroups_;
	// Current axis target for slice selection in Viewer
	int sliceAxis_;
	// Current value along axis in slice selection
	double sliceValue_;
	// Current slice data
	ExtractedSlice currentSlice_;

	private:
	// Add / retrieve group
	ExtractedSliceGroup* addOrRetrieveGroup(QString name);

	public:
	// Set slice axis
	void setSliceAxis(int axis);
	// Return current axis target for slice selection
	int sliceAxis();
	// Update slice axis position from specified screen coordinates
	bool updateSliceValue(int mouseX, int mouseY);
	// Return current slice value along axis
	double sliceValue();
	// Return current slice coordinate along axis
	double sliceCoordinate();
	// Return axis bin value of closest point to supplied value
	int closestBin(int axis, double value);
	// Return current slice data
	ExtractedSlice* currentSlice();

	public slots:
	// Add current slice data to extracted slices list
	void addSurfaceSlice();

	signals:
	// Slice data has changed
	void sliceDataChanged();


	/*
	 * Extras
	 */
	public:
	// Available Bounding Boxes
	enum BoundingBox { NoBox, PlaneBox, CubeBox, nBoundingBoxes };

	private:
	// Current bounding box type
	BoundingBox boundingBox_;
	// Y-intercept of XZ plane
	double boundingBoxPlaneY_;
};

#endif
