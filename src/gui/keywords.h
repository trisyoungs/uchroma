/*
	*** Keyword Definitions
	*** src/gui/keywords.h
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

#ifndef UCHROMA_KEYWORDS_H
#define UCHROMA_KEYWORDS_H

// Forward Declarations
/* None */

/*
 * Keyword Definition
 */
class Keywords
{
	/*
	 * Input Block Keywords
	 */
	public:
	// Input File Block Keyword Enum
	enum InputBlock
	{
		AxisBlock,
		CollectionBlock,
		SettingsBlock,
		ViewBlock,
		nInputBlocks
	};
	// Convert text string to InputBlock
	static InputBlock inputBlock(const char* s);
	// Convert InputBlock to text string
	static const char* inputBlock(InputBlock id);
	// Print list of valid keywords for InputBlock specified
	static void printValidKeywords(Keywords::InputBlock block);


	/*
	 * Axis Block Keywords
	 */
	public:
	// Axis Block Keyword Enum
	enum AxisKeyword
	{
		AutoTicksKeyword,
		EndAxisKeyword,
		FirstTickKeyword,
		InvertKeyword,
		LabelOrientationKeyword,
		LimitsKeyword,
		LogarithmicKeyword,
		MinorTicksKeyword,
		PositionKeyword,
		StretchKeyword,
		TickDeltaKeyword,
		TickDirectionKeyword,
		TitleKeyword,
		TitleAnchorKeyword,
		TitleOrientationKeyword,
		VisibleKeyword,
		nAxisKeywords
	};
	// Convert text string to AxisKeyword
	static AxisKeyword axisKeyword(const char* s);
	// Convert AxisKeyword to text string
	static const char* axisKeyword(AxisKeyword kwd);
	// Return minimum number of expected arguments
	static int axisKeywordNArguments(AxisKeyword kwd);


	/*
	 * Collection Block Keywords
	 */
	public:
	// Collection Block Keyword Enum
	enum CollectionKeyword
	{
		ColourAlphaControlKeyword,
		ColourAlphaFixedKeyword,
		ColourCustomGradientKeyword,
		ColourRGBGradientAKeyword,
		ColourRGBGradientBKeyword,
		ColourHSVGradientAKeyword,
		ColourHSVGradientBKeyword,
		ColourSingleKeyword,
		ColourSourceKeyword,
		DataDirectoryKeyword,
		EndCollectionKeyword,
		InterpolateKeyword,
		InterpolateConstrainKeyword,
		InterpolateStepKeyword,
		SliceDefinitionKeyword,
		SurfaceStyleKeyword,
		TransformXKeyword,
		TransformYKeyword,
		TransformZKeyword,
		nCollectionKeywords
	};
	// Convert text string to CollectionKeyword
	static CollectionKeyword collectionKeyword(const char* s);
	// Convert CollectionKeyword to text string
	static const char* collectionKeyword(CollectionKeyword kwd);
	// Return minimum number of expected arguments
	static int collectionKeywordNArguments(CollectionKeyword kwd);


	/*
	 * Settings Block Keywords
	 */
	public:
	// Settings Block Keyword Enum
	enum SettingsKeyword
	{
		EndSettingsKeyword,
		ImageExportKeyword,
		nSettingsKeywords
	};
	// Convert text string to SettingsKeyword
	static SettingsKeyword settingsKeyword(const char* s);
	// Convert SettingsKeyword to text string
	static const char* settingsKeyword(SettingsKeyword kwd);
	// Return minimum number of expected arguments
	static int settingsKeywordNArguments(SettingsKeyword kwd);


	/*
	 * Slice Block Keywords
	 */
	public:
	// Slice Block Keyword Enum
	enum SliceKeyword
	{
		DataKeyword,
		EndSliceKeyword,
		SourceKeyword,
		ZKeyword,
		nSliceKeywords
	};
	// Convert text string to SliceKeyword
	static SliceKeyword sliceKeyword(const char* s);
	// Convert SliceKeyword to text string
	static const char* sliceKeyword(SliceKeyword kwd);
	// Return minimum number of expected arguments
	static int sliceKeywordNArguments(SliceKeyword kwd);


	/*
	 * View Block Keywords
	 */
	public:
	// View Block Keyword Enum
	enum ViewKeyword
	{
		BoundingBoxKeyword,
		BoundingBoxPlaneYKeyword,
		EndViewKeyword,
		LabelFaceViewerKeyword,
		LabelScaleKeyword,
		MatrixWKeyword,
		MatrixXKeyword,
		MatrixYKeyword,
		MatrixZKeyword,
		PerspectiveKeyword,
		TitleScaleKeyword,
		nViewKeywords
	};
	// Convert text string to ViewKeyword
	static ViewKeyword viewKeyword(const char* s);
	// Convert ViewKeyword to text string
	static const char* viewKeyword(ViewKeyword kwd);
	// Return minimum number of expected arguments
	static int viewKeywordNArguments(ViewKeyword kwd);
};

#endif
