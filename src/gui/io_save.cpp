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
#include "kernels/fit.h"
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
bool UChromaWindow::writeAxisBlock(LineParser& parser, Axes& axes, int axis)
{
	parser.writeLineF("    %s %i\n", Keywords::viewPaneKeyword(Keywords::AxisBlockKeyword), axis);
	parser.writeLineF("      %s %s\n", Keywords::axisKeyword(Keywords::AutoTicksKeyword), stringBool(axes.axisAutoTicks(axis)));
	parser.writeLineF("      %s %f\n", Keywords::axisKeyword(Keywords::FirstTickKeyword), axes.axisFirstTick(axis));
	parser.writeLineF("      %s %s\n", Keywords::axisKeyword(Keywords::FractionalPositioningKeyword), stringBool(axes.axisPositionIsFractional(axis)));
	parser.writeLineF("      %s %f\n", Keywords::axisKeyword(Keywords::TickDeltaKeyword), axes.axisTickDelta(axis));
	parser.writeLineF("      %s %i\n", Keywords::axisKeyword(Keywords::MinorTicksKeyword), axes.axisMinorTicks(axis));
	parser.writeLineF("      %s %i\n", Keywords::axisKeyword(Keywords::TitleAnchorKeyword), axes.axisTitleAnchor(axis));
	parser.writeLineF("      %s %f %f %f\n", Keywords::axisKeyword(Keywords::PositionFractionalKeyword), axes.axisPositionFractional(axis).x, axes.axisPositionFractional(axis).y, axes.axisPositionFractional(axis).z);
	parser.writeLineF("      %s %f %f %f\n", Keywords::axisKeyword(Keywords::PositionRealKeyword), axes.axisPositionReal(axis).x, axes.axisPositionReal(axis).y, axes.axisPositionReal(axis).z);
	parser.writeLineF("      %s %f %f %f\n", Keywords::axisKeyword(Keywords::LabelOrientationKeyword), axes.axisLabelOrientation(axis).x, axes.axisLabelOrientation(axis).y, axes.axisLabelOrientation(axis).z);
	parser.writeLineF("      %s '%s'\n", Keywords::axisKeyword(Keywords::TitleKeyword), qPrintable(axes.axisTitle(axis)));
	parser.writeLineF("      %s %f %f %f %f\n", Keywords::axisKeyword(Keywords::TitleOrientationKeyword), axes.axisTitleOrientation(axis).x, axes.axisTitleOrientation(axis).y, axes.axisTitleOrientation(axis).z, axes.axisTitleOrientation(axis).w);
	parser.writeLineF("      %s %s\n", Keywords::axisKeyword(Keywords::InvertKeyword), stringBool(axes.axisInverted(axis)));
	parser.writeLineF("      %s %f %f\n", Keywords::axisKeyword(Keywords::LimitsKeyword), axes.axisMin(axis), axes.axisMax(axis));
	parser.writeLineF("      %s %s\n", Keywords::axisKeyword(Keywords::LogarithmicKeyword), stringBool(axes.axisLogarithmic(axis)));
	parser.writeLineF("      %s %s\n", Keywords::axisKeyword(Keywords::VisibleAxisKeyword), stringBool(axes.axisVisible(axis)));
	parser.writeLineF("      %s %f\n", Keywords::axisKeyword(Keywords::StretchKeyword), axes.axisStretch(axis));
	parser.writeLineF("    %s\n", Keywords::axisKeyword(Keywords::EndAxisKeyword));

	return true;
}

