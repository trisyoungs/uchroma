/*
	*** Session Keywords
	*** src/session/keywords.cpp
	Copyright T. Youngs 2013-2015

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
#include "base/sysfunc.h"
#include "base/messenger.h"
#include <string.h>

/*
// Input Block Keywords
*/

// Input File Block Keywords
const char* InputBlockKeywords[] = { "Collection", "Settings", "View" };

/*!
 * \brief Convert text string to InputBlock
 */
UChromaSession::InputBlock UChromaSession::inputBlock(QString s)
{
	for (int n=0; n<UChromaSession::nInputBlocks; ++n) if (s == InputBlockKeywords[n]) return (UChromaSession::InputBlock) n;
	return UChromaSession::nInputBlocks;
}

/*!
 * \brief Convert InputBlock to text string
 */
const char* UChromaSession::inputBlock(UChromaSession::InputBlock id)
{
	return InputBlockKeywords[id];
}

/*
// Axis Keywords
*/

// Axis Block Keywords
const char* AxisBlockKeywords[] = { "AutoScale", "AutoTicks", "EndAxis", "FirstTick", "FractionalPositioning", "GridLines", "GridLineMajorStyle", "GridLineMinorStyle", "Invert", "LabelAnchor", "LabelOrientation", "Limits", "Logarithmic", "MinorTicks", "NumberFormat", "PositionFractional", "PositionReal", "Stretch", "TickDelta", "TickDirection", "Title", "TitleAnchor", "TitleOrientation", "Visible" };

// Axis Block NArguments
int AxisKeywordNArguments[] = { 1, 1, 0, 1, 1, 3, 6, 6, 1, 1, 3, 2, 1, 1, 4, 3, 3, 1, 1, 3, 1, 1, 4, 1 };

/*!
 * \brief Convert text string to AxisKeyword
 */
UChromaSession::AxisKeyword UChromaSession::axisKeyword(QString s)
{
	for (int n=0; n<nAxisKeywords; ++n) if (s == AxisBlockKeywords[n]) return (UChromaSession::AxisKeyword) n;
	return nAxisKeywords;
}

/*!
 * \brief Convert AxisBlockKeywords to text string
 */
const char* UChromaSession::axisKeyword(UChromaSession::AxisKeyword kwd)
{
	return AxisBlockKeywords[kwd];
}

/*!
 * \brief Return minimum number of expected arguments
 */
int UChromaSession::axisKeywordNArguments(UChromaSession::AxisKeyword kwd)
{
	return AxisKeywordNArguments[kwd];
}

/*
 * Collection Keywords
 */

// Collection Block Keywords
const char* CollectionBlockKeywords[] = { "ColourAlphaControl", "ColourAlphaFixed", "ColourCustomGradient", "ColourLinearRGBA", "ColourLinearRGBB", "ColourLinearHSVA", "ColourLinearHSVB", "ColourSingle", "ColourSource", "DataDirectory", "DataSet", "EndCollection", "Fit", "FitParameters", "Interpolate", "InterpolateConstrain", "InterpolateStep", "LineStyle", "Shininess", "Slice", "Style", "TransformX", "TransformY", "TransformZ", "Visible" };

// Collection Block NArguments
int CollectionKeywordNArguments[] = { 1, 1, 5, 5, 5, 5, 5, 4, 1, 1, 1, 0, 1, 0, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1 };

/*!
 * \brief Convert text string to CollectionKeyword
 */
UChromaSession::CollectionKeyword UChromaSession::collectionKeyword(QString s)
{
	for (int n=0; n<nCollectionKeywords; ++n) if (s == CollectionBlockKeywords[n]) return (UChromaSession::CollectionKeyword) n;
	return nCollectionKeywords;
}

/*!
 * \brief Convert CollectionKeyword to text string
 */
const char* UChromaSession::collectionKeyword(UChromaSession::CollectionKeyword kwd)
{
	return CollectionBlockKeywords[kwd];
}

/*!
 * \brief Return minimum number of expected arguments
 */
int UChromaSession::collectionKeywordNArguments(UChromaSession::CollectionKeyword kwd)
{
	return CollectionKeywordNArguments[kwd];
}

/*
 * FitParameters Keywords
 */

// FitParameters Block Keywords
const char* FitParametersBlockKeywords[] = { "EndFitParameters", "Equation", "FitResults", "Global", "LimitStrength", "Orthogonal", "Reference", "Variable", "XRangeType", "XRangeAbsolute", "XRangeIndex", "XRangeSingle", "ZRangeType", "ZRangeAbsolute", "ZRangeIndex", "ZRangeSingle" };

// FitParameters Block NArguments
int FitParametersKeywordNArguments[] = { 0, 1, 1, 1, 1, 1, 7, 7, 1, 2, 2, 1, 1, 2, 2, 1 };

/*!
 * \brief Convert text string to FitParametersKeyword
 */
UChromaSession::FitParametersKeyword UChromaSession::fitParametersKeyword(QString s)
{
	for (int n=0; n<nFitParametersKeywords; ++n) if (s == FitParametersBlockKeywords[n]) return (UChromaSession::FitParametersKeyword) n;
	return nFitParametersKeywords;
}

/*!
 * \brief Convert FitParametersKeyword to text string
 */
const char* UChromaSession::fitParametersKeyword(UChromaSession::FitParametersKeyword kwd)
{
	return FitParametersBlockKeywords[kwd];
}

/*!
 * \brief Return minimum number of expected arguments
 */
int UChromaSession::fitParametersKeywordNArguments(UChromaSession::FitParametersKeyword kwd)
{
	return FitParametersKeywordNArguments[kwd];
}

/*
 * FitResults Keywords
 */

