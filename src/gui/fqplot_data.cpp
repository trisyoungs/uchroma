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
const char* DataFileKeywordStrings[FQPlotWindow::nDataFileKeywords] = { "ColourScalePoint", "InvertZAxis", "LimitX", "LimitY", "LimitZ", "PostTransformShiftX", "PostTransformShiftY", "PostTransformShiftZ", "PreTransformShiftX", "PreTransformShiftY", "PreTransformShiftZ", "SliceDir", "Slice", "TransformX", "TransformY", "TransformZ", "ViewMatrixX", "ViewMatrixY", "ViewMatrixZ", "ViewMatrixW" };
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
const char* DataTransformKeywords[FQPlotWindow::nDataTransforms] = { "Multiply", "Divide", "LogBase10", "NaturalLog" };
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

// Recalculate tick deltas for specified axis
void FQPlotWindow::calculateTickDeltas(int axis)
{
	const int nBaseValues = 5, maxIterations = 10, maxTicks = 10;
	int power = 1, baseValues[nBaseValues] = { 1, 2, 3, 4, 5 }, baseValueIndex = 0, nTicks, iteration, minTicks = maxTicks/2;

	baseValueIndex = 0;
	power = int(log10((limitMax_[axis]-limitMin_[axis]) / maxTicks) - 1);
	iteration = 0;

	if ((limitMax_[axis] - limitMin_[axis]) > 1.0e-10)
	{
		do
		{
			// Calculate current tickDelta
			axisTickDelta_[axis] = baseValues[baseValueIndex]*pow(10.0,power);

			// Get first tickmark value
			axisFirstTick_[axis] = int(limitMin_[axis] / axisTickDelta_[axis]) * axisTickDelta_[axis];
			if (axisFirstTick_[axis] < limitMin_[axis]) axisFirstTick_[axis] += axisTickDelta_[axis];

			// How many ticks now fit between the firstTick and max value?
			// Add 1 to get total ticks for this delta (i.e. including firstTick)
			nTicks = int((limitMax_[axis]-axisFirstTick_[axis]) / axisTickDelta_[axis]);
			++nTicks;

			// Check n...
			if (nTicks > maxTicks)
			{
				++baseValueIndex;
				if (baseValueIndex == nBaseValues) ++power;
				baseValueIndex = baseValueIndex%nBaseValues;
			}
			else if (nTicks < minTicks)
			{
				--baseValueIndex;
				if (baseValueIndex == -1)
				{
					--power;
					baseValueIndex += nBaseValues;
				}
			}

			++iteration;
			if (iteration == maxIterations) break;

		} while ((nTicks > maxTicks) || (nTicks < minTicks));
	}
	else
	{
		axisFirstTick_[axis] = limitMin_[axis];
		axisTickDelta_[axis] = 1.0;
	}
}

