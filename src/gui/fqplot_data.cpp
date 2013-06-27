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
const char* DataFileKeywordStrings[FQPlotWindow::nDataFileKeywords] = { "ColourScalePoint", "SliceDir", "Slice" };
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
	ui.ColourScaleWidget->clear();
}

// Create default ColourScale
void FQPlotWindow::createDefaultColourScale()
{
	// Construct default colourscale
	ui.ColourScaleWidget->clear();
	ui.ColourScaleWidget->addPoint(0.0, QColor(0, 0, 0));
	ui.ColourScaleWidget->addPoint(0.05, QColor(255, 255, 0));
	ui.ColourScaleWidget->addPoint(0.1, QColor(255, 0, 0));
	ui.ColourScaleWidget->addPoint(0.15, QColor(0, 106, 106));
	ui.ColourScaleWidget->addPoint(0.2, QColor(170, 170, 127));
	ui.ColourScaleWidget->addPoint(0.25, QColor(0, 0, 255));
	ui.ColourScaleWidget->addPoint(0.3, QColor(106, 84, 127));
	ui.ColourScaleWidget->addPoint(0.35, QColor(255, 255, 255));
	ui.ColourScaleWidget->addPoint(1.0, QColor(44, 184, 212));
	ui.ColourScaleWidget->addPoint(1.1, QColor(64, 64, 193));
	ui.ColourScaleWidget->addPoint(1.2, QColor(255, 255, 255));
	ui.ColourScaleWidget->addPoint(1.5, QColor(127, 127, 127));
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
			// ColourScalePoint definition
			case (FQPlotWindow::ColourScalePointKeyword):
				ui.ColourScaleWidget->addPoint(parser.argd(1), QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)));
				break;
			// Datafile directory
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

	// Add default colourscale?
	if (ui.ColourScaleWidget->nPoints() == 0) createDefaultColourScale();

	// Recreate surface
	updateSurface();

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

	// Write view setup
	// -- ColourScale
	for (ColourScalePoint* csp = ui.ColourScaleWidget->firstPoint(); csp != NULL; csp = csp->next)
	{
		parser.writeLineF("%s %f %i %i %i %i\n", DataFileKeywordStrings[FQPlotWindow::ColourScalePointKeyword], csp->value(), csp->colour().red(), csp->colour().green(), csp->colour().blue(), csp->colour().alpha());
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

	// Determine automatic Z placement for slice
	double newZ = (slices_.last() != NULL ? slices_.last()->z() + 1.0 : 0.0);
	
	// Create new slice
	slice = slices_.add();
	slice->setFileName(relativeFileName);
	slice->setZ(newZ);
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

	// Copy all slices to the array
	surfaceData_ = slices_;
	
	// Loop over slices and apply any transforms (X or Y)
	for (Slice* slice = surfaceData_.first(); slice != NULL; slice = slice->next)
	{
		if (axisLog_.x) slice->data().arrayX().takeLog();
		if (axisLog_.y)
		{
			slice->data().arrayY() += 2.0;
			slice->data().arrayY().takeLog();
		}
		if (axisLog_.y) slice->setZ(log10(slice->z()));
	}

	ui.MainView->createSurface(surfaceData_, ui.ColourScaleWidget);
	ui.MainView->update();
}
