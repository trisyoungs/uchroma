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

#include <QtCore/QString>

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
		CollectionBlock,
		SettingsBlock,
		ViewBlock,
		nInputBlocks
	};
	// Convert text string to InputBlock
	static InputBlock inputBlock(QString s);
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
		AutoScaleKeyword,
		AutoTicksKeyword,
		EndAxisKeyword,
		FirstTickKeyword,
		FractionalPositioningKeyword,
		GridLinesKeyword,
		GridLineMajorStyleKeyword,
		GridLineMinorStyleKeyword,
		InvertKeyword,
		LabelAnchorKeyword,
		LabelOrientationKeyword,
		LimitsKeyword,
		LogarithmicKeyword,
		MinorTicksKeyword,
		NumberFormatKeyword,
		PositionFractionalKeyword,
		PositionRealKeyword,
		StretchKeyword,
		TickDeltaKeyword,
		TickDirectionKeyword,
		TitleKeyword,
		TitleAnchorKeyword,
		TitleOrientationKeyword,
		VisibleAxisKeyword,
		nAxisKeywords
	};
	// Convert text string to AxisKeyword
	static AxisKeyword axisKeyword(QString s);
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
		DataSetDefinitionKeyword,
		EndCollectionKeyword,
		FitBlockKeyword,
		FitParametersBlockKeyword,
		InterpolateKeyword,
		InterpolateConstrainKeyword,
		InterpolateStepKeyword,
		SliceBlockKeyword,
		StyleKeyword,
		TransformXKeyword,
		TransformYKeyword,
		TransformZKeyword,
		VisibleCollectionKeyword,
		nCollectionKeywords
	};
	// Convert text string to CollectionKeyword
	static CollectionKeyword collectionKeyword(QString s);
	// Convert CollectionKeyword to text string
	static const char* collectionKeyword(CollectionKeyword kwd);
	// Return minimum number of expected arguments
	static int collectionKeywordNArguments(CollectionKeyword kwd);


	/*
	 * DataSet Block Keywords
	 */
	public:
	// DataSet Block Keyword Enum
	enum DataSetKeyword
	{
		DataKeyword,
		EndDataSetKeyword,
		SourceKeyword,
		ZKeyword,
		nDataSetKeywords
	};
	// Convert text string to DataSetKeyword
	static DataSetKeyword dataSetKeyword(QString s);
	// Convert DataSetKeyword to text string
	static const char* dataSetKeyword(DataSetKeyword kwd);
	// Return minimum number of expected arguments
	static int dataSetKeywordNArguments(DataSetKeyword kwd);


	/*
	 * FitParameters Block Keywords
	 */
	public:
	// FitParameters Block Keyword Enum
	enum FitParametersKeyword
	{
		EndFitParametersKeyword,
		EquationKeyword,
		GlobalKeyword,
		LimitStrengthKeyword,
		OrthogonalKeyword,
		ReferenceKeyword,
		VariableKeyword,
		XRangeTypeKeyword,
		XRangeAbsoluteKeyword,
		XRangeIndexKeyword,
		XRangeIndexSingleKeyword,
		ZRangeTypeKeyword,
		ZRangeAbsoluteKeyword,
		ZRangeIndexKeyword,
		ZRangeIndexSingleKeyword,
		nFitParametersKeywords
	};
	// Convert text string to FitParametersKeyword
	static FitParametersKeyword fitParametersKeyword(QString s);
	// Convert FitParametersKeyword to text string
	static const char* fitParametersKeyword(FitParametersKeyword kwd);
	// Return minimum number of expected arguments
	static int fitParametersKeywordNArguments(FitParametersKeyword kwd);


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
	static SettingsKeyword settingsKeyword(QString s);
	// Convert SettingsKeyword to text string
	static const char* settingsKeyword(SettingsKeyword kwd);
	// Return minimum number of expected arguments
	static int settingsKeywordNArguments(SettingsKeyword kwd);


	/*
	 * View Block Keywords
	 */
	public:
	// View Block Keyword Enum
	enum ViewKeyword
	{
		EndViewKeyword,
		GridKeyword,
		LabelFaceViewerKeyword,
		ViewPaneBlockKeyword,
		nViewKeywords
	};
	// Convert text string to ViewKeyword
	static ViewKeyword viewKeyword(QString s);
	// Convert ViewKeyword to text string
	static const char* viewKeyword(ViewKeyword kwd);
	// Return minimum number of expected arguments
	static int viewKeywordNArguments(ViewKeyword kwd);

	/*
	 * ViewPane Block Keywords
	 */
	public:
	// ViewPane Block Keyword Enum
	enum ViewPaneKeyword
	{
		AxisBlockKeyword,
		BoundingBoxKeyword,
		BoundingBoxPlaneYKeyword,
		EndViewPaneKeyword,
		GeometryKeyword,
		LabelPointSizeKeyword,
		PerspectiveKeyword,
		RoleKeyword,
		RoleTargetCollectionKeyword,
		RoleTargetPaneKeyword,
		RotationXKeyword,
		RotationYKeyword,
		RotationZKeyword,
		TitlePointSizeKeyword,
		TranslationKeyword,
		ViewTypeKeyword,
		nViewPaneKeywords
	};
	// Convert text string to ViewKeyword
	static ViewPaneKeyword viewPaneKeyword(QString s);
	// Convert ViewKeyword to text string
	static const char* viewPaneKeyword(ViewPaneKeyword kwd);
	// Return minimum number of expected arguments
	static int viewPaneKeywordNArguments(ViewPaneKeyword kwd);
};

#endif
