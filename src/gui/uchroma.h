/*
	*** uChroma Main Window
	*** src/gui/uchroma.h
	Copyright T. Youngs 2013

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
#include "gui/dataimport.h"
#include "gui/slicemonitor.h"
#include "base/slice.h"
#include "base/transformer.h"

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
	// Update all tabs
	void updateAllTabs();
	// Update title bar
	void updateTitleBar();
	// Update GUI after loading data
	void updateAfterLoad();


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
	// Settings Menu
	*/
	private slots:
	void on_actionSettingsChooseFont_triggered(bool checked);


	/*
	 * Left Tabs -- Source Data
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
	void updateSourceDataTab();


	/*
	 * Left Tabs -- Transform
	 */
	private:
	// Change functions
	bool transformEnabledChanged(int axis, bool enabled);
	bool transformEquationChanged(int axis, QString equation);
	bool transformLimitChanged(int axis, bool minLim, double value);
	bool transformLimitSetExtreme(int axis, bool minLim);
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
	void on_LimitXMinSpin_valueChanged(double value);
	void on_LimitYMinSpin_valueChanged(double value);
	void on_LimitZMinSpin_valueChanged(double value);
	void on_LimitXMinSetMinimumButton_clicked(bool checked);
	void on_LimitYMinSetMinimumButton_clicked(bool checked);
	void on_LimitZMinSetMinimumButton_clicked(bool checked);
	void on_LimitXMaxSpin_valueChanged(double value);
	void on_LimitYMaxSpin_valueChanged(double value);
	void on_LimitZMaxSpin_valueChanged(double value);
	void on_LimitXMaxSetMaximumButton_clicked(bool checked);
	void on_LimitYMaxSetMaximumButton_clicked(bool checked);
	void on_LimitZMaxSetMaximumButton_clicked(bool checked);
	void on_TransformXInterpolateCheck_clicked(bool checked);
	void on_TransformXInterpolateStepSpin_valueChanged(double value);
	void on_TransformXInterpolateConstrainCheck_clicked(bool checked);
	void on_TransformZInterpolateCheck_clicked(bool checked);
	void on_TransformZInterpolateStepSpin_valueChanged(double value);
	void on_TransformZInterpolateConstrainCheck_clicked(bool checked);


	public:
	// Update Transform tab
	void updateTransformTab();


	/*
	 * Left Tabs -- Colour
	 */
	private:
	// Update Gradient Bar
	void updateGradientBar();

	private slots:
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
	void updateColourTab();


	/*
	 * Left Tabs -- View
	 */
	private:
	bool viewAxisInvertChanged(int axis, bool checked);
	bool viewAxisLogarithmicChanged(int axis, bool checked);
	bool viewAxisVisibleChanged(int axis, bool checked);
	bool viewAxisStretchChanged(int axis, double value);
	bool viewAxisCrossChanged(int axis, int dir, double value);
	bool viewAxisCrossSet(int axis, int dir, int type);
	bool viewAxisTicksChanged(int axis, bool start, double value);
	bool viewAxisTickOrientationChanged(int axis, int dir, double value);
	bool viewAxisLabelOrientationChanged(int axis, int component, double value);
	bool viewAxisMinorTicksChanged(int axis, int value);
	bool viewAxisTitleChanged(int axis, QString& title);
	bool viewAxisTitleOrientationChanged(int axis, int component, double value);
	bool viewAxisTitleAlignmentChanged(int axis, TextPrimitive::HorizontalAnchor anchor);

	private slots:
	// -- X Axis Tab
	void on_ViewXAxisInvertCheck_clicked(bool checked);
	void on_ViewXAxisLogarithmicCheck_clicked(bool checked);
	void on_ViewXAxisVisibleCheck_clicked(bool checked);
	void on_ViewXAxisTitleEdit_textChanged(QString text);
	void on_ViewXAxisStretchSpin_valueChanged(double value);
	void on_ViewXAxisCrossAtYSpin_valueChanged(double value);
	void on_ViewXAxisCrossAtZSpin_valueChanged(double value);
	void on_ViewXAxisCrossAtYSetMinimumButton_clicked(bool checked);
	void on_ViewXAxisCrossAtYSetZeroButton_clicked(bool checked);
	void on_ViewXAxisCrossAtYSetMaximumButton_clicked(bool checked);
	void on_ViewXAxisCrossAtZSetMinimumButton_clicked(bool checked);
	void on_ViewXAxisCrossAtZSetZeroButton_clicked(bool checked);
	void on_ViewXAxisCrossAtZSetMaximumButton_clicked(bool checked);
	void on_ViewXAxisAutoTicksCheck_clicked(bool checked);
	void on_ViewXAxisTicksStartSpin_valueChanged(double value);
	void on_ViewXAxisTicksDeltaSpin_valueChanged(double value);
	void on_ViewXAxisMinorTicksSpin_valueChanged(int value);
	void on_ViewXAxisTickDirectionXSpin_valueChanged(double value);
	void on_ViewXAxisTickDirectionYSpin_valueChanged(double value);
	void on_ViewXAxisTickDirectionZSpin_valueChanged(double value);
	void on_ViewXAxisLabelAxialRotationSlider_valueChanged(int value);
	void on_ViewXAxisLabelAxialRotationSpin_valueChanged(int value);
	void on_ViewXAxisLabelInPlaneRotationSlider_valueChanged(int value);
	void on_ViewXAxisLabelInPlaneRotationSpin_valueChanged(int value);
	void on_ViewXAxisLabelDistanceSpin_valueChanged(double value);
	void on_ViewXAxisTitleHOffsetSlider_valueChanged(int value);
	void on_ViewXAxisTitleHOffsetLeftButton_clicked(bool checked);
	void on_ViewXAxisTitleHOffsetCentreButton_clicked(bool checked);
	void on_ViewXAxisTitleHOffsetRightButton_clicked(bool checked);
	void on_ViewXAxisTitleAnchorCombo_currentIndexChanged(int index);
	void on_ViewXAxisTitleAxialRotationSlider_valueChanged(int value);
	void on_ViewXAxisTitleAxialRotationSpin_valueChanged(int value);
	void on_ViewXAxisTitleInPlaneRotationSlider_valueChanged(int value);
	void on_ViewXAxisTitleInPlaneRotationSpin_valueChanged(int value);
	void on_ViewXAxisTitleDistanceSpin_valueChanged(double value);
	// -- Y Axis Tab
	void on_ViewYAxisInvertCheck_clicked(bool checked);
	void on_ViewYAxisLogarithmicCheck_clicked(bool checked);
	void on_ViewYAxisVisibleCheck_clicked(bool checked);
	void on_ViewYAxisTitleEdit_textChanged(QString text);
	void on_ViewYAxisStretchSpin_valueChanged(double value);
	void on_ViewYAxisCrossAtXSpin_valueChanged(double value);
	void on_ViewYAxisCrossAtZSpin_valueChanged(double value);
	void on_ViewYAxisCrossAtXSetMinimumButton_clicked(bool checked);
	void on_ViewYAxisCrossAtXSetZeroButton_clicked(bool checked);
	void on_ViewYAxisCrossAtXSetMaximumButton_clicked(bool checked);
	void on_ViewYAxisCrossAtZSetMinimumButton_clicked(bool checked);
	void on_ViewYAxisCrossAtZSetZeroButton_clicked(bool checked);
	void on_ViewYAxisCrossAtZSetMaximumButton_clicked(bool checked);
	void on_ViewYAxisAutoTicksCheck_clicked(bool checked);
	void on_ViewYAxisTicksStartSpin_valueChanged(double value);
	void on_ViewYAxisTicksDeltaSpin_valueChanged(double value);
	void on_ViewYAxisMinorTicksSpin_valueChanged(int value);
	void on_ViewYAxisTickDirectionXSpin_valueChanged(double value);
	void on_ViewYAxisTickDirectionYSpin_valueChanged(double value);
	void on_ViewYAxisTickDirectionZSpin_valueChanged(double value);
	void on_ViewYAxisLabelAxialRotationSlider_valueChanged(int value);
	void on_ViewYAxisLabelAxialRotationSpin_valueChanged(int value);
	void on_ViewYAxisLabelInPlaneRotationSlider_valueChanged(int value);
	void on_ViewYAxisLabelInPlaneRotationSpin_valueChanged(int value);
	void on_ViewYAxisLabelDistanceSpin_valueChanged(double value);
	void on_ViewYAxisTitleHOffsetSlider_valueChanged(int value);
	void on_ViewYAxisTitleHOffsetLeftButton_clicked(bool checked);
	void on_ViewYAxisTitleHOffsetCentreButton_clicked(bool checked);
	void on_ViewYAxisTitleHOffsetRightButton_clicked(bool checked);
	void on_ViewYAxisTitleAnchorCombo_currentIndexChanged(int index);
	void on_ViewYAxisTitleAxialRotationSlider_valueChanged(int value);
	void on_ViewYAxisTitleAxialRotationSpin_valueChanged(int value);
	void on_ViewYAxisTitleInPlaneRotationSlider_valueChanged(int value);
	void on_ViewYAxisTitleInPlaneRotationSpin_valueChanged(int value);
	void on_ViewYAxisTitleDistanceSpin_valueChanged(double value);
	// -- Z Axis Tab
	void on_ViewZAxisInvertCheck_clicked(bool checked);
	void on_ViewZAxisLogarithmicCheck_clicked(bool checked);
	void on_ViewZAxisVisibleCheck_clicked(bool checked);
	void on_ViewZAxisTitleEdit_textChanged(QString text);
	void on_ViewZAxisStretchSpin_valueChanged(double value);
	void on_ViewZAxisCrossAtXSpin_valueChanged(double value);
	void on_ViewZAxisCrossAtYSpin_valueChanged(double value);
	void on_ViewZAxisCrossAtXSetMinimumButton_clicked(bool checked);
	void on_ViewZAxisCrossAtXSetZeroButton_clicked(bool checked);
	void on_ViewZAxisCrossAtXSetMaximumButton_clicked(bool checked);
	void on_ViewZAxisCrossAtYSetMinimumButton_clicked(bool checked);
	void on_ViewZAxisCrossAtYSetZeroButton_clicked(bool checked);
	void on_ViewZAxisCrossAtYSetMaximumButton_clicked(bool checked);
	void on_ViewZAxisAutoTicksCheck_clicked(bool checked);
	void on_ViewZAxisTicksStartSpin_valueChanged(double value);
	void on_ViewZAxisTicksDeltaSpin_valueChanged(double value);
	void on_ViewZAxisMinorTicksSpin_valueChanged(int value);
	void on_ViewZAxisTickDirectionXSpin_valueChanged(double value);
	void on_ViewZAxisTickDirectionYSpin_valueChanged(double value);
	void on_ViewZAxisTickDirectionZSpin_valueChanged(double value);
	void on_ViewZAxisLabelAxialRotationSlider_valueChanged(int value);
	void on_ViewZAxisLabelAxialRotationSpin_valueChanged(int value);
	void on_ViewZAxisLabelInPlaneRotationSlider_valueChanged(int value);
	void on_ViewZAxisLabelInPlaneRotationSpin_valueChanged(int value);
	void on_ViewZAxisLabelDistanceSpin_valueChanged(double value);
	void on_ViewZAxisTitleHOffsetSlider_valueChanged(int value);
	void on_ViewZAxisTitleHOffsetLeftButton_clicked(bool checked);
	void on_ViewZAxisTitleHOffsetCentreButton_clicked(bool checked);
	void on_ViewZAxisTitleHOffsetRightButton_clicked(bool checked);
	void on_ViewZAxisTitleAnchorCombo_currentIndexChanged(int index);
	void on_ViewZAxisTitleAxialRotationSlider_valueChanged(int value);
	void on_ViewZAxisTitleAxialRotationSpin_valueChanged(int value);
	void on_ViewZAxisTitleInPlaneRotationSlider_valueChanged(int value);
	void on_ViewZAxisTitleInPlaneRotationSpin_valueChanged(int value);
	void on_ViewZAxisTitleDistanceSpin_valueChanged(double value);
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
	// Update View tab
	void updateViewTab();


	/*
	 * Right Tabs -- Surface
	 */
	private slots:
	void on_SurfaceSliceNoneRadio_clicked(bool checked);
	void on_SurfaceSliceXRadio_clicked(bool checked);
	void on_SurfaceSliceYRadio_clicked(bool checked);
	void on_SurfaceSliceZRadio_clicked(bool checked);
	void on_SurfaceSliceMonitorCheck_clicked(bool checked);

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
	 * Data
	 */
	public:
	// Datafile keywords
	enum DataFileKeyword {
		AxisAutoTicksKeyword, AxisFirstTickKeyword, AxisInvertKeyword, AxisLabelOrientationKeyword, AxisLogarithmicKeyword, AxisMinorTicksKeyword, AxisPositionKeyword, AxisStretchKeyword, AxisTickDeltaKeyword, AxisTickDirectionKeyword, AxisTitleKeyword, AxisTitleAnchorKeyword, AxisTitleOrientationKeyword, AxisVisibleKeyword,
		BoundingBoxKeyword, BoundingBoxPlaneYKeyword,
		ColourAlphaControlKeyword, ColourAlphaFixedKeyword, ColourCustomGradientKeyword, ColourRGBGradientAKeyword, ColourRGBGradientBKeyword, ColourHSVGradientAKeyword, ColourHSVGradientBKeyword, ColourSingleKeyword, ColourSourceKeyword,
		DataKeyword,
		ImageExportKeyword, InterpolateKeyword, InterpolateConstrainKeyword, InterpolateStepKeyword,
		LabelFaceViewerKeyword, LabelScaleKeyword, LimitXKeyword, LimitYKeyword, LimitZKeyword,
		PerspectiveKeyword,
		SliceDirectoryKeyword, SliceKeyword,
		TitleScaleKeyword, TransformXKeyword, TransformYKeyword, TransformZKeyword,
		ViewMatrixXKeyword, ViewMatrixYKeyword, ViewMatrixZKeyword, ViewMatrixWKeyword,
		nDataFileKeywords };
	static DataFileKeyword dataFileKeyword(const char* s);
	static const char* dataFileKeyword(DataFileKeyword dfk);

	private:
	// Whether current data has been modified
	bool modified_;
	// Current input filename
	QString inputFile_;
	// Root directory for datafiles
	QDir dataFileDirectory_;
	// Current image export filename
	QString imageExportFile_;
	// Format for exported image
	Viewer::ImageFormat imageExportFormat_;
	// Size of exported image
	int imageExportWidth_, imageExportHeight_;
	// Whether to maintain current aspect ratio on image export
	bool imageExportMaintainAspect_;
	// List of slices
	List<Slice> slices_;
	// List of data slices todisplay
	List<Data2D> surfaceData_;
	
	// Font file to use for viewer
	QString viewerFont_;

	private:
	// Recalculate tick deltas for specified axis
	void calculateTickDeltas(int axis);

	public:
	// Clear current data
	void clearData();
	// Load data from file specified
	bool loadData(QString fileName);
	// Save current data to file specified
	bool saveData(QString fileName);
	// Add slice
	Slice* addSlice(double z, QString fileName, QString title);
	// Find slice with corresponding title
	Slice* findSlice(QString dataName);
	// Return number of slices with no data present
	int nEmptySlices();
	// Recalculate data limits
	void calculateDataLimits();
	// Update data transforms and calculate transform limits
	void updateDataTransforms();
	// Set display limits to show all available data
	void showAllData();
	// Flag data as modified, and update titlebar
	void setAsModified();


	/*
	 * Transform
	 */
	private:
	// Extreme values of raw data
	Vec3<double> dataMin_, dataMax_;
	// Extreme values of transformed data 
	Vec3<double> transformMin_, transformMax_;
	// Data limits for surface generation
	Vec3<double> limitMin_, limitMax_;
	// Transform for data
	Transformer transforms_[3];
	// Pre-transform shift value
	Vec3<double> preTransformShift_;
	// Post-transform shift value
	Vec3<double> postTransformShift_;
	// Interpolation flags
	Vec3<bool> interpolate_, interpolateConstrained_;
	// Interpolation step sizes
	Vec3<double> interpolationStep_;

	public:
	// Set limits to show all data
	void showAll(bool changeX = true, bool changeY = true, bool changeZ = true);
	// Return minimum limit for specified axis
	double limitMin(int axis);
	// Return maximum limit for specified axis
	double limitMax(int axis);


	/*
	 * Axes
	 */
	private:
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

	public:
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


	/*
	 * Colours
	 */
	public:
	// Available colourscale sources
	enum ColourSource { SingleColourSource, RGBGradientSource, HSVGradientSource, CustomGradientSource, nColourSources };
	// Alpha control options
	enum AlphaControl { OwnAlpha, FixedAlpha, nAlphaControls };

	private:
	// Points for SingleColour, RGBGradient and HSVGradient sources
	ColourScalePoint colourSinglePoint_;
	ColourScalePoint colourRGBGradientAPoint_, colourRGBGradientBPoint_;
	ColourScalePoint colourHSVGradientAPoint_, colourHSVGradientBPoint_;
	// ColourScale used by surface
	ColourScale colourScale_;
	// Custom ColourScale source
	ColourScale customColourScale_;
	// Current colourscale source to use
	ColourSource colourSource_;
	// Current alpha control
	AlphaControl alphaControl_;
	// Fixed alpha value (for FixedAlpha option)
	int fixedAlpha_;

	public:
	// Update colour scale
	void updateColourScale();


	/*
	 * Surface
	 */
	private:
	// Central coordinate of surface
	Vec3<double> surfaceCentre_;

	public:
	// Update surface data after data change
	void updateSurface(bool dataHasChanged = true);
	// Return central coordinate of surface
	Vec3<double> surfaceCentre();


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
