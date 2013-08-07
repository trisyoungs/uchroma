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
const char* DataFileKeywordStrings[FQPlotWindow::nDataFileKeywords] = { "AxisAutoTicks", "AxisFirstTick", "AxisInvert", "AxisLabelDirection", "AxisLabelRotation", "AxisLabelUp", "AxisLogarithmic", "AxisMinorTicks", "AxisPosition", "AxisStretch", "AxisTickDelta", "AxisVisible", "ColourAlphaControl", "ColourAlphaFixed", "ColourCustomGradient", "ColourLinearA", "ColourLinearB", "ColourSingle", "ColourSource", "Interpolate", "InterpolateConstrain", "InterpolateStep", "LabelScale", "LimitX", "LimitY", "LimitZ", "Perspective", "PostTransformShift", "PreTransformShift", "SliceDir", "Slice", "TitleScale", "TransformX", "TransformY", "TransformZ", "ViewMatrixX", "ViewMatrixY", "ViewMatrixZ", "ViewMatrixW" };
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
	double clampedStartValue;

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
			clampedStartValue = (axisFirstTick_[axis] < limitMin_[axis] ? axisFirstTick_[axis] + axisTickDelta_[axis] : axisFirstTick_[axis]);

			// How many ticks now fit between the firstTick and max value?
			// Add 1 to get total ticks for this delta (i.e. including firstTick)
			nTicks = int((limitMax_[axis]-limitMin_[axis]) / axisTickDelta_[axis]);
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
	interpolateConstrained_.set(false, false, false);
	interpolationStep_.set(1.0, 1.0, 1.0);
	transformValue_.set(1.0, 1.0, 1.0);
	transformType_[0] = FQPlotWindow::MultiplyTransform;
	transformType_[1] = FQPlotWindow::MultiplyTransform;
	transformType_[2] = FQPlotWindow::MultiplyTransform;
	preTransformShift_.zero();
	postTransformShift_.zero();
	modified_ = false;
	axisInvert_.set(false, false, false);
	axisVisible_.set(true, true, true);
	labelScale_ = 0.25;
	titleScale_ = 0.3;
	axisPosition_[0].zero();
	axisPosition_[1].zero();
	axisPosition_[2].zero();
	axisFirstTick_.zero();
	axisTickDelta_.set(1.0,1.0,1.0);
	axisAutoTicks_.set(true, true, true);
	axisMinorTicks_.set(1,1,1);
	axisLabelDirection_[0].set(1.0, 0.0, 0.0);
	axisLabelDirection_[1].set(1.0, 0.0, 0.0);
	axisLabelDirection_[2].set(1.0, 0.0, 0.0);
	axisLabelUp_[0].set(0.0, 1.0, 0.0);
	axisLabelUp_[1].set(0.0, 1.0, 0.0);
	axisLabelUp_[2].set(0.0, 1.0, 0.0);
	axisLabelRotation_.zero();
	axisLogarithmic_.set(false, false, false);
	axisStretch_.set(1.0, 1.0, 1.0);
	colourSource_ = SingleColourSource;
	colourScale_[SingleColourSource].clear();
	colourScale_[SingleColourSource].addPoint(0.0, QColor(255,255,255));
	colourScale_[LinearGradientSource].clear();
	colourScale_[LinearGradientSource].addPoint(0.0, QColor(255,255,255));
	colourScale_[LinearGradientSource].addPoint(1.0, QColor(0,0,255));
	colourScale_[CustomGradientSource].clear();
	fixedAlpha_ = 128;
	alphaControl_ = FQPlotWindow::OwnAlpha;
	ui.actionViewPerspective->setChecked(false);
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
			// Axis auto ticks
			case (FQPlotWindow::AxisAutoTicksKeyword):
				axisAutoTicks_.set(parser.argi(1), parser.argi(2), parser.argi(3));
				break;
			// Axis first ticks
			case (FQPlotWindow::AxisFirstTickKeyword):
				axisFirstTick_.set(parser.argd(1), parser.argd(2), parser.argd(3));
				break;
			// Axis invert flags
			case (FQPlotWindow::AxisInvertKeyword):
				axisInvert_.set(parser.argi(1), parser.argi(2), parser.argi(3));
				break;
			// Axis label direction
			case (FQPlotWindow::AxisLabelDirectionKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) > 2)) printf("Axis index is out of range for %s keyword.\n", dataFileKeyword(kwd));
				else axisLabelDirection_[parser.argi(1)].set(parser.argd(2), parser.argd(3), parser.argd(4));
				break;
			// Axis label rotation
			case (FQPlotWindow::AxisLabelRotationKeyword):
				axisLabelRotation_.set(parser.argi(1), parser.argi(2), parser.argi(3));
				break;
			// Axis label direction
			case (FQPlotWindow::AxisLabelUpKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) > 2)) printf("Axis index is out of range for %s keyword.\n", dataFileKeyword(kwd));
				else axisLabelUp_[parser.argi(1)].set(parser.argd(2), parser.argd(3), parser.argd(4));
				break;
			// Axis logarithmic flag
			case (FQPlotWindow::AxisLogarithmicKeyword):
				axisLogarithmic_.set(parser.argi(1), parser.argi(2), parser.argi(3));
				break;
			// Axis minor ticks
			case (FQPlotWindow::AxisMinorTicksKeyword):
				axisMinorTicks_.set(parser.argi(1), parser.argi(2), parser.argi(3));
				break;
			// Axis position
			case (FQPlotWindow::AxisPositionKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) > 2)) printf("Axis index is out of range for %s keyword.\n", dataFileKeyword(kwd));
				else axisPosition_[parser.argi(1)].set(parser.argd(2), parser.argd(3), parser.argd(4));
				break;
			// Axis stretch factors
			case (FQPlotWindow::AxisStretchKeyword):
				axisStretch_.set(parser.argd(1), parser.argd(2), parser.argd(3));
				break;
			// Axis tick deltas
			case (FQPlotWindow::AxisTickDeltaKeyword):
				axisTickDelta_.set(parser.argd(1), parser.argd(2), parser.argd(3));
				break;
			// Axis visibility
			case (FQPlotWindow::AxisVisibleKeyword):
				axisVisible_.set(parser.argi(1), parser.argi(2), parser.argi(3));
				break;
			// Colour alpha control
			case (FQPlotWindow::ColourAlphaControlKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) > 2)) printf("Value is out of range for %s keyword.\n", dataFileKeyword(kwd));
				else alphaControl_ = (FQPlotWindow::AlphaControl) parser.argi(1);
				break;
			// Colour alpha fixed value
			case (FQPlotWindow::ColourAlphaFixedKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) > 255)) printf("Value is out of range for %s keyword.\n", dataFileKeyword(kwd));
				else fixedAlpha_ = parser.argi(1);
				break;
			// Colour Custom Gradient point definition
			case (FQPlotWindow::ColourCustomGradientKeyword):
				colourScale_[CustomGradientSource].addPoint(parser.argd(1), QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)));
				break;
			// Colour Linear Gradient point definition
			case (FQPlotWindow::ColourLinearAKeyword):
			case (FQPlotWindow::ColourLinearBKeyword):
				colourScale_[LinearGradientSource].setPoint(kwd-ColourLinearAKeyword, parser.argd(1), QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)));
				break;
			// Colour single colour definition
			case (FQPlotWindow::ColourSingleKeyword):
				colourScale_[SingleColourSource].setPoint(0, parser.argd(1), QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)));
				break;
			// Colour source
			case (FQPlotWindow::ColourSourceKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) > 2)) printf("Value is out of range for %s keyword.\n", dataFileKeyword(kwd));
				else colourSource_ = (FQPlotWindow::ColourSource) parser.argi(1);
				break;
			// Interpolate flags
			case (FQPlotWindow::InterpolateKeyword):
				interpolate_.set(parser.argi(1), false, parser.argi(2));
				break;
			// Interpolate constrain flags
			case (FQPlotWindow::InterpolateConstrainKeyword):
				interpolateConstrained_.set(parser.argi(1), false, parser.argi(2));
				break;
			// Interpolation step flags
			case (FQPlotWindow::InterpolateStepKeyword):
				interpolationStep_.set(parser.argd(1), 0.0, parser.argd(2));
				break;
			// Label scale
			case (FQPlotWindow::LabelScaleKeyword):
				labelScale_ = parser.argd(1);
				break;
			// Limits
			case (FQPlotWindow::LimitXKeyword):
			case (FQPlotWindow::LimitYKeyword):
			case (FQPlotWindow::LimitZKeyword):
				xyz = kwd - FQPlotWindow::LimitXKeyword;
				limitMin_[xyz] = parser.argd(1);
				limitMax_[xyz] = parser.argd(2);
				break;
			// Orthographic view
			case (FQPlotWindow::PerspectiveKeyword):
				ui.actionViewPerspective->setChecked(true);
				break;
			// Pre-Transform Shifts
			case (FQPlotWindow::PreTransformShiftKeyword):
				preTransformShift_.set(parser.argd(1), parser.argd(2), parser.argd(3));
				break;
			// Post-Transform Shifts
			case (FQPlotWindow::PostTransformShiftKeyword):
				postTransformShift_.set(parser.argd(1), parser.argd(2), parser.argd(3));
				break;
			// Slice directory
			case (FQPlotWindow::SliceDirectoryKeyword):
				dataFileDirectory_ = parser.argc(1);
				if (!dataFileDirectory_.isReadable())
				{
					QMessageBox::StandardButton button = QMessageBox::warning(this, "Error", "The slice directory specified (" + dataFileDirectory_.absolutePath() + ") does not exist or is unreadable.\nDo you want to reset the datafile location?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
				}
				break;
			// Slice specification
			case (FQPlotWindow::SliceKeyword):
				slice = loadSlice(parser.argc(1));
				if (slice) slice->setZ(parser.argd(2));
				break;
			// Title scale
			case (FQPlotWindow::TitleScaleKeyword):
				titleScale_ = parser.argd(1);
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
	parser.writeLineF("%s %f %f %f\n", DataFileKeywordStrings[FQPlotWindow::PreTransformShiftKeyword], preTransformShift_.x, preTransformShift_.y, preTransformShift_.z);
	parser.writeLineF("%s %f %f %f\n", DataFileKeywordStrings[FQPlotWindow::PostTransformShiftKeyword], postTransformShift_.x, postTransformShift_.y, postTransformShift_.z);
	// -- Limits
	parser.writeLineF("%s %f %f\n", DataFileKeywordStrings[FQPlotWindow::LimitXKeyword], limitMin_.x, limitMax_.x);
	parser.writeLineF("%s %f %f\n", DataFileKeywordStrings[FQPlotWindow::LimitYKeyword], limitMin_.y, limitMax_.y);
	parser.writeLineF("%s %f %f\n", DataFileKeywordStrings[FQPlotWindow::LimitZKeyword], limitMin_.z, limitMax_.z);
	// -- Interpolation
	parser.writeLineF("%s %i %i\n", DataFileKeywordStrings[FQPlotWindow::InterpolateKeyword], interpolate_.x, interpolate_.z);
	parser.writeLineF("%s %i %i\n", DataFileKeywordStrings[FQPlotWindow::InterpolateConstrainKeyword], interpolateConstrained_.x, interpolateConstrained_.z);
	parser.writeLineF("%s %f %f\n", DataFileKeywordStrings[FQPlotWindow::InterpolateStepKeyword], interpolationStep_.x, interpolationStep_.z);

	// Write colour setup
	parser.writeLineF("%s %i", DataFileKeywordStrings[FQPlotWindow::ColourSourceKeyword], colourSource_);
	ColourScalePoint* csp;
	// -- Single Colour
	csp = colourScale_[SingleColourSource].point(0);
	parser.writeLineF("%s %f %i %i %i %i\n", DataFileKeywordStrings[FQPlotWindow::ColourSingleKeyword], csp->value(), csp->colour().red(), csp->colour().green(), csp->colour().blue(), csp->colour().alpha());
	// -- Linear Gradient
	csp = colourScale_[LinearGradientSource].point(0);
	parser.writeLineF("%s %f %i %i %i %i\n", DataFileKeywordStrings[FQPlotWindow::ColourLinearAKeyword], csp->value(), csp->colour().red(), csp->colour().green(), csp->colour().blue(), csp->colour().alpha());
	csp = colourScale_[LinearGradientSource].point(1);
	parser.writeLineF("%s %f %i %i %i %i\n", DataFileKeywordStrings[FQPlotWindow::ColourLinearBKeyword], csp->value(), csp->colour().red(), csp->colour().green(), csp->colour().blue(), csp->colour().alpha());
	// -- Custom Gradient
	for (csp = colourScale_[CustomGradientSource].firstPoint(); csp != NULL; csp = csp->next)
	{
		parser.writeLineF("%s %f %i %i %i %i\n", DataFileKeywordStrings[FQPlotWindow::ColourCustomGradientKeyword], csp->value(), csp->colour().red(), csp->colour().green(), csp->colour().blue(), csp->colour().alpha());
	}
	// -- Alpha control
	parser.writeLineF("%s %i", DataFileKeywordStrings[FQPlotWindow::ColourAlphaControlKeyword, ColourAlphaFixedKeyword], colourSource_);

	
	// Write view setup
	// -- Font Scaling
	parser.writeLineF("%s %f\n", DataFileKeywordStrings[FQPlotWindow::LabelScaleKeyword], labelScale_);
	parser.writeLineF("%s %f\n", DataFileKeywordStrings[FQPlotWindow::TitleScaleKeyword], titleScale_);
	// -- Axes
	parser.writeLineF("%s %i %i %i\n", DataFileKeywordStrings[FQPlotWindow::AxisAutoTicksKeyword], axisAutoTicks_.x, axisAutoTicks_.y, axisAutoTicks_.z);
	parser.writeLineF("%s %f %f %f\n", DataFileKeywordStrings[FQPlotWindow::AxisFirstTickKeyword], axisFirstTick_.x, axisFirstTick_.y, axisFirstTick_.z);
	parser.writeLineF("%s %f %f %f\n", DataFileKeywordStrings[FQPlotWindow::AxisTickDeltaKeyword], axisTickDelta_.x, axisTickDelta_.y, axisTickDelta_.z);
	parser.writeLineF("%s %i %i %i\n", DataFileKeywordStrings[FQPlotWindow::AxisMinorTicksKeyword], axisMinorTicks_.x, axisMinorTicks_.y, axisMinorTicks_.z);
	for (int axis=0; axis<3; ++axis)
	{
		parser.writeLineF("%s %i %f %f %f\n", DataFileKeywordStrings[FQPlotWindow::AxisPositionKeyword], axis, axisPosition_[axis].x, axisPosition_[axis].y, axisPosition_[axis].z);
		parser.writeLineF("%s %i %f %f %f\n", DataFileKeywordStrings[FQPlotWindow::AxisLabelDirectionKeyword], axis, axisLabelDirection_[axis].x, axisLabelDirection_[axis].y, axisLabelDirection_[axis].z);
		parser.writeLineF("%s %i %f %f %f\n", DataFileKeywordStrings[FQPlotWindow::AxisLabelUpKeyword], axis, axisLabelUp_[axis].x, axisLabelUp_[axis].y, axisLabelUp_[axis].z);
	}
	parser.writeLineF("%s %i %i %i\n", DataFileKeywordStrings[FQPlotWindow::AxisLabelRotationKeyword], axisLabelRotation_.x, axisLabelRotation_.y, axisLabelRotation_.z);
	parser.writeLineF("%s %i %i %i\n", DataFileKeywordStrings[FQPlotWindow::AxisInvertKeyword], axisInvert_.x, axisInvert_.y, axisInvert_.z);
	parser.writeLineF("%s %i %i %i\n", DataFileKeywordStrings[FQPlotWindow::AxisLogarithmicKeyword], axisLogarithmic_.x, axisLogarithmic_.y, axisLogarithmic_.z);
	parser.writeLineF("%s %i %i %i\n", DataFileKeywordStrings[FQPlotWindow::AxisVisibleKeyword], axisVisible_.x, axisVisible_.y, axisVisible_.z);
	parser.writeLineF("%s %f %f %f\n", DataFileKeywordStrings[FQPlotWindow::AxisStretchKeyword], axisStretch_.x, axisStretch_.y, axisStretch_.z);
	// -- Transformation Matrix
	Matrix mat = ui.MainView->viewMatrix();
	parser.writeLineF("%s %f %f %f %f\n", DataFileKeywordStrings[FQPlotWindow::ViewMatrixXKeyword], mat[0], mat[1], mat[2], mat[3]);
	parser.writeLineF("%s %f %f %f %f\n", DataFileKeywordStrings[FQPlotWindow::ViewMatrixYKeyword], mat[4], mat[5], mat[6], mat[7]);
	parser.writeLineF("%s %f %f %f %f\n", DataFileKeywordStrings[FQPlotWindow::ViewMatrixZKeyword], mat[8], mat[9], mat[10], mat[11]);
	parser.writeLineF("%s %f %f %f %f\n", DataFileKeywordStrings[FQPlotWindow::ViewMatrixWKeyword], mat[12], mat[13], mat[14], mat[15]);
	if (ui.actionViewPerspective->isChecked()) parser.writeLineF("%s\n", DataFileKeywordStrings[FQPlotWindow::PerspectiveKeyword]);

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
	// Determine surface center and Y clip limits
	Vec3<double> center;
	for (int n=0; n<3; ++n)
	{
		if (axisLogarithmic_[n]) center[n] = (axisInvert_[n] ? log10(limitMax_[n]/limitMin_[n]) : log10(limitMax_[n]*limitMin_[n])) * 0.5 * axisStretch_[n];
		else center[n] = (limitMax_[n]+limitMin_[n]) * 0.5 * axisStretch_[n];
	}
	ui.MainView->setSurfaceCenter(center);
	if (axisLogarithmic_.y) ui.MainView->setYClip(log10(limitMin_.y) * axisStretch_.y, log10(limitMax_.y) * axisStretch_.y);
	else ui.MainView->setYClip(limitMin_.y * axisStretch_.y, limitMax_.y * axisStretch_.y);

	// Make sure view variables are up-to-date
	ui.MainView->setLabelScale(labelScale_);
	ui.MainView->setTitleScale(titleScale_);

	if (dataHasChanged)
	{
		// Clear existing display slices
		surfaceData_.clear();
		double x, y;

		// Loop over slices, apply any transforms (X or Y) and check limits
		Slice* slice = axisInvert_.z ? slices_.last() : slices_.first();
		while (slice)
		{
			// Z
			double z = transformValue(slice->z(), preTransformShift_[2], postTransformShift_[2], transformType_[2], transformValue_[2]);
			// -- Is the transformed Z value within range?
			if ((z < limitMin_.z) || (z > limitMax_.z))
			{
				slice = axisInvert_.z ? slice->prev : slice->next;
				continue;
			}
			if (axisInvert_.z) z = (limitMax_.z - z) + limitMin_.z;
			if (axisLogarithmic_.z) z = log10(z);

			// Add new item to surfaceData_ array
			Slice* surfaceSlice = surfaceData_.add();
			surfaceSlice->setZ(z * axisStretch_.z);

			// Copy / interpolate arrays
			Array<double> array[2];
			if (interpolate_.x)
			{
				slice->data().interpolate(interpolateConstrained_.x);
				double x = limitMin_.x;
				while (x <= limitMax_.x)
				{
					array[0].add(x);
					array[1].add(slice->data().interpolated(x));
					x += interpolationStep_.x;
				}
			}
			else
			{
				array[0] = slice->data().arrayX();
				array[1] = slice->data().arrayY();
			}

			// X / Y
			for (int n=0; n<2; ++n)
			{
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
			if (axisInvert_.x) for (int n=array[0].nItems()-1; n >= 0; --n)
			{
				x = array[0].value(n);
				if ((x < limitMin_.x) || (x > limitMax_.x)) continue;
				if (axisLogarithmic_.x) x = log10(limitMax_.x / x);
				else x = (limitMax_.x - x) + limitMin_.x;
				x *= axisStretch_.x;
				y = array[1].value(n);
				if (axisLogarithmic_.y) y = (axisInvert_.y ? log10(limitMax_.y / y) : log10(y));
				else if (axisInvert_.y) y = (limitMax_.y - y) + limitMin_.y;
				y *= axisStretch_.y;
				surfaceSlice->data().addPoint(x, y);
			}
			else for (int n=0; n<array[0].nItems(); ++n)
			{
				x = array[0].value(n);
				if ((x < limitMin_.x) || (x > limitMax_.x)) continue;
				if (axisLogarithmic_.x) x = log10(x);
				x *= axisStretch_.x;
				y = array[1].value(n);
				if (axisLogarithmic_.y) y = (axisInvert_.y ? log10(limitMax_.y / y) : log10(y));
				else if (axisInvert_.y) y = (limitMax_.y - y) + limitMin_.y;
				y *= axisStretch_.y;
				surfaceSlice->data().addPoint(x, y);
			}

			// Move to next Z slice
			slice = axisInvert_.z ? slice->prev : slice->next;
		}
	}

	// Create temporary colourScale_
	ColourScale scale = colourScale_[colourSource_];
	if (alphaControl_ == FQPlotWindow::FixedAlpha) scale.setAllAlpha(fixedAlpha_);

	// Update surface GL object
	ui.MainView->createSurface(surfaceData_, scale, axisStretch_.y);

	// Construct axes
	for (int axis = 0; axis < 3; ++axis)
	{
		if (!axisVisible_[axis])
		{
			ui.MainView->clearAxisPrimitive(axis);
			continue;
		}

		// Set position, taking into account logarithmic axes and scale factors
		Vec3<double> pos;
		for (int n=0; n<3; ++n)
		{
			if (n == axis) pos.set(n, 0.0);
			else if (axisLogarithmic_[n]) pos.set(n, (axisInvert_[n] ? log10(limitMax_[n] / axisPosition_[axis][n]) : log10(axisPosition_[axis][n])) * axisStretch_[n]);
			else pos.set(n, (axisInvert_[n] ? limitMax_[n] - axisPosition_[axis][n] : axisPosition_[axis][n]) * axisStretch_[n]);
		}

		if (axisLogarithmic_[axis]) ui.MainView->createLogAxis(axis, pos, limitMin_[axis], limitMax_[axis], axisMinorTicks_[axis], axisLabelDirection_[axis], axisLabelUp_[axis], axisLabelRotation_[axis], axisInvert_[axis], axisStretch_[axis]);
		else
		{
			// Calculate autoticks if necessary
			if (axisAutoTicks_[axis]) calculateTickDeltas(axis);

			ui.MainView->createAxis(axis, pos, limitMin_[axis], limitMax_[axis], axisFirstTick_[axis], axisTickDelta_[axis], axisMinorTicks_[axis], axisLabelDirection_[axis], axisLabelUp_[axis], axisLabelRotation_[axis], axisInvert_[axis], axisStretch_[axis]);
		}
	}

	ui.MainView->update();

	ui.NTrianglesLabel->setText("NTriangles: " + QString::number(ui.MainView->surfaceNTriangles()));
}
