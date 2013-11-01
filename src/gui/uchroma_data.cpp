/*
	*** Main Window - Data 
	*** src/gui/uchroma_data.cpp
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

#include "gui/uchroma.h"
#include "base/lineparser.h"

// DataFile Keywords
const char* DataFileKeywordStrings[UChromaWindow::nDataFileKeywords] = { "AxisAutoTicks", "AxisFirstTick", "AxisInvert", "AxisLabelOrientation", "AxisLogarithmic", "AxisMinorTicks", "AxisPosition", "AxisStretch", "AxisTickDelta", "AxisTickDirection", "AxisTitle", "AxisTitleAnchor", "AxisTitleOrientation", "AxisVisible", "BoundingBox", "BoundingBoxPlaneY", "ColourAlphaControl", "ColourAlphaFixed", "ColourCustomGradient", "ColourLinearA", "ColourLinearB", "ColourLinearHSVA", "ColourLinearHSVB", "ColourSingle", "ColourSource", "Data", "ImageExport", "Interpolate", "InterpolateConstrain", "InterpolateStep", "LabelFaceViewer", "LabelScale", "LimitX", "LimitY", "LimitZ", "Perspective", "PostTransformShift", "PreTransformShift", "SliceDir", "Slice", "TitleScale", "TransformX", "TransformY", "TransformZ", "ViewMatrixX", "ViewMatrixY", "ViewMatrixZ", "ViewMatrixW" };
UChromaWindow::DataFileKeyword UChromaWindow::dataFileKeyword(const char* s)
{
	for (int n=0; n<UChromaWindow::nDataFileKeywords; ++n) if (strcmp(s, DataFileKeywordStrings[n]) == 0) return (UChromaWindow::DataFileKeyword) n;
	return UChromaWindow::nDataFileKeywords;
}
const char* UChromaWindow::dataFileKeyword(UChromaWindow::DataFileKeyword dfk)
{
	return DataFileKeywordStrings[dfk];
}

// Data Transform types
const char* DataTransformKeywords[UChromaWindow::nDataTransforms] = { "Multiply", "Divide", "LogBase10", "NaturalLog" };
UChromaWindow::DataTransform UChromaWindow::dataTransform(const char* s)
{
	for (int n=0; n<UChromaWindow::nDataTransforms; ++n) if (strcmp(s, DataTransformKeywords[n]) == 0) return (UChromaWindow::DataTransform) n;
	return UChromaWindow::nDataTransforms;
}
const char* UChromaWindow::dataTransform(UChromaWindow::DataTransform dt)
{
	return DataTransformKeywords[dt];
}

/*
 * Data
 */

// Recalculate tick deltas for specified axis
void UChromaWindow::calculateTickDeltas(int axis)
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
void UChromaWindow::clearData()
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
	transformType_[0] = UChromaWindow::MultiplyTransform;
	transformType_[1] = UChromaWindow::MultiplyTransform;
	transformType_[2] = UChromaWindow::MultiplyTransform;
	preTransformShift_.zero();
	postTransformShift_.zero();
	modified_ = false;
	axisInvert_.set(false, false, false);
	axisVisible_.set(true, true, true);
	labelScale_ = 0.25;
	titleScale_ = 0.3;
	labelFaceViewer_ = false;
	labelCorrectOrientation_ = true;
	axisPosition_[0].zero();
	axisPosition_[1].zero();
	axisPosition_[2].zero();
	axisFirstTick_.zero();
	axisTickDelta_.set(1.0,1.0,1.0);
	axisAutoTicks_.set(true, true, true);
	axisMinorTicks_.set(1,1,1);
	axisTickDirection_[0].set(0.0, -1.0, 0.0);
	axisTickDirection_[1].set(-1.0, 0.0, 0.0);
	axisTickDirection_[2].set(1.0, 0.0, 0.0);
	axisLabelOrientation_[0].set(0.0, 0.0, 0.2);
	axisLabelOrientation_[1].set(0.0, 0.0, 0.2);
	axisLabelOrientation_[2].set(0.0, 180.0, 0.2);
	axisTitle_[0] = "X Axis";
	axisTitle_[1] = "Y Axis";
	axisTitle_[2] = "Z Axis";
	axisTitleOrientation_[0].set(0.0, 0.0, 1.5, 0.5);
	axisTitleOrientation_[1].set(0.0, 0.0, 1.5, 0.5);
	axisTitleOrientation_[2].set(0.0, 0.0, 1.5, 0.5);
	axisTitleAnchor_[0] = UChromaWindow::CentreAlign;
	axisTitleAnchor_[1] = UChromaWindow::CentreAlign;
	axisTitleAnchor_[2] = UChromaWindow::CentreAlign;
	axisLogarithmic_.set(false, false, false);
	axisStretch_.set(1.0, 1.0, 1.0);
	colourSource_ = SingleColourSource;
	colourSinglePoint_.set(0.0, QColor(255,255,255));
	colourRGBGradientAPoint_.set(0.0, QColor(255,255,255));
	colourRGBGradientBPoint_.set(1.0, QColor(0,0,255));
	colourHSVGradientAPoint_.set(0.0, QColor(255,0,0));
	colourHSVGradientBPoint_.set(1.0, QColor(100,40,255));
	customColourScale_.clear();
	fixedAlpha_ = 128;
	alphaControl_ = UChromaWindow::OwnAlpha;
	boundingBox_ = UChromaWindow::NoBox;
	boundingBoxPlaneY_ = 0.0;
	imageExportFile_ = "image.png";
	imageExportWidth_ = 800;
	imageExportHeight_ = 600;
	imageExportMaintainAspect_ = true;
	imageExportFormat_ = Viewer::PNGFormat;
	ui.actionViewPerspective->setChecked(false);
}

