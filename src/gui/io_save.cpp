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
	parser.writeLineF("      %s %s\n", Keywords::axisKeyword(Keywords::AutoTicksKeyword), stringBool(axes.autoTicks(axis)));
	parser.writeLineF("      %s %f\n", Keywords::axisKeyword(Keywords::FirstTickKeyword), axes.tickFirst(axis));
	parser.writeLineF("      %s %s\n", Keywords::axisKeyword(Keywords::FractionalPositioningKeyword), stringBool(axes.positionIsFractional(axis)));
	parser.writeLineF("      %s %s %s %s\n", Keywords::axisKeyword(Keywords::GridLinesKeyword), stringBool(axes.gridLinesMajor(axis)), stringBool(axes.gridLinesMinor(axis)), stringBool(axes.gridLinesFull(axis)));
	LineStyle style = axes.gridLineMajorStyle(axis);
	parser.writeLineF("      %s %f '%s' %f %f %f %f\n", Keywords::axisKeyword(Keywords::GridLineMajorStyleKeyword), style.width(), LineStipple::stipple[style.stipple()].name, style.colour().redF(), style.colour().greenF(), style.colour().blueF(), style.colour().alphaF());
	style = axes.gridLineMinorStyle(axis);
	parser.writeLineF("      %s %f '%s' %f %f %f %f\n", Keywords::axisKeyword(Keywords::GridLineMinorStyleKeyword), style.width(), LineStipple::stipple[style.stipple()].name, style.colour().redF(), style.colour().greenF(), style.colour().blueF(), style.colour().alphaF());
	parser.writeLineF("      %s %s\n", Keywords::axisKeyword(Keywords::InvertKeyword), stringBool(axes.inverted(axis)));
	parser.writeLineF("      %s %s\n", Keywords::axisKeyword(Keywords::LabelAnchorKeyword), TextPrimitive::textAnchor(axes.labelAnchor(axis)));
	parser.writeLineF("      %s %f %f %f\n", Keywords::axisKeyword(Keywords::LabelOrientationKeyword), axes.labelOrientation(axis).x, axes.labelOrientation(axis).y, axes.labelOrientation(axis).z);
	parser.writeLineF("      %s %f %f\n", Keywords::axisKeyword(Keywords::LimitsKeyword), axes.min(axis), axes.max(axis));
	parser.writeLineF("      %s %s\n", Keywords::axisKeyword(Keywords::LogarithmicKeyword), stringBool(axes.logarithmic(axis)));
	parser.writeLineF("      %s %i\n", Keywords::axisKeyword(Keywords::MinorTicksKeyword), axes.minorTicks(axis));
	NumberFormat fmt = axes.numberFormat(axis);
	parser.writeLineF("      %s '%s' %i %s %s\n", Keywords::axisKeyword(Keywords::NumberFormatKeyword), NumberFormat::formatType(fmt.type()), fmt.nDecimals(), stringBool(fmt.useUpperCaseExponent()), stringBool(fmt.forcePrecedingPlus()));
	parser.writeLineF("      %s %f %f %f\n", Keywords::axisKeyword(Keywords::PositionFractionalKeyword), axes.positionFractional(axis).x, axes.positionFractional(axis).y, axes.positionFractional(axis).z);
	parser.writeLineF("      %s %f %f %f\n", Keywords::axisKeyword(Keywords::PositionRealKeyword), axes.positionReal(axis).x, axes.positionReal(axis).y, axes.positionReal(axis).z);
	parser.writeLineF("      %s %f\n", Keywords::axisKeyword(Keywords::StretchKeyword), axes.stretch(axis));
	parser.writeLineF("      %s %f\n", Keywords::axisKeyword(Keywords::TickDeltaKeyword), axes.tickDelta(axis));
	parser.writeLineF("      %s %s\n", Keywords::axisKeyword(Keywords::TitleAnchorKeyword), TextPrimitive::textAnchor(axes.titleAnchor(axis)));
	parser.writeLine(QString("      ")+Keywords::axisKeyword(Keywords::TitleKeyword)+" '"+axes.title(axis)+"'\n");
	parser.writeLineF("      %s %f %f %f %f\n", Keywords::axisKeyword(Keywords::TitleOrientationKeyword), axes.titleOrientation(axis).x, axes.titleOrientation(axis).y, axes.titleOrientation(axis).z, axes.titleOrientation(axis).w);
	parser.writeLineF("      %s %s\n", Keywords::axisKeyword(Keywords::VisibleAxisKeyword), stringBool(axes.visible(axis)));
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

	if (type == Collection::MasterCollection) parser.writeLineF("%s%s '%s'\n", indent, Keywords::inputBlock(Keywords::CollectionBlock), qPrintable(collection->name()));
	else if (type == Collection::FitCollection) parser.writeLineF("%s%s '%s'\n", indent, Keywords::collectionKeyword(Keywords::FitBlockKeyword), qPrintable(collection->name()));
	else if (type == Collection::ExtractedCollection) parser.writeLineF("%s%s '%s'\n", indent, Keywords::collectionKeyword(Keywords::SliceBlockKeyword), qPrintable(collection->name()));
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
	parser.writeLineF("%s  %s '%s'\n", indent, Keywords::collectionKeyword(Keywords::ColourSourceKeyword), Collection::colourSource(collection->colourSource()));
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
	parser.writeLineF("%s  %s '%s'\n", indent, Keywords::collectionKeyword(Keywords::ColourAlphaControlKeyword), Collection::alphaControl(collection->alphaControl()));
	parser.writeLineF("%s  %s %f\n", indent, Keywords::collectionKeyword(Keywords::ColourAlphaFixedKeyword), collection->fixedAlpha());

	// Display
	parser.writeLineF("%s  %s %s\n", indent, Keywords::collectionKeyword(Keywords::StyleKeyword), Collection::displayStyle(collection->displayStyle()));
	parser.writeLineF("%s  %s %s\n", indent, Keywords::collectionKeyword(Keywords::VisibleCollectionKeyword), stringBool(collection->visible()));

	// Loop over datasets
	for (DataSet* dataSet = collection->dataSets(); dataSet != NULL; dataSet = dataSet->next) writeDataSetBlock(parser, dataSet, indentLevel);

	// Write FitKernel data if present
	if (collection->fitKernel()) writeFitParametersBlock(parser, collection->fitKernel(), indentLevel);

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

	parser.writeLineF("%s  %s '%s'\n", indent, Keywords::collectionKeyword(Keywords::DataSetDefinitionKeyword), qPrintable(dataSet->name()));
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
	for (RefListItem<ReferenceVariable,bool>* ri = fitKernel->usedReferences(); ri != NULL; ri = ri->next)
	{
		ReferenceVariable* refVar = ri->item;
		parser.writeLineF("%s    %s %s %s %i %i %s %i %i '%s'\n", indent, Keywords::fitParametersKeyword(Keywords::ReferenceKeyword), qPrintable(refVar->name()), IndexData::indexType(refVar->xIndex().type()), refVar->xIndex().index(), refVar->xIndex().offset(), IndexData::indexType(refVar->zIndex().type()), refVar->zIndex().index(), refVar->zIndex().offset(), qPrintable(refVar->zDataSetName()));
	}
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
	for (int axis=0; axis < 3; ++axis) writeAxisBlock(parser, pane->axes(), axis);
	parser.writeLineF("    %s %s\n", Keywords::viewPaneKeyword(Keywords::AutoScaleKeyword), ViewPane::autoScaleMethod(pane->autoScale()));
	parser.writeLineF("    %s %f\n", Keywords::viewPaneKeyword(Keywords::LabelPointSizeKeyword), pane->labelPointSize());
	parser.writeLineF("    %s %f\n", Keywords::viewPaneKeyword(Keywords::TitlePointSizeKeyword), pane->titlePointSize());
	parser.writeLineF("    %s %i\n", Keywords::viewPaneKeyword(Keywords::BoundingBoxKeyword), pane->boundingBox());
	parser.writeLineF("    %s %f\n", Keywords::viewPaneKeyword(Keywords::BoundingBoxPlaneYKeyword), pane->boundingBoxPlaneY());
	Matrix mat = pane->viewRotation();
	Vec3<double> trans = pane->viewTranslation();
	parser.writeLineF("    %s %f %f %f\n", Keywords::viewPaneKeyword(Keywords::RotationXKeyword), mat[0], mat[1], mat[2]);
	parser.writeLineF("    %s %f %f %f\n", Keywords::viewPaneKeyword(Keywords::RotationYKeyword), mat[4], mat[5], mat[6]);
	parser.writeLineF("    %s %f %f %f\n", Keywords::viewPaneKeyword(Keywords::RotationZKeyword), mat[8], mat[9], mat[10]);
	parser.writeLineF("    %s %f %f %f\n", Keywords::viewPaneKeyword(Keywords::TranslationKeyword), trans.x, trans.y, trans.z);
	parser.writeLineF("    %s %s\n", Keywords::viewPaneKeyword(Keywords::PerspectiveKeyword), stringBool(pane->hasPerspective()));
	parser.writeLineF("    %s '%s'\n", Keywords::viewPaneKeyword(Keywords::RoleKeyword), ViewPane::paneRole(pane->role()));
	for (RefListItem<Collection,bool>* ri = pane->collectionTargets(); ri != NULL; ri = ri->next)
	{
		if (!Collection::objectValid(ri->item, "collection in UChromaWindow::writeViewPaneBlock")) continue;
		parser.writeLineF("    %s '%s'\n", Keywords::viewPaneKeyword(Keywords::RoleTargetCollectionKeyword), qPrintable(ri->item->locator()));
	}
	for (RefListItem<ViewPane,bool>* ri = pane->paneTargets(); ri != NULL; ri = ri->next) parser.writeLineF("    %s '%s'\n", Keywords::viewPaneKeyword(Keywords::RoleTargetPaneKeyword), qPrintable(ri->item->name()));
	parser.writeLineF("    %s '%s'\n", Keywords::viewPaneKeyword(Keywords::ViewTypeKeyword), ViewPane::viewType(pane->viewType()));
	parser.writeLineF("  %s\n", Keywords::viewPaneKeyword(Keywords::EndViewPaneKeyword));

	return true;
}

// Save current data to file specified
bool UChromaWindow::saveInputFile(QString fileName)
{
	LineParser parser(fileName, true);
	if (!parser.ready())
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
