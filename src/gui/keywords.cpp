/*
	*** Keyword Definitions
	*** src/gui/keywords.cpp
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

#include "gui/keywords.h"
#include "base/sysfunc.h"
#include "base/messenger.h"
#include <string.h>


// Print list of valid keywords for InputBlock specified
void Keywords::printValidKeywords(Keywords::InputBlock block)
{
	msg.print("Valid Keywords for '%s' block are:\n", inputBlock(block));
	int n;
	switch (block)
	{
		case (Keywords::AxisBlock):
			for (n=0; n<Keywords::nAxisKeywords; ++n) msg.print("%s%s", n == 0 ? "" : ", ", axisKeyword( (AxisKeyword) n ));
			break;
		case (Keywords::CollectionBlock):
			for (n=0; n<Keywords::nCollectionKeywords; ++n) msg.print("%s%s", n == 0 ? "" : ", ", collectionKeyword( (CollectionKeyword) n ));
			break;
		default:
			msg.print("\nUnrecognised block given to Keywords::printValidKeywords.\n");
			break;
	}
	msg.print("\n");
}

/*
// Input Block Keywords
*/

// Input File Block Keywords
const char *InputBlockKeywords[] = { "Axis", "Collection", "Settings", "View" };

/*!
 * \brief Convert text string to InputBlock
 */
Keywords::InputBlock Keywords::inputBlock(const char* s)
{
	for (int n=0; n<Keywords::nInputBlocks; ++n) if (strcmp(s,InputBlockKeywords[n]) == 0) return (Keywords::InputBlock) n;
	return Keywords::nInputBlocks;
}

/*!
 * \brief Convert InputBlock to text string
 */
const char* Keywords::inputBlock(Keywords::InputBlock id)
{
	return InputBlockKeywords[id];
}

/*
// Axis Keywords
*/

// Axis Block Keywords
const char* AxisBlockKeywords[] = { "AutoTicks", "EndAxis", "FirstTick", "FractionalPositioning", "Invert", "LabelOrientation", "Limits", "Logarithmic", "MinorTicks", "PositionFractional", "PositionReal", "Stretch", "TickDelta", "TickDirection", "Title", "TitleAnchor", "TitleOrientation", "Visible" };

// Axis Block NArguments
int AxisKeywordNArguments[] = { 1, 0, 1, 1, 1, 3, 2, 1, 1, 3, 3, 1, 1, 3, 1, 1, 4, 1 };

/*!
 * \brief Convert text string to AxisKeyword
 */
Keywords::AxisKeyword Keywords::axisKeyword(const char* s)
{
	for (int n=0; n<nAxisKeywords; ++n) if (strcmp(s,AxisBlockKeywords[n]) == 0) return (Keywords::AxisKeyword) n;
	return nAxisKeywords;
}

/*!
 * \brief Convert AxisBlockKeywords to text string
 */
const char* Keywords::axisKeyword(Keywords::AxisKeyword kwd)
{
	return AxisBlockKeywords[kwd];
}

/*!
 * \brief Return minimum number of expected arguments
 */
int Keywords::axisKeywordNArguments(Keywords::AxisKeyword kwd)
{
	return AxisKeywordNArguments[kwd];
}

/*
// Collection Keywords
*/

// Collection Block Keywords
const char* CollectionBlockKeywords[] = { "ColourAlphaControl", "ColourAlphaFixed", "ColourCustomGradient", "ColourLinearRGBA", "ColourLinearRGBB", "ColourLinearHSVA", "ColourLinearHSVB", "ColourSingle", "ColourSource", "DataDirectory", "DataSet", "EndCollection", "ExtractedData", "Fit", "Interpolate", "InterpolateConstrain", "InterpolateStep", "Style", "TransformX", "TransformY", "TransformZ", "Visible" };

// Collection Block NArguments
int CollectionKeywordNArguments[] = { 1, 1, 5, 5, 5, 5, 5, 4, 1, 1, 1, 0, 1, 1, 2, 2, 2, 1, 2, 2, 2, 1 };

/*!
 * \brief Convert text string to CollectionKeyword
 */