// FitResults Block Keywords
const char* FitResultsBlockKeywords[] = { "EndFitResults", "Variable" };

// FitResults Block NArguments
int FitResultsKeywordNArguments[] = { 0, 1 };

/*!
 * \brief Convert text string to FitResultsKeyword
 */
UChromaSession::FitResultsKeyword UChromaSession::fitResultsKeyword(QString s)
{
	for (int n=0; n<nFitResultsKeywords; ++n) if (s == FitResultsBlockKeywords[n]) return (UChromaSession::FitResultsKeyword) n;
	return nFitResultsKeywords;
}

/*!
 * \brief Convert FitResultsKeyword to text string
 */
const char* UChromaSession::fitResultsKeyword(UChromaSession::FitResultsKeyword kwd)
{
	return FitResultsBlockKeywords[kwd];
}

/*!
 * \brief Return minimum number of expected arguments
 */
int UChromaSession::fitResultsKeywordNArguments(UChromaSession::FitResultsKeyword kwd)
{
	return FitResultsKeywordNArguments[kwd];
}

/*
 * Settings Keywords
 */

// Settings Block Keywords
const char* SettingsBlockKeywords[] = { "EndSettings", "ImageExport" };

// Settings Block NArguments
int SettingsKeywordNArguments[] = { 0, 5 };

/*!
 * \brief Convert text string to SettingsKeyword
 */
UChromaSession::SettingsKeyword UChromaSession::settingsKeyword(QString s)
{
	for (int n=0; n<nSettingsKeywords; ++n) if (s == SettingsBlockKeywords[n]) return (UChromaSession::SettingsKeyword) n;
	return nSettingsKeywords;
}

/*!
 * \brief Convert SettingsKeyword to text string
 */
const char* UChromaSession::settingsKeyword(UChromaSession::SettingsKeyword kwd)
{
	return SettingsBlockKeywords[kwd];
}

/*!
 * \brief Return minimum number of expected arguments
 */
int UChromaSession::settingsKeywordNArguments(UChromaSession::SettingsKeyword kwd)
{
	return SettingsKeywordNArguments[kwd];
}

/*
 * DataSet Keywords
 */

// DataSet Block Keywords
const char* DataSetBlockKeywords[] = { "Data", "EndDataSet", "Source", "Z" };

// DataSet Block NArguments
int DataSetKeywordNArguments[] = { 0, 0, 1, 1 };

/*!
 * \brief Convert text string to DataSetKeyword
 */
UChromaSession::DataSetKeyword UChromaSession::dataSetKeyword(QString s)
{
	for (int n=0; n<nDataSetKeywords; ++n) if (s == DataSetBlockKeywords[n]) return (UChromaSession::DataSetKeyword) n;
	return nDataSetKeywords;
}

/*!
 * \brief Convert DataSetKeyword to text string
 */
const char* UChromaSession::dataSetKeyword(UChromaSession::DataSetKeyword kwd)
{
	return DataSetBlockKeywords[kwd];
}

/*!
 * \brief Return minimum number of expected arguments
 */
int UChromaSession::dataSetKeywordNArguments(UChromaSession::DataSetKeyword kwd)
{
	return DataSetKeywordNArguments[kwd];
}

/*
 * View Keywords
 */

// View Block Keywords
const char* ViewBlockKeywords[] = { "EndView", "Grid", "ViewPane" };

// View Block NArguments
int ViewKeywordNArguments[] = { 0, 2, 1 };

/*!
 * \brief Convert text string to ViewKeyword
 */
UChromaSession::ViewKeyword UChromaSession::viewKeyword(QString s)
{
	for (int n=0; n<nViewKeywords; ++n) if (s == ViewBlockKeywords[n]) return (UChromaSession::ViewKeyword) n;
	return nViewKeywords;
}

/*!
 * \brief Convert ViewKeyword to text string
 */
const char* UChromaSession::viewKeyword(UChromaSession::ViewKeyword kwd)
{
	return ViewBlockKeywords[kwd];
}

/*!
 * \brief Return minimum number of expected arguments
 */
int UChromaSession::viewKeywordNArguments(UChromaSession::ViewKeyword kwd)
{
	return ViewKeywordNArguments[kwd];
}

/*
 * ViewPane Keywords
 */

// ViewPane Block Keywords
const char* ViewPaneBlockKeywords[] = { "AutoPositionTitles", "Axis", "BoundingBox", "BoundingBoxPlaneY", "EndViewPane", "FlatLabels", "Geometry", "LabelPointSize", "Perspective", "Role", "RoleTargetCollection", "RoleTargetPane", "RotationX", "RotationY", "RotationZ", "TitlePointSize", "Translation", "UseBestFlatView", "ViewType" };

// ViewPane Block NArguments
int ViewPaneKeywordNArguments[] = { 1, 1, 1, 1, 0, 1, 4, 1, 1, 1, 1, 1, 3, 3, 3, 1, 3, 1, 1 };

/*!
 * \brief Convert text string to ViewPaneKeyword
 */
UChromaSession::ViewPaneKeyword UChromaSession::viewPaneKeyword(QString s)
{
	for (int n=0; n<nViewPaneKeywords; ++n) if (s == ViewPaneBlockKeywords[n]) return (UChromaSession::ViewPaneKeyword) n;
	return nViewPaneKeywords;
}

/*!
 * \brief Convert ViewPaneKeyword to text string
 */
const char* UChromaSession::viewPaneKeyword(UChromaSession::ViewPaneKeyword kwd)
{
	return ViewPaneBlockKeywords[kwd];
}

/*!
 * \brief Return minimum number of expected arguments
 */
int UChromaSession::viewPaneKeywordNArguments(UChromaSession::ViewPaneKeyword kwd)
{
	return ViewPaneKeywordNArguments[kwd];
}
