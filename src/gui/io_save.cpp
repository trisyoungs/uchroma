/*
	*** File I/O - Save
	*** src/gui/io_save.cpp
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

// Write AxisBlock keywords
bool UChromaWindow::writeAxisBlock(LineParser& parser, int axis)
{
	parser.writeLineF("%s %i\n", Keywords::inputBlock(Keywords::AxisBlock), axis);
	parser.writeLineF("  %s %s\n", Keywords::axisKeyword(Keywords::AutoTicksKeyword), stringBool(axisAutoTicks_[axis]));
	parser.writeLineF("  %s %f\n", Keywords::axisKeyword(Keywords::FirstTickKeyword), axisFirstTick_[axis]);
	parser.writeLineF("  %s %s\n", Keywords::axisKeyword(Keywords::FractionalPositioningKeyword), stringBool(axisPositionIsFractional_[axis]));
	parser.writeLineF("  %s %f\n", Keywords::axisKeyword(Keywords::TickDeltaKeyword), axisTickDelta_[axis]);
	parser.writeLineF("  %s %i\n", Keywords::axisKeyword(Keywords::MinorTicksKeyword), axisMinorTicks_[axis]);
	parser.writeLineF("  %s %i\n", Keywords::axisKeyword(Keywords::TitleAnchorKeyword), axisTitleAnchor_[axis]);
	parser.writeLineF("  %s %f %f %f\n", Keywords::axisKeyword(Keywords::PositionFractionalKeyword), axisPositionFractional_[axis].x, axisPositionFractional_[axis].y, axisPositionFractional_[axis].z);
	parser.writeLineF("  %s %f %f %f\n", Keywords::axisKeyword(Keywords::PositionRealKeyword), axisPositionReal_[axis].x, axisPositionReal_[axis].y, axisPositionReal_[axis].z);
	parser.writeLineF("  %s %f %f %f\n", Keywords::axisKeyword(Keywords::LabelOrientationKeyword), axisLabelOrientation_[axis].x, axisLabelOrientation_[axis].y, axisLabelOrientation_[axis].z);
	parser.writeLineF("  %s '%s'\n", Keywords::axisKeyword(Keywords::TitleKeyword), qPrintable(axisTitle_[axis]));
	parser.writeLineF("  %s %f %f %f %f\n", Keywords::axisKeyword(Keywords::TitleOrientationKeyword), axisTitleOrientation_[axis].x, axisTitleOrientation_[axis].y, axisTitleOrientation_[axis].z, axisTitleOrientation_[axis].w);
	parser.writeLineF("  %s %s\n", Keywords::axisKeyword(Keywords::InvertKeyword), stringBool(axisInverted_[axis]));
	parser.writeLineF("  %s %f %f\n", Keywords::axisKeyword(Keywords::LimitsKeyword), axisMin_[axis], axisMax_[axis]);
	parser.writeLineF("  %s %s\n", Keywords::axisKeyword(Keywords::LogarithmicKeyword), stringBool(axisLogarithmic_[axis]));
	parser.writeLineF("  %s %s\n", Keywords::axisKeyword(Keywords::VisibleAxisKeyword), stringBool(axisVisible_[axis]));
	parser.writeLineF("  %s %f\n", Keywords::axisKeyword(Keywords::StretchKeyword), axisStretch_[axis]);
	parser.writeLineF("%s\n", Keywords::axisKeyword(Keywords::EndAxisKeyword));
}

// Write CollectionBlock keywords
bool UChromaWindow::writeCollectionBlock(LineParser& parser, Collection* collection)
{
	parser.writeLineF("%s '%s'\n", Keywords::inputBlock(Keywords::CollectionBlock), qPrintable(collection->title()));
	if (collection->type() != Collection::MasterCollection)
	{
		if (collection->parent() == NULL) printf("Internal Error: A sub-collection ('%s') has a NULL parent.\n", qPrintable(collection->title()));
		else parser.writeLineF("  %s '%s'\n", Keywords::collectionKeyword(Keywords::ParentKeyword), qPrintable(collection->parent()->title()));
	}
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
	parser.writeLineF("  %s %f\n", Keywords::collectionKeyword(Keywords::ColourAlphaFixedKeyword), collection->fixedAlpha());

	// Display
	parser.writeLineF("  %s %s\n", Keywords::collectionKeyword(Keywords::StyleKeyword), Collection::displayStyle(collection->displayStyle()));
	parser.writeLineF("  %s %s\n", Keywords::collectionKeyword(Keywords::VisibleCollectionKeyword), stringBool(collection->visible()));

	// Loop over datasets
	for (DataSet* dataSet = collection->dataSets(); dataSet != NULL; dataSet = dataSet->next) writeDataSetBlock(parser, dataSet);

	parser.writeLineF("%s\n", Keywords::collectionKeyword(Keywords::EndCollectionKeyword));
}

// Write DataSetBlock keywords
bool UChromaWindow::writeDataSetBlock(LineParser& parser, DataSet* dataSet)
{
	parser.writeLineF("  %s '%s'\n", Keywords::collectionKeyword(Keywords::DataSetDefinitionKeyword), qPrintable(dataSet->title()));
	if (dataSet->dataSource() == DataSet::FileSource) parser.writeLineF("    %s  %s  '%s'\n", Keywords::dataSetKeyword(Keywords::SourceKeyword), DataSet::dataSource(dataSet->dataSource()), qPrintable(dataSet->sourceFileName()));
	else parser.writeLineF("    %s  %s\n", Keywords::dataSetKeyword(Keywords::SourceKeyword), DataSet::dataSource(dataSet->dataSource()));
	parser.writeLineF("    %s  %f\n", Keywords::dataSetKeyword(Keywords::ZKeyword), dataSet->data().z());
	parser.writeLineF("    %s\n", Keywords::dataSetKeyword(Keywords::DataKeyword));
	for (int n=0; n< dataSet->data().nPoints(); ++n) parser.writeLineF("      %f  %f\n", dataSet->data().x(n), dataSet->data().y(n));
	parser.writeLineF("    End%s\n", Keywords::dataSetKeyword(Keywords::DataKeyword));
	parser.writeLineF("  %s\n", Keywords::dataSetKeyword(Keywords::EndDataSetKeyword));
}

// Write SettingsBlock keywords
bool UChromaWindow::writeSettingsBlock(LineParser& parser)
{
	parser.writeLineF("%s\n", Keywords::inputBlock(Keywords::SettingsBlock));
	parser.writeLineF("  %s \"%s\" %i %i %s %i\n", Keywords::settingsKeyword(Keywords::ImageExportKeyword), qPrintable(imageExportFile_), imageExportWidth_, imageExportHeight_, Viewer::imageFormatExtension(imageExportFormat_), imageExportMaintainAspect_);
	parser.writeLineF("%s\n", Keywords::settingsKeyword(Keywords::EndSettingsKeyword));
}

// Write ViewBlock keywords
bool UChromaWindow::writeViewBlock(LineParser& parser)
{
	Matrix mat = ui.MainView->viewMatrix();
	parser.writeLineF("%s\n", Keywords::inputBlock(Keywords::ViewBlock));
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
	parser.writeLineF("%s\n", Keywords::viewKeyword(Keywords::EndViewKeyword));
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
	for (int axis=0; axis < 3; ++axis) writeAxisBlock(parser, axis);

	// Write Settings Data
	writeSettingsBlock(parser);

	// Write View Data
	writeViewBlock(parser);

	// Write Collection Data
	for (Collection* collection = collections_.first(); collection != NULL; collection = collection->next)
	{
		// Write main collection block
		writeCollectionBlock(parser, collection);

		// Loop over fit data
		for (Collection* fit = collection->fits(); fit != NULL; fit = fit->next)
		{
			parser.writeLine("");
			writeCollectionBlock(parser, fit);
		}
	}

	parser.closeFiles();
	return true;
}

// Flag data as modified, and update titlebar
void UChromaWindow::setAsModified(bool isModified)
{
	modified_ = isModified;
	updateTitleBar();
}
