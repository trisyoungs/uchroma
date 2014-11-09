/*
	*** Session Save
	*** src/session/save.cpp
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

#include "session/session.h"
#include "gui/uchroma.h"
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
bool UChromaSession::writeAxisBlock(LineParser& parser, Axes& axes, int axis)
{
	parser.writeLineF("    %s %i\n", UChromaSession::viewPaneKeyword(UChromaSession::AxisBlockKeyword), axis);
	parser.writeLineF("      %s %s\n", UChromaSession::axisKeyword(UChromaSession::AutoScaleKeyword), Axes::autoScaleMethod(axes.autoScale(axis)));
	parser.writeLineF("      %s %s\n", UChromaSession::axisKeyword(UChromaSession::AutoTicksKeyword), stringBool(axes.autoTicks(axis)));
	parser.writeLineF("      %s %f\n", UChromaSession::axisKeyword(UChromaSession::FirstTickKeyword), axes.tickFirst(axis));
	parser.writeLineF("      %s %s\n", UChromaSession::axisKeyword(UChromaSession::FractionalPositioningKeyword), stringBool(axes.positionIsFractional(axis)));
	parser.writeLineF("      %s %s %s %s\n", UChromaSession::axisKeyword(UChromaSession::GridLinesKeyword), stringBool(axes.gridLinesMajor(axis)), stringBool(axes.gridLinesMinor(axis)), stringBool(axes.gridLinesFull(axis)));
	LineStyle style = axes.gridLineMajorStyle(axis);
	parser.writeLineF("      %s %f '%s' %f %f %f %f\n", UChromaSession::axisKeyword(UChromaSession::GridLineMajorStyleKeyword), style.width(), LineStipple::stipple[style.stipple()].name, style.colour().redF(), style.colour().greenF(), style.colour().blueF(), style.colour().alphaF());
	style = axes.gridLineMinorStyle(axis);
	parser.writeLineF("      %s %f '%s' %f %f %f %f\n", UChromaSession::axisKeyword(UChromaSession::GridLineMinorStyleKeyword), style.width(), LineStipple::stipple[style.stipple()].name, style.colour().redF(), style.colour().greenF(), style.colour().blueF(), style.colour().alphaF());
	parser.writeLineF("      %s %s\n", UChromaSession::axisKeyword(UChromaSession::InvertKeyword), stringBool(axes.inverted(axis)));
	parser.writeLineF("      %s %s\n", UChromaSession::axisKeyword(UChromaSession::LabelAnchorKeyword), TextPrimitive::textAnchor(axes.labelAnchor(axis)));
	parser.writeLineF("      %s %f %f %f\n", UChromaSession::axisKeyword(UChromaSession::LabelOrientationKeyword), axes.labelOrientation(axis).x, axes.labelOrientation(axis).y, axes.labelOrientation(axis).z);
	parser.writeLineF("      %s %f %f\n", UChromaSession::axisKeyword(UChromaSession::LimitsKeyword), axes.min(axis), axes.max(axis));
	parser.writeLineF("      %s %s\n", UChromaSession::axisKeyword(UChromaSession::LogarithmicKeyword), stringBool(axes.logarithmic(axis)));
	parser.writeLineF("      %s %i\n", UChromaSession::axisKeyword(UChromaSession::MinorTicksKeyword), axes.minorTicks(axis));
	NumberFormat fmt = axes.numberFormat(axis);
	parser.writeLineF("      %s '%s' %i %s %s\n", UChromaSession::axisKeyword(UChromaSession::NumberFormatKeyword), NumberFormat::formatType(fmt.type()), fmt.nDecimals(), stringBool(fmt.useUpperCaseExponent()), stringBool(fmt.forcePrecedingPlus()));
	parser.writeLineF("      %s %f %f %f\n", UChromaSession::axisKeyword(UChromaSession::PositionFractionalKeyword), axes.positionFractional(axis).x, axes.positionFractional(axis).y, axes.positionFractional(axis).z);
	parser.writeLineF("      %s %f %f %f\n", UChromaSession::axisKeyword(UChromaSession::PositionRealKeyword), axes.positionReal(axis).x, axes.positionReal(axis).y, axes.positionReal(axis).z);
	parser.writeLineF("      %s %f\n", UChromaSession::axisKeyword(UChromaSession::StretchKeyword), axes.stretch(axis));
	parser.writeLineF("      %s %f\n", UChromaSession::axisKeyword(UChromaSession::TickDeltaKeyword), axes.tickDelta(axis));
	parser.writeLineF("      %s %f %f %f\n", UChromaSession::axisKeyword(UChromaSession::TickDirectionKeyword), axes.tickDirection(axis).x, axes.tickDirection(axis).y, axes.tickDirection(axis).z);
	parser.writeLineF("      %s %s\n", UChromaSession::axisKeyword(UChromaSession::TitleAnchorKeyword), TextPrimitive::textAnchor(axes.titleAnchor(axis)));
	parser.writeLine(QString("      ")+UChromaSession::axisKeyword(UChromaSession::TitleKeyword)+" '"+axes.title(axis)+"'\n");
	parser.writeLineF("      %s %f %f %f %f\n", UChromaSession::axisKeyword(UChromaSession::TitleOrientationKeyword), axes.titleOrientation(axis).x, axes.titleOrientation(axis).y, axes.titleOrientation(axis).z, axes.titleOrientation(axis).w);
	parser.writeLineF("      %s %s\n", UChromaSession::axisKeyword(UChromaSession::VisibleAxisKeyword), stringBool(axes.visible(axis)));
	parser.writeLineF("    %s\n", UChromaSession::axisKeyword(UChromaSession::EndAxisKeyword));

	return true;
}

// Write CollectionBlock keywords
bool UChromaSession::writeCollectionBlock(LineParser& parser, Collection* collection, Collection::CollectionType type, int indentLevel)
{
	// Construct indent string
	char* indent = new char[indentLevel*2+1];
	for (int n=0; n<indentLevel*2; ++n) indent[n] = ' ';
	indent[indentLevel*2] = '\0';

	if (type == Collection::MasterCollection) parser.writeLineF("%s%s '%s'\n", indent, UChromaSession::inputBlock(UChromaSession::CollectionBlock), qPrintable(collection->name()));
	else if (type == Collection::FitCollection) parser.writeLineF("%s%s '%s'\n", indent, UChromaSession::collectionKeyword(UChromaSession::FitBlockKeyword), qPrintable(collection->name()));
	else if (type == Collection::ExtractedCollection) parser.writeLineF("%s%s '%s'\n", indent, UChromaSession::collectionKeyword(UChromaSession::SliceBlockKeyword), qPrintable(collection->name()));
	parser.writeLineF("%s  %s \"%s\"\n", indent, UChromaSession::collectionKeyword(UChromaSession::DataDirectoryKeyword), qPrintable(collection->dataFileDirectory().absolutePath()));

	// -- Transforms
	parser.writeLineF("%s  %s %s %s\n", indent, UChromaSession::collectionKeyword(UChromaSession::TransformXKeyword), stringBool(collection->transformEnabled(0)), qPrintable(collection->transformEquation(0)));
	parser.writeLineF("%s  %s %s %s\n", indent, UChromaSession::collectionKeyword(UChromaSession::TransformYKeyword), stringBool(collection->transformEnabled(1)), qPrintable(collection->transformEquation(1)));
	parser.writeLineF("%s  %s %s %s\n", indent, UChromaSession::collectionKeyword(UChromaSession::TransformZKeyword), stringBool(collection->transformEnabled(2)), qPrintable(collection->transformEquation(2)));

	// -- Interpolation
	parser.writeLineF("%s  %s %s %s\n", indent, UChromaSession::collectionKeyword(UChromaSession::InterpolateKeyword), stringBool(collection->interpolate(0)), stringBool(collection->interpolate(2)));
	parser.writeLineF("%s  %s %s %s\n", indent, UChromaSession::collectionKeyword(UChromaSession::InterpolateConstrainKeyword), stringBool(collection->interpolateConstrained(0)), stringBool(collection->interpolateConstrained(2)));
	parser.writeLineF("%s  %s %f %f\n", indent, UChromaSession::collectionKeyword(UChromaSession::InterpolateStepKeyword), collection->interpolationStep(0), collection->interpolationStep(2));

	// Colour Setup
	parser.writeLineF("%s  %s '%s'\n", indent, UChromaSession::collectionKeyword(UChromaSession::ColourSourceKeyword), Collection::colourSource(collection->colourSource()));
	ColourScalePoint* csp;
	QColor colour;
	double value;
	// -- Single Colour
	colour = collection->colourScalePointColour(Collection::SingleColourSource);
	parser.writeLineF("%s  %s %i %i %i %i\n", indent, UChromaSession::collectionKeyword(UChromaSession::ColourSingleKeyword), colour.red(), colour.green(), colour.blue(), colour.alpha());
	// -- RGB Gradient
	colour = collection->colourScalePointColour(Collection::RGBGradientSource, 0);
	value = collection->colourScalePointValue(Collection::RGBGradientSource, 0);
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, UChromaSession::collectionKeyword(UChromaSession::ColourRGBGradientAKeyword), value, colour.red(), colour.green(), colour.blue(), colour.alpha());
	colour = collection->colourScalePointColour(Collection::RGBGradientSource, 1);
	value = collection->colourScalePointValue(Collection::RGBGradientSource, 1);
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, UChromaSession::collectionKeyword(UChromaSession::ColourRGBGradientBKeyword), value, colour.red(), colour.green(), colour.blue(), colour.alpha());
	// -- HSV Gradient
	colour = collection->colourScalePointColour(Collection::HSVGradientSource, 0);
	value = collection->colourScalePointValue(Collection::HSVGradientSource, 0);
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, UChromaSession::collectionKeyword(UChromaSession::ColourHSVGradientAKeyword), value, colour.hue(), colour.saturation(), colour.value(), colour.alpha());
	colour = collection->colourScalePointColour(Collection::HSVGradientSource, 1);
	value = collection->colourScalePointValue(Collection::HSVGradientSource, 1);
	parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, UChromaSession::collectionKeyword(UChromaSession::ColourHSVGradientBKeyword), value, colour.hue(), colour.saturation(), colour.value(), colour.alpha());
	// -- Custom Gradient
	for (csp = collection->customColourScalePoints(); csp != NULL; csp = csp->next)
	{
		parser.writeLineF("%s  %s %f %i %i %i %i\n", indent, UChromaSession::collectionKeyword(UChromaSession::ColourCustomGradientKeyword), csp->value(), csp->colour().red(), csp->colour().green(), csp->colour().blue(), csp->colour().alpha());
	}
	// -- Alpha control
	parser.writeLineF("%s  %s '%s'\n", indent, UChromaSession::collectionKeyword(UChromaSession::ColourAlphaControlKeyword), Collection::alphaControl(collection->alphaControl()));
	parser.writeLineF("%s  %s %f\n", indent, UChromaSession::collectionKeyword(UChromaSession::ColourAlphaFixedKeyword), collection->fixedAlpha());

	// Display
	parser.writeLineF("%s  %s %f '%s'\n", indent, UChromaSession::collectionKeyword(UChromaSession::LineStyleKeyword), collection->displayLineStyle().width(), LineStipple::stipple[collection->displayLineStyle().stipple()].name);
	parser.writeLineF("%s  %s %f\n", indent, UChromaSession::collectionKeyword(UChromaSession::ShininessKeyword), collection->displaySurfaceShininess());
	parser.writeLineF("%s  %s %s\n", indent, UChromaSession::collectionKeyword(UChromaSession::StyleKeyword), Collection::displayStyle(collection->displayStyle()));
	parser.writeLineF("%s  %s %s\n", indent, UChromaSession::collectionKeyword(UChromaSession::VisibleCollectionKeyword), stringBool(collection->visible()));

	// Loop over datasets
	for (DataSet* dataSet = collection->dataSets(); dataSet != NULL; dataSet = dataSet->next) writeDataSetBlock(parser, dataSet, indentLevel);

	// Write FitKernel data if present
	if (collection->fitKernel()) writeFitParametersBlock(parser, collection->fitKernel(), indentLevel);

	// Additional data
	// -- Fits
	for (Collection* fit = collection->fits(); fit != NULL; fit = fit->next) writeCollectionBlock(parser, fit, Collection::FitCollection, indentLevel+1);
	// -- Extracted Data
	for (Collection* extract = collection->slices(); extract != NULL; extract = extract->next) writeCollectionBlock(parser, extract, Collection::ExtractedCollection, indentLevel+1);

	parser.writeLineF("%s%s\n", indent, UChromaSession::collectionKeyword(UChromaSession::EndCollectionKeyword));

	return true;
}

// Write DataSetBlock keywords
bool UChromaSession::writeDataSetBlock(LineParser& parser, DataSet* dataSet, int indentLevel)
{
	// Construct indent string
	char* indent = new char[indentLevel*2+1];
	for (int n=0; n<indentLevel*2; ++n) indent[n] = ' ';
	indent[indentLevel*2] = '\0';

	parser.writeLineF("%s  %s '%s'\n", indent, UChromaSession::collectionKeyword(UChromaSession::DataSetDefinitionKeyword), qPrintable(dataSet->name()));
	if (dataSet->dataSource() == DataSet::FileSource) parser.writeLineF("%s    %s %s '%s'\n", indent, UChromaSession::dataSetKeyword(UChromaSession::SourceKeyword), DataSet::dataSource(dataSet->dataSource()), qPrintable(dataSet->sourceFileName()));
	else parser.writeLineF("%s    %s %s\n", indent, UChromaSession::dataSetKeyword(UChromaSession::SourceKeyword), DataSet::dataSource(dataSet->dataSource()));
	parser.writeLineF("%s    %s %f\n", indent, UChromaSession::dataSetKeyword(UChromaSession::ZKeyword), dataSet->data().z());
	parser.writeLineF("%s    %s\n", indent, UChromaSession::dataSetKeyword(UChromaSession::DataKeyword));
	for (int n=0; n< dataSet->data().nPoints(); ++n) parser.writeLineF("%s      %f  %f\n", indent, dataSet->data().x(n), dataSet->data().y(n));
	parser.writeLineF("%s    End%s\n", indent, UChromaSession::dataSetKeyword(UChromaSession::DataKeyword));
	parser.writeLineF("%s  %s\n", indent, UChromaSession::dataSetKeyword(UChromaSession::EndDataSetKeyword));

	return true;
}

// Write FitParametersBlock keywords
bool UChromaSession::writeFitParametersBlock(LineParser& parser, FitKernel* fitKernel, int indentLevel)
{
	// Construct indent string
	char* indent = new char[indentLevel*2+1];
	for (int n=0; n<indentLevel*2; ++n) indent[n] = ' ';
	indent[indentLevel*2] = '\0';

	parser.writeLineF("%s  %s\n", indent, UChromaSession::collectionKeyword(UChromaSession::FitParametersBlockKeyword));
	for (RefListItem<ReferenceVariable,bool>* ri = fitKernel->usedReferences(); ri != NULL; ri = ri->next)
	{
		ReferenceVariable* refVar = ri->item;
		parser.writeLineF("%s    %s %s %s %i %i %s %i %i '%s'\n", indent, UChromaSession::fitParametersKeyword(UChromaSession::ReferenceKeyword), qPrintable(refVar->name()), IndexData::indexType(refVar->xIndex().type()), refVar->xIndex().index(), refVar->xIndex().offset(), IndexData::indexType(refVar->zIndex().type()), refVar->zIndex().index(), refVar->zIndex().offset(), qPrintable(refVar->zDataSetName()));
	}
	parser.writeLineF("%s    %s '%s'\n", indent, UChromaSession::fitParametersKeyword(UChromaSession::EquationKeyword), qPrintable(fitKernel->equationText()));
	parser.writeLineF("%s    %s %s\n", indent, UChromaSession::fitParametersKeyword(UChromaSession::GlobalKeyword), stringBool(fitKernel->global()));
	parser.writeLineF("%s    %s %s\n", indent, UChromaSession::fitParametersKeyword(UChromaSession::OrthogonalKeyword), stringBool(fitKernel->orthogonal()));
	parser.writeLineF("%s    %s %f\n", indent, UChromaSession::fitParametersKeyword(UChromaSession::LimitStrengthKeyword), fitKernel->limitStrength());
	for (RefListItem<EquationVariable,bool>* ri = fitKernel->usedVariables(); ri != NULL; ri = ri->next)
	{
		EquationVariable* eqVar = ri->item;
		parser.writeLineF("%s    %s %s %s %f %s %f %s %f\n", indent, UChromaSession::fitParametersKeyword(UChromaSession::VariableKeyword), qPrintable(eqVar->name()), stringBool(eqVar->fit()), eqVar->value(), stringBool(eqVar->maximumLimitEnabled()), eqVar->minimumLimit(), stringBool(eqVar->maximumLimitEnabled()), eqVar->maximumLimit());
	}
	parser.writeLineF("%s    %s %s\n", indent, UChromaSession::fitParametersKeyword(UChromaSession::XRangeTypeKeyword), FitKernel::rangeType(fitKernel->xRange()));
	parser.writeLineF("%s    %s %f %f\n", indent, UChromaSession::fitParametersKeyword(UChromaSession::XRangeAbsoluteKeyword), fitKernel->absoluteXMin(), fitKernel->absoluteXMax());
	parser.writeLineF("%s    %s %i %i\n", indent, UChromaSession::fitParametersKeyword(UChromaSession::XRangeIndexKeyword), fitKernel->indexXMin()+1, fitKernel->indexXMax()+1);
	parser.writeLineF("%s    %s %i\n", indent, UChromaSession::fitParametersKeyword(UChromaSession::XRangeIndexSingleKeyword), fitKernel->indexXSingle()+1);
	parser.writeLineF("%s    %s %s\n", indent, UChromaSession::fitParametersKeyword(UChromaSession::ZRangeTypeKeyword), FitKernel::rangeType(fitKernel->zRange()));
	parser.writeLineF("%s    %s %f %f\n", indent, UChromaSession::fitParametersKeyword(UChromaSession::ZRangeAbsoluteKeyword), fitKernel->absoluteZMin(), fitKernel->absoluteZMax());
	parser.writeLineF("%s    %s %i %i\n", indent, UChromaSession::fitParametersKeyword(UChromaSession::ZRangeIndexKeyword), fitKernel->indexZMin()+1, fitKernel->indexZMax()+1);
	parser.writeLineF("%s    %s %i\n", indent, UChromaSession::fitParametersKeyword(UChromaSession::ZRangeIndexSingleKeyword), fitKernel->indexZSingle()+1);
	parser.writeLineF("%s  %s\n", indent, UChromaSession::fitParametersKeyword(UChromaSession::EndFitParametersKeyword));

	return true;
}

// Write SettingsBlock keywords
bool UChromaSession::writeSettingsBlock(LineParser& parser)
{
	parser.writeLineF("%s\n", UChromaSession::inputBlock(UChromaSession::SettingsBlock));
	parser.writeLineF("  %s \"%s\" %i %i %s %i\n", UChromaSession::settingsKeyword(UChromaSession::ImageExportKeyword), qPrintable(imageExportFileName_), imageExportWidth_, imageExportHeight_, UChromaSession::imageFormatExtension(imageExportFormat_), imageExportMaintainAspect_);
	parser.writeLineF("%s\n", UChromaSession::settingsKeyword(UChromaSession::EndSettingsKeyword));

	return true;
}

// Write ViewBlock keywords
bool UChromaSession::writeViewBlock(LineParser& parser)
{
	parser.writeLineF("%s\n", UChromaSession::inputBlock(UChromaSession::ViewBlock));
	parser.writeLineF("  %s %i %i\n", UChromaSession::viewKeyword(UChromaSession::GridKeyword), viewLayout_.nColumns(), viewLayout_.nRows());

	// Loop over defined panes
	for (ViewPane* pane = viewLayout_.panes(); pane != NULL; pane = pane->next) writeViewPaneBlock(parser, pane);

	parser.writeLineF("%s\n", UChromaSession::viewKeyword(UChromaSession::EndViewKeyword));

	return true;
}

// Write ViewPaneBlock keywords
bool UChromaSession::writeViewPaneBlock(LineParser& parser, ViewPane* pane)
{
	parser.writeLineF("  %s '%s'\n", UChromaSession::viewKeyword(UChromaSession::ViewPaneBlockKeyword), qPrintable(pane->name()));
	parser.writeLineF("    %s %s\n", UChromaSession::viewPaneKeyword(UChromaSession::AutoPositionTitlesKeyword), stringBool(pane->axes().autoPositionTitles()));
	for (int axis=0; axis < 3; ++axis) writeAxisBlock(parser, pane->axes(), axis);
	parser.writeLineF("    %s %i\n", UChromaSession::viewPaneKeyword(UChromaSession::BoundingBoxKeyword), pane->boundingBox());
	parser.writeLineF("    %s %f\n", UChromaSession::viewPaneKeyword(UChromaSession::BoundingBoxPlaneYKeyword), pane->boundingBoxPlaneY());
	parser.writeLineF("    %s %s\n", UChromaSession::viewPaneKeyword(UChromaSession::FlatLabelsKeyword), stringBool(pane->flatLabels()));
	parser.writeLineF("    %s %i %i %i %i\n", UChromaSession::viewPaneKeyword(UChromaSession::GeometryKeyword), pane->bottomEdge(), pane->leftEdge(), pane->width(), pane->height()); 
	parser.writeLineF("    %s %f\n", UChromaSession::viewPaneKeyword(UChromaSession::LabelPointSizeKeyword), pane->labelPointSize());
	parser.writeLineF("    %s %f\n", UChromaSession::viewPaneKeyword(UChromaSession::TitlePointSizeKeyword), pane->titlePointSize());
	Matrix mat = pane->viewRotation();
	Vec3<double> trans = pane->viewTranslation();
	parser.writeLineF("    %s %f %f %f\n", UChromaSession::viewPaneKeyword(UChromaSession::RotationXKeyword), mat[0], mat[1], mat[2]);
	parser.writeLineF("    %s %f %f %f\n", UChromaSession::viewPaneKeyword(UChromaSession::RotationYKeyword), mat[4], mat[5], mat[6]);
	parser.writeLineF("    %s %f %f %f\n", UChromaSession::viewPaneKeyword(UChromaSession::RotationZKeyword), mat[8], mat[9], mat[10]);
	parser.writeLineF("    %s %f %f %f\n", UChromaSession::viewPaneKeyword(UChromaSession::TranslationKeyword), trans.x, trans.y, trans.z);
	parser.writeLineF("    %s %s\n", UChromaSession::viewPaneKeyword(UChromaSession::PerspectiveKeyword), stringBool(pane->hasPerspective()));
	parser.writeLineF("    %s '%s'\n", UChromaSession::viewPaneKeyword(UChromaSession::RoleKeyword), ViewPane::paneRole(pane->role()));
	for (TargetData* target = pane->collectionTargets(); target != NULL; target = target->next)
	{
		if (!Collection::objectValid(target->collection(), "collection in UChromaSession::writeViewPaneBlock")) continue;
		parser.writeLineF("    %s '%s'\n", UChromaSession::viewPaneKeyword(UChromaSession::RoleTargetCollectionKeyword), qPrintable(target->collection()->locator()));
	}
	for (RefListItem<ViewPane,bool>* ri = pane->paneTargets(); ri != NULL; ri = ri->next) parser.writeLineF("    %s '%s'\n", UChromaSession::viewPaneKeyword(UChromaSession::RoleTargetPaneKeyword), qPrintable(ri->item->name()));
	parser.writeLineF("    %s %s\n", UChromaSession::viewPaneKeyword(UChromaSession::UseBestFlatViewKeyword), stringBool(pane->axes().useBestFlatView()));
	parser.writeLineF("    %s '%s'\n", UChromaSession::viewPaneKeyword(UChromaSession::ViewTypeKeyword), ViewPane::viewType(pane->viewType()));
	parser.writeLineF("  %s\n", UChromaSession::viewPaneKeyword(UChromaSession::EndViewPaneKeyword));

	return true;
}

// Save current data to file specified
bool UChromaSession::saveSession(QString fileName)
{
	LineParser parser(fileName, true);
	if (!parser.ready())
	{
		QMessageBox::warning(uChroma_, "Error", "Can't open specified file for writing.");
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