Keywords::CollectionKeyword Keywords::collectionKeyword(const char* s)
{
	for (int n=0; n<nCollectionKeywords; ++n) if (strcmp(s,CollectionBlockKeywords[n]) == 0) return (Keywords::CollectionKeyword) n;
	return nCollectionKeywords;
}

/*!
 * \brief Convert CollectionKeyword to text string
 */
const char* Keywords::collectionKeyword(Keywords::CollectionKeyword kwd)
{
	return CollectionBlockKeywords[kwd];
}

/*!
 * \brief Return minimum number of expected arguments
 */
int Keywords::collectionKeywordNArguments(Keywords::CollectionKeyword kwd)
{
	return CollectionKeywordNArguments[kwd];
}

/*
// Settings Keywords
*/

// Settings Block Keywords
const char* SettingsBlockKeywords[] = { "EndSettings", "ImageExport" };

// Settings Block NArguments
int SettingsKeywordNArguments[] = { 0, 5 };

/*!
 * \brief Convert text string to SettingsKeyword
 */
Keywords::SettingsKeyword Keywords::settingsKeyword(const char* s)
{
	for (int n=0; n<nSettingsKeywords; ++n) if (strcmp(s,SettingsBlockKeywords[n]) == 0) return (Keywords::SettingsKeyword) n;
	return nSettingsKeywords;
}

/*!
 * \brief Convert SettingsKeyword to text string
 */
const char* Keywords::settingsKeyword(Keywords::SettingsKeyword kwd)
{
	return SettingsBlockKeywords[kwd];
}

/*!
 * \brief Return minimum number of expected arguments
 */
int Keywords::settingsKeywordNArguments(Keywords::SettingsKeyword kwd)
{
	return SettingsKeywordNArguments[kwd];
}

/*
// DataSet Keywords
*/

// DataSet Block Keywords
const char* DataSetBlockKeywords[] = { "Data", "EndDataSet", "Source", "Z" };

// DataSet Block NArguments
int DataSetKeywordNArguments[] = { 0, 0, 1, 1 };

/*!
 * \brief Convert text string to DataSetKeyword
 */
Keywords::DataSetKeyword Keywords::dataSetKeyword(const char* s)
{
	for (int n=0; n<nDataSetKeywords; ++n) if (strcmp(s,DataSetBlockKeywords[n]) == 0) return (Keywords::DataSetKeyword) n;
	return nDataSetKeywords;
}

/*!
 * \brief Convert DataSetKeyword to text string
 */
const char* Keywords::dataSetKeyword(Keywords::DataSetKeyword kwd)
{
	return DataSetBlockKeywords[kwd];
}

/*!
 * \brief Return minimum number of expected arguments
 */
int Keywords::dataSetKeywordNArguments(Keywords::DataSetKeyword kwd)
{
	return DataSetKeywordNArguments[kwd];
}

/*
// View Keywords
*/

// View Block Keywords
const char* ViewBlockKeywords[] = { "BoundingBox", "BoundingBoxPlaneY", "EndView", "LabelFaceViewer", "LabelScale", "MatrixW", "MatrixX", "MatrixY", "MatrixZ", "Perspective", "TitleScale" };

// View Block NArguments
int ViewKeywordNArguments[] = { 1, 1, 0, 1, 1, 4, 4, 4, 4, 1, 1 };

/*!
 * \brief Convert text string to ViewKeyword
 */
Keywords::ViewKeyword Keywords::viewKeyword(const char* s)
{
	for (int n=0; n<nViewKeywords; ++n) if (strcmp(s,ViewBlockKeywords[n]) == 0) return (Keywords::ViewKeyword) n;
	return nViewKeywords;
}

/*!
 * \brief Convert ViewKeyword to text string
 */
const char* Keywords::viewKeyword(Keywords::ViewKeyword kwd)
{
	return ViewBlockKeywords[kwd];
}

/*!
 * \brief Return minimum number of expected arguments
 */
int Keywords::viewKeywordNArguments(Keywords::ViewKeyword kwd)
{
	return ViewKeywordNArguments[kwd];
}
