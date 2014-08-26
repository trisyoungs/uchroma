/*
	*** File I/O - Load
	*** src/gui/io_load.cpp
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
#include "base/currentproject.h"

#define CHECKIOFAIL if (hardIOFail_) { return false; } else { break; }

// Parse AxisBlock keywords
bool UChromaWindow::readAxisBlock(LineParser& parser, Axes& axes, int axis)
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
				axes.setAxisAutoTicks(axis, parser.argb(1));
				break;
			// End input block
			case (Keywords::EndAxisKeyword):
				return true;
				break;
			// First ticks
			case (Keywords::FirstTickKeyword):
				axes.setAxisFirstTick(axis, parser.argd(1));
				break;
			// Fractional positioning flag
			case (Keywords::FractionalPositioningKeyword):
				axes.setAxisPositionIsFractional(axis, parser.argb(1));
				break;
			// Invert
			case (Keywords::InvertKeyword):
				axes.setAxisInverted(axis, parser.argb(1));
				break;
			// Axis label orientation
			case (Keywords::LabelOrientationKeyword):
				axes.setAxisLabelOrientation(axis, 0, parser.argd(1));
				axes.setAxisLabelOrientation(axis, 1, parser.argd(2));
				axes.setAxisLabelOrientation(axis, 2, parser.argd(3));
				break;
			// Limits
			case (Keywords::LimitsKeyword):
				axes.setAxisMin(axis, parser.argd(1));
				axes.setAxisMax(axis, parser.argd(2));
				break;
			// Axis logarithmic flag
			case (Keywords::LogarithmicKeyword):
				axes.setAxisLogarithmic(axis, parser.argb(1));
				break;
			// Axis minor ticks
			case (Keywords::MinorTicksKeyword):
				axes.setAxisMinorTicks(axis, parser.argi(1));
				break;
			// Axis position (in fractional axis coordinates)
			case (Keywords::PositionFractionalKeyword):
				axes.setAxisPositionFractional(axis, 0, parser.argd(1));
				axes.setAxisPositionFractional(axis, 1, parser.argd(2));
				axes.setAxisPositionFractional(axis, 2, parser.argd(3));
				break;
			// Axis position (in real surface-space coordinates)
			case (Keywords::PositionRealKeyword):
				axes.setAxisPositionReal(axis, 0, parser.argd(1));
				axes.setAxisPositionReal(axis, 1, parser.argd(2));
				axes.setAxisPositionReal(axis, 2, parser.argd(3));
				break;
			// Axis stretch factors
			case (Keywords::StretchKeyword):
				axes.setAxisStretch(axis, parser.argd(1));
				break;
			// Axis tick deltas
			case (Keywords::TickDeltaKeyword):
				axes.setAxisTickDelta(axis, parser.argd(1));
				break;
			// Axis title
			case (Keywords::TitleKeyword):
				axes.setAxisTitle(axis, parser.argc(1));
				break;
			// Axis title anchors
			case (Keywords::TitleAnchorKeyword):
				axes.setAxisTitleAnchor(axis, (Axes::AxisAnchor) parser.argi(1));
				break;
			// Axis title orientation
			case (Keywords::TitleOrientationKeyword):
				axes.setAxisTitleOrientation(axis, 0, parser.argd(1));
				axes.setAxisTitleOrientation(axis, 1, parser.argd(2));
				axes.setAxisTitleOrientation(axis, 2, parser.argd(3));
				axes.setAxisTitleOrientation(axis, 3, parser.argd(4));
				break;
			// Axis visibility
			case (Keywords::VisibleAxisKeyword):
				axes.setAxisVisible(axis, parser.argb(1));
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
	DataSet* dataSet;
	int xyz;
	Collection::DisplayStyle ds;
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
			// Dataset directory
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
			// DataSet
			case (Keywords::DataSetDefinitionKeyword):
				// Create new dataset
				dataSet = collection->addDataSet();
				dataSet->setTitle(parser.argc(1));
				if (!readDataSetBlock(parser, dataSet, collection)) return false;
				break;
			// End input block
			case (Keywords::EndCollectionKeyword):
				return true;
				break;
			// Fit data block
			case (Keywords::FitBlockKeyword):
				if (!readCollectionBlock(parser, collection->addFit(parser.argc(1)))) return false;
				break;
			// Fit parameters block
			case (Keywords::FitParametersBlockKeyword):
				// Check that a FitKernel exists in the current collection
				if (!collection->fitKernel())
				{
					msg.print("Warning: No FitKernel exists in the collection '%s', but a FitParameters block was found.\nCreating a new one...\n", qPrintable(collection->title()));
					collection->addFitKernel();
				}
				if (!readFitParametersBlock(parser, collection->fitKernel())) return false;
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
			// Slice data block
			case (Keywords::SliceBlockKeyword):
				if (!readCollectionBlock(parser, collection->addSlice(parser.argc(1)))) return false;
				break;
			// Display style
			case (Keywords::StyleKeyword):
				ds = Collection::displayStyle(parser.argc(1));
				if (ds == Collection::nDisplayStyles)
				{
					msg.print("Unrecognised display style '%s'.\n", parser.argc(1));
					return false;
				}
				collection->setDisplayStyle(ds);
				break;
			// Data Transform
			case (Keywords::TransformXKeyword):
			case (Keywords::TransformYKeyword):
			case (Keywords::TransformZKeyword):
				xyz = collectionKwd - Keywords::TransformXKeyword;
				collection->setTransformEnabled(xyz, parser.argb(1));
				collection->setTransformEquation(xyz,  parser.argc(2));
				break;
			// Visible flag
			case (Keywords::VisibleCollectionKeyword):
				collection->setVisible(parser.argb(1));
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

// Read DataSetBlock keywords
bool UChromaWindow::readDataSetBlock(LineParser& parser, DataSet* dataSet, Collection* collection)
{
	int nPoints;
	bool foundEnd;
	DataSet::DataSource source;
	while (!parser.eofOrBlank())
	{
		// Get line from file
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks);

		// Get keyword and check number of arguments provided
		Keywords::DataSetKeyword dataSetKwd = Keywords::dataSetKeyword(parser.argc(0));
		if ((dataSetKwd != Keywords::nDataSetKeywords) && (Keywords::dataSetKeywordNArguments(dataSetKwd) > (parser.nArgs()-1)))
		{
			msg.print("Error : DataSet keyword '%s' requires %i arguments, but only %i have been provided.\n", Keywords::dataSetKeyword(dataSetKwd), Keywords::dataSetKeywordNArguments(dataSetKwd), parser.nArgs()-1);
			return false;
		}
		switch (dataSetKwd)
		{
			case (Keywords::DataKeyword):
				dataSet->data().reset();
				foundEnd = false;
				do
				{
					parser.getArgsDelim(LineParser::Defaults);
					// Check for 'EndData'
					if (strcmp(parser.argc(0),"EndData") == 0) foundEnd = true;
					else dataSet->data().addPoint(parser.argd(0), parser.argd(1));
				} while ((!foundEnd) && (!parser.eofOrBlank()));
				if (!foundEnd)
				{
					msg.print("Error : Unterminated 'Data' block in dataSet '%s'.\n", qPrintable(dataSet->title()));
					return false;
				}
				break;
			case (Keywords::EndDataSetKeyword):
				return true;
				break;
			case (Keywords::SourceKeyword):
				source = DataSet::dataSource(parser.argc(1));
				if (source == DataSet::nDataSources)
				{
					msg.print("Error :Datasource for dataSet not recognised (%s)\n", parser.argc(1));
					return false;
				}
				dataSet->setDataSource(source);
				// Depending on the source, we might expect other data here...
				if (source == DataSet::FileSource)
				{
					if (parser.hasArg(2)) dataSet->setSourceFileName(parser.argc(2));
					else
					{
						msg.print("Error :Expected data file name after 'Source File' declaration in dataSet '%s'.\n", qPrintable(dataSet->title()));
						return false;
					}
				}
				break;
			case (Keywords::ZKeyword):
				collection->setDataSetZ(dataSet, parser.argd(1));
				break;
			// Unrecognised Keyword
			default:
				msg.print("Error : Unrecognised DataSet keyword: %s\n", parser.argc(0));
				return false;
		}
	}
	msg.print("Error : Unterminated 'DataSet' block.\n");
	return false;
}

// Read FitParametersBlock keywords
bool UChromaWindow::readFitParametersBlock(LineParser& parser, FitKernel* fitKernel)
{
	FitKernel::RangeType rangeType;
	EquationVariable* eqVar;
	while (!parser.eofOrBlank())
	{
		// Get line from file
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks);

		// Get keyword and check number of arguments provided
		Keywords::FitParametersKeyword fitParamsKwd = Keywords::fitParametersKeyword(parser.argc(0));
		if ((fitParamsKwd != Keywords::nFitParametersKeywords) && (Keywords::fitParametersKeywordNArguments(fitParamsKwd) > (parser.nArgs()-1)))
		{
			msg.print("Error : FitParameters keyword '%s' requires %i arguments, but only %i have been provided.\n", Keywords::fitParametersKeyword(fitParamsKwd), Keywords::fitParametersKeywordNArguments(fitParamsKwd), parser.nArgs()-1);
			return false;
		}
		switch (fitParamsKwd)
		{
			case (Keywords::EndFitParametersKeyword):
				return true;
				break;
			case (Keywords::EquationKeyword):
				fitKernel->setEquation(parser.argc(1));
				break;
			case (Keywords::GlobalKeyword):
				fitKernel->setGlobal(parser.argb(1));
				break;
			case (Keywords::LimitStrengthKeyword):
				fitKernel->setLimitStrength(parser.argd(1));
				break;
			case (Keywords::OrthogonalKeyword):
				fitKernel->setOrthogonal(parser.argb(1));
				break;
			case (Keywords::VariableKeyword):
				// First, see if named variable exists
				eqVar = fitKernel->variable(parser.argc(1));
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
			case (Keywords::XRangeTypeKeyword):
				rangeType = FitKernel::rangeType(parser.argc(1));
				if (rangeType == FitKernel::nRangeTypes)
				{
					msg.print("Warning: Unrecognised range type '%s' given for X in fit. Defaulting to '%s'\n", parser.argc(1), FitKernel::rangeType(FitKernel::AbsoluteRange));
					CHECKIOFAIL
				}
				fitKernel->setXRange(rangeType);
				break;
			case (Keywords::XRangeAbsoluteKeyword):
				fitKernel->setAbsoluteXMin(parser.argd(1));
				fitKernel->setAbsoluteXMax(parser.argd(2));
				break;
			case (Keywords::XRangeIndexKeyword):
				fitKernel->setIndexXMin(parser.argi(1)-1);
				fitKernel->setIndexXMax(parser.argi(2)-1);
				break;
			case (Keywords::XRangeIndexSingleKeyword):
				fitKernel->setIndexXSingle(parser.argi(1)-1);
				break;
			case (Keywords::ZRangeTypeKeyword):
				rangeType = FitKernel::rangeType(parser.argc(1));
				if (rangeType == FitKernel::nRangeTypes)
				{
					msg.print("Warning: Unrecognised range type '%s' given for Z in fit. Defaulting to '%s'\n", parser.argc(1), FitKernel::rangeType(FitKernel::AbsoluteRange));
					CHECKIOFAIL
				}
				fitKernel->setZRange(rangeType);
				break;
			case (Keywords::ZRangeAbsoluteKeyword):
				fitKernel->setAbsoluteZMin(parser.argd(1));
				fitKernel->setAbsoluteZMax(parser.argd(2));
				break;
			case (Keywords::ZRangeIndexKeyword):
				fitKernel->setIndexZMin(parser.argi(1)-1);
				fitKernel->setIndexZMax(parser.argi(2)-1);
				break;
			case (Keywords::ZRangeIndexSingleKeyword):
				fitKernel->setIndexZSingle(parser.argi(1)-1);
				break;
			// Unrecognised Keyword
			default:
				msg.print("Error : Unrecognised FitParameters keyword: %s\n", parser.argc(0));
				return false;
		}
	}
	msg.print("Error : Unterminated 'FitParameters' block.\n");
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

// Read ViewBlock keywords
bool UChromaWindow::readViewBlock(LineParser& parser)
{
	ViewPane* pane;
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
			// End input block
			case (Keywords::EndViewKeyword):
				return true;
				break;
			// Grid specification
			case (Keywords::GridKeyword):
				viewLayout_.setGrid(parser.argi(1), parser.argi(2));
				break;
			// Labels face viewer flag
			case (Keywords::LabelFaceViewerKeyword):
				labelFaceViewer_ = parser.argb(1);
				break;
			// ViewPane definition
			case (Keywords::ViewPaneBlockKeyword):
				// Check to see if pane has already been created (through it being referenced by another pane)
				pane = viewLayout_.pane(parser.argc(1));
				if (!pane) pane = viewLayout_.addPane(parser.argc(1));
				if (!readViewPaneBlock(parser, pane)) return false;
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

// Read ViewPane keywords
bool UChromaWindow::readViewPaneBlock(LineParser& parser, ViewPane* pane)
{
	int xyzw, axis;
	Collection* collection;
	Matrix mat;
	ViewPane* associatedPane;
	ViewPane::PaneRole role;
	ViewPane::AutoScaleMethod as;
	while (!parser.eofOrBlank())
	{
		// Get line from file
		parser.getArgsDelim(LineParser::UseQuotes + LineParser::SkipBlanks);

		// Get keyword and check number of arguments provided
		Keywords::ViewPaneKeyword viewPaneKwd = Keywords::viewPaneKeyword(parser.argc(0));
		if ((viewPaneKwd != Keywords::nViewPaneKeywords) && (Keywords::viewPaneKeywordNArguments(viewPaneKwd) > (parser.nArgs()-1)))
		{
			msg.print("Error : ViewPane keyword '%s' requires %i arguments, but only %i have been provided.\n", Keywords::viewPaneKeyword(viewPaneKwd), Keywords::viewPaneKeywordNArguments(viewPaneKwd), parser.nArgs()-1);
			return false;
		}
		switch (viewPaneKwd)
		{
			// Autoscale method
			case (Keywords::AutoScaleKeyword):
				as = ViewPane::autoScaleMethod(parser.argc(1));
				if (as == ViewPane::nAutoScaleMethods)
				{
					msg.print("Warning: Unrecognised autoscale method '%s'.\n", parser.argc(1));
					CHECKIOFAIL
				}
				pane->setAutoScale(as);
				break;
			// Auto stretch 3D 
			case (Keywords::AutoStretch3DKeyword):
				pane->setAutoStretch3D(parser.argb(1));
				break;
			// Axis block
			case (Keywords::AxisBlockKeyword):
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
			case (Keywords::BoundingBoxKeyword):
				if ((parser.argi(1) < 0) || (parser.argi(1) >= ViewPane::nBoundingBoxes))
				{
					msg.print("Warning: Value is out of range for %s keyword.\n", Keywords::viewPaneKeyword(viewPaneKwd));
					CHECKIOFAIL
				}
				pane->setBoundingBox((ViewPane::BoundingBox) parser.argi(1));
				break;
			// Bounding Box plane y intercept
			case (Keywords::BoundingBoxPlaneYKeyword):
				pane->setBoundingBoxPlaneY(parser.argd(1));
				break;
			// Collection association
			case (Keywords::CollectionAssociatedKeyword):
				// Find named collection
				collection = findCollection(parser.argc(1));
				if (collection == NULL)
				{
					msg.print("Warning - Collection '%s' is listed in ViewPane '%s', but no collection by this name exists.\n", parser.argc(1), qPrintable(pane->name()));
					CHECKIOFAIL
				}
				else pane->addCollection(collection);
				break;
			// End input block
			case (Keywords::EndViewPaneKeyword):
				return true;
				break;
			// Name
			case (Keywords::GeometryKeyword):
				pane->setBottomLeft(parser.argi(1), parser.argi(2));
				pane->setSize(parser.argi(3), parser.argi(4));
				break;
			// Label scale
			case (Keywords::LabelPointSizeKeyword):
				pane->setLabelPointSize(parser.argd(1));
				break;
			// View Matrix
			case (Keywords::MatrixXKeyword):
			case (Keywords::MatrixYKeyword):
			case (Keywords::MatrixZKeyword):
			case (Keywords::MatrixWKeyword):
				xyzw = viewPaneKwd - Keywords::MatrixXKeyword;
				pane->setViewMatrixColumn(xyzw, parser.argd(1), parser.argd(2), parser.argd(3), parser.argd(4));
				break;
			// Perspective
			case (Keywords::PerspectiveKeyword):
				pane->setHasPerspective(true);
				break;
			// Role
			case (Keywords::RoleKeyword):
				role = ViewPane::paneRole(parser.argc(1));
				if (role == ViewPane::nPaneRoles)
				{
					msg.print("Warning: Unrecognised role '%s' for pane '%s'. Defaulting to '%s'.\n", parser.argc(1), qPrintable(pane->name()), ViewPane::paneRole(ViewPane::StandardRole));
					CHECKIOFAIL
				}
				pane->setRole(role);
				break;
			// Role associated collection
			case (Keywords::RoleTargetCollectionKeyword):
				collection = findCollection(parser.argc(1));
				if (!collection)
				{
					msg.print("Warning: Collection '%s' not found, and can't be associated to pane '%s'.\n", parser.argc(1), qPrintable(pane->name()));
					CHECKIOFAIL
				}
				pane->addRoleTargetCollection(collection);
				break;
			// Role target pane
			case (Keywords::RoleTargetPaneKeyword):
				associatedPane = viewLayout_.pane(parser.argc(1));
				if (!associatedPane) associatedPane = viewLayout_.addPane(parser.argc(1));
				pane->addRoleTargetPane(associatedPane);
				break;
			// Title scale
			case (Keywords::TitlePointSizeKeyword):
				pane->setTitlePointSize(parser.argd(1));
				break;
			// Two Dimensional flag
			case (Keywords::TwoDimensionalKeyword):
				pane->setTwoDimensional(parser.argb(1));
				break;
			// Unrecognised Keyword
			default:
				msg.print("Error : Unrecognised ViewPane keyword: %s\n", parser.argc(0));
				return false;
		}
	}
	msg.print("Error : Unterminated 'ViewPane' block.\n");
	return false;
}

// Set whether to enforce hard fail on input file error
void UChromaWindow::setHardIOFail(bool hardFail)
{
	hardIOFail_ = hardFail;
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
	clearData(true);

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
			// Collection Block
			case (Keywords::CollectionBlock):
				// Create new master Collection and set its title
				currentCollection_ = addCollection(parser.argc(1));

				// Load the collection data
				success = readCollectionBlock(parser, currentCollection_);

				// Check for empty slices
				nEmpty = currentCollection_->nEmptyDataSets();
				if (nEmpty != 0)
				{
					QMessageBox::StandardButton button = QMessageBox::warning(this, "Empty Data", QString("There are ") + QString::number(nEmpty) + " defined slices which contain no data in collection '" + currentCollection_->title() + "'.\nWould you like to reload these now from their source files?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					if (button == QMessageBox::Yes)
					{
						nEmpty = currentCollection_->loadAllDataSets();
						
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
	currentViewPane_ = viewLayout_.panes();

	// Set current project data
	CurrentProject::setInputFile(fileName);
	CurrentProject::setAsNotModified();

	return true;
}
