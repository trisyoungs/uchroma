/*
	*** Session Load
	*** src/session/load.cpp
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

#define CHECKIOFAIL { if (hardIOFail_) { return false; } else { break; } }

// Parse AxisBlock keywords
bool UChromaSession::readAxisBlock(LineParser& parser, Axes& axes, int axis)
{
	TextPrimitive::TextAnchor anchor;
	LineStipple::StippleType stipple;
	NumberFormat::FormatType ft;
	Axes::AutoScaleMethod as;

	while (!parser.atEnd())
	{
		// Get line from file
		parser.getArgs(LineParser::UseQuotes + LineParser::SkipBlanks);

		// Get keyword and check number of arguments provided
		UChromaSession::AxisKeyword axisKwd = UChromaSession::axisKeyword(parser.argString(0));
		if ((axisKwd != UChromaSession::nAxisKeywords) && (UChromaSession::axisKeywordNArguments(axisKwd) > (parser.nArgs()-1)))
		{
			msg.print("Error : Axis keyword '%s' requires %i arguments, but only %i have been provided.\n", UChromaSession::axisKeyword(axisKwd), UChromaSession::axisKeywordNArguments(axisKwd), parser.nArgs()-1);
			return false;
		}
		switch (axisKwd)
		{
			// Autoscale method
			case (UChromaSession::AutoScaleKeyword):
				as = Axes::autoScaleMethod(parser.argString(1));
				if (as == Axes::nAutoScaleMethods)
				{
					msg.print("Warning: Unrecognised autoscale method '%s'. Defaulting to '%s'.\n", parser.argChar(1), Axes::autoScaleMethod(Axes::NoAutoScale));
					as = Axes::NoAutoScale;
					CHECKIOFAIL
				}
				axes.setAutoScale(axis, as);
				break;
			// Auto ticks
			case (UChromaSession::AutoTicksKeyword):
				axes.setAutoTicks(axis, parser.argb(1));
				break;
			// End input block
			case (UChromaSession::EndAxisKeyword):
				return true;
				break;
			// First ticks
			case (UChromaSession::FirstTickKeyword):
				axes.setFirstTick(axis, parser.argd(1));
				break;
			// Fractional positioning flag
			case (UChromaSession::FractionalPositioningKeyword):
				axes.setPositionIsFractional(axis, parser.argb(1));
				break;
			// GridLines
			case (UChromaSession::GridLinesKeyword):
				axes.setGridLinesMajor(axis, parser.argb(1));
				axes.setGridLinesMinor(axis, parser.argb(2));
				axes.setGridLinesFull(axis, parser.argb(3));
				break;
			// GridLine major style
			case (UChromaSession::GridLineMajorStyleKeyword):
				axes.gridLineMajorStyle(axis).setWidth(parser.argd(1));
				stipple = LineStipple::stippleType(parser.argString(2));
				if (stipple == LineStipple::nStippleTypes)
				{
					msg.print("Warning: Unrecognised line stipple type '%s'. Defaulting to 'NoStipple'.\n", parser.argChar(2));
					stipple = LineStipple::NoStipple;
					CHECKIOFAIL
				}
				axes.gridLineMajorStyle(axis).setStipple(stipple);
				axes.gridLineMajorStyle(axis).setColour(parser.argd(3), parser.argd(4), parser.argd(5), parser.argd(6));
				break;
			// GridLine minor style
			case (UChromaSession::GridLineMinorStyleKeyword):
				axes.gridLineMinorStyle(axis).setWidth(parser.argd(1));
				stipple = LineStipple::stippleType(parser.argString(2));
				if (stipple == LineStipple::nStippleTypes)
				{
					msg.print("Warning: Unrecognised line stipple type '%s'. Defaulting to 'NoStipple'.\n", parser.argChar(2));
					stipple = LineStipple::NoStipple;
					CHECKIOFAIL
				}
				axes.gridLineMinorStyle(axis).setStipple(stipple);
				axes.gridLineMinorStyle(axis).setColour(parser.argd(3), parser.argd(4), parser.argd(5), parser.argd(6));
				break;
			// Invert
			case (UChromaSession::InvertKeyword):
				axes.setInverted(axis, parser.argb(1));
				break;
			// Axis label anchor
			case (UChromaSession::LabelAnchorKeyword):
				anchor = TextPrimitive::textAnchor(parser.argString(1));
				if (anchor == TextPrimitive::nTextAnchors)
				{
					msg.print("Warning: Unrecognised text anchor '%s'. Defaulting to '%s'.\n", parser.argChar(1), TextPrimitive::textAnchor(TextPrimitive::TopMiddleAnchor));
					anchor = TextPrimitive::TopMiddleAnchor;
					CHECKIOFAIL
				}
				axes.setLabelAnchor(axis, anchor);
				break;
			// Axis label orientation
			case (UChromaSession::LabelOrientationKeyword):
				axes.setLabelOrientation(axis, 0, parser.argd(1));
				axes.setLabelOrientation(axis, 1, parser.argd(2));
				axes.setLabelOrientation(axis, 2, parser.argd(3));
				break;
			// Limits
			case (UChromaSession::LimitsKeyword):
				axes.setMin(axis, parser.argd(1));
				axes.setMax(axis, parser.argd(2));
				break;
			// Axis logarithmic flag
			case (UChromaSession::LogarithmicKeyword):
				axes.setLogarithmic(axis, parser.argb(1));
				break;
			// Axis minor ticks
			case (UChromaSession::MinorTicksKeyword):
				axes.setMinorTicks(axis, parser.argi(1));
				break;
			// Number Format
			case (UChromaSession::NumberFormatKeyword):
				ft = NumberFormat::formatType(parser.argString(1));
				if (ft == NumberFormat::nNumberFormats)
				{
					msg.print("Warning: Unrecognised number format '%s'. Defaulting to '%s'.\n", parser.argChar(1), NumberFormat::formatType(NumberFormat::DecimalFormat));
					ft = NumberFormat::DecimalFormat;
					CHECKIOFAIL
				}
				axes.numberFormat(axis).setType(ft);
				axes.numberFormat(axis).setNDecimals(parser.argi(2));
				axes.numberFormat(axis).setUseUpperCaseExponent(parser.argb(3));
				axes.numberFormat(axis).setForcePrecedingPlus(parser.argb(4));
				break;
			// Axis position (in fractional axis coordinates)
			case (UChromaSession::PositionFractionalKeyword):
				axes.setPositionFractional(axis, 0, parser.argd(1));
				axes.setPositionFractional(axis, 1, parser.argd(2));
				axes.setPositionFractional(axis, 2, parser.argd(3));
				break;
			// Axis position (in real surface-space coordinates)
			case (UChromaSession::PositionRealKeyword):
				axes.setPositionReal(axis, 0, parser.argd(1));
				axes.setPositionReal(axis, 1, parser.argd(2));
				axes.setPositionReal(axis, 2, parser.argd(3));
				break;
			// Axis stretch factors
			case (UChromaSession::StretchKeyword):
				axes.setStretch(axis, parser.argd(1));
				break;
			// Axis tick deltas
			case (UChromaSession::TickDeltaKeyword):
				axes.setTickDelta(axis, parser.argd(1));
				break;
			// Axis tick direction
			case (UChromaSession::TickDirectionKeyword):
				axes.setTickDirection(axis, 0, parser.argd(1));
				axes.setTickDirection(axis, 1, parser.argd(2));
				axes.setTickDirection(axis, 2, parser.argd(3));
				break;
			// Axis title
			case (UChromaSession::TitleKeyword):
				axes.setTitle(axis, parser.argString(1));
				break;
			// Axis title anchor
			case (UChromaSession::TitleAnchorKeyword):
				anchor = TextPrimitive::textAnchor(parser.argString(1));
				if (anchor == TextPrimitive::nTextAnchors)
				{
					msg.print("Warning: Unrecognised text anchor '%s'. Defaulting to 'TopMiddle'.\n");
					anchor = TextPrimitive::TopMiddleAnchor;
					CHECKIOFAIL
				}
				axes.setTitleAnchor(axis, anchor);
				break;
			// Axis title orientation
			case (UChromaSession::TitleOrientationKeyword):
				axes.setTitleOrientation(axis, 0, parser.argd(1));
				axes.setTitleOrientation(axis, 1, parser.argd(2));
				axes.setTitleOrientation(axis, 2, parser.argd(3));
				axes.setTitleOrientation(axis, 3, parser.argd(4));
				break;
			// Axis visibility
			case (UChromaSession::VisibleAxisKeyword):
				axes.setVisible(axis, parser.argb(1));
				break;
			// Unrecognised keyword
			default:
				msg.print("Warning: Unrecognised axis keyword: %s\n", parser.argChar(0));
				CHECKIOFAIL
				break;
		}
	}
	msg.print("Error : Unterminated 'Axis' block.\n");
	return false;
}

// Read CollectionBlock keywords
bool UChromaSession::readCollectionBlock(LineParser& parser, Collection* collection)
{
	DataSet* dataSet;
	int xyz;
	double alpha;
	Collection::AlphaControl ac;
	Collection::ColourSource cs;
	Collection::DisplayStyle ds;
	LineStipple::StippleType stipple;

	while (!parser.atEnd())
	{
		// Get line from file
		parser.getArgs(LineParser::UseQuotes + LineParser::SkipBlanks);

		// Get keyword and check number of arguments provided
		UChromaSession::CollectionKeyword collectionKwd = collectionKeyword(parser.argString(0));
		if ((collectionKwd != UChromaSession::nCollectionKeywords) && (collectionKeywordNArguments(collectionKwd) > (parser.nArgs()-1)))
		{
			msg.print("Error: Collection keyword '%s' requires %i arguments, but only %i have been provided.\n", collectionKeyword(collectionKwd), collectionKeywordNArguments(collectionKwd), parser.nArgs()-1);
			return false;
		}
		switch (collectionKwd)
		{
			// Colour alpha control
			case (UChromaSession::ColourAlphaControlKeyword):
				ac = Collection::alphaControl(parser.argString(1));
				if (ac == Collection::nAlphaControls)
				{
					msg.print("Warning: Unrecognised alpha control type '%s'. Defaulting to '%s'.\n", parser.argChar(1), Collection::alphaControl(Collection::OwnAlpha));
					ac = Collection::OwnAlpha;
					CHECKIOFAIL
				}
				collection->setAlphaControl(ac);
				break;
			// Colour alpha fixed value
			case (UChromaSession::ColourAlphaFixedKeyword):
				alpha = parser.argd(1);
				if ((alpha < 0.0) || (alpha > 1.0))
				{
					msg.print("Warning: Alpha value (%f) is out of range for %s keyword - it will be reset to 1.0.\n", alpha, collectionKeyword(collectionKwd));
					alpha = 1.0;
					CHECKIOFAIL
				}
				collection->setFixedAlpha(alpha);
				break;
			// Colour Custom Gradient point definition
			case (UChromaSession::ColourCustomGradientKeyword):
				collection->addCustomColourScalePoint(parser.argd(1), QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)));
				break;
			// Colour Linear Gradient point definition
			case (UChromaSession::ColourRGBGradientAKeyword):
			case (UChromaSession::ColourRGBGradientBKeyword):
				collection->setColourScalePoint(Collection::RGBGradientSource, QColor(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)), parser.argd(1), collectionKwd == UChromaSession::ColourRGBGradientAKeyword ? 0 : 1);
				break;
			// Colour Linear HSV Gradient point definition
			case (UChromaSession::ColourHSVGradientAKeyword):
			case (UChromaSession::ColourHSVGradientBKeyword):
				collection->setColourScalePoint(Collection::HSVGradientSource, QColor::fromHsv(parser.argi(2), parser.argi(3), parser.argi(4), parser.argi(5)), parser.argd(1), collectionKwd == UChromaSession::ColourHSVGradientAKeyword ? 0 : 1);
				break;
			// Colour single colour definition
			case (UChromaSession::ColourSingleKeyword):
				collection->setColourScalePoint(Collection::SingleColourSource, QColor(parser.argi(1), parser.argi(2), parser.argi(3), parser.argi(4)));
				break;
			// Colour source
			case (UChromaSession::ColourSourceKeyword):
				cs = Collection::colourSource(parser.argString(1));
				if (cs == Collection::nColourSources)
				{
					msg.print("Warning: Unrecognised colour source '%s'. Defaulting to '%s'.\n", parser.argChar(1), Collection::colourSource(Collection::SingleColourSource));
					cs = Collection::SingleColourSource;
					CHECKIOFAIL
				}
				collection->setColourSource(cs);
				break;
			// Dataset directory
			case (UChromaSession::DataDirectoryKeyword):
				collection->setDataFileDirectory(QDir(parser.argString(1)));
				if (!collection->dataFileDirectory().isReadable())
				{
					QMessageBox::StandardButton button = QMessageBox::warning(uChroma_, "Error", "The data directory specified (" + collection->dataFileDirectory().absolutePath() + ") does not exist or is unreadable.\nDo you want to reset the datafile location?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					if (button == QMessageBox::Yes)
					{
						QString dir = QFileDialog::getExistingDirectory(uChroma_, "Data Directory", "Choose the directory containing the required files:");
						if (!dir.isEmpty()) collection->setDataFileDirectory(dir);
					}
				}
				break;
			// DataSet
			case (UChromaSession::DataSetDefinitionKeyword):
				// Create new dataset
				dataSet = collection->addDataSet();
				dataSet->setName(parser.argString(1));
				if (!readDataSetBlock(parser, dataSet, collection)) return false;
				break;
			// End input block
			case (UChromaSession::EndCollectionKeyword):
				return true;
				break;
			// Fit data block
			case (UChromaSession::FitBlockKeyword):
				if (!readCollectionBlock(parser, collection->addFit(parser.argString(1)))) return false;
				break;
			// Fit parameters block
			case (UChromaSession::FitParametersBlockKeyword):
				// Check that a FitKernel exists in the current collection
				if (!collection->fitKernel())
				{
					msg.print("Warning: No FitKernel exists in the collection '%s', but a FitParameters block was found.\nCreating a new one...\n", qPrintable(collection->name()));
					collection->addFitKernel();
				}
				if (!readFitParametersBlock(parser, collection->fitKernel())) return false;
				break;
			// Interpolate flags
			case (UChromaSession::InterpolateKeyword):
				collection->setInterpolate(0, parser.argb(1));
				collection->setInterpolate(2, parser.argb(2));
				break;
			// Interpolate constrain flags
			case (UChromaSession::InterpolateConstrainKeyword):
				collection->setInterpolateConstrained(0, parser.argb(1));
				collection->setInterpolateConstrained(2, parser.argb(2));
				break;
			// Interpolation step flags
			case (UChromaSession::InterpolateStepKeyword):
				collection->setInterpolationStep(0, parser.argd(1));
				collection->setInterpolationStep(2, parser.argd(2));
				break;
			// Line style
			case (UChromaSession::LineStyleKeyword):
				collection->displayLineStyle().setWidth(parser.argd(1));
				stipple = LineStipple::stippleType(parser.argString(2));
				if (stipple == LineStipple::nStippleTypes)
				{
					msg.print("Warning: Unrecognised line stipple type '%s'. Defaulting to 'NoStipple'.\n", parser.argChar(2));
					stipple = LineStipple::NoStipple;
					CHECKIOFAIL
				}
				collection->displayLineStyle().setStipple(stipple);
				break;
			// Surface shininess
			case (UChromaSession::ShininessKeyword):
				collection->setDisplaySurfaceShininess(parser.argd(1));
				break;
			// Slice data block
			case (UChromaSession::SliceBlockKeyword):
				if (!readCollectionBlock(parser, collection->addSlice(parser.argString(1)))) return false;
				break;
			// Display style
			case (UChromaSession::StyleKeyword):
				ds = Collection::displayStyle(parser.argString(1));
				if (ds == Collection::nDisplayStyles)
				{
					msg.print("Warning: Unrecognised display style '%s'.\n", parser.argChar(1));
					CHECKIOFAIL
				}
				collection->setDisplayStyle(ds);
				break;
			// Data Transform
			case (UChromaSession::TransformXKeyword):
			case (UChromaSession::TransformYKeyword):
			case (UChromaSession::TransformZKeyword):
				xyz = collectionKwd - UChromaSession::TransformXKeyword;
				collection->setTransformEnabled(xyz, parser.argb(1));
				collection->setTransformEquation(xyz,  parser.argString(2));
				break;
			// Visible flag
			case (UChromaSession::VisibleCollectionKeyword):
				collection->setVisible(parser.argb(1));
				break;
			// Unrecognised Keyword
			default:
				msg.print("Warning: Unrecognised collection keyword: %s\n", parser.argChar(0));
				CHECKIOFAIL
				break;
		}
	}
	msg.print("Error : Unterminated 'Collection' block.\n");
	return false;
}

// Read DataSetBlock keywords
bool UChromaSession::readDataSetBlock(LineParser& parser, DataSet* dataSet, Collection* collection)
{
	bool foundEnd;
	DataSet::DataSource source;
	Data2D data;
	while (!parser.atEnd())
	{
		// Get line from file
		parser.getArgs(LineParser::UseQuotes + LineParser::SkipBlanks);

		// Get keyword and check number of arguments provided
		UChromaSession::DataSetKeyword dataSetKwd = dataSetKeyword(parser.argString(0));
		if ((dataSetKwd != UChromaSession::nDataSetKeywords) && (dataSetKeywordNArguments(dataSetKwd) > (parser.nArgs()-1)))
		{
			msg.print("Error : DataSet keyword '%s' requires %i arguments, but only %i have been provided.\n", dataSetKeyword(dataSetKwd), dataSetKeywordNArguments(dataSetKwd), parser.nArgs()-1);
			return false;
		}
		switch (dataSetKwd)
		{
			case (UChromaSession::DataKeyword):
				data.reset();
				foundEnd = false;
				do
				{
					parser.getArgs(LineParser::Defaults);
					// Check for 'EndData'
					if (parser.argString(0) == "EndData") foundEnd = true;
					else data.addPoint(parser.argd(0), parser.argd(1));
				} while ((!foundEnd) && (!parser.atEnd()));
				if (!foundEnd)
				{
					msg.print("Error : Unterminated 'Data' block in dataSet '%s'.\n", qPrintable(dataSet->name()));
					return false;
				}
				break;
			case (UChromaSession::EndDataSetKeyword):
				// Store acquired data before we return
				dataSet->setData(data);
				return true;
				break;
			case (UChromaSession::SourceKeyword):
				source = DataSet::dataSource(parser.argString(1));
				if (source == DataSet::nDataSources)
				{
					msg.print("Warning: Datasource for dataSet not recognised (%s)\n", parser.argChar(1));
					CHECKIOFAIL
					break;
				}
				dataSet->setDataSource(source);
				// Depending on the source, we might expect other data here...
				if (source == DataSet::FileSource)
				{
					if (parser.hasArg(2)) dataSet->setSourceFileName(parser.argString(2));
					else
					{
						msg.print("Error: Expected data file name after 'Source File' declaration in dataSet '%s'.\n", qPrintable(dataSet->name()));
						return false;
					}
				}
				break;
			case (UChromaSession::ZKeyword):
				data.setZ(parser.argd(1));
				collection->setDataSetZ(dataSet, parser.argd(1));
				break;
			// Unrecognised Keyword
			default:
				msg.print("Warning: Unrecognised DataSet keyword: %s\n", parser.argChar(0));
				CHECKIOFAIL
				break;
		}
	}
	msg.print("Error : Unterminated 'DataSet' block.\n");
	return false;
}

// Read FitParametersBlock keywords
bool UChromaSession::readFitParametersBlock(LineParser& parser, FitKernel* fitKernel)
{
	FitKernel::RangeType rangeType;
	IndexData::IndexType indexType;
	EquationVariable* eqVar;
	ReferenceVariable* refVar;
	DataSpaceRange* dataSpaceRange;
	while (!parser.atEnd())
	{
		// Get line from file
		parser.getArgs(LineParser::UseQuotes + LineParser::SkipBlanks);

		// Get keyword and check number of arguments provided
		UChromaSession::FitParametersKeyword fitParamsKwd = fitParametersKeyword(parser.argString(0));
		if ((fitParamsKwd != UChromaSession::nFitParametersKeywords) && (fitParametersKeywordNArguments(fitParamsKwd) > (parser.nArgs()-1)))
		{
			msg.print("Error : FitParameters keyword '%s' requires %i arguments, but only %i have been provided.\n", fitParametersKeyword(fitParamsKwd), fitParametersKeywordNArguments(fitParamsKwd), parser.nArgs()-1);
			return false;
		}
		switch (fitParamsKwd)
		{
			case (UChromaSession::EndFitParametersKeyword):
				return true;
				break;
			case (UChromaSession::EquationKeyword):
				fitKernel->setEquation(parser.argString(1));
				break;
			case (UChromaSession::FitResultsBlockKeyword):
				// Make sure that the DataSpace has been initialised....
				dataSpaceRange = NULL;
				if (!fitKernel->initialiseDataSpace()) msg.print("Warning: Failed to create DataSpace for FitKernel, so cannot store FitResults.\n");
				else
				{
					// Get DataSpaceRange with index specified
					dataSpaceRange = fitKernel->dataSpaceRange(parser.argi(1)-1);
					if (dataSpaceRange == NULL) msg.print("Warning: Failed to get range with index %i in FitKernel. FitResults will be ignored.\n", parser.argi(1));
				}
				if (!readFitResultsBlock(parser, dataSpaceRange)) return false;
				break;
			case (UChromaSession::GlobalKeyword):
				fitKernel->setGlobal(parser.argb(1));
				break;
			case (UChromaSession::LimitStrengthKeyword):
				fitKernel->setLimitStrength(parser.argd(1));
				break;
			case (UChromaSession::OrthogonalKeyword):
				fitKernel->setOrthogonal(parser.argb(1));
				break;
			case (UChromaSession::ReferenceKeyword):
				// Create new reference with this name
				refVar = fitKernel->addReference(parser.argString(1));
				if (!refVar) CHECKIOFAIL
				indexType = IndexData::indexType(parser.argString(2));
				if (indexType == IndexData::nIndexTypes)
				{
					msg.print("Warning: Unrecognised type '%s' for reference '%s' - defaulting to 'Normal'.\n", parser.argChar(2), parser.argChar(1));
					CHECKIOFAIL
				}
				refVar->xIndex().setType(indexType);
				refVar->xIndex().setIndex(parser.argi(3));
				refVar->xIndex().setOffset(parser.argi(4));
				indexType = IndexData::indexType(parser.argString(5));
				if (indexType == IndexData::nIndexTypes)
				{
					msg.print("Warning: Unrecognised type '%s' for reference '%s' - defaulting to 'Normal'.\n", parser.argChar(5), parser.argChar(1));
					CHECKIOFAIL
				}
				refVar->zIndex().setType(indexType);
				refVar->zIndex().setIndex(parser.argi(6));
				refVar->zIndex().setOffset(parser.argi(7));
				refVar->setZDataSetName(parser.argString(8));
				break;
			case (UChromaSession::VariableKeyword):
				// First, see if named variable exists
				eqVar = fitKernel->variable(parser.argString(1));
				if (!eqVar)
				{
					msg.print("Warning: Variable '%s' is not in fit equation, so will ignore settings in FitParameters.\n");
					CHECKIOFAIL
				}
				eqVar->setFit(parser.argb(2));
				eqVar->setValue(parser.argd(3));
				eqVar->setMinimumLimit(parser.argb(4), parser.argd(5));
				eqVar->setMaximumLimit(parser.argb(6), parser.argd(7));
				break;
			case (UChromaSession::XRangeTypeKeyword):
				rangeType = FitKernel::rangeType(parser.argString(1));
				if (rangeType == FitKernel::nRangeTypes)
				{
					msg.print("Warning: Unrecognised range type '%s' given for X in fit. Defaulting to '%s'\n", parser.argChar(1), FitKernel::rangeType(FitKernel::AbsoluteRange));
					rangeType = FitKernel::AbsoluteRange;
					CHECKIOFAIL
				}
				fitKernel->setXRange(rangeType);
				break;
			case (UChromaSession::XRangeAbsoluteKeyword):
				fitKernel->setAbsoluteXMin(parser.argd(1));
				fitKernel->setAbsoluteXMax(parser.argd(2));
				break;
			case (UChromaSession::XRangeIndexKeyword):
				fitKernel->setIndexXMin(parser.argi(1)-1);
				fitKernel->setIndexXMax(parser.argi(2)-1);
				break;
			case (UChromaSession::XRangeIndexSingleKeyword):
				fitKernel->setIndexXSingle(parser.argi(1)-1);
				break;
			case (UChromaSession::ZRangeTypeKeyword):
				rangeType = FitKernel::rangeType(parser.argString(1));
				if (rangeType == FitKernel::nRangeTypes)
				{
					msg.print("Warning: Unrecognised range type '%s' given for Z in fit. Defaulting to '%s'\n", parser.argChar(1), FitKernel::rangeType(FitKernel::AbsoluteRange));
					rangeType = FitKernel::AbsoluteRange;
					CHECKIOFAIL
				}
				fitKernel->setZRange(rangeType);
				break;
			case (UChromaSession::ZRangeAbsoluteKeyword):
				fitKernel->setAbsoluteZMin(parser.argd(1));
				fitKernel->setAbsoluteZMax(parser.argd(2));
				break;
			case (UChromaSession::ZRangeIndexKeyword):
				fitKernel->setIndexZMin(parser.argi(1)-1);
				fitKernel->setIndexZMax(parser.argi(2)-1);
				break;
			case (UChromaSession::ZRangeIndexSingleKeyword):
				fitKernel->setIndexZSingle(parser.argi(1)-1);
				break;
			// Unrecognised Keyword
			default:
				msg.print("Warning: Unrecognised FitParameters keyword: %s\n", parser.argChar(0));
				CHECKIOFAIL
				break;
		}
	}
	msg.print("Error : Unterminated 'FitParameters' block.\n");
	return false;
}

// Read FitResultsBlock keywords
bool UChromaSession::readFitResultsBlock(LineParser& parser, DataSpaceRange* dataSpaceRange)
{
	while (!parser.atEnd())
	{
		// Get line from file
		parser.getArgs(LineParser::UseQuotes + LineParser::SkipBlanks);

		// Get keyword and check number of arguments provided
		UChromaSession::FitResultsKeyword fitParamsKwd = fitResultsKeyword(parser.argString(0));
		if ((fitParamsKwd != UChromaSession::nFitResultsKeywords) && (fitResultsKeywordNArguments(fitParamsKwd) > (parser.nArgs()-1)))
		{
			msg.print("Error : FitResults keyword '%s' requires %i arguments, but only %i have been provided.\n", fitResultsKeyword(fitParamsKwd), fitResultsKeywordNArguments(fitParamsKwd), parser.nArgs()-1);
			return false;
		}
		switch (fitParamsKwd)
		{
			case (UChromaSession::EndFitResultsKeyword):
				return true;
				break;
			case (UChromaSession::FittedValueKeyword):
				if (!dataSpaceRange) continue;
				dataSpaceRange->setFittedValue(parser.argString(1), parser.argd(2));
				break;
			// Unrecognised Keyword
			default:
				msg.print("Warning: Unrecognised FitResults keyword: %s\n", parser.argChar(0));
				CHECKIOFAIL
				break;
		}
	}
	msg.print("Error : Unterminated 'FitResults' block.\n");
	return false;
}

// Read SettingsBlock keywords
bool UChromaSession::readSettingsBlock(LineParser& parser)
{
	UChromaSession::ImageFormat fmt;
	while (!parser.atEnd())
	{
		// Get line from file
		parser.getArgs(LineParser::UseQuotes + LineParser::SkipBlanks);

		// Get keyword and check number of arguments provided
		UChromaSession::SettingsKeyword settingsKwd = UChromaSession::settingsKeyword(parser.argString(0));
		if ((settingsKwd != UChromaSession::nSettingsKeywords) && (UChromaSession::settingsKeywordNArguments(settingsKwd) > (parser.nArgs()-1)))
		{
			msg.print("Error : Settings keyword '%s' requires %i arguments, but only %i have been provided.\n", UChromaSession::settingsKeyword(settingsKwd), UChromaSession::settingsKeywordNArguments(settingsKwd), parser.nArgs()-1);
			return false;
		}
		switch (settingsKwd)
		{
			// End input block
			case (UChromaSession::EndSettingsKeyword):
				return true;
				break;
			// Image Export info
			case (UChromaSession::ImageExportKeyword):
				imageExportFileName_ = parser.argString(1);
				imageExportWidth_ = parser.argi(2);
				imageExportHeight_ = parser.argi(3);
				fmt = UChromaSession::imageFormat(parser.argString(4));
				if (fmt == UChromaSession::nImageFormats)
				{
					msg.print("Warning: Unrecognised image format '%s'. Defaulting to '%s'.\n", parser.argChar(4), UChromaSession::imageFormatExtension(UChromaSession::PNGFormat));
					fmt = UChromaSession::PNGFormat;
					CHECKIOFAIL
				}
				imageExportFormat_ = fmt;
				imageExportMaintainAspect_ = parser.argi(5);
				break;
			// Unrecognised Keyword
			default:
				msg.print("Warning: Unrecognised settings keyword: %s\n", parser.argChar(0));
				CHECKIOFAIL
				break;
		}
	}
	msg.print("Error : Unterminated 'Settings' block.\n");
	return false;
}

// Read ViewBlock keywords
bool UChromaSession::readViewBlock(LineParser& parser)
{
	ViewPane* pane;
	while (!parser.atEnd())
	{
		// Get line from file
		parser.getArgs(LineParser::UseQuotes + LineParser::SkipBlanks);

		// Get keyword and check number of arguments provided
		UChromaSession::ViewKeyword viewKwd = UChromaSession::viewKeyword(parser.argString(0));
		if ((viewKwd != UChromaSession::nViewKeywords) && (UChromaSession::viewKeywordNArguments(viewKwd) > (parser.nArgs()-1)))
		{
			msg.print("Error : View keyword '%s' requires %i arguments, but only %i have been provided.\n", UChromaSession::viewKeyword(viewKwd), UChromaSession::viewKeywordNArguments(viewKwd), parser.nArgs()-1);
			return false;
		}
		switch (viewKwd)
		{
			// End input block
			case (UChromaSession::EndViewKeyword):
				return true;
				break;
			// Grid specification
			case (UChromaSession::GridKeyword):
				viewLayout_.setGrid(parser.argi(1), parser.argi(2));
				break;
			// ViewPane definition
			case (UChromaSession::ViewPaneBlockKeyword):
				// Check to see if pane has already been created (through it being referenced by another pane)
				pane = viewLayout_.pane(parser.argString(1));
				if (!pane) pane = viewLayout_.addPane(parser.argString(1));
				if (!readViewPaneBlock(parser, pane)) return false;
				break;
			// Unrecognised Keyword
			default:
				msg.print("Warning: Unrecognised view keyword: %s\n", parser.argChar(0));
				CHECKIOFAIL
				break;
		}
	}
	msg.print("Error : Unterminated 'View' block.\n");
	return false;
}

// Read ViewPane keywords
bool UChromaSession::readViewPaneBlock(LineParser& parser, ViewPane* pane)
{
	int xyz, axis;
	Collection* collection;
	Matrix mat;
	ViewPane* associatedPane;
	ViewPane::PaneRole role;
	ViewPane::ViewType vt;
	while (!parser.atEnd())
	{
		// Get line from file
		parser.getArgs(LineParser::UseQuotes + LineParser::SkipBlanks);

		// Get keyword and check number of arguments provided
		UChromaSession::ViewPaneKeyword viewPaneKwd = UChromaSession::viewPaneKeyword(parser.argString(0));
		if ((viewPaneKwd != UChromaSession::nViewPaneKeywords) && (UChromaSession::viewPaneKeywordNArguments(viewPaneKwd) > (parser.nArgs()-1)))
		{
			msg.print("Error: ViewPane keyword '%s' requires %i arguments, but only %i have been provided.\n", UChromaSession::viewPaneKeyword(viewPaneKwd), UChromaSession::viewPaneKeywordNArguments(viewPaneKwd), parser.nArgs()-1);
			return false;
		}
		switch (viewPaneKwd)
		{
			// Auto Position Axis Titles
			case (UChromaSession::AutoPositionTitlesKeyword):
				pane->axes().setAutoPositionTitles(parser.argb(1));
				break;
			// Axis block
			case (UChromaSession::AxisBlockKeyword):
				// Get target axis...
				axis = parser.argi(1);
				if ((axis < 0) || (axis > 2))
				{
					msg.print("Warning: Axis index is out of range in input file. Defaults will be used.\n");
					CHECKIOFAIL
				}
				if (!readAxisBlock(parser, pane->axes(), axis)) return false;
				break;
			// Bounding Box
			case (UChromaSession::BoundingBoxKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) >= ViewPane::nBoundingBoxes))
				{
					msg.print("Warning: Value is out of range for %s keyword.\n", UChromaSession::viewPaneKeyword(viewPaneKwd));
					CHECKIOFAIL
				}
				pane->setBoundingBox((ViewPane::BoundingBox) parser.argi(1));
				break;
			// Bounding Box plane y intercept
			case (UChromaSession::BoundingBoxPlaneYKeyword):
				pane->setBoundingBoxPlaneY(parser.argd(1));
				break;
			// End input block
			case (UChromaSession::EndViewPaneKeyword):
				return true;
				break;
			// Flat labels flag
			case (UChromaSession::FlatLabelsKeyword):
				pane->setFlatLabels(parser.argb(1));
				break;
			// Name
			case (UChromaSession::GeometryKeyword):
				pane->setBottomLeft(parser.argi(1), parser.argi(2));
				pane->setSize(parser.argi(3), parser.argi(4));
				break;
			// Label scale
			case (UChromaSession::LabelPointSizeKeyword):
				pane->setLabelPointSize(parser.argd(1));
				break;
			// Rotation
			case (UChromaSession::RotationXKeyword):
			case (UChromaSession::RotationYKeyword):
			case (UChromaSession::RotationZKeyword):
				xyz = viewPaneKwd - UChromaSession::RotationXKeyword;
				pane->setViewRotationColumn(xyz, parser.argd(1), parser.argd(2), parser.argd(3));
				break;
			// Perspective
			case (UChromaSession::PerspectiveKeyword):
				pane->setHasPerspective(parser.argb(1));
				break;
			// Role
			case (UChromaSession::RoleKeyword):
				role = ViewPane::paneRole(parser.argString(1));
				if (role == ViewPane::nPaneRoles)
				{
					msg.print("Warning: Unrecognised role '%s' for pane '%s'. Defaulting to '%s'.\n", parser.argChar(1), qPrintable(pane->name()), ViewPane::paneRole(ViewPane::StandardRole));
					role = ViewPane::StandardRole;
					CHECKIOFAIL
				}
				pane->setRole(role);
				break;
			// Role associated collection
			case (UChromaSession::RoleTargetCollectionKeyword):
				// Locate named collection
				collection = locateCollection(parser.argString(1));
				if (!collection)
				{
					msg.print("Warning: Collection '%s' not found, and can't be associated to pane '%s'.\n", parser.argChar(1), qPrintable(pane->name()));
					CHECKIOFAIL
				}
				pane->addCollectionTarget(collection);
				break;
			// Role target pane
			case (UChromaSession::RoleTargetPaneKeyword):
				associatedPane = viewLayout_.pane(parser.argString(1));
				if (!associatedPane) associatedPane = viewLayout_.addPane(parser.argString(1));
				pane->addPaneTarget(associatedPane);
				break;
			// Title scale
			case (UChromaSession::TitlePointSizeKeyword):
				pane->setTitlePointSize(parser.argd(1));
				break;
			// Translation
			case (UChromaSession::TranslationKeyword):
				pane->setViewTranslation(parser.argd(1), parser.argd(2), parser.argd(3));
				break;
			// Use best flat view
			case (UChromaSession::UseBestFlatViewKeyword):
				pane->axes().setUseBestFlatView(parser.argb(1));
				break;
			// View Type
			case (UChromaSession::ViewTypeKeyword):
				vt = ViewPane::viewType(parser.argString(1));
				if (vt == ViewPane::nViewTypes)
				{
					msg.print("Warning: Unrecognised view type '%s'. Defaulting to '%s'.\n", parser.argChar(1), ViewPane::viewType(ViewPane::NormalView));
					vt = ViewPane::NormalView;
					CHECKIOFAIL
				}
				pane->setViewType(vt);
				break;
			// Unrecognised Keyword
			default:
				msg.print("Warning: Unrecognised ViewPane keyword: %s\n", parser.argChar(0));
				CHECKIOFAIL
				break;
		}
	}
	msg.print("Error : Unterminated 'ViewPane' block.\n");
	return false;
}

// Return current session file directory
QDir UChromaSession::sessionFileDirectory()
{
	return sessionFileDirectory_;
}

// Set whether to enforce hard fail on input file error
void UChromaSession::setHardIOFail(bool hardFail)
{
	hardIOFail_ = hardFail;
}

// Load session from file specified
bool UChromaSession::loadSession(QString fileName)
{
	LineParser parser(fileName);

	if (!parser.ready())
	{
		QMessageBox::warning(uChroma_, "Error", "Can't open specified file for reading.");
		return false;
	}

	// Clear existing data
	startNewSession(false);

	// Set input file directory
	sessionFileDirectory_ = fileName;

	// Read line from file and decide what to do with it
	UChromaSession::InputBlock block;
	int nEmpty;
	bool success;
	while (!parser.atEnd())
	{
		parser.getArgs(LineParser::UseQuotes + LineParser::SkipBlanks);

		// We expect a block keyword in this loop...
		block = UChromaSession::inputBlock(parser.argString(0));
		switch (block)
		{
			// Collection Block
			case (UChromaSession::CollectionBlock):
				// Create new master Collection and set its title
				currentCollection_ = addCollection(parser.argString(1));

				// Load the collection data
				success = readCollectionBlock(parser, currentCollection_);

				// Check for empty slices
				nEmpty = currentCollection_->nEmptyDataSets();
				if (nEmpty != 0)
				{
					QMessageBox::StandardButton button = QMessageBox::warning(uChroma_, "Empty Data", QString("There are ") + QString::number(nEmpty) + " defined slices which contain no data in collection '" + currentCollection_->name() + "'.\nWould you like to reload these now from their source files?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					if (button == QMessageBox::Yes)
					{
						nEmpty = currentCollection_->loadAllDataSets();
						
						if (nEmpty > 0)
						{
							QMessageBox::warning(uChroma_, "Empty Data", QString("There are still ") + QString::number(nEmpty) + " defined slices which contain no data or whose original files could not be found.\nCheck the slice data directory, and/or the datafiles themselves.");
						}
					}
				}
				break;
			// Settings
			case (UChromaSession::SettingsBlock):
				success = readSettingsBlock(parser);
				break;
			// View
			case (UChromaSession::ViewBlock):
				success = readViewBlock(parser);
				break;
			default:
				msg.print("Warning: Unrecognised input block: %s\n", parser.argChar(0));
				CHECKIOFAIL
				break;
		}

		// If we have failed for any reason, exit now
		if (!success) break;
	}
	parser.closeFiles();

	// Show a message if we encountered problems...
	if (!success)
	{
		QMessageBox::warning(uChroma_, "Problems Loading File", "Errors were encountered while loading the file.\nCheck the Log window for possible error messages.");
	}

	// Set necessary variables
	currentViewPane_ = viewLayout_.panes();

	// Set current project data
	setInputFile(fileName);
	setAsNotModified();

	return true;
}