// Write CollectionBlock keywords
bool UChromaWindow::writeCollectionBlock(LineParser& parser, Collection* collection, Collection::CollectionType type, int indentLevel)
{
	// Construct indent string
	char* indent = new char[indentLevel*2+1];
	for (int n=0; n<indentLevel*2; ++n) indent[n] = ' ';
	indent[indentLevel*2] = '\0';

	if (type == Collection::MasterCollection) parser.writeLineF("%s%s '%s'\n", indent, Keywords::inputBlock(Keywords::CollectionBlock), qPrintable(collection->title()));
	else if (type == Collection::FitCollection) parser.writeLineF("%s%s '%s'\n", indent, Keywords::collectionKeyword(Keywords::FitBlockKeyword), qPrintable(collection->title()));
	else if (type == Collection::ExtractedCollection) parser.writeLineF("%s%s '%s'\n", indent, Keywords::collectionKeyword(Keywords::SliceBlockKeyword), qPrintable(collection->title()));
	parser.writeLineF("%s  %s \"%s\"\n", indent, Keywords::collectionKeyword(Keywords::DataDirectoryKeyword), qPrintable(collection->dataFileDirectory().absolutePath()));

	// -- Transforms
	parser.writeLineF("%s  %s %s %s\n", indent, Keywords::collectionKeyword(Keywords::TransformXKeyword), stringBool(collection->transformEnabled(0)), qPrintable(collection->transformEquation(0)));
	parser.writeLineF("%s  %s %s %s\n", indent, Keywords::collectionKeyword(Keywords::TransformYKeyword), stringBool(collection->transformEnabled(1)), qPrintable(collection->transformEquation(1)));
	parser.writeLineF("%s  %s %s %s\n", indent, Keywords::collectionKeyword(Keywords::TransformZKeyword), stringBool(collection->transformEnabled(2)), qPrintable(collection->transformEquation(2)));

	// -- Interpolation
	parser.writeLineF("%s  %s %s %s\n", indent, Keywords::collectionKeyword(Keywords::InterpolateKeyword), stringBool(collection->interpolate(0)), stringBool(collection->interpolate(2)));
	parser.writeLineF("%s  %s %s %s\n", indent, Keywords::collectionKeyword(Keywords::InterpolateConstrainKeyword), stringBool(collection->interpolateConstrained(0)), stringBool(collection->interpolateConstrained(2)));
	parser.writeLineF("%s  %s %f %f\n", indent, Keywords::collectionKeyword(Keywords::InterpolateStepKeyword), collection->interpolationStep(0), collection->interpolationStep(2));

	// Colour Setup
	parser.writeLineF("%s  %s %i\n", indent, Keywords::collectionKeyword(Keywords::ColourSourceKeyword), collection->colourSource());
	ColourScalePoint* csp;
	QColor colour;
	double value;
	// -- Single Colour
	colour = collection->colourScalePointColour(Collection::SingleColourSource);
	parser.writeLineF("%s  %s %i %i %i %i\n", indent, Keywords::collectionKeyword(Keywords::ColourSingleKeyword), colour.red(), colour.green(), colour.blue(), colour.alpha());
	// -- RGB Gradient
	colour = collection->colourScalePointColour(Collection::RGBGradientSource, 0);
	value = collection->colourScalePointValue(Collection::RGBGradientSource, 0);
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, Keywords::collectionKeyword(Keywords::ColourRGBGradientAKeyword), value, colour.red(), colour.green(), colour.blue(), colour.alpha());
	colour = collection->colourScalePointColour(Collection::RGBGradientSource, 1);
	value = collection->colourScalePointValue(Collection::RGBGradientSource, 1);
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, Keywords::collectionKeyword(Keywords::ColourRGBGradientBKeyword), value, colour.red(), colour.green(), colour.blue(), colour.alpha());
	// -- HSV Gradient
	colour = collection->colourScalePointColour(Collection::HSVGradientSource, 0);
	value = collection->colourScalePointValue(Collection::HSVGradientSource, 0);
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, Keywords::collectionKeyword(Keywords::ColourHSVGradientAKeyword), value, colour.hue(), colour.saturation(), colour.value(), colour.alpha());
	colour = collection->colourScalePointColour(Collection::HSVGradientSource, 1);
	value = collection->colourScalePointValue(Collection::HSVGradientSource, 1);
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, Keywords::collectionKeyword(Keywords::ColourHSVGradientBKeyword), value, colour.hue(), colour.saturation(), colour.value(), colour.alpha());
	// -- Custom Gradient
	for (csp = collection->customColourScalePoints(); csp != NULL; csp = csp->next)
	{
		parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, Keywords::collectionKeyword(Keywords::ColourCustomGradientKeyword), csp->value(), csp->colour().red(), csp->colour().green(), csp->colour().blue(), csp->colour().alpha());
	}
	// -- Alpha control
	parser.writeLineF("%s  %s %i\n", indent, Keywords::collectionKeyword(Keywords::ColourAlphaControlKeyword), collection->alphaControl());
	parser.writeLineF("%s  %s %f\n", indent, Keywords::collectionKeyword(Keywords::ColourAlphaFixedKeyword), collection->fixedAlpha());

	// Display
	parser.writeLineF("%s  %s %s\n", indent, Keywords::collectionKeyword(Keywords::StyleKeyword), Collection::displayStyle(collection->displayStyle()));
	parser.writeLineF("%s  %s %s\n", indent, Keywords::collectionKeyword(Keywords::VisibleCollectionKeyword), stringBool(collection->visible()));

	// Loop over datasets
	for (DataSet* dataSet = collection->dataSets(); dataSet != NULL; dataSet = dataSet->next) writeDataSetBlock(parser, dataSet, indentLevel);

	// Write FitKernel data if present
	if (collection->fitKernel()) writeFitParametersBlock(parser, collection->fitKernel(), indentLevel+1);

	// Additional data
	// -- Fits
	for (Collection* fit = collection->fits(); fit != NULL; fit = fit->next) writeCollectionBlock(parser, fit, Collection::FitCollection, indentLevel+1);
	// -- Extracted Data
	for (Collection* extract = collection->slices(); extract != NULL; extract = extract->next) writeCollectionBlock(parser, extract, Collection::ExtractedCollection, indentLevel+1);

	parser.writeLineF("%s%s\n", indent, Keywords::collectionKeyword(Keywords::EndCollectionKeyword));

	return true;
}