// Load data from file specified
bool UChromaWindow::loadData(QString fileName)
{
	LineParser parser;
	parser.openInput(qPrintable(fileName));
	if (!parser.isFileGoodForReading())
	{
		QMessageBox::warning(this, "Error", "Can't open specified file for reading.");
		return false;
	}

	// Read line from file and decide what to do with it
	UChromaWindow::DataFileKeyword kwd;
	UChromaWindow::DataTransform dt;
	Slice* slice;
	int xyz, nPoints;
	Matrix mat;
	while (!parser.eofOrBlank())
	{
		parser.getArgsDelim(LineParser::UseQuotes);
		
		// Get keyword and act accordingly
		kwd = dataFileKeyword(parser.argc(0));
		switch (kwd)
		{
			// Axis auto ticks
			case (UChromaWindow::AxisAutoTicksKeyword):
				axisAutoTicks_.set(parser.argi(1), parser.argi(2), parser.argi(3));
				break;
			// Axis first ticks
			case (UChromaWindow::AxisFirstTickKeyword):
				axisFirstTick_.set(parser.argd(1), parser.argd(2), parser.argd(3));
				break;
			// Axis invert flags
			case (UChromaWindow::AxisInvertKeyword):
				axisInvert_.set(parser.argi(1), parser.argi(2), parser.argi(3));
				break;
			// Axis label orientation
			case (UChromaWindow::AxisLabelOrientationKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) > 2)) printf("Axis index is out of range for %s keyword.\n", dataFileKeyword(kwd));
				else axisLabelOrientation_[parser.argi(1)].set(parser.argd(2), parser.argd(3), parser.argd(4));
				break;
			// Axis logarithmic flag
			case (UChromaWindow::AxisLogarithmicKeyword):
				axisLogarithmic_.set(parser.argi(1), parser.argi(2), parser.argi(3));
				break;
			// Axis minor ticks
			case (UChromaWindow::AxisMinorTicksKeyword):
				axisMinorTicks_.set(parser.argi(1), parser.argi(2), parser.argi(3));
				break;
			// Axis position
			case (UChromaWindow::AxisPositionKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) > 2)) printf("Axis index is out of range for %s keyword.\n", dataFileKeyword(kwd));
				else axisPosition_[parser.argi(1)].set(parser.argd(2), parser.argd(3), parser.argd(4));
				break;
			// Axis stretch factors
			case (UChromaWindow::AxisStretchKeyword):
				axisStretch_.set(parser.argd(1), parser.argd(2), parser.argd(3));
				break;
			// Axis tick deltas
			case (UChromaWindow::AxisTickDeltaKeyword):
				axisTickDelta_.set(parser.argd(1), parser.argd(2), parser.argd(3));
				break;
			// Axis title
			case (UChromaWindow::AxisTitleKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) > 2)) printf("Axis index is out of range for %s keyword.\n", dataFileKeyword(kwd));
				else axisTitle_[parser.argi(1)] = parser.argc(2);
				break;
			// Axis title anchors
			case (UChromaWindow::AxisTitleAnchorKeyword):
				axisTitleAnchor_[0] = (UChromaWindow::HorizontalAlignment) parser.argi(1);
				axisTitleAnchor_[1] = (UChromaWindow::HorizontalAlignment) parser.argi(2);
				axisTitleAnchor_[2] = (UChromaWindow::HorizontalAlignment) parser.argi(3);
				break;
			// Axis title orientation
			case (UChromaWindow::AxisTitleOrientationKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) > 2)) printf("Axis index is out of range for %s keyword.\n", dataFileKeyword(kwd));
				else axisTitleOrientation_[parser.argi(1)].set(parser.argd(2), parser.argd(3), parser.argd(4), parser.argd(4));
				break;
			// Axis visibility
			case (UChromaWindow::AxisVisibleKeyword):
				axisVisible_.set(parser.argi(1), parser.argi(2), parser.argi(3));
				break;
			// Bounding Box
			case (UChromaWindow::BoundingBoxKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) >= UChromaWindow::nBoundingBoxes)) printf("Value is out of range for %s keyword.\n", dataFileKeyword(kwd));
				boundingBox_ = (UChromaWindow::BoundingBox) parser.argi(1);
				break;
			// Bounding Box plane y intercept
			case (UChromaWindow::BoundingBoxPlaneYKeyword):
				boundingBoxPlaneY_ = parser.argd(1);
				break;
			// Colour alpha control
			case (UChromaWindow::ColourAlphaControlKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) >= UChromaWindow::nAlphaControls)) printf("Value is out of range for %s keyword.\n", dataFileKeyword(kwd));
				else alphaControl_ = (UChromaWindow::AlphaControl) parser.argi(1);
				break;
			// Colour alpha fixed value
			case (UChromaWindow::ColourAlphaFixedKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) > 255)) printf("Value is out of range for %s keyword.\n", dataFileKeyword(kwd));
				else fixedAlpha_ = parser.argi(1);
				break;
			// Colour Custom Gradient point definition
			case (UChromaWindow::ColourCustomGradientKeyword):
				customColourScale_.addPoint(parser.argd(1), QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)));
				break;
			// Colour Linear Gradient point definition
			case (UChromaWindow::ColourRGBGradientAKeyword):
			case (UChromaWindow::ColourRGBGradientBKeyword):
				if (kwd == ColourRGBGradientAKeyword) colourRGBGradientAPoint_.set(parser.argd(1), QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)));
				else colourRGBGradientBPoint_.set(parser.argd(1), QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)));
				break;
			// Colour Linear HSV Gradient point definition
			case (UChromaWindow::ColourHSVGradientAKeyword):
			case (UChromaWindow::ColourHSVGradientBKeyword):
				if (kwd == ColourHSVGradientAKeyword) colourHSVGradientAPoint_.set(parser.argd(1), QColor::fromHsv(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)));
				else colourHSVGradientBPoint_.set(parser.argd(1), QColor::fromHsv(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)));
				break;
			// Colour single colour definition
			case (UChromaWindow::ColourSingleKeyword):
				colourSinglePoint_.setColour(QColor(parser.argi(1), parser.argi(2), parser.argi(3), parser.argi(4)));
				break;
			// Colour source
			case (UChromaWindow::ColourSourceKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) >= UChromaWindow::nColourSources)) printf("Value is out of range for %s keyword.\n", dataFileKeyword(kwd));
				else colourSource_ = (UChromaWindow::ColourSource) parser.argi(1);
				break;
			// Data
			case (UChromaWindow::DataKeyword):
				// Get title of associated slice and number of points to expect
				slice = findSlice(parser.argc(1));
				if (slice == NULL)
				{
					msg.print("Data associated to slice '%s' found, but no slice with this title exists.\n", parser.argc(1));
					parser.skipLines(parser.argi(2));
					continue;
				}
				// Load data into slice...
				nPoints = parser.argi(2);
				for (int n=0; n<nPoints; ++n)
				{
					parser.getArgsDelim(LineParser::Defaults);
					slice->data().addPoint(parser.argd(0), parser.argd(1));
				}
				break;
			// Image Export info
			case (UChromaWindow::ImageExportKeyword):
				imageExportFile_ = parser.argc(1);
				imageExportWidth_ = parser.argi(2);
				imageExportHeight_ = parser.argi(3);
				imageExportFormat_ = Viewer::imageFormat(parser.argc(4));
				imageExportMaintainAspect_ = parser.argi(5);
				break;
			// Interpolate flags
			case (UChromaWindow::InterpolateKeyword):
				interpolate_.set(parser.argi(1), false, parser.argi(2));
				break;
			// Interpolate constrain flags
			case (UChromaWindow::InterpolateConstrainKeyword):
				interpolateConstrained_.set(parser.argi(1), false, parser.argi(2));
				break;
			// Interpolation step flags
			case (UChromaWindow::InterpolateStepKeyword):
				interpolationStep_.set(parser.argd(1), 0.0, parser.argd(2));
				break;
			// Labels face viewer flag
			case (UChromaWindow::LabelFaceViewerKeyword):
				labelFaceViewer_ = parser.argb(1);
				break;
			// Label scale
			case (UChromaWindow::LabelScaleKeyword):
				labelScale_ = parser.argd(1);
				break;
			// Limits
			case (UChromaWindow::LimitXKeyword):
			case (UChromaWindow::LimitYKeyword):
			case (UChromaWindow::LimitZKeyword):
				xyz = kwd - UChromaWindow::LimitXKeyword;
				limitMin_[xyz] = parser.argd(1);
				limitMax_[xyz] = parser.argd(2);
				break;
			// Orthographic view
			case (UChromaWindow::PerspectiveKeyword):
				ui.actionViewPerspective->setChecked(true);
				break;
			// Pre-Transform Shifts
			case (UChromaWindow::PreTransformShiftKeyword):
				preTransformShift_.set(parser.argd(1), parser.argd(2), parser.argd(3));
				break;
			// Post-Transform Shifts
			case (UChromaWindow::PostTransformShiftKeyword):
				postTransformShift_.set(parser.argd(1), parser.argd(2), parser.argd(3));
				break;
			// Slice directory
			case (UChromaWindow::SliceDirectoryKeyword):
				dataFileDirectory_ = parser.argc(1);
				if (!dataFileDirectory_.isReadable())
				{
					QMessageBox::StandardButton button = QMessageBox::warning(this, "Error", "The slice directory specified (" + dataFileDirectory_.absolutePath() + ") does not exist or is unreadable.\nDo you want to reset the datafile location?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					if (button == QMessageBox::Yes)
					{
						QString dir = QFileDialog::getExistingDirectory(this, "Data Directory", "Choose the directory containing the required files:");
						if (!dir.isEmpty()) dataFileDirectory_ = dir;
					}
				}
				break;
			// Slice specification
			case (UChromaWindow::SliceKeyword):
				slice = addSlice(parser.argd(1), parser.argc(2), parser.argc(3));
				break;
			// Title scale
			case (UChromaWindow::TitleScaleKeyword):
				titleScale_ = parser.argd(1);
				break;
			// Data Transform
			case (UChromaWindow::TransformXKeyword):
			case (UChromaWindow::TransformYKeyword):
			case (UChromaWindow::TransformZKeyword):
				xyz = kwd - UChromaWindow::TransformXKeyword;
				dt = dataTransform(parser.argc(1));
				transformType_[xyz] = dt;
				transformValue_[xyz] = parser.argd(2);
				break;
			// View Matrix
			case (UChromaWindow::ViewMatrixXKeyword):
			case (UChromaWindow::ViewMatrixYKeyword):
			case (UChromaWindow::ViewMatrixZKeyword):
			case (UChromaWindow::ViewMatrixWKeyword):
				xyz = kwd - UChromaWindow::ViewMatrixXKeyword;
				mat = ui.MainView->viewMatrix();
				mat.setColumn(xyz, parser.argd(1), parser.argd(2), parser.argd(3), parser.argd(4));
				ui.MainView->setViewMatrix(mat);
				break;
		}
	}
	parser.closeFiles();

	// Check for empty slices
	int nEmpty = nEmptySlices();
	if (nEmpty != 0)
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Empty Data", QString("There are ") + QString::number(nEmpty) + " defined slices which contain no data.\nWould you like to reload these now from their source files?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		if (button == QMessageBox::Yes)
		{
			nEmpty = 0;
			for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next) if (!slice->loadData(dataFileDirectory_)) ++nEmpty;
			
			if (nEmpty > 0)
			{
				QMessageBox::StandardButton button = QMessageBox::warning(this, "Empty Data", QString("There are still ") + QString::number(nEmpty) + " defined slices which contain no data or whose original files could not be found.\nCheck the slice data directory, and/or the datafiles themselves.");
			}
		}
	}

	// Set necessary variables
	inputFile_ = fileName;
	modified_ = false;
	
	return true;
}

