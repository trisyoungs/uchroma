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
const char* DataFileKeywordStrings[FQPlotWindow::nDataFileKeywords] = { "ColourScalePoint", "PostTransformShiftX", "PostTransformShiftY", "PostTransformShiftZ", "PreTransformShiftX", "PreTransformShiftY", "PreTransformShiftZ", "SliceDir", "Slice", "TransformX", "TransformY", "TransformZ", "ViewMatrixX", "ViewMatrixY", "ViewMatrixZ", "ViewMatrixW" };
FQPlotWindow::DataFileKeyword FQPlotWindow::dataFileKeyword(const char* s)
{
	for (int n=0; n<FQPlotWindow::nDataFileKeywords; ++n) if (strcmp(s, DataFileKeywordStrings[n]) == 0) return (FQPlotWindow::DataFileKeyword) n;
	return FQPlotWindow::nDataFileKeywords;
}
const char* FQPlotWindow::dataFileKeyword(FQPlotWindow::DataFileKeyword dfk)
{
	return DataFileKeywordStrings[dfk];
}

// Data Transform types
const char* DataTransformKeywords[FQPlotWindow::nDataTransforms] = { "Multiply", "Divide", "LogBase", "NaturalLog" };
FQPlotWindow::DataTransform FQPlotWindow::dataTransform(const char* s)
{
	for (int n=0; n<FQPlotWindow::nDataTransforms; ++n) if (strcmp(s, DataTransformKeywords[n]) == 0) return (FQPlotWindow::DataTransform) n;
	return FQPlotWindow::nDataTransforms;
}
const char* FQPlotWindow::dataTransform(FQPlotWindow::DataTransform dt)
{
	return DataTransformKeywords[dt];
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
	transformValue_.set(1.0,1.0,1.0);
	transformType_[0] = FQPlotWindow::MultiplyTransform;
	transformType_[1] = FQPlotWindow::MultiplyTransform;
	transformType_[2] = FQPlotWindow::MultiplyTransform;
	preTransformShift_.zero();
	postTransformShift_.zero();
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
	FQPlotWindow::DataTransform dt;
	Slice* slice;
	int xyz;
	Matrix mat;
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
			// Pre-Transform Shifts
			case (FQPlotWindow::PreTransformShiftXKeyword):
			case (FQPlotWindow::PreTransformShiftYKeyword):
			case (FQPlotWindow::PreTransformShiftZKeyword):
				xyz = kwd - FQPlotWindow::PreTransformShiftXKeyword;
				preTransformShift_[xyz] = parser.argd(1);
				break;
			// Post-Transform Shifts
			case (FQPlotWindow::PostTransformShiftXKeyword):
			case (FQPlotWindow::PostTransformShiftYKeyword):
			case (FQPlotWindow::PostTransformShiftZKeyword):
				xyz = kwd - FQPlotWindow::PostTransformShiftXKeyword;
				postTransformShift_[xyz] = parser.argd(1);
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
			// Data Transform
			case (FQPlotWindow::TransformXKeyword):
			case (FQPlotWindow::TransformYKeyword):
			case (FQPlotWindow::TransformZKeyword):
				xyz = kwd - FQPlotWindow::TransformXKeyword;
				dt = dataTransform(parser.argc(1));
				transformType_[xyz] = dt;
				transformValue_[xyz] = parser.argd(2);
				break;
			// View Matrix
			case (FQPlotWindow::ViewMatrixXKeyword):
			case (FQPlotWindow::ViewMatrixYKeyword):
			case (FQPlotWindow::ViewMatrixZKeyword):
			case (FQPlotWindow::ViewMatrixWKeyword):
				xyz = kwd - FQPlotWindow::ViewMatrixXKeyword;
				mat = ui.MainView->viewMatrix();
				mat.setColumn(xyz, parser.argd(1), parser.argd(2), parser.argd(3), parser.argd(4));
				ui.MainView->setViewMatrix(mat);
				break;
		}
	}
	parser.closeFiles();

	// Add default colourscale?
	if (ui.ColourScaleWidget->nPoints() == 0) createDefaultColourScale();

	// Recreate surface
	updateSurface();

	// Set necessary variables
	inputFile_ = fileName;
	modified_ = false;
	
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

	// Write transform setup
	// -- Transforms
	parser.writeLineF("%s %s %f\n", DataFileKeywordStrings[FQPlotWindow::TransformXKeyword], dataTransform(transformType_[0]), transformValue_.x);
	parser.writeLineF("%s %s %f\n", DataFileKeywordStrings[FQPlotWindow::TransformYKeyword], dataTransform(transformType_[1]), transformValue_.y);
	parser.writeLineF("%s %s %f\n", DataFileKeywordStrings[FQPlotWindow::TransformZKeyword], dataTransform(transformType_[2]), transformValue_.z);
	// -- Shifts
	parser.writeLineF("%s %f\n", DataFileKeywordStrings[FQPlotWindow::PreTransformShiftXKeyword], preTransformShift_.x);
	parser.writeLineF("%s %f\n", DataFileKeywordStrings[FQPlotWindow::PreTransformShiftYKeyword], preTransformShift_.y);
	parser.writeLineF("%s %f\n", DataFileKeywordStrings[FQPlotWindow::PreTransformShiftZKeyword], preTransformShift_.z);
	parser.writeLineF("%s %f\n", DataFileKeywordStrings[FQPlotWindow::PostTransformShiftXKeyword], postTransformShift_.x);
	parser.writeLineF("%s %f\n", DataFileKeywordStrings[FQPlotWindow::PostTransformShiftYKeyword], postTransformShift_.y);
	parser.writeLineF("%s %f\n", DataFileKeywordStrings[FQPlotWindow::PostTransformShiftZKeyword], postTransformShift_.z);

	// Write view setup
	// -- ColourScale
	for (ColourScalePoint* csp = ui.ColourScaleWidget->firstPoint(); csp != NULL; csp = csp->next)
	{
		parser.writeLineF("%s %f %i %i %i %i\n", DataFileKeywordStrings[FQPlotWindow::ColourScalePointKeyword], csp->value(), csp->colour().red(), csp->colour().green(), csp->colour().blue(), csp->colour().alpha());
	}
	// -- Transformation Matrix
	Matrix mat = ui.MainView->viewMatrix();
	parser.writeLineF("%s %f %f %f %f\n", DataFileKeywordStrings[FQPlotWindow::ViewMatrixXKeyword], mat[0], mat[1], mat[2], mat[3]);
	parser.writeLineF("%s %f %f %f %f\n", DataFileKeywordStrings[FQPlotWindow::ViewMatrixYKeyword], mat[4], mat[5], mat[6], mat[7]);
	parser.writeLineF("%s %f %f %f %f\n", DataFileKeywordStrings[FQPlotWindow::ViewMatrixZKeyword], mat[8], mat[9], mat[10], mat[11]);
	parser.writeLineF("%s %f %f %f %f\n", DataFileKeywordStrings[FQPlotWindow::ViewMatrixWKeyword], mat[12], mat[13], mat[14], mat[15]);

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

