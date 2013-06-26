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


	/*
	// File Menu
	*/
	private slots:
	void on_actionFileLoad_triggered(bool checked);
	void on_actionFileSave_triggered(bool checked);
	void on_actionFileQuit_triggered(bool checked);


	/*
	// View Menu
	*/
	private slots:
	void on_actionViewReset_triggered(bool checked);


	/*
	 * Tabs
	 */
	private slots:
	// -- Source Data
	void on_AddFilesButton_clicked(bool checked);
	void on_RemoveFilesButton_clicked(bool checked);
	void on_SourceDataTable_itemSelectionChanged();
	// -- View
	void on_ColourScaleTable_cellDoubleClicked(QTableWidgetItem* item, int col);   // XXX Correct?


	/*
	 * Data
	 */
	private:
	// Whether current data has been modified
	bool modified_;
	// Current input filename
	QString inputFile_;
	// Root directory for datafiles
	QDir dataFileDirectory_;
	// List of slices
	List<Slice> slices_;
	// Minima, maxima, and step sizes for surface generation (if interpolating)
	Vec3<double> axisMin_, axisMax_, axisStep_;
	// Interpolation flags
	Vec3<bool> interpolate_;
	// List of slices for display
	List<Slice> surfaceData_;

	public:
	// Datafile keywords
	enum DataFileKeyword { SliceDirectoryKeyword, SliceKeyword, nDataFileKeywords };
	static DataFileKeyword dataFileKeyword(const char* s);

	public:
	// Clear current data
	void clearData();
	// Load data from file specified
	bool loadData(QString fileName);
	// Save current data to file specified
	bool saveData(QString fileName);
	// Load slice
	Slice* loadSlice(QString fileName);
	// Set limits to encompass entire dataspace
	void resetPlotLimits();
	// Update surface data after data change
	void updateSurface();
};

#endif