// Clear current data
void FQPlotWindow::clearData()
{
	slices_.clear();
	dataMin_.zero();
	dataMax_.zero();
	limitMin_.set(0.0, 0.0, 0.0);
	limitMax_.set(10.0, 10.0, 10.0);
	interpolate_.set(false, false, false);
	transformValue_.set(1.0,1.0,1.0);
	transformType_[0] = FQPlotWindow::MultiplyTransform;
	transformType_[1] = FQPlotWindow::MultiplyTransform;
	transformType_[2] = FQPlotWindow::MultiplyTransform;
	preTransformShift_.zero();
	postTransformShift_.zero();
	modified_ = false;
	ui.ColourScaleWidget->clear();
	invertZAxis_ = false;
	axisLabelScale_ = 0.5;
	axisTitleScale_ = 0.62;
	axisPosition_[0].zero();
	axisPosition_[1].zero();
	axisPosition_[2].zero();
	axisFirstTick_.zero();
	axisTickDelta_.set(1.0,1.0,1.0);
	axisAutoTicks_.set(true, true, true);
	axisLabelDirection_[0].set(1.0, 0.0, 0.0);
	axisLabelDirection_[1].set(1.0, 0.0, 0.0);
	axisLabelDirection_[2].set(1.0, 0.0, 0.0);
	axisLabelRotation_.zero();
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
			// Invert Z-Axis
			case (FQPlotWindow::InvertZAxisKeyword):
				invertZAxis_ = true;
				break;
			// Limits
			case (FQPlotWindow::LimitXKeyword):
			case (FQPlotWindow::LimitYKeyword):
			case (FQPlotWindow::LimitZKeyword):
				xyz = kwd - FQPlotWindow::LimitXKeyword;
				limitMin_[xyz] = parser.argd(1);
				limitMax_[xyz] = parser.argd(2);
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
	// -- Limits
	parser.writeLineF("%s %f %f\n", DataFileKeywordStrings[FQPlotWindow::LimitXKeyword], limitMin_.x, limitMax_.x);
	parser.writeLineF("%s %f %f\n", DataFileKeywordStrings[FQPlotWindow::LimitYKeyword], limitMin_.y, limitMax_.y);
	parser.writeLineF("%s %f %f\n", DataFileKeywordStrings[FQPlotWindow::LimitZKeyword], limitMin_.z, limitMax_.z);
	
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

// Recalculate data limits
void FQPlotWindow::calculateDataLimits()
{
	dataMin_ = 0.0;
	dataMax_ = 0.0;
	if (slices_.nItems() > 0)
	{
		// Grab first slice and set initial values
		Slice* slice = slices_.first();
		dataMin_.set(slice->data().xMin(), slice->data().yMin(), slice->z());
		dataMax_.set(slice->data().xMax(), slice->data().yMax(), slice->z());
		double mmin, mmax;
		for (slice = slice->next; slice != NULL; slice = slice->next)
		{
			mmin = slice->data().xMin();
			mmax = slice->data().xMax();
			if (mmin < dataMin_.x) dataMin_.x = mmin;
			if (mmax > dataMax_.x) dataMax_.x = mmax;
			mmin = slice->data().yMin();
			mmax = slice->data().yMax();
			if (mmin < dataMin_.y) dataMin_.y = mmin;
			if (mmax > dataMax_.y) dataMax_.y = mmax;
			if (slice->z() < dataMin_.z) dataMin_.z = slice->z();
			else if (slice->z() > dataMax_.z) dataMax_.z = slice->z();
		}
	}
	calculateTransformLimits();
}

// Recalculate transform limits
void FQPlotWindow::calculateTransformLimits()
{
	transformMin_ = 0.0;
	transformMax_ = 0.0;
	if (slices_.nItems() == 0) return;
	
	for (int n=0; n<3; ++n)
	{
		transformMin_[n] = transformValue(dataMin_[n], preTransformShift_[n], postTransformShift_[n], transformType_[n], transformValue_[n]);
		transformMax_[n] = transformValue(dataMax_[n], preTransformShift_[n], postTransformShift_[n], transformType_[n], transformValue_[n]);

		// Values may have swapped...
		if (transformMin_[n] > transformMax_[n])
		{
			double x = transformMin_[n];
			transformMin_[n] = transformMax_[n];
			transformMax_[n] = x;
		}

		// Might also need to clamp current limits to the new range...
		if (limitMin_[n] < transformMin_[n]) limitMin_[n] = transformMin_[n];
		else if (limitMin_[n] > transformMax_[n]) limitMin_[n] = transformMax_[n];
		if (limitMax_[n] < transformMin_[n]) limitMax_[n] = transformMin_[n];
		else if (limitMax_[n] > transformMax_[n]) limitMax_[n] = transformMax_[n];
		
		// Is limited range acceptable?
		if ((fabs(limitMax_[n] - limitMin_[n]) < 1.0e-5))
		{
			limitMin_[n] = transformMin_[n];
			limitMax_[n] = transformMax_[n];
		}

		// Are axis positions acceptable
		for (int m=0; m < 3; ++m)
		{
			if (n == m) continue;
			if (axisPosition_[m].get(n) < limitMin_[n]) axisPosition_[m].set(n, limitMin_[n]);
			if (axisPosition_[m].get(n) > limitMax_[n]) axisPosition_[m].set(n, limitMax_[n]);
		}
	}
}

// Transform single value
double FQPlotWindow::transformValue(double x, double preShift, double postShift, FQPlotWindow::DataTransform transformType, double transformValue)
{
	switch (transformType)
	{
		case (FQPlotWindow::MultiplyTransform):
			return  (x+preShift)*transformValue + postShift;
			break;
		case (FQPlotWindow::DivideTransform):
			return (x+preShift)/transformValue + postShift;
			break;
		case (FQPlotWindow::LogBase10Transform):
			return log10(x+preShift) + postShift;
			break;
		case (FQPlotWindow::NaturalLogTransform):
			return log(x+preShift) + postShift;
			break;
	}
	return 0.0;
}

// Flag data as modified, and update titlebar
void FQPlotWindow::setAsModified()
{
	modified_ = true;
	updateTitleBar();
}

// Update surface data after data change
void FQPlotWindow::updateSurface(bool dataHasChanged)
{
	// Make sure view variables are up-to-date
	ui.MainView->setSurfaceCenter((limitMax_+limitMin_)*0.5);
	ui.MainView->setAxisLabelScale(axisLabelScale_);

	if (dataHasChanged)
	{
		// Clear existing display slices
		surfaceData_.clear();

		// Loop over slices, apply any transforms (X or Y) and check limits
		for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next)
		{
			// Z
			double z = transformValue(slice->z(), preTransformShift_[2], postTransformShift_[2], transformType_[2], transformValue_[2]);
			// -- Is the transformed Z value within range?
			if ((z < limitMin_.z) || (z > limitMax_.z)) continue;

			// Add new item to surfaceData_ array
			Slice* surfaceSlice = surfaceData_.add();
			surfaceSlice->setZ(z);

			// X / Y
			Array<double> array[2];
			for (int n=0; n<2; ++n)
			{
				array[n] = n == 0 ? slice->data().arrayX() : slice->data().arrayY();

				// Apply pre-transform shift
				array[n] += preTransformShift_[n];

				// Apply transform
				switch (transformType_[n])
				{
					case (FQPlotWindow::MultiplyTransform):
						array[n] *= transformValue_[n];
						break;
					case (FQPlotWindow::DivideTransform):
						array[n] /= transformValue_[n];
						break;
					case (FQPlotWindow::LogBase10Transform):
						array[n].takeLog();
						break;
					case (FQPlotWindow::NaturalLogTransform):
						array[n].takeLn();
						break;
				}
				// Apply post-transform shift
				array[n] += postTransformShift_[n];
			}

			// Add data to surfaceSlice, obeying defined x-limits
			for (int n=0; n<array[0].nItems(); ++n)
			{
				if ((array[0].value(n) < limitMin_.x) || (array[0].value(n) > limitMax_.x)) continue;
				surfaceSlice->data().addPoint(array[0].value(n), array[1].value(n));
			}
		}
	}

	// Update surface GL object
	ui.MainView->createSurface(surfaceData_, ui.ColourScaleWidget);

	// Construct axes
	for (int axis = 0; axis < 3; ++axis)
	{
		if (axisAutoTicks_[axis]) calculateTickDeltas(axis);
		ui.MainView->createAxis(axis, axisPosition_[axis], limitMin_[axis], limitMax_[axis], axisFirstTick_[axis], axisTickDelta_[axis]);
	}

	ui.MainView->update();

	ui.NTrianglesLabel->setText("NTriangles: " + QString::number(ui.MainView->surfaceNTriangles()));
}