// Flag data as modified, and update titlebar
void FQPlotWindow::setAsModified()
{
	modified_ = true;
	updateTitleBar();
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
		// X / Y
		for (int n=0; n<2; ++n)
		{
			Array<double>& arrayRef = n == 0 ? slice->data().arrayX() : slice->data().arrayY();

			// Apply pre-transform shift
			arrayRef += preTransformShift_[n];

			// Apply transform
			switch (transformType_[n])
			{
				case (FQPlotWindow::MultiplyTransform):
					arrayRef *= transformValue_[n];
					break;
				case (FQPlotWindow::DivideTransform):
					arrayRef /= transformValue_[n];
					break;
				case (FQPlotWindow::LogBase10Transform):
					arrayRef.takeLog();
					break;
				case (FQPlotWindow::NaturalLogTransform):
					arrayRef.takeLn();
					break;
			}
			// Apply post-transform shift
			arrayRef += postTransformShift_[n];
		}
		
		// Z
		double z = slice->z();
		switch (transformType_[2])
		{
			case (FQPlotWindow::MultiplyTransform):
				slice->setZ( (z-preTransformShift_.z)*transformValue_.z + postTransformShift_.z);
				break;
			case (FQPlotWindow::DivideTransform):
				slice->setZ( (z-preTransformShift_.z)/transformValue_.z + postTransformShift_.z);
				break;
			case (FQPlotWindow::LogBase10Transform):
				slice->setZ( log10(z-preTransformShift_.z) + postTransformShift_.z);
				break;
			case (FQPlotWindow::NaturalLogTransform):
				slice->setZ( log(z-preTransformShift_.z) + postTransformShift_.z);
				break;
		}
	}

	ui.MainView->createSurface(surfaceData_, ui.ColourScaleWidget);

	ui.MainView->update();

	ui.NTrianglesLabel->setText("NTriangles: " + QString::number(ui.MainView->surfaceNTriangles()));

}