// Save current data to file specified
bool UChromaWindow::saveData(QString fileName)
{
	LineParser parser;
	parser.openOutput(qPrintable(fileName), true);
	if (!parser.isFileGoodForWriting())
	{
		QMessageBox::warning(this, "Error", "Can't open specified file for writing.");
		return false;
	}

	// Write datafile directory
	parser.writeLineF("%s \"%s\"\n", DataFileKeywordStrings[UChromaWindow::SliceDirectoryKeyword], qPrintable(dataFileDirectory_.absolutePath()));

	// Write list of slices
	// Line format:  slice z "filename" "dataname"
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next)
	{
		parser.writeLineF("%s  %f  \"%s\"  \"%s\"\n", DataFileKeywordStrings[UChromaWindow::SliceKeyword], slice->z(), qPrintable(slice->sourceFileName()), qPrintable(slice->dataName()));
	}

	// Write image info
	parser.writeLineF("%s \"%s\" %i %i %s %i\n", DataFileKeywordStrings[UChromaWindow::ImageExportKeyword], qPrintable(imageExportFile_), imageExportWidth_, imageExportHeight_, Viewer::imageFormatExtension(imageExportFormat_), imageExportMaintainAspect_);

	// Write transform setup
	// -- Transforms
	parser.writeLineF("%s %s %f\n", DataFileKeywordStrings[UChromaWindow::TransformXKeyword], dataTransform(transformType_[0]), transformValue_.x);
	parser.writeLineF("%s %s %f\n", DataFileKeywordStrings[UChromaWindow::TransformYKeyword], dataTransform(transformType_[1]), transformValue_.y);
	parser.writeLineF("%s %s %f\n", DataFileKeywordStrings[UChromaWindow::TransformZKeyword], dataTransform(transformType_[2]), transformValue_.z);
	// -- Shifts
	parser.writeLineF("%s %f %f %f\n", DataFileKeywordStrings[UChromaWindow::PreTransformShiftKeyword], preTransformShift_.x, preTransformShift_.y, preTransformShift_.z);
	parser.writeLineF("%s %f %f %f\n", DataFileKeywordStrings[UChromaWindow::PostTransformShiftKeyword], postTransformShift_.x, postTransformShift_.y, postTransformShift_.z);
	// -- Limits
	parser.writeLineF("%s %f %f\n", DataFileKeywordStrings[UChromaWindow::LimitXKeyword], limitMin_.x, limitMax_.x);
	parser.writeLineF("%s %f %f\n", DataFileKeywordStrings[UChromaWindow::LimitYKeyword], limitMin_.y, limitMax_.y);
	parser.writeLineF("%s %f %f\n", DataFileKeywordStrings[UChromaWindow::LimitZKeyword], limitMin_.z, limitMax_.z);
	// -- Interpolation
	parser.writeLineF("%s %i %i\n", DataFileKeywordStrings[UChromaWindow::InterpolateKeyword], interpolate_.x, interpolate_.z);
	parser.writeLineF("%s %i %i\n", DataFileKeywordStrings[UChromaWindow::InterpolateConstrainKeyword], interpolateConstrained_.x, interpolateConstrained_.z);
	parser.writeLineF("%s %f %f\n", DataFileKeywordStrings[UChromaWindow::InterpolateStepKeyword], interpolationStep_.x, interpolationStep_.z);

	// Write view setup
	// -- Font Scaling / Orientation
	parser.writeLineF("%s %i\n", DataFileKeywordStrings[UChromaWindow::LabelFaceViewerKeyword], labelFaceViewer_);
	parser.writeLineF("%s %f\n", DataFileKeywordStrings[UChromaWindow::LabelScaleKeyword], labelScale_);
	parser.writeLineF("%s %f\n", DataFileKeywordStrings[UChromaWindow::TitleScaleKeyword], titleScale_);
	// -- Axes
	parser.writeLineF("%s %i %i %i\n", DataFileKeywordStrings[UChromaWindow::AxisAutoTicksKeyword], axisAutoTicks_.x, axisAutoTicks_.y, axisAutoTicks_.z);
	parser.writeLineF("%s %f %f %f\n", DataFileKeywordStrings[UChromaWindow::AxisFirstTickKeyword], axisFirstTick_.x, axisFirstTick_.y, axisFirstTick_.z);
	parser.writeLineF("%s %f %f %f\n", DataFileKeywordStrings[UChromaWindow::AxisTickDeltaKeyword], axisTickDelta_.x, axisTickDelta_.y, axisTickDelta_.z);
	parser.writeLineF("%s %i %i %i\n", DataFileKeywordStrings[UChromaWindow::AxisMinorTicksKeyword], axisMinorTicks_.x, axisMinorTicks_.y, axisMinorTicks_.z);
	parser.writeLineF("%s %i %i %i\n", DataFileKeywordStrings[UChromaWindow::AxisTitleAnchorKeyword], axisTitleAnchor_.x, axisTitleAnchor_.y, axisTitleAnchor_.z);
	for (int axis=0; axis<3; ++axis)
	{
		parser.writeLineF("%s %i %f %f %f\n", DataFileKeywordStrings[UChromaWindow::AxisPositionKeyword], axis, axisPosition_[axis].x, axisPosition_[axis].y, axisPosition_[axis].z);
		parser.writeLineF("%s %i %f %f %f\n", DataFileKeywordStrings[UChromaWindow::AxisLabelOrientationKeyword], axis, axisLabelOrientation_[axis].x, axisLabelOrientation_[axis].y, axisLabelOrientation_[axis].z);
		parser.writeLineF("%s %i '%s'\n", DataFileKeywordStrings[UChromaWindow::AxisTitleKeyword], axis, qPrintable(axisTitle_[axis]));
		parser.writeLineF("%s %i %f %f %f %f\n", DataFileKeywordStrings[UChromaWindow::AxisTitleOrientationKeyword], axis, axisTitleOrientation_[axis].x, axisTitleOrientation_[axis].y, axisTitleOrientation_[axis].z, axisTitleOrientation_[axis].w);
	}
	parser.writeLineF("%s %i %i %i\n", DataFileKeywordStrings[UChromaWindow::AxisInvertKeyword], axisInvert_.x, axisInvert_.y, axisInvert_.z);
	parser.writeLineF("%s %i %i %i\n", DataFileKeywordStrings[UChromaWindow::AxisLogarithmicKeyword], axisLogarithmic_.x, axisLogarithmic_.y, axisLogarithmic_.z);
	parser.writeLineF("%s %i %i %i\n", DataFileKeywordStrings[UChromaWindow::AxisVisibleKeyword], axisVisible_.x, axisVisible_.y, axisVisible_.z);
	parser.writeLineF("%s %f %f %f\n", DataFileKeywordStrings[UChromaWindow::AxisStretchKeyword], axisStretch_.x, axisStretch_.y, axisStretch_.z);
	// -- Transformation Matrix
	Matrix mat = ui.MainView->viewMatrix();
	parser.writeLineF("%s %f %f %f %f\n", DataFileKeywordStrings[UChromaWindow::ViewMatrixXKeyword], mat[0], mat[1], mat[2], mat[3]);
	parser.writeLineF("%s %f %f %f %f\n", DataFileKeywordStrings[UChromaWindow::ViewMatrixYKeyword], mat[4], mat[5], mat[6], mat[7]);
	parser.writeLineF("%s %f %f %f %f\n", DataFileKeywordStrings[UChromaWindow::ViewMatrixZKeyword], mat[8], mat[9], mat[10], mat[11]);
	parser.writeLineF("%s %f %f %f %f\n", DataFileKeywordStrings[UChromaWindow::ViewMatrixWKeyword], mat[12], mat[13], mat[14], mat[15]);
	if (ui.actionViewPerspective->isChecked()) parser.writeLineF("%s\n", DataFileKeywordStrings[UChromaWindow::PerspectiveKeyword]);

	// Write colour setup
	parser.writeLineF("%s %i\n", DataFileKeywordStrings[UChromaWindow::ColourSourceKeyword], colourSource_);
	ColourScalePoint* csp;
	// -- Single Colour
	parser.writeLineF("%s %i %i %i %i\n", DataFileKeywordStrings[UChromaWindow::ColourSingleKeyword], colourSinglePoint_.colour().red(), colourSinglePoint_.colour().green(), colourSinglePoint_.colour().blue(), colourSinglePoint_.colour().alpha());
	// -- RGB Gradient
	parser.writeLineF("%s %f %i %i %i %i\n", DataFileKeywordStrings[UChromaWindow::ColourRGBGradientAKeyword], colourRGBGradientAPoint_.value(), colourRGBGradientAPoint_.colour().red(), colourRGBGradientAPoint_.colour().green(), colourRGBGradientAPoint_.colour().blue(), colourRGBGradientAPoint_.colour().alpha());
	parser.writeLineF("%s %f %i %i %i %i\n", DataFileKeywordStrings[UChromaWindow::ColourRGBGradientBKeyword], colourRGBGradientBPoint_.value(), colourHSVGradientBPoint_.colour().red(), colourHSVGradientBPoint_.colour().green(), colourHSVGradientBPoint_.colour().blue(), colourHSVGradientBPoint_.colour().alpha());
	// -- HSV Gradient
	parser.writeLineF("%s %f %i %i %i %i\n", DataFileKeywordStrings[UChromaWindow::ColourHSVGradientAKeyword], colourHSVGradientAPoint_.value(), colourHSVGradientAPoint_.colour().hue(), colourHSVGradientAPoint_.colour().saturation(), colourHSVGradientAPoint_.colour().value(), colourHSVGradientAPoint_.colour().alpha());
	parser.writeLineF("%s %f %i %i %i %i\n", DataFileKeywordStrings[UChromaWindow::ColourHSVGradientBKeyword], colourHSVGradientBPoint_.value(), colourHSVGradientBPoint_.colour().hue(), colourHSVGradientBPoint_.colour().saturation(), colourHSVGradientBPoint_.colour().value(), colourHSVGradientBPoint_.colour().alpha());

	// -- Custom Gradient
	for (csp = customColourScale_.firstPoint(); csp != NULL; csp = csp->next)
	{
		parser.writeLineF("%s %f %i %i %i %i\n", DataFileKeywordStrings[UChromaWindow::ColourCustomGradientKeyword], csp->value(), csp->colour().red(), csp->colour().green(), csp->colour().blue(), csp->colour().alpha());
	}
	// -- Alpha control
	parser.writeLineF("%s %i\n", DataFileKeywordStrings[UChromaWindow::ColourAlphaControlKeyword], alphaControl_);
	parser.writeLineF("%s %i\n", DataFileKeywordStrings[UChromaWindow::ColourAlphaFixedKeyword], fixedAlpha_);

	// Extras setup
	parser.writeLineF("%s %i\n", DataFileKeywordStrings[UChromaWindow::BoundingBoxKeyword], boundingBox_);
	parser.writeLineF("%s %f\n", DataFileKeywordStrings[UChromaWindow::BoundingBoxPlaneYKeyword], boundingBoxPlaneY_);

	// Data Section
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next)
	{
		Data2D& data = slice->data();
		parser.writeLineF("%s  \"%s\"  %i\n", DataFileKeywordStrings[UChromaWindow::DataKeyword], qPrintable(slice->dataName()), data.nPoints());
		for (int n=0; n<data.nPoints(); ++n) parser.writeLineF("%10.4e  %10.4e\n", data.x(n), data.y(n));
	}

	parser.closeFiles();
	return true;
}

