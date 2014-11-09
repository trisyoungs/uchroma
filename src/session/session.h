/*
	*** Session
	*** src/session/session.h
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

#ifndef UCHROMA_SESSION_H
#define UCHROMA_SESSION_H

#include <QtCore/QString>
#include <QtCore/QDir>
#include "base/collection.h"
#include "base/viewlayout.h"

// Forward Declarations
class QMainWindow;
class LineParser;
class Axes;
class ViewPane;

// Session
class UChromaSession
{
	/*
	 * Main Window Link
	 */
	private:
	// Pointer to main window
	static UChromaWindow* uChroma_;

	private:
	// Update title bar of main window
	static void updateTitleBar();
	// Last modification status that the title bar was updated at
	static bool lastTitleBarModificationStatus_;

	public:
	// Set main window pointer
	static void setMainWindow(UChromaWindow* ptr);


	/*
	 * Modification Flag
	 */
	private:
	// Modified flag
	static bool modified_;

	public:
	// Set status to true
	static void setAsModified();
	// Set status to false
	static void setAsNotModified();
	// Return modified status
	static bool isModified();


	/*
	 * General Data
	 */
	public:
	// Available image export formats
        enum ImageFormat { BMPFormat, PGFormat, PNGFormat, PPMFormat, XBMFormat, X11Format, nImageFormats };
        static ImageFormat imageFormat(QString s);
        static ImageFormat imageFormatFromFilter(QString s);
        static const char* imageFormatFilter(ImageFormat bf);
        static const char* imageFormatExtension(ImageFormat bf);

	private:
	// Current image export filename
	static QString imageExportFileName_;
	// Format for exported image
	static ImageFormat imageExportFormat_;
	// Size of exported image
	static int imageExportWidth_, imageExportHeight_;
	// Whether to maintain current aspect ratio on image export
	static bool imageExportMaintainAspect_;
	// Font file to use for viewer
	static QString viewerFontFileName_;

	public:
	// Set current image export filename
	static void setImageExportFileName(QString fileName);
	// Return current image export filename
	static QString imageExportFileName();
	// Set format for exported image
	static void setImageExportFormat(ImageFormat fmt);
	// Return format for exported image
	static ImageFormat imageExportFormat();
	// Set width of exported image
	static void setImageExportWidth(int width);
	// Return width of exported image
	static int imageExportWidth();
	// Set height of exported image
	static void setImageExportHeight(int height);
	// Return height of exported image
	static int imageExportHeight();
	// Set whether to maintain current aspect ratio on image export
	static void setImageExportMaintainAspect(bool b);
	// Return whether to maintain current aspect ratio on image export
	static bool imageExportMaintainAspect();
	// Set font filename to use for viewer
	static void setViewerFontFileName(QString fileName);
	// Return font file to use for viewer
	static QString viewerFontFileName();


	/*
	 * Collections
	 */
	private:
	// List of Collections
	static List<Collection> collections_;
	// Currently-selected Collection
	static Collection* currentCollection_;

	private:
	// Return unique name based on supplied baseName
	static QString uniqueCollectionName(QString baseName);

	public:
	// Setup new, empty session
	static void startNewSession(bool createDefaults);
	// Add new collection
	static Collection* addCollection(QString name = QString());
	// Remove existing collection
	static void removeCollection(Collection* collection);
	// Remove current collection
	static void removeCurrentCollection();
	// Return number of collections
	static int nCollections();
	// Return first collection in list
	static Collection* collections();
	// Return nth collection in list
	static Collection* collection(int index);
	// Locate named collection
	static Collection* locateCollection(QString locator);
	// Move collection focus to next in list
	static void focusNextCollection();
	// Move collection focus to previous in list
	static void focusPreviousCollection();
	// Set current Collection
	static void setCurrentCollection(Collection* collection);
	// Return current Collection
	static Collection* currentCollection();
	// Return whether specified collection is the current collection
	static bool isCurrentCollection(Collection* collection);


	/*
	 * View Layout
	 */
	private:
	// Current view layout
	static ViewLayout viewLayout_;
	// Current view pane
	static ViewPane* currentViewPane_;

	public:
	// Return current view layout
	static ViewLayout& viewLayout();
	// Set view layout
	static void setViewLayout(ViewLayout& source);
	// Recalculate layout pane positions / sizes (after context resize etc.)
	static void recalculateViewLayout(int contextWidth, int contextHeight);
	// Set current view pane to the one under the specified screen coordinates
	static bool setCurrentViewPane(int layoutX, int layoutY);
	// Return current view pane
	static ViewPane* currentViewPane();


	/*
	 * Data
	 */
	private:
	// Current session file directory
	static QDir sessionFileDirectory_;
	// Current input filename
	static QString inputFile_;
	// Whether to enforce hard fail on input file error
	static bool hardIOFail_;

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
	static void printValidKeywords(UChromaSession::InputBlock block);

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
		LineStyleKeyword,
		ShininessKeyword,
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

	// View Block Keyword Enum
	enum ViewKeyword
	{
		EndViewKeyword,
		GridKeyword,
		ViewPaneBlockKeyword,
		nViewKeywords
	};
	// Convert text string to ViewKeyword
	static ViewKeyword viewKeyword(QString s);
	// Convert ViewKeyword to text string
	static const char* viewKeyword(ViewKeyword kwd);
	// Return minimum number of expected arguments
	static int viewKeywordNArguments(ViewKeyword kwd);

	// ViewPane Block Keyword Enum
	enum ViewPaneKeyword
	{
		AutoPositionTitlesKeyword,
		AxisBlockKeyword,
		BoundingBoxKeyword,
		BoundingBoxPlaneYKeyword,
		EndViewPaneKeyword,
		FlatLabelsKeyword,
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
		UseBestFlatViewKeyword,
		ViewTypeKeyword,
		nViewPaneKeywords
	};
	// Convert text string to ViewKeyword
	static ViewPaneKeyword viewPaneKeyword(QString s);
	// Convert ViewKeyword to text string
	static const char* viewPaneKeyword(ViewPaneKeyword kwd);
	// Return minimum number of expected arguments
	static int viewPaneKeywordNArguments(ViewPaneKeyword kwd);

	private:
	// Read AxisBlock keywords
	static bool readAxisBlock(LineParser& parser, Axes& axes, int axis);
	// Read CollectionBlock keywords
	static bool readCollectionBlock(LineParser& parser, Collection* collection);
	// Read DataSetBlock keywords
	static bool readDataSetBlock(LineParser& parser, DataSet* dataSet, Collection* collection);
	// Read FitParametersBlock keywords
	static bool readFitParametersBlock(LineParser& parser, FitKernel* fitKernel);
	// Read SettingsBlock keywords
	static bool readSettingsBlock(LineParser& parser);
	// Read ViewBlock keywords
	static bool readViewBlock(LineParser& parser);
	// Read ViewPaneBlock keywords
	static bool readViewPaneBlock(LineParser& parser, ViewPane* pane);
	// Write AxisBlock keywords
	static bool writeAxisBlock(LineParser& parser, Axes& axes, int axis);
	// Write CollectionBlock keywords
	static bool writeCollectionBlock(LineParser& parser, Collection* collection, Collection::CollectionType type = Collection::MasterCollection, int indentLevel = 0);
	// Write DataSetBlock keywords
	static bool writeDataSetBlock(LineParser& parser, DataSet* dataSet, int indentLevel = 0);
	// Write FitParametersBlock keywords
	static bool writeFitParametersBlock(LineParser& parser, FitKernel* fitKernel, int indentLevel = 0);
	// Write SettingsBlock keywords
	static bool writeSettingsBlock(LineParser& parser);
	// Write ViewBlock keywords
	static bool writeViewBlock(LineParser& parser);
	// Write ViewPaneBlock keywords
	static bool writeViewPaneBlock(LineParser& parser, ViewPane* pane);
	
	public:
	// Return current session file directory
	static QDir sessionFileDirectory();
	// Set name of input file
	static void setInputFile(QString fileName);
	// Return name of input file
	static QString inputFile();
	// Set whether to enforce hard fail on session file error
	static void setHardIOFail(bool hardFail);
	// Load session from file specified
	static bool loadSession(QString fileName);
	// Save session input to file specified
	static bool saveSession(QString fileName);
};

#endif