// Write DataSetBlock keywords
bool UChromaWindow::writeDataSetBlock(LineParser& parser, DataSet* dataSet, int indentLevel)
{
	// Construct indent string
	char* indent = new char[indentLevel*2+1];
	for (int n=0; n<indentLevel*2; ++n) indent[n] = ' ';
	indent[indentLevel*2] = '\0';

	parser.writeLineF("%s  %s '%s'\n", indent, Keywords::collectionKeyword(Keywords::DataSetDefinitionKeyword), qPrintable(dataSet->title()));
	if (dataSet->dataSource() == DataSet::FileSource) parser.writeLineF("%s    %s %s '%s'\n", indent, Keywords::dataSetKeyword(Keywords::SourceKeyword), DataSet::dataSource(dataSet->dataSource()), qPrintable(dataSet->sourceFileName()));
	else parser.writeLineF("%s    %s %s\n", indent, Keywords::dataSetKeyword(Keywords::SourceKeyword), DataSet::dataSource(dataSet->dataSource()));
	parser.writeLineF("%s    %s %f\n", indent, Keywords::dataSetKeyword(Keywords::ZKeyword), dataSet->data().z());
	parser.writeLineF("%s    %s\n", indent, Keywords::dataSetKeyword(Keywords::DataKeyword));
	for (int n=0; n< dataSet->data().nPoints(); ++n) parser.writeLineF("%s      %f  %f\n", indent, dataSet->data().x(n), dataSet->data().y(n));
	parser.writeLineF("%s    End%s\n", indent, Keywords::dataSetKeyword(Keywords::DataKeyword));
	parser.writeLineF("%s  %s\n", indent, Keywords::dataSetKeyword(Keywords::EndDataSetKeyword));

	return true;
}

