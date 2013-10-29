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
#include "base/slice.h"

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
	 * Tabs -- Source Data
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
	 * Tabs -- Transform
	 */
	private:
	// Change functions
	bool transformTypeChanged(int axis, int index);
	bool transformValueChanged(int axis, double value);
	bool transformShiftChanged(int axis, bool pre, double value);
	bool transformLimitChanged(int axis, bool minLim, double value);
	bool transformLimitSetExtreme(int axis, bool minLim);
	bool transformInterpolateChanged(int axis, bool checked);
	bool transformInterpolateStepChanged(int axis, double step);
	bool transformInterpolateConstrainChanged(int axis, bool checked);
	private slots:
	void on_TransformXTypeCombo_currentIndexChanged(int index);
	void on_TransformYTypeCombo_currentIndexChanged(int index);
	void on_TransformZTypeCombo_currentIndexChanged(int index);
	void on_TransformXValueSpin_valueChanged(double value);
	void on_TransformYValueSpin_valueChanged(double value);
	void on_TransformZValueSpin_valueChanged(double value);
	void on_TransformXPreShiftSpin_valueChanged(double value);
	void on_TransformYPreShiftSpin_valueChanged(double value);
	void on_TransformZPreShiftSpin_valueChanged(double value);
	void on_TransformXPostShiftSpin_valueChanged(double value);
	void on_TransformYPostShiftSpin_valueChanged(double value);
	void on_TransformZPostShiftSpin_valueChanged(double value);
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
	 * Tabs -- Colour
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
	 * Tabs -- View
	 */
	private:
	bool viewAxisInvertChanged(int axis, bool checked);
	bool viewAxisLogarithmicChanged(int axis, bool checked);
	bool viewAxisVisibleChanged(int axis, bool checked);
	bool viewAxisStretchChanged(int axis, double value);
	bool viewAxisCrossChanged(int axis, int dir, double value);
	bool viewAxisCrossSet(int axis, int dir, int type);
	bool viewAxisTicksChanged(int axis, bool start, double value);
	bool viewAxisLabelOrientationChanged(int axis, int dir, bool direction, double value);
	bool viewAxisTitleOrientationChanged(int axis, int dir, bool direction, double value);
	bool viewAxisMinorTicksChanged(int axis, int value);
	bool viewAxisLabelRotationChanged(int axis, int rotation);
	bool viewAxisTitleRotationChanged(int axis, int rotation);

	private slots:
	void on_ViewLabelScaleSpin_valueChanged(double value);
	void on_ViewTitleScaleSpin_valueChanged(double value);
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
	void on_ViewXAxisLabelDirectionXSpin_valueChanged(double value);
	void on_ViewXAxisLabelDirectionYSpin_valueChanged(double value);
	void on_ViewXAxisLabelDirectionZSpin_valueChanged(double value);
	void on_ViewXAxisLabelUpXSpin_valueChanged(double value);
	void on_ViewXAxisLabelUpYSpin_valueChanged(double value);
	void on_ViewXAxisLabelUpZSpin_valueChanged(double value);
	void on_ViewXAxisLabelRotationSpin_valueChanged(int value);
	void on_ViewXAxisTitleDirectionXSpin_valueChanged(double value);
	void on_ViewXAxisTitleDirectionYSpin_valueChanged(double value);
	void on_ViewXAxisTitleDirectionZSpin_valueChanged(double value);
	void on_ViewXAxisTitleUpXSpin_valueChanged(double value);
	void on_ViewXAxisTitleUpYSpin_valueChanged(double value);
	void on_ViewXAxisTitleUpZSpin_valueChanged(double value);
	void on_ViewXAxisTitleRotationSpin_valueChanged(int value);
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
	void on_ViewYAxisLabelDirectionXSpin_valueChanged(double value);
	void on_ViewYAxisLabelDirectionYSpin_valueChanged(double value);
	void on_ViewYAxisLabelDirectionZSpin_valueChanged(double value);
	void on_ViewYAxisLabelUpXSpin_valueChanged(double value);
	void on_ViewYAxisLabelUpYSpin_valueChanged(double value);
	void on_ViewYAxisLabelUpZSpin_valueChanged(double value);
	void on_ViewYAxisLabelRotationSpin_valueChanged(int value);
	void on_ViewYAxisTitleDirectionXSpin_valueChanged(double value);
	void on_ViewYAxisTitleDirectionYSpin_valueChanged(double value);
	void on_ViewYAxisTitleDirectionZSpin_valueChanged(double value);
	void on_ViewYAxisTitleUpXSpin_valueChanged(double value);
	void on_ViewYAxisTitleUpYSpin_valueChanged(double value);
	void on_ViewYAxisTitleUpZSpin_valueChanged(double value);
	void on_ViewYAxisTitleRotationSpin_valueChanged(int value);
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
	void on_ViewZAxisLabelDirectionXSpin_valueChanged(double value);
	void on_ViewZAxisLabelDirectionYSpin_valueChanged(double value);
	void on_ViewZAxisLabelDirectionZSpin_valueChanged(double value);
	void on_ViewZAxisLabelUpXSpin_valueChanged(double value);
	void on_ViewZAxisLabelUpYSpin_valueChanged(double value);
	void on_ViewZAxisLabelUpZSpin_valueChanged(double value);
	void on_ViewZAxisLabelRotationSpin_valueChanged(int value);
	void on_ViewZAxisTitleDirectionXSpin_valueChanged(double value);
	void on_ViewZAxisTitleDirectionYSpin_valueChanged(double value);
	void on_ViewZAxisTitleDirectionZSpin_valueChanged(double value);
	void on_ViewZAxisTitleUpXSpin_valueChanged(double value);
	void on_ViewZAxisTitleUpYSpin_valueChanged(double value);
	void on_ViewZAxisTitleUpZSpin_valueChanged(double value);
	void on_ViewZAxisTitleRotationSpin_valueChanged(int value);
	// -- Extras Tab
	void on_ViewBoundingBoxNoneRadio_clicked(bool checked);
	void on_ViewBoundingBoxPlaneRadio_clicked(bool checked);
	void on_ViewBoundingBoxCubeRadio_clicked(bool checked);
	void on_ViewBoundingBoxPlaneYSpin_valueChanged(double value);
	void on_ViewBoundingBoxPlaneYSetMinimumButton_clicked(bool checked);
	void on_ViewBoundingBoxPlaneYSetZeroButton_clicked(bool checked);
	void on_ViewBoundingBoxPlaneYSetMaximumButton_clicked(bool checked);

	public:
	// Update View tab
	void updateViewTab();


	/*
	 * Tabs -- Analyse
	 */
	private slots:
	void on_AnalyseSliceNoneRadio_clicked(bool checked);
	void on_AnalyseSliceXRadio_clicked(bool checked);
	void on_AnalyseSliceYRadio_clicked(bool checked);
	void on_AnalyseSliceZRadio_clicked(bool checked);
	void on_AnalyseSurfaceSliceMonitorCheck_clicked(bool checked);
	void on_AnalyseSurfaceSliceShowLegendCheck_clicked(bool checked);
	void on_AnalyseSurfaceSliceAutoScaleCheck_clicked(bool checked);
	void on_AnalyseSurfaceSliceClearButton_clicked(bool checked);
	void on_AnalyseSurfaceSliceSaveButton_clicked(bool checked);
	void on_AnalyseSurfaceSliceList_currentRowChanged(int index);

	public slots:
	void surfaceSliceAxisValueChanged(int axis, double value);
	void addSurfaceSlice(int axis, double value);

	private:
	// Current slice axis
	int sliceAxis_;
	// Current slice axis value
	double sliceAxisValue_;
	// Current slice data
	Data2D sliceData_;
	
	private:
	// Return axis bin value of closest point to supplied value
	int closestBin(int axis, double value);

	public:
	// Update slice data
	void updateSurfaceSliceData(bool setStatic);


	/*
	 * Data
	 */
	public:
	// Datafile keywords
	enum DataFileKeyword {
		AxisAutoTicksKeyword, AxisFirstTickKeyword, AxisInvertKeyword, AxisLabelDirectionKeyword, AxisLabelRotationKeyword, AxisLabelUpKeyword, AxisLogarithmicKeyword, AxisMinorTicksKeyword, AxisPositionKeyword, AxisStretchKeyword, AxisTickDeltaKeyword, AxisTitleKeyword, AxisTitleDirectionKeyword, AxisTitleRotationKeyword, AxisTitleUpKeyword, AxisVisibleKeyword,
		BoundingBoxKeyword, BoundingBoxPlaneYKeyword,
		ColourAlphaControlKeyword, ColourAlphaFixedKeyword, ColourCustomGradientKeyword, ColourRGBGradientAKeyword, ColourRGBGradientBKeyword, ColourHSVGradientAKeyword, ColourHSVGradientBKeyword, ColourSingleKeyword, ColourSourceKeyword,
		DataKeyword,
		ImageExportKeyword, InterpolateKeyword, InterpolateConstrainKeyword, InterpolateStepKeyword,
		LabelScaleKeyword, LimitXKeyword, LimitYKeyword, LimitZKeyword,
		PerspectiveKeyword, PostTransformShiftKeyword, PreTransformShiftKeyword,
		SliceDirectoryKeyword, SliceKeyword,
		TitleScaleKeyword, TransformXKeyword, TransformYKeyword, TransformZKeyword,
		ViewMatrixXKeyword, ViewMatrixYKeyword, ViewMatrixZKeyword, ViewMatrixWKeyword,
		nDataFileKeywords };
	static DataFileKeyword dataFileKeyword(const char* s);
	static const char* dataFileKeyword(DataFileKeyword dfk);
	// Data Transform types
	enum DataTransform { MultiplyTransform, DivideTransform, LogBase10Transform, NaturalLogTransform, nDataTransforms };
	static DataTransform dataTransform(const char* s);
	static const char* dataTransform(DataTransform dt);
	// 

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
	// List of slices for display
	List<Slice> surfaceData_;
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
	Slice* findSlice(QString title);
	// Return number of slices with no data present
	int nEmptySlices();
	// Recalculate data limits
	void calculateDataLimits();
	// Recalculate transform limits
	void calculateTransformLimits();
	// Set display limits to show all available data
	void showAllData();
	// Flag data as modified, and update titlebar
	void setAsModified();
	// Update surface data after data change
	void updateSurface(bool dataHasChanged = true);


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
	// Transform multipliers for data
	Vec3<double> transformValue_;
	// Transform types for data
	DataTransform transformType_[3];
	// Pre-transform shift value
	Vec3<double> preTransformShift_;
	// Post-transform shift value
	Vec3<double> postTransformShift_;
	// Interpolation flags
	Vec3<bool> interpolate_, interpolateConstrained_;
	// Interpolation step sizes
	Vec3<double> interpolationStep_;

	public:
	// Transform single value on the axis specified
	double transformValue(double x, int axis);
	// Set limits to show all data
	void showAll(bool changeX = true, bool changeY = true, bool changeZ = true);


	/*
	 * Axes
	 */
	private:
	// Whether to invert axes
	Vec3<bool> axisInvert_;
	// Axis visibility
	Vec3<bool> axisVisible_;
	// Axis position (in real surface-space coordinates)
	Vec3<double> axisPosition_[3];
	// Position of first tick delta on axes
	Vec3<double> axisFirstTick_;
	// Tick delta for axes
	Vec3<double> axisTickDelta_;
	// Whether to calculate ticks automatically
	Vec3<bool> axisAutoTicks_;
	// Number of minor ticks in major tick intervals
	Vec3<int> axisMinorTicks_;
	// Orientation of axis labels
	Vec3<double> axisLabelDirection_[3], axisLabelUp_[3];
	// Rotation of axis labels (about calculated Z)
	Vec3<int> axisLabelRotation_;
	// Axis titles
	QString axisTitle_[3];
	// Orientation of axis titles
	Vec3<double> axisTitleDirection_[3], axisTitleUp_[3];
	// Rotation of axis titles (about calculated Z)
	Vec3<int> axisTitleRotation_;
	// Whether axes should be plotted as logarithms
	Vec3<bool> axisLogarithmic_;
	// Stretch factors to apply to axes
	Vec3<double> axisStretch_;
	// Font scaling for axis value labels
	double labelScale_;
	// Font scaling for titles
	double titleScale_;


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