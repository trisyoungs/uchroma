/*
	*** File I/O 
	*** src/gui/io.cpp
	Copyright T. Youngs 2013-2014

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
#include "gui/keywords.h"
#include "base/lineparser.h"

// Return boolean string based on integer value
const char* stringBool(int i)
{
	return (i > 0 ? "true" : "false");
}

// Return boolean string based on boolean value
const char* stringBool(bool b)
{
	return (b ? "true" : "false");
}

// Parse AxisBlock keywords
bool UChromaWindow::readAxisBlock(LineParser& parser, int axis)
{
	while (!parser.eofOrBlank())
	{
		// Get line from file
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks);

		// Get keyword and check number of arguments provided
		Keywords::AxisKeyword axisKwd = Keywords::axisKeyword(parser.argc(0));
		if ((axisKwd != Keywords::nAxisKeywords) && (Keywords::axisKeywordNArguments(axisKwd) > (parser.nArgs()-1)))
		{
			msg.print("Error : Axis keyword '%s' requires %i arguments, but only %i have been provided.\n", Keywords::axisKeyword(axisKwd), Keywords::axisKeywordNArguments(axisKwd), parser.nArgs()-1);
			return false;
		}
		switch (axisKwd)
		{
			// Auto ticks
			case (Keywords::AutoTicksKeyword):
				axisAutoTicks_[axis] = parser.argb(1);
				break;
			// End input block
			case (Keywords::EndAxisKeyword):
				return true;
				break;
			// First ticks
			case (Keywords::FirstTickKeyword):
				axisFirstTick_[axis] = parser.argd(1);
				break;
			// Invert
			case (Keywords::InvertKeyword):
				axisInverted_[axis] = parser.argb(1);
				break;
			// Axis label orientation
			case (Keywords::LabelOrientationKeyword):
				axisLabelOrientation_[axis].set(parser.argd(1), parser.argd(2), parser.argd(3));
				break;
			// Limits
			case (Keywords::LimitsKeyword):
				axisMin_[axis] = parser.argd(1);
				axisMax_[axis] = parser.argd(2);
				break;
			// Axis logarithmic flag
			case (Keywords::LogarithmicKeyword):
				axisLogarithmic_[axis] = parser.argb(1);
				break;
			// Axis minor ticks
			case (Keywords::MinorTicksKeyword):
				axisMinorTicks_[axis] = parser.argi(1);
				break;
			// Axis position
			case (Keywords::PositionKeyword):
				axisPosition_[axis].set(parser.argd(1), parser.argd(2), parser.argd(3));
				break;
			// Axis stretch factors
			case (Keywords::StretchKeyword):
				axisStretch_[axis] = parser.argd(1);
				break;
			// Axis tick deltas
			case (Keywords::TickDeltaKeyword):
				axisTickDelta_[axis] = parser.argd(1);
				break;
			// Axis title
			case (Keywords::TitleKeyword):
				axisTitle_[axis] = parser.argc(1);
				break;
			// Axis title anchors
			case (Keywords::TitleAnchorKeyword):
				axisTitleAnchor_[axis] = (TextPrimitive::HorizontalAnchor) parser.argi(1);
				break;
			// Axis title orientation
			case (Keywords::TitleOrientationKeyword):
				axisTitleOrientation_[axis].set(parser.argd(1), parser.argd(2), parser.argd(3), parser.argd(4));
				break;
			// Axis visibility
			case (Keywords::VisibleKeyword):
				axisVisible_[axis] = parser.argb(1);
				break;
			// Unrecognised keyword
			default:
				msg.print("Error : Unrecognised axis keyword: %s\n", parser.argc(0));
				return false;
		}
	}
	msg.print("Error : Unterminated 'Axis' block.\n");
	return false;
}

// Read CollectionBlock keywords
bool UChromaWindow::readCollectionBlock(LineParser& parser, Collection* collection)
{
	Slice* slice;
	int xyz;
	while (!parser.eofOrBlank())
	{
		// Get line from file
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks);

		// Get keyword and check number of arguments provided
		Keywords::CollectionKeyword collectionKwd = Keywords::collectionKeyword(parser.argc(0));
		if ((collectionKwd != Keywords::nCollectionKeywords) && (Keywords::collectionKeywordNArguments(collectionKwd) > (parser.nArgs()-1)))
		{
			msg.print("Error : Collection keyword '%s' requires %i arguments, but only %i have been provided.\n", Keywords::collectionKeyword(collectionKwd), Keywords::collectionKeywordNArguments(collectionKwd), parser.nArgs()-1);
			return false;
		}
		switch (collectionKwd)
		{
			// Colour alpha control
			case (Keywords::ColourAlphaControlKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) >= Collection::nAlphaControls)) msg.print("Value is out of range for %s keyword.\n", Keywords::collectionKeyword(collectionKwd));
				else collection->setAlphaControl((Collection::AlphaControl) parser.argi(1));
				break;
			// Colour alpha fixed value
			case (Keywords::ColourAlphaFixedKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) > 255)) msg.print("Value is out of range for %s keyword.\n", Keywords::collectionKeyword(collectionKwd));
				else collection->setFixedAlpha(parser.argi(1));
				break;
			// Colour Custom Gradient point definition
			case (Keywords::ColourCustomGradientKeyword):
				collection->addCustomColourScalePoint(parser.argd(1), QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)));
				break;
			// Colour Linear Gradient point definition
			case (Keywords::ColourRGBGradientAKeyword):
			case (Keywords::ColourRGBGradientBKeyword):
				collection->setColourScalePoint(Collection::RGBGradientSource, QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)), parser.argd(1), collectionKwd == Keywords::ColourRGBGradientAKeyword ? 0 : 1);
				break;
			// Colour Linear HSV Gradient point definition
			case (Keywords::ColourHSVGradientAKeyword):
			case (Keywords::ColourHSVGradientBKeyword):
				collection->setColourScalePoint(Collection::HSVGradientSource, QColor::fromHsv(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)), parser.argd(1), collectionKwd == Keywords::ColourHSVGradientAKeyword ? 0 : 1);
				break;
			// Colour single colour definition
			case (Keywords::ColourSingleKeyword):
				collection->setColourScalePoint(Collection::SingleColourSource, QColor(parser.argi(1), parser.argi(2), parser.argi(3), parser.argi(4)));
				break;
			// Colour source
			case (Keywords::ColourSourceKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) >= Collection::nColourSources)) msg.print("Value is out of range for %s keyword.\n", Keywords::collectionKeyword(collectionKwd));
				else collection->setColourSource((Collection::ColourSource) parser.argi(1));
				break;
				// Slice directory
			case (Keywords::DataDirectoryKeyword):
				collection->setDataFileDirectory(QDir(parser.argc(1)));
				if (!collection->dataFileDirectory().isReadable())
				{
					QMessageBox::StandardButton button = QMessageBox::warning(this, "Error", "The data directory specified (" + collection->dataFileDirectory().absolutePath() + ") does not exist or is unreadable.\nDo you want to reset the datafile location?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					if (button == QMessageBox::Yes)
					{
						QString dir = QFileDialog::getExistingDirectory(this, "Data Directory", "Choose the directory containing the required files:");
						if (!dir.isEmpty()) collection->setDataFileDirectory(dir);
					}
				}
				break;
			// End input block
			case (Keywords::EndCollectionKeyword):
				return true;
				break;
			// Interpolate flags
			case (Keywords::InterpolateKeyword):
				collection->setInterpolate(0, parser.argb(1));
				collection->setInterpolate(2, parser.argb(2));
				break;
			// Interpolate constrain flags
			case (Keywords::InterpolateConstrainKeyword):
				collection->setInterpolateConstrained(0, parser.argb(1));
				collection->setInterpolateConstrained(2, parser.argb(2));
				break;
			// Interpolation step flags
			case (Keywords::InterpolateStepKeyword):
				collection->setInterpolationStep(0, parser.argd(1));
				collection->setInterpolationStep(2, parser.argd(2));
				break;
			// Slice
			case (Keywords::SliceDefinitionKeyword):
				// Create new slice
				slice = collection->addSlice();
				slice->setTitle(parser.argc(1));
				if (!readSliceBlock(parser, slice, collection)) return false;
				break;
			// Data Transform
			case (Keywords::TransformXKeyword):
			case (Keywords::TransformYKeyword):
			case (Keywords::TransformZKeyword):
				xyz = collectionKwd - Keywords::TransformXKeyword;
				collection->setTransformEnabled(xyz, parser.argb(1));
				collection->setTransformEquation(xyz,  parser.argc(2));
				break;
			// Unrecognised Keyword
			default:
				msg.print("Error : Unrecognised collection keyword: %s\n", parser.argc(0));
				return false;
		}
	}
	msg.print("Error : Unterminated 'Collection' block.\n");
	return false;
}

// Read SettingsBlock keywords
bool UChromaWindow::readSettingsBlock(LineParser& parser)
{
	while (!parser.eofOrBlank())
	{
		// Get line from file
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks);

		// Get keyword and check number of arguments provided
		Keywords::SettingsKeyword settingsKwd = Keywords::settingsKeyword(parser.argc(0));
		if ((settingsKwd != Keywords::nSettingsKeywords) && (Keywords::settingsKeywordNArguments(settingsKwd) > (parser.nArgs()-1)))
		{
			msg.print("Error : Settings keyword '%s' requires %i arguments, but only %i have been provided.\n", Keywords::settingsKeyword(settingsKwd), Keywords::settingsKeywordNArguments(settingsKwd), parser.nArgs()-1);
			return false;
		}
		switch (settingsKwd)
		{
			// End input block
			case (Keywords::EndSettingsKeyword):
				return true;
				break;
			// Image Export info
			case (Keywords::ImageExportKeyword):
				imageExportFile_ = parser.argc(1);
				imageExportWidth_ = parser.argi(2);
				imageExportHeight_ = parser.argi(3);
				imageExportFormat_ = Viewer::imageFormat(parser.argc(4));
				imageExportMaintainAspect_ = parser.argi(5);
				break;
			// Unrecognised Keyword
			default:
				msg.print("Error : Unrecognised settings keyword: %s\n", parser.argc(0));
				return false;
		}
	}
	msg.print("Error : Unterminated 'Settings' block.\n");
	return false;
}

// Read SliceBlock keywords
bool UChromaWindow::readSliceBlock(LineParser& parser, Slice* slice, Collection* collection)
{
	int nPoints;
	bool foundEnd;
	Slice::DataSource source;
	while (!parser.eofOrBlank())
	{
		// Get line from file
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks);

		// Get keyword and check number of arguments provided
		Keywords::SliceKeyword sliceKwd = Keywords::sliceKeyword(parser.argc(0));
		if ((sliceKwd != Keywords::nSliceKeywords) && (Keywords::sliceKeywordNArguments(sliceKwd) > (parser.nArgs()-1)))
		{
			msg.print("Error : Slice keyword '%s' requires %i arguments, but only %i have been provided.\n", Keywords::sliceKeyword(sliceKwd), Keywords::sliceKeywordNArguments(sliceKwd), parser.nArgs()-1);
			return false;
		}
		switch (sliceKwd)
		{
			case (Keywords::DataKeyword):
				slice->data().reset();
				foundEnd = false;
				do
				{
					parser.getArgsDelim(LineParser::Defaults);
					// Check for 'EndData'
					if (strcmp(parser.argc(0),"EndData") == 0) foundEnd = true;
					else slice->data().addPoint(parser.argd(0), parser.argd(1));
				} while ((!foundEnd) && (!parser.eofOrBlank()));
				if (!foundEnd)
				{
					msg.print("Error : Unterminated 'Data' block in slice '%s'.\n", qPrintable(slice->title()));
					return false;
				}
				break;
			case (Keywords::EndSliceKeyword):
				return true;
				break;
			case (Keywords::SourceKeyword):
				source = Slice::dataSource(parser.argc(1));
				if (source == Slice::nDataSources)
				{
					msg.print("Error :Datasource for slice not recognised (%s)\n", parser.argc(1));
					return false;
				}
				slice->setDataSource(source);
				// Depending on the source, we might expect other data here...
				if (source == Slice::FileSource)
				{
					if (parser.hasArg(2)) slice->setSourceFileName(parser.argc(2));
					else
					{
						msg.print("Error :Expected data file name after 'Source File' declaration in slice '%s'.\n", qPrintable(slice->title()));
						return false;
					}
				}
				break;
			case (Keywords::ZKeyword):
				collection->setSliceZ(slice, parser.argd(1));
				break;
			// Unrecognised Keyword
			default:
				msg.print("Error : Unrecognised slice keyword: %s\n", parser.argc(0));
				return false;
		}
	}
	msg.print("Error : Unterminated 'Slice' block.\n");
	return false;

}

// Read AxisBlock keywords
bool UChromaWindow::readViewBlock(LineParser& parser)
{
	int xyzw;
	Matrix mat;
	while (!parser.eofOrBlank())
	{
		// Get line from file
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks);

		// Get keyword and check number of arguments provided
		Keywords::ViewKeyword viewKwd = Keywords::viewKeyword(parser.argc(0));
		if ((viewKwd != Keywords::nViewKeywords) && (Keywords::viewKeywordNArguments(viewKwd) > (parser.nArgs()-1)))
		{
			msg.print("Error : View keyword '%s' requires %i arguments, but only %i have been provided.\n", Keywords::viewKeyword(viewKwd), Keywords::viewKeywordNArguments(viewKwd), parser.nArgs()-1);
			return false;
		}
		switch (viewKwd)
		{
			// Bounding Box
			case (Keywords::BoundingBoxKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) >= UChromaWindow::nBoundingBoxes)) msg.print("Value is out of range for %s keyword.\n", Keywords::viewKeyword(viewKwd));
				boundingBox_ = (UChromaWindow::BoundingBox) parser.argi(1);
				break;
			// Bounding Box plane y intercept
			case (Keywords::BoundingBoxPlaneYKeyword):
				boundingBoxPlaneY_ = parser.argd(1);
				break;
			// End input block
			case (Keywords::EndViewKeyword):
				return true;
				break;
			// Labels face viewer flag
			case (Keywords::LabelFaceViewerKeyword):
				labelFaceViewer_ = parser.argb(1);
				break;
			// Label scale
			case (Keywords::LabelScaleKeyword):
				labelScale_ = parser.argd(1);
				break;
			// Orthographic view
			case (Keywords::PerspectiveKeyword):
				ui.actionViewPerspective->setChecked(true);
				break;
			// Title scale
			case (Keywords::TitleScaleKeyword):
				titleScale_ = parser.argd(1);
				break;
			// View Matrix
			case (Keywords::MatrixXKeyword):
			case (Keywords::MatrixYKeyword):
			case (Keywords::MatrixZKeyword):
			case (Keywords::MatrixWKeyword):
				xyzw = viewKwd - Keywords::MatrixXKeyword;
				mat = ui.MainView->viewMatrix();
				mat.setColumn(xyzw, parser.argd(1), parser.argd(2), parser.argd(3), parser.argd(4));
				ui.MainView->setViewMatrix(mat);
				break;
			// Unrecognised Keyword
			default:
				msg.print("Error : Unrecognised view keyword: %s\n", parser.argc(0));
				return false;
		}
	}
	msg.print("Error : Unterminated 'View' block.\n");
	return false;
}

// Load data from file specified
bool UChromaWindow::loadInputFile(QString fileName)
{
	LineParser parser;
	parser.openInput(qPrintable(fileName));
	if (!parser.isFileGoodForReading())
	{
		QMessageBox::warning(this, "Error", "Can't open specified file for reading.");
		return false;
	}

	// Clear existing data
	clearData();

	// Read line from file and decide what to do with it
	Keywords::InputBlock block;
	int axis, nEmpty;
	bool success;
	while (!parser.eofOrBlank())
	{
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks);

		// We expect a block keyword in this loop...
		block = Keywords::inputBlock(parser.argc(0));
		switch (block)
		{
			// Axis Block
			case (Keywords::AxisBlock):
				// Get target axis...
				axis = parser.argi(1);
				if ((axis < 0) || (axis > 2))
				{
					QMessageBox::warning(this, "Error", "Axis index is out of range in input file.");
					return false;
				}
				success = readAxisBlock(parser, axis);
				break;
			// Collection Block
			case (Keywords::CollectionBlock):
				// Create new Collection and set its title
				currentCollection_ = collections_.add();
				ui.MainView->addSurfacePrimitive(&currentCollection_->surfacePrimitive());
				currentCollection_->setTitle(parser.argc(1));
				success = readCollectionBlock(parser, currentCollection_);
				// Check for empty slices
				nEmpty = currentCollection_->nEmptySlices();
				if (nEmpty != 0)
				{
					QMessageBox::StandardButton button = QMessageBox::warning(this, "Empty Data", QString("There are ") + QString::number(nEmpty) + " defined slices which contain no data in collection '" + currentCollection_->title() + "'.\nWould you like to reload these now from their source files?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					if (button == QMessageBox::Yes)
					{
						nEmpty = currentCollection_->loadAllSlices();
						
						if (nEmpty > 0)
						{
							QMessageBox::StandardButton button = QMessageBox::warning(this, "Empty Data", QString("There are still ") + QString::number(nEmpty) + " defined slices which contain no data or whose original files could not be found.\nCheck the slice data directory, and/or the datafiles themselves.");
						}
					}
				}
				break;
			// Settings
			case (Keywords::SettingsBlock):
				success = readSettingsBlock(parser);
				break;
			// View
			case (Keywords::ViewBlock):
				success = readViewBlock(parser);
				break;
			default:
				msg.print("Error : Unrecognised input block: %s\n", parser.argc(0));
				return false;
		}
		if (!success) return false;
	}
	parser.closeFiles();

	// Set necessary variables
	inputFile_ = fileName;
	modified_ = false;
	
	return true;
}

// Save current data to file specified
bool UChromaWindow::saveInputFile(QString fileName)
{
	LineParser parser;
	parser.openOutput(qPrintable(fileName), true);
	if (!parser.isFileGoodForWriting())
	{
		QMessageBox::warning(this, "Error", "Can't open specified file for writing.");
		return false;
	}
	// Write axis definitions
	for (int axis=0; axis < 3; ++axis)
	{
		parser.writeLineF("Axis %i\n", axis);
		parser.writeLineF("  %s %s\n", Keywords::axisKeyword(Keywords::AutoTicksKeyword), stringBool(axisAutoTicks_[axis]));
		parser.writeLineF("  %s %f\n", Keywords::axisKeyword(Keywords::FirstTickKeyword), axisFirstTick_[axis]);
		parser.writeLineF("  %s %f\n", Keywords::axisKeyword(Keywords::TickDeltaKeyword), axisTickDelta_[axis]);
		parser.writeLineF("  %s %i\n", Keywords::axisKeyword(Keywords::MinorTicksKeyword), axisMinorTicks_[axis]);
		parser.writeLineF("  %s %i\n", Keywords::axisKeyword(Keywords::TitleAnchorKeyword), axisTitleAnchor_[axis]);
		parser.writeLineF("  %s %f %f %f\n", Keywords::axisKeyword(Keywords::PositionKeyword), axisPosition_[axis].x, axisPosition_[axis].y, axisPosition_[axis].z);
		parser.writeLineF("  %s %f %f %f\n", Keywords::axisKeyword(Keywords::LabelOrientationKeyword), axisLabelOrientation_[axis].x, axisLabelOrientation_[axis].y, axisLabelOrientation_[axis].z);
		parser.writeLineF("  %s '%s'\n", Keywords::axisKeyword(Keywords::TitleKeyword), qPrintable(axisTitle_[axis]));
		parser.writeLineF("  %s %f %f %f %f\n", Keywords::axisKeyword(Keywords::TitleOrientationKeyword), axisTitleOrientation_[axis].x, axisTitleOrientation_[axis].y, axisTitleOrientation_[axis].z, axisTitleOrientation_[axis].w);
		parser.writeLineF("  %s %s\n", Keywords::axisKeyword(Keywords::InvertKeyword), stringBool(axisInverted_[axis]));
		parser.writeLineF("  %s %f %f\n", Keywords::axisKeyword(Keywords::LimitsKeyword), axisMin_[axis], axisMax_[axis]);
		parser.writeLineF("  %s %s\n", Keywords::axisKeyword(Keywords::LogarithmicKeyword), stringBool(axisLogarithmic_[axis]));
		parser.writeLineF("  %s %s\n", Keywords::axisKeyword(Keywords::VisibleKeyword), stringBool(axisVisible_[axis]));
		parser.writeLineF("  %s %f\n", Keywords::axisKeyword(Keywords::StretchKeyword), axisStretch_[axis]);
		parser.writeLineF("EndAxis\n");
	}

	// Write Settings Data
	parser.writeLineF("Settings\n");
	parser.writeLineF("  %s \"%s\" %i %i %s %i\n", Keywords::settingsKeyword(Keywords::ImageExportKeyword), qPrintable(imageExportFile_), imageExportWidth_, imageExportHeight_, Viewer::imageFormatExtension(imageExportFormat_), imageExportMaintainAspect_);
	parser.writeLineF("EndSettings\n");

	// Write View Data
	Matrix mat = ui.MainView->viewMatrix();
	parser.writeLineF("View\n");
	parser.writeLineF("  %s %i\n", Keywords::viewKeyword(Keywords::BoundingBoxKeyword), boundingBox_);
	parser.writeLineF("  %s %f\n", Keywords::viewKeyword(Keywords::BoundingBoxPlaneYKeyword), boundingBoxPlaneY_);
	parser.writeLineF("  %s %s\n", Keywords::viewKeyword(Keywords::LabelFaceViewerKeyword), stringBool(labelFaceViewer_));
	parser.writeLineF("  %s %f\n", Keywords::viewKeyword(Keywords::LabelScaleKeyword), labelScale_);
	parser.writeLineF("  %s %f\n", Keywords::viewKeyword(Keywords::TitleScaleKeyword), titleScale_);
	parser.writeLineF("  %s %f %f %f %f\n", Keywords::viewKeyword(Keywords::MatrixXKeyword), mat[0], mat[1], mat[2], mat[3]);
	parser.writeLineF("  %s %f %f %f %f\n", Keywords::viewKeyword(Keywords::MatrixYKeyword), mat[4], mat[5], mat[6], mat[7]);
	parser.writeLineF("  %s %f %f %f %f\n", Keywords::viewKeyword(Keywords::MatrixZKeyword), mat[8], mat[9], mat[10], mat[11]);
	parser.writeLineF("  %s %f %f %f %f\n", Keywords::viewKeyword(Keywords::MatrixWKeyword), mat[12], mat[13], mat[14], mat[15]);
	if (ui.actionViewPerspective->isChecked()) parser.writeLineF("  %s\n", Keywords::viewKeyword(Keywords::PerspectiveKeyword));
	parser.writeLineF("EndView\n");

	// Write Collection Data
	for (Collection* collection = collections_.first(); collection != NULL; collection = collection->next)
	{
		parser.writeLineF("Collection '%s'\n", qPrintable(collection->title()));
		parser.writeLineF("  %s \"%s\"\n", Keywords::collectionKeyword(Keywords::DataDirectoryKeyword), qPrintable(collection->dataFileDirectory().absolutePath()));

		// -- Transforms
		parser.writeLineF("  %s %s %s\n", Keywords::collectionKeyword(Keywords::TransformXKeyword), stringBool(collection->transformEnabled(0)), qPrintable(collection->transformEquation(0)));
		parser.writeLineF("  %s %s %s\n", Keywords::collectionKeyword(Keywords::TransformYKeyword), stringBool(collection->transformEnabled(1)), qPrintable(collection->transformEquation(1)));
		parser.writeLineF("  %s %s %s\n", Keywords::collectionKeyword(Keywords::TransformZKeyword), stringBool(collection->transformEnabled(2)), qPrintable(collection->transformEquation(2)));

		// -- Interpolation
		parser.writeLineF("  %s %s %s\n", Keywords::collectionKeyword(Keywords::InterpolateKeyword), stringBool(collection->interpolate(0)), stringBool(collection->interpolate(2)));
		parser.writeLineF("  %s %s %s\n", Keywords::collectionKeyword(Keywords::InterpolateConstrainKeyword), stringBool(collection->interpolateConstrained(0)), stringBool(collection->interpolateConstrained(2)));
		parser.writeLineF("  %s %f %f\n", Keywords::collectionKeyword(Keywords::InterpolateStepKeyword), collection->interpolationStep(0), collection->interpolationStep(2));

		// Colour Setup
		parser.writeLineF("  %s %i\n", Keywords::collectionKeyword(Keywords::ColourSourceKeyword), collection->colourSource());
		ColourScalePoint* csp;
		QColor colour;
		double value;
		// -- Single Colour
		colour = collection->colourScalePointColour(Collection::SingleColourSource);
		parser.writeLineF("  %s %i %i %i %i\n", Keywords::collectionKeyword(Keywords::ColourSingleKeyword), colour.red(), colour.green(), colour.blue(), colour.alpha());
		// -- RGB Gradient
		colour = collection->colourScalePointColour(Collection::RGBGradientSource, 0);
		value = collection->colourScalePointValue(Collection::RGBGradientSource, 0);
		parser.writeLineF("  %s %f %i %i %i %i\n", Keywords::collectionKeyword(Keywords::ColourRGBGradientAKeyword), value, colour.red(), colour.green(), colour.blue(), colour.alpha());
		colour = collection->colourScalePointColour(Collection::RGBGradientSource, 1);
		value = collection->colourScalePointValue(Collection::RGBGradientSource, 1);
		parser.writeLineF("  %s %f %i %i %i %i\n", Keywords::collectionKeyword(Keywords::ColourRGBGradientBKeyword), value, colour.red(), colour.green(), colour.blue(), colour.alpha());
		// -- HSV Gradient
		colour = collection->colourScalePointColour(Collection::HSVGradientSource, 0);
		value = collection->colourScalePointValue(Collection::HSVGradientSource, 0);
		parser.writeLineF("  %s %f %i %i %i %i\n", Keywords::collectionKeyword(Keywords::ColourHSVGradientAKeyword), value, colour.hue(), colour.saturation(), colour.value(), colour.alpha());
		colour = collection->colourScalePointColour(Collection::HSVGradientSource, 1);
		value = collection->colourScalePointValue(Collection::HSVGradientSource, 1);
		parser.writeLineF("  %s %f %i %i %i %i\n", Keywords::collectionKeyword(Keywords::ColourHSVGradientBKeyword), value, colour.hue(), colour.saturation(), colour.value(), colour.alpha());
		// -- Custom Gradient
		for (csp = collection->customColourScalePoints(); csp != NULL; csp = csp->next)
		{
			parser.writeLineF("  %s %f %i %i %i %i\n", Keywords::collectionKeyword(Keywords::ColourCustomGradientKeyword), csp->value(), csp->colour().red(), csp->colour().green(), csp->colour().blue(), csp->colour().alpha());
		}
		// -- Alpha control
		parser.writeLineF("  %s %i\n", Keywords::collectionKeyword(Keywords::ColourAlphaControlKeyword), collection->alphaControl());
		parser.writeLineF("  %s %i\n", Keywords::collectionKeyword(Keywords::ColourAlphaFixedKeyword), collection->fixedAlpha());

		// Loop over slices
		for (Slice* slice = collection->slices(); slice != NULL; slice = slice->next)
		{
			parser.writeLineF("  Slice '%s'\n", qPrintable(slice->title()));
			if (slice->dataSource() == Slice::FileSource) parser.writeLineF("  %s  %s  '%s'\n", Keywords::sliceKeyword(Keywords::SourceKeyword), Slice::dataSource(slice->dataSource()), qPrintable(slice->sourceFileName()));
			else parser.writeLineF("  %s  %s\n", Keywords::sliceKeyword(Keywords::SourceKeyword), Slice::dataSource(slice->dataSource()));
			parser.writeLineF("    %s  %f\n", Keywords::sliceKeyword(Keywords::ZKeyword), slice->data().z());
			parser.writeLineF("    Data\n");
			for (int n=0; n<slice->data().nPoints(); ++n) parser.writeLineF("      %f  %f\n", slice->data().x(n), slice->data().y(n));
			parser.writeLineF("    EndData\n");
			parser.writeLineF("  EndSlice\n");
		}

		parser.writeLineF("EndCollection\n");
	}

	parser.closeFiles();
	return true;
}

// Flag data as modified, and update titlebar
void UChromaWindow::setAsModified()
{
	modified_ = true;
	updateTitleBar();
}