// Write FitParametersBlock keywords
bool UChromaWindow::writeFitParametersBlock(LineParser& parser, FitKernel* fitKernel, int indentLevel)
{
	// Construct indent string
	char* indent = new char[indentLevel*2+1];
	for (int n=0; n<indentLevel*2; ++n) indent[n] = ' ';
	indent[indentLevel*2] = '\0';

	parser.writeLineF("%s  %s\n", indent, Keywords::collectionKeyword(Keywords::FitParametersBlockKeyword));
	parser.writeLineF("%s    %s '%s'\n", indent, Keywords::fitParametersKeyword(Keywords::EquationKeyword), qPrintable(fitKernel->equationText()));
	parser.writeLineF("%s    %s %s\n", indent, Keywords::fitParametersKeyword(Keywords::GlobalKeyword), stringBool(fitKernel->global()));
	parser.writeLineF("%s    %s %s\n", indent, Keywords::fitParametersKeyword(Keywords::OrthogonalKeyword), stringBool(fitKernel->orthogonal()));
	parser.writeLineF("%s    %s %f\n", indent, Keywords::fitParametersKeyword(Keywords::LimitStrengthKeyword), fitKernel->limitStrength());
	for (RefListItem<EquationVariable,bool>* ri = fitKernel->usedVariables(); ri != NULL; ri = ri->next)
	{
		EquationVariable* eqVar = ri->item;
		parser.writeLineF("%s    %s %s %s %f %s %f %s %f\n", indent, Keywords::fitParametersKeyword(Keywords::VariableKeyword), qPrintable(eqVar->name()), stringBool(eqVar->fit()), eqVar->value(), stringBool(eqVar->maximumLimitEnabled()), eqVar->minimumLimit(), stringBool(eqVar->maximumLimitEnabled()), eqVar->maximumLimit());
	}
	parser.writeLineF("%s    %s %s\n", indent, Keywords::fitParametersKeyword(Keywords::XRangeTypeKeyword), FitKernel::rangeType(fitKernel->xRange()));
	parser.writeLineF("%s    %s %f %f\n", indent, Keywords::fitParametersKeyword(Keywords::XRangeAbsoluteKeyword), fitKernel->absoluteXMin(), fitKernel->absoluteXMax());
	parser.writeLineF("%s    %s %i %i\n", indent, Keywords::fitParametersKeyword(Keywords::XRangeIndexKeyword), fitKernel->indexXMin()+1, fitKernel->indexXMax()+1);
	parser.writeLineF("%s    %s %i\n", indent, Keywords::fitParametersKeyword(Keywords::XRangeIndexSingleKeyword), fitKernel->indexXSingle()+1);
	parser.writeLineF("%s    %s %s\n", indent, Keywords::fitParametersKeyword(Keywords::ZRangeTypeKeyword), FitKernel::rangeType(fitKernel->zRange()));
	parser.writeLineF("%s    %s %f %f\n", indent, Keywords::fitParametersKeyword(Keywords::ZRangeAbsoluteKeyword), fitKernel->absoluteZMin(), fitKernel->absoluteZMax());
	parser.writeLineF("%s    %s %i %i\n", indent, Keywords::fitParametersKeyword(Keywords::ZRangeIndexKeyword), fitKernel->indexZMin()+1, fitKernel->indexZMax()+1);
	parser.writeLineF("%s    %s %i\n", indent, Keywords::fitParametersKeyword(Keywords::ZRangeIndexSingleKeyword), fitKernel->indexZSingle()+1);
	parser.writeLineF("%s  %s\n", indent, Keywords::fitParametersKeyword(Keywords::EndFitParametersKeyword));

	return true;
}

// Write SettingsBlock keywords
bool UChromaWindow::writeSettingsBlock(LineParser& parser)
{
	parser.writeLineF("%s\n", Keywords::inputBlock(Keywords::SettingsBlock));
	parser.writeLineF("  %s \"%s\" %i %i %s %i\n", Keywords::settingsKeyword(Keywords::ImageExportKeyword), qPrintable(imageExportFile_), imageExportWidth_, imageExportHeight_, Viewer::imageFormatExtension(imageExportFormat_), imageExportMaintainAspect_);
	parser.writeLineF("%s\n", Keywords::settingsKeyword(Keywords::EndSettingsKeyword));

	return true;
}

// Write ViewBlock keywords
bool UChromaWindow::writeViewBlock(LineParser& parser)
{
	parser.writeLineF("%s\n", Keywords::inputBlock(Keywords::ViewBlock));
	parser.writeLineF("  %s %i %i\n", Keywords::viewKeyword(Keywords::GridKeyword), viewLayout_.nColumns(), viewLayout_.nRows());
	parser.writeLineF("  %s %s\n", Keywords::viewKeyword(Keywords::LabelFaceViewerKeyword), stringBool(labelFaceViewer_));

	// Loop over defined panes
	for (ViewPane* pane = viewLayout_.panes(); pane != NULL; pane = pane->next) writeViewPaneBlock(parser, pane);

	parser.writeLineF("%s\n", Keywords::viewKeyword(Keywords::EndViewKeyword));

	return true;
}

