/*
	*** FQPlot Main Window
	*** src/gui/fqplot.h
	Copyright T. Youngs 2013

	This file is part of FQPlot.

	FQPlot is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	FQPlot is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FQPlot.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FQPLOT_MAINWINDOW_H
#define FQPLOT_MAINWINDOW_H

#include "gui/ui_fqplot.h"
#include "base/slice.h"

// Forward Declarations
/* None */

class FQPlotWindow : public QMainWindow
{
	// All Qt declarations must include this macro
	Q_OBJECT


	/*
	// Window Functions
	*/
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;

	public:
	// Constructor / Destructor
	FQPlotWindow(QMainWindow *parent = 0);
	~FQPlotWindow();
	// Main form declaration
	Ui::FQPlotWindow ui;
	// Finalise widgets (things that we couldn't do in Qt Designer)
	void finaliseUi();

	protected:
	// Window close event
	void closeEvent(QCloseEvent *event);
	
	public:
	// Update all tabs
	void updateAllTabs();
	// Update title bar
	void updateTitleBar();


	/*
	// File Menu
	*/
	private slots:
	void on_actionFileLoad_triggered(bool checked);
	void on_actionFileSave_triggered(bool checked);
	void on_actionFileSaveAs_triggered(bool checked);
	void on_actionFileQuit_triggered(bool checked);


	/*
	// View Menu
	*/
	private slots:
	void on_actionViewReset_triggered(bool checked);


	/*
	 * Tabs -- Source Data
	 */
	private slots:
	void on_SourceDirSelectButton_clicked(bool checked);
	void on_AddFilesButton_clicked(bool checked);
	void on_RemoveFilesButton_clicked(bool checked);
	void on_SourceDataTable_itemSelectionChanged();
	void on_GetZFromTimeStampButton_clicked(bool checked);

	public:
	// Update source data tab
	void updateSourceDataTab();


	/*
	 * Tabs -- Transform
	 */
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

	public:
	// Update Transform tab
	void updateTransformTab();


	/*
	 * Tabs -- View
	 */
	private slots:
	void on_ColourScaleTable_cellDoubleClicked(QTableWidgetItem* item, int col);
	void on_XMinSpin_valueChanged(double value);
	void on_YMinSpin_valueChanged(double value);
	void on_ZMinSpin_valueChanged(double value);
	void on_XMaxSpin_valueChanged(double value);
	void on_YMaxSpin_valueChanged(double value);
	void on_ZMaxSpin_valueChanged(double value);
	void on_XLogCheck_clicked(bool checked);
	void on_YLogCheck_clicked(bool checked);
	void on_ZLogCheck_clicked(bool checked);

	public:
	// Update View tab
	void updateViewTab();


	/*
	 * Data
	 */
	public:
	// Datafile keywords
	enum DataFileKeyword { ColourScalePointKeyword, PostTransformShiftXKeyword, PostTransformShiftYKeyword, PostTransformShiftZKeyword, PreTransformShiftXKeyword, PreTransformShiftYKeyword, PreTransformShiftZKeyword, SliceDirectoryKeyword, SliceKeyword, TransformXKeyword, TransformYKeyword, TransformZKeyword, ViewMatrixXKeyword, ViewMatrixYKeyword, ViewMatrixZKeyword, ViewMatrixWKeyword, nDataFileKeywords };
	static DataFileKeyword dataFileKeyword(const char* s);
	static const char* dataFileKeyword(DataFileKeyword dfk);
	// Data Transform types
	enum DataTransform { MultiplyTransform, DivideTransform, LogBase10Transform, NaturalLogTransform, nDataTransforms };
	static DataTransform dataTransform(const char* s);
	static const char* dataTransform(DataTransform dt);

	private:
	// Whether current data has been modified
	bool modified_;
	// Current input filename
	QString inputFile_;
	// Root directory for datafiles
	QDir dataFileDirectory_;
	// List of slices
	List<Slice> slices_;
	// Minima, maxima, and (if interpolating) step sizes for surface generation
	Vec3<double> axisMin_, axisMax_, axisStep_;
	// Transform multipliers for data
	Vec3<double> transformValue_;
	// Transform types for data
	DataTransform transformType_[3];
	// Pre-transform shift value
	Vec3<double> preTransformShift_;
	// Post-transform shift value
	Vec3<double> postTransformShift_;
	// Interpolation flags
	Vec3<bool> interpolate_;
	// List of slices for display
	List<Slice> surfaceData_;

	public:
	// Clear current data
	void clearData();
	// Create default colourscale
	void createDefaultColourScale();
	// Load data from file specified
	bool loadData(QString fileName);
	// Save current data to file specified
	bool saveData(QString fileName);
	// Load slice
	Slice* loadSlice(QString fileName);
	// Set limits to encompass entire dataspace
	void resetPlotLimits();
	// Flag data as modified, and update titlebar
	void setAsModified();
	// Update surface data after data change
	void updateSurface();
};

#endif
