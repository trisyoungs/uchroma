/*
	*** Main Window - Data 
	*** src/gui/fqplot_data.cpp
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

#include "gui/fqplot.h"
#include "base/lineparser.h"

// DataFile Keywords
const char* DataFileKeywordStrings[FQPlotWindow::nDataFileKeywords] = { "slicedir", "slice" };
FQPlotWindow::DataFileKeyword FQPlotWindow::dataFileKeyword(const char* s)
{
	for (int n=0; n<FQPlotWindow::nDataFileKeywords; ++n) if (strcmp(s, DataFileKeywordStrings[n]) == 0) return (FQPlotWindow::DataFileKeyword) n;
	return FQPlotWindow::nDataFileKeywords;
}

/*
 * Data
*/

// Clear current data
void FQPlotWindow::clearData()
{
	slices_.clear();
	axisMin_.set(0.0, 0.0, 0.0);
	axisMax_.set(10.0, 10.0, 10.0);
	axisStep_.set(0.1, 0.1, 0.1);
	interpolate_.set(false, false, false);
	modified_ = false;
}

// Load data from file specified
bool FQPlotWindow::loadData(QString fileName)
{
	LineParser parser;
	parser.openInput(qPrintable(fileName));
	if (!parser.isFileGoodForReading())
	{
		QMessageBox::warning(this, "Error", "Can't open specified file for reading.");
		return false;
	}

	// Read line from file and decide what to do with it
	FQPlotWindow::DataFileKeyword kwd;
	Slice* slice;
	while (!parser.eofOrBlank())
	{
		parser.getArgsDelim(LineParser::UseQuotes);
		
		// Get keyword and act accordingly
		kwd = dataFileKeyword(parser.argc(0));
		switch (kwd)
		{
			// Dafatile directory
			case (FQPlotWindow::SliceDirectoryKeyword):
				dataFileDirectory_ = parser.argc(1);
				if (!dataFileDirectory_.isReadable())
				{
					QMessageBox::StandardButton button = QMessageBox::warning(this, "Error", "The slice directory specified (" + dataFileDirectory_.absolutePath() + ") does not exist or is unreadable.\nDo you want to reset the datafile location?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
				}
				break;
			// Datafile slice specification
			case (FQPlotWindow::SliceKeyword):
				slice = loadSlice(parser.argc(1));
				if (slice) slice->setZ(parser.argd(2));
				break;
		}
	}

	parser.closeFiles();
	return true;
}

// Save current data to file specified
bool FQPlotWindow::saveData(QString fileName)
{
	LineParser parser;
	parser.openOutput(qPrintable(fileName), true);
	if (!parser.isFileGoodForWriting())
	{
		QMessageBox::warning(this, "Error", "Can't open specified file for writing.");
		return false;
	}

	// Write datafile directory
	parser.writeLineF("%s \"%s\"\n", DataFileKeywordStrings[FQPlotWindow::SliceDirectoryKeyword], qPrintable(dataFileDirectory_.absolutePath()));

	// Write list of datafiles (slices)
	// Line format:  slice "filename" z	
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next)
	{
		parser.writeLineF("%s \"%s\" %f\n", DataFileKeywordStrings[FQPlotWindow::SliceKeyword], qPrintable(slice->fileName()), slice->z());
	}
	
	parser.closeFiles();
	return true;
}

// Load slice
Slice* FQPlotWindow::loadSlice(QString fileName)
{
	QString relativeFileName = dataFileDirectory_.relativeFilePath(fileName);

	// Is this file already loaded
	Slice* slice = NULL;
	for (slice = slices_.first(); slice != NULL; slice = slice->next) if (relativeFileName == slice->fileName()) break;
	if (slice)
	{
		printf("File %s is already loaded - will not load it again.\n", qPrintable(relativeFileName));
		return NULL;
	}
	else slice = slices_.add();
	
	slice->setFileName(relativeFileName);
	slice->setZ(slices_.last()->z() + 1.0);

	slice->loadData(dataFileDirectory_);
	
	return slice;
}

// Set limits to encompass entire dataspace
void FQPlotWindow::resetPlotLimits()
{
}

// Update surface data after data change
void FQPlotWindow::updateSurface()
{
	// Clear existing display slices
	surfaceData_.clear();

	// TEST Just add all slices to the array
	surfaceData_ = slices_;
	
	ui.MainView->createSurface(surfaceData_);
}