// Write ViewPaneBlock keywords
bool UChromaWindow::writeViewPaneBlock(LineParser& parser, ViewPane* pane)
{
	parser.writeLineF("  %s '%s'\n", Keywords::viewKeyword(Keywords::ViewPaneBlockKeyword), qPrintable(pane->name()));
	parser.writeLineF("    %s %i %i %i %i\n", Keywords::viewPaneKeyword(Keywords::GeometryKeyword), pane->bottomEdge(), pane->leftEdge(), pane->width(), pane->height()); 
	for (RefListItem<Collection,bool>* ri = pane->collections(); ri != NULL; ri = ri->next) parser.writeLineF("    %s '%s'\n", Keywords::viewPaneKeyword(Keywords::CollectionAssociatedKeyword), qPrintable(ri->item->title())); 
	for (int axis=0; axis < 3; ++axis) writeAxisBlock(parser, pane->axes(), axis);
	parser.writeLineF("    %s %s\n", Keywords::viewPaneKeyword(Keywords::AutoScaleKeyword), ViewPane::autoScaleMethod(pane->autoScale()));
	parser.writeLineF("    %s %s\n", Keywords::viewPaneKeyword(Keywords::TwoDimensionalKeyword), stringBool(pane->twoDimensional()));
	parser.writeLineF("    %s %s\n", Keywords::viewPaneKeyword(Keywords::AutoStretch3DKeyword), stringBool(pane->autoStretch3D()));
	parser.writeLineF("    %s %f\n", Keywords::viewPaneKeyword(Keywords::LabelPointSizeKeyword), pane->labelPointSize());
	parser.writeLineF("    %s %f\n", Keywords::viewPaneKeyword(Keywords::TitlePointSizeKeyword), pane->titlePointSize());
	parser.writeLineF("    %s %i\n", Keywords::viewPaneKeyword(Keywords::BoundingBoxKeyword), pane->boundingBox());
	parser.writeLineF("    %s %f\n", Keywords::viewPaneKeyword(Keywords::BoundingBoxPlaneYKeyword), pane->boundingBoxPlaneY());
	Matrix mat = pane->viewMatrix();
	parser.writeLineF("    %s %f %f %f %f\n", Keywords::viewPaneKeyword(Keywords::MatrixXKeyword), mat[0], mat[1], mat[2], mat[3]);
	parser.writeLineF("    %s %f %f %f %f\n", Keywords::viewPaneKeyword(Keywords::MatrixYKeyword), mat[4], mat[5], mat[6], mat[7]);
	parser.writeLineF("    %s %f %f %f %f\n", Keywords::viewPaneKeyword(Keywords::MatrixZKeyword), mat[8], mat[9], mat[10], mat[11]);
	parser.writeLineF("    %s %f %f %f %f\n", Keywords::viewPaneKeyword(Keywords::MatrixWKeyword), mat[12], mat[13], mat[14], mat[15]);
	if (pane->hasPerspective()) parser.writeLineF("  %s\n", Keywords::viewPaneKeyword(Keywords::PerspectiveKeyword));
	parser.writeLineF("    %s '%s'\n", Keywords::viewPaneKeyword(Keywords::RoleKeyword), ViewPane::paneRole(pane->role()));
	for (RefListItem<Collection,TargetData>* ri = pane->roleTargetCollections(); ri != NULL; ri = ri->next) parser.writeLineF("    %s '%s'\n", Keywords::viewPaneKeyword(Keywords::RoleTargetCollectionKeyword), qPrintable(ri->item->title()));
	for (RefListItem<ViewPane,bool>* ri = pane->roleTargetPanes(); ri != NULL; ri = ri->next) parser.writeLineF("    %s '%s'\n", Keywords::viewPaneKeyword(Keywords::RoleTargetPaneKeyword), qPrintable(ri->item->name()));
	parser.writeLineF("  %s\n", Keywords::viewPaneKeyword(Keywords::EndViewPaneKeyword));

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

	// Write Settings Data
	writeSettingsBlock(parser);

	// Write Collection Data
	for (Collection* collection = collections_.first(); collection != NULL; collection = collection->next) writeCollectionBlock(parser, collection);

	// Write View Data
	writeViewBlock(parser);

	parser.closeFiles();
	return true;
}