// Add slice
Slice* UChromaWindow::addSlice(double z, QString fileName, QString title)
{
	// Create new slice
	Slice* slice = slices_.add();

	if (fileName.length() > 0)
	{
		QString relativeFileName = dataFileDirectory_.relativeFilePath(fileName);

		// Is this file already loaded
		Slice* otherSlice = NULL;
		for (otherSlice = slices_.first(); otherSlice != NULL; otherSlice = otherSlice->next) if (relativeFileName == otherSlice->sourceFileName()) break;
		if (otherSlice)
		{
			printf("File %s is already loaded - will not load it again.\n", qPrintable(relativeFileName));
			return NULL;
		}
		slice->setSourceFileName(relativeFileName);
	}
	slice->setDataName(title);
	slice->setZ(z);

	return slice;
}

// Find slice with corresponding data name
Slice* UChromaWindow::findSlice(QString dataName)
{
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next) if (slice->dataName() == dataName) return slice;
	return NULL;
}

// Return number of slices with no data present
int UChromaWindow::nEmptySlices()
{
	int count = 0;
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next) if (slice->data().nPoints() < 2) ++count;
	return count;
}

// Recalculate data limits
void UChromaWindow::calculateDataLimits()
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
void UChromaWindow::calculateTransformLimits()
{
	transformMin_ = 0.0;
	transformMax_ = 0.0;
	if (slices_.nItems() == 0) return;
	
	for (int n=0; n<3; ++n)
	{
		transformMin_[n] = transformValue(dataMin_[n], n);
		transformMax_[n] = transformValue(dataMax_[n], n);

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

// Set display limits to show all available data
void UChromaWindow::showAllData()
{
	limitMin_ = transformMin_;
	limitMax_ = transformMax_;
}

// Transform single value
double UChromaWindow::transformValue(double x, int axis)
{
	switch (transformType_[axis])
	{
		case (UChromaWindow::MultiplyTransform):
			return  (x+preTransformShift_[axis])*transformValue_[axis] + postTransformShift_[axis];
			break;
		case (UChromaWindow::DivideTransform):
			return (x+preTransformShift_[axis])/transformValue_[axis] + postTransformShift_[axis];
			break;
		case (UChromaWindow::LogBase10Transform):
			return log10(x+preTransformShift_[axis]) + postTransformShift_[axis];
			break;
		case (UChromaWindow::NaturalLogTransform):
			return log(x+preTransformShift_[axis]) + postTransformShift_[axis];
			break;
	}
	return 0.0;
}

// Set limits to show all data
void UChromaWindow::showAll(bool changeX, bool changeY, bool changeZ)
{
	bool flags[3] = { changeX, changeY, changeZ };
	for (int axis = 0; axis < 3; ++axis) if (flags[axis])
	{
		limitMin_[axis] = transformMin_[axis];
		limitMax_[axis] = transformMax_[axis];
	}
}

// Flag data as modified, and update titlebar
void UChromaWindow::setAsModified()
{
	modified_ = true;
	updateTitleBar();
}

// Update surface data after data change
void UChromaWindow::updateSurface(bool dataHasChanged)
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

	// Construct axes
	for (int axis = 0; axis < 3; ++axis)
	{
		// Clear old axis primitives
		ui.MainView->clearAxisPrimitives(axis);

		// Create label and title transformation matrices
		Matrix labelTransform, titleTransform;
		// Set left-to-right direction of text to be along axis direction
		Vec3<double> leftToRight(0.0,0.0,0.0);
		leftToRight.set(axis, 1.0);
		// Our basic 'up' vector will be another orthogonal axis (but never z)
		Vec3<double> upVector(0.0,0.0,0.0);
		if (axis == 0) upVector.set(0.0, 1.0, 0.0);
		else upVector.set(-1.0, 0.0, 0.0);
		labelTransform.setColumn(0, leftToRight, 0.0);
		labelTransform.setColumn(1, upVector, 0.0);
		labelTransform.setColumn(2, leftToRight * upVector, 0.0);
		labelTransform.setColumn(3, 0.0, 0.0, 0.0, 1.0);
		titleTransform = labelTransform;
	
		if (labelFaceViewer_)
		{
			labelTransform = ui.MainView->viewMatrix();
			labelTransform.removeTranslationAndScaling();
			labelTransform.invert();

			titleTransform = ui.MainView->viewMatrix();
			titleTransform.removeTranslationAndScaling();
			titleTransform.invert();
		}
		else
		{
			// Label Transform
			// -- Apply rotation out of AB plane...
// 			labelTransform.applyTranslation(0.0, 1.0, 0.0);
			Matrix rotMat;
			rotMat.createRotationZ(axisLabelOrientation_[axis].y);
			rotMat.print();
			rotMat.createRotationAxis(0.0, 0.0, -1.0, axisLabelOrientation_[axis].y, false);
			rotMat.print();
			labelTransform *= rotMat;
// 			labelTransform.applyRotationAxis(0.0, 0.0, -1.0, axisLabelOrientation_[axis].y, false);
// 			labelTransform.applyRotationAxis(leftToRight.x, leftToRight.y, leftToRight.z, axisLabelOrientation_[axis].x, false);
			upVector = labelTransform.columnAsVec3(1);
			// ...and in current text plane

			// Title transform
			// -- Apply rotation out of AB plane...
			titleTransform.applyRotationAxis(leftToRight.x, leftToRight.y, leftToRight.z, axisTitleOrientation_[axis].x, false);
			upVector = titleTransform.columnAsVec3(1);
			// ...and in current text plane
			titleTransform.applyRotationAxis(titleTransform[8], titleTransform[9], titleTransform[10], axisTitleOrientation_[axis].y, false);
			titleTransform.setTranslation(0.0, axisTitleOrientation_[axis].z, 0.0);
		}
		labelTransform.columnMultiply( Vec3<double>(labelScale_,labelScale_,labelScale_) );
		titleTransform.columnMultiply( Vec3<double>(titleScale_,titleScale_,titleScale_) );

		// Set position, taking into account logarithmic axes and scale factors
		Vec3<double> pos;
		for (int n=0; n<3; ++n)
		{
			if (n == axis) pos.set(n, 0.0);
			else if (axisLogarithmic_[n]) pos.set(n, (axisInvert_[n] ? log10(limitMax_[n] / axisPosition_[axis][n]) : log10(axisPosition_[axis][n])) * axisStretch_[n]);
			else pos.set(n, (axisInvert_[n] ? limitMax_[n] - axisPosition_[axis][n] : axisPosition_[axis][n]) * axisStretch_[n]);
		}

		if (axisLogarithmic_[axis])
		{
			ui.MainView->createLogAxis(axis, pos, limitMin_[axis], limitMax_[axis], axisInvert_[axis], axisStretch_[axis], axisMinorTicks_[axis], axisTickDirection_[axis], labelTransform);
		}
		else
		{
			// Calculate autoticks if necessary
			if (axisAutoTicks_[axis]) calculateTickDeltas(axis);

			ui.MainView->createAxis(axis, pos, limitMin_[axis], limitMax_[axis], axisInvert_[axis], axisStretch_[axis], axisFirstTick_[axis], axisTickDelta_[axis], axisMinorTicks_[axis], axisTickDirection_[axis], axisLabelOrientation_[axis], axisTitleOrientation_[axis], axisTitleAnchor_[axis], axisTitle_[axis]);
		}

		ui.MainView->setAxisVisible(axis, axisVisible_[axis]);
	}

	// Reconstruct surface
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
			double z = transformValue(slice->z(), 2);
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
					case (UChromaWindow::MultiplyTransform):
						array[n] *= transformValue_[n];
						break;
					case (UChromaWindow::DivideTransform):
						array[n] /= transformValue_[n];
						break;
					case (UChromaWindow::LogBase10Transform):
						array[n].takeLog();
						break;
					case (UChromaWindow::NaturalLogTransform):
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
				if (axisLogarithmic_.y)
				{
					if (y < 0.0) y = 1.0e-10;
					else y = (axisInvert_.y ? log10(limitMax_.y / y) : log10(y));
				}
				else if (axisInvert_.y) y = (limitMax_.y - y) + limitMin_.y;
				y *= axisStretch_.y;
				surfaceSlice->data().addPoint(x, y);
			}

			// Move to next Z slice
			slice = axisInvert_.z ? slice->prev : slice->next;
		}
	}

	// Create temporary colourScale_
	ColourScale scale = colourScale_;
	if (alphaControl_ == UChromaWindow::FixedAlpha) scale.setAllAlpha(fixedAlpha_);

	// Update surface GL object
	ui.MainView->createSurface(scale, axisStretch_.y);

	// Setup Bounding Box
	ui.MainView->createBoundingBox(boundingBox_, axisLogarithmic_.y ? log10(boundingBoxPlaneY_) : boundingBoxPlaneY_);

	ui.MainView->update();

	ui.NTrianglesLabel->setText("NTriangles: " + QString::number(ui.MainView->surfaceNTriangles()));
}
