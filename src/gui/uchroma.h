/*
	*** uChroma Main Window
	*** src/gui/uchroma.h
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

#ifndef UCHROMA_MAINWINDOW_H
#define UCHROMA_MAINWINDOW_H

#include "gui/ui_uchroma.h"
#include "gui/axes.h"
#include "gui/create.h"
#include "gui/data.h"
#include "gui/fit.h"
#include "gui/layout.h"
#include "gui/slicemonitor.h"
#include "gui/style.h"
#include "gui/transform.h"
#include "gui/view.h"
#include "gui/saveimage.h"
#include "gui/dataimport.h"
#include "base/collection.h"
#include "base/transformer.h"
#include "base/lineparser.h"
#include "base/viewlayout.h"

// Forward Declarations
/* None */

/*
 * Interaction Mode Information
 */
class InteractionMode
{
	public:
	// Interaction Modes
	enum Mode
	{
		FitDialogSelectXInteraction,
		ViewInteraction,
		ZoomInteraction,
		nInteractionModes
	};
	// Name of mode
	QString name;
	// Usage text
	QString modeText;


	/*
	 * Singleton
	 */
	public:
	// Static list of interaction modes
	static InteractionMode interactionModes[];
};

/*
 * Main uChroma Window
 */
class UChromaWindow : public QMainWindow
{
	// All Qt declarations must include this macro
	Q_OBJECT


	/*
	 * Window Functions
	 */
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;
	// Axes Window
	AxesWindow axesWindow_;
	// Create Window
	CreateWindow createWindow_;
	// Data Window
	DataWindow dataWindow_;
	// Fit Window
	FitWindow fitWindow_;
	// Layout Window
	LayoutWindow layoutWindow_;
	// Slice Monitor Window
	SliceMonitorWindow sliceMonitorWindow_;
	// Style Window
	StyleWindow styleWindow_;
	// Transform Window
	TransformWindow transformWindow_;
	// View Window
	ViewWindow viewWindow_;
	// Save Image Dialog
	SaveImageDialog saveImageDialog_;
	// Data Import Dialog
	DataImportDialog dataImportDialog_;
	// Statusbar info line label
	QLabel* statusBarInfoLabel_;

	public:
	// Constructor / Destructor
	UChromaWindow(QMainWindow *parent = 0);
	~UChromaWindow();
	// Main form declaration
	Ui::UChromaWindow ui;
	// Return centre coordinate of main window
	QPoint centrePos();

	protected:
	// Window close event
	void closeEvent(QCloseEvent *event);

	public:
	// Load program settings
	void loadSettings();
	// Save program settings
	void saveSettings();


	/*
	 * File Actions
	 */
	private slots:
	void on_actionFileNew_triggered(bool checked);
	void on_actionFileLoad_triggered(bool checked);
	void on_actionFileSave_triggered(bool checked);
	void on_actionFileSaveAs_triggered(bool checked);
	void on_actionFileExportImage_triggered(bool checked);
	void on_actionFileQuit_triggered(bool checked);

	private:
	// Check for modified data before closing
	bool checkBeforeClose();


	/*
	 * View Actions
	 */
	private slots:
	void on_actionViewPerspective_triggered(bool checked);
	void on_actionViewReset_triggered(bool checked);


	/*
	 * Collections Actions
	 */
	private slots:
	void on_actionCollectionsNew_triggered(bool checked);
	void on_actionCollectionsFocusNext_triggered(bool checked);
	void on_actionCollectionsFocusPrevious_triggered(bool checked);


	/*
	 * Data Actions
	 */
	private slots:
	void on_actionDataLoadXY_triggered(bool checked);
	void on_actionDataImport_triggered(bool checked);


	/*
	 * Axes Actions
	 */
	private:
	// Change interaction axis target
	void interactionActionTriggered(int axis);

	private slots:
	void on_actionAxesShowAll_triggered(bool checked);
	void on_actionAxesInteractX_triggered(bool checked);
	void on_actionAxesInteractY_triggered(bool checked);
	void on_actionAxesInteractZ_triggered(bool checked);
	void on_actionAxesInteractNone_triggered(bool checked);


	/*
	 * Windows Actions
	 */
	private slots:
	void on_actionWindowsCollections_triggered(bool checked);
	void on_actionWindowsData_triggered(bool checked);
	void on_actionWindowsStyle_triggered(bool checked);
	void on_actionWindowsTransform_triggered(bool checked);
	void on_actionWindowsView_triggered(bool checked);
	void on_actionWindowsAxes_triggered(bool checked);
	void on_actionWindowsCreate_triggered(bool checked);
	void on_actionWindowsFit_triggered(bool checked);
	void on_actionWindowsLayout_triggered(bool checked);
	void on_actionWindowsSliceMonitor_triggered(bool checked);


	/*
	 * Settings Menu
	 */
	private slots:
	void on_actionSettingsChooseFont_triggered(bool checked);


	/*
	 * Collections
	 */
	private:
	// Update data for specified tree item
	void updateCollectionTreeItem(QTreeWidgetItem* item);
	// Add collection data to CollectionTree under specified item
	void addCollectionsToTree(Collection* collection, QTreeWidgetItem* parent);

	private slots:
	void on_CollectionTree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	void on_CollectionTree_itemClicked(QTreeWidgetItem* item, int column);
	void on_CollectionTree_itemDoubleClicked(QTreeWidgetItem* item, int column);
	void on_CollectionTree_itemChanged(QTreeWidgetItem* item, int column);
	void on_CollectionAddButton_clicked(bool checked);
	void on_CollectionRemoveButton_clicked(bool checked);

	public slots:
	// Context menu requested for CollectionTree
	void collectionTreeContextMenuRequested(const QPoint& point);
	// Refresh collection list
	void refreshCollections();
	// Update displayed info for current collection
	void updateCollectionInfo();


	/*
	 * InfoBar
	 */
	public slots:
	// Update coordinate info
	void updateCoordinateInfo();
	// Update text of renderTimeLabel_ in statusbar
	void updateRenderTimeLabel(QString text);


	/*
	 * Update
	 */
	public slots:
	// Update all aspects of GUI
	void updateGUI();
	// Update all subwindows
	void updateSubWindows();
	// Update title bar
	void updateTitleBar();
	// Update display data (used for surfaces) in all collections
	void updateDisplayData();
	// Update main display
	void updateDisplay();


	/*
	 * Input File
	 */
	private:
	// Whether current data has been modified
	bool modified_;
	// Current input file directory
	QDir inputFileDirectory_;
	// Current input filename
	QString inputFile_;

	private:
	// Read AxisBlock keywords
	bool readAxisBlock(LineParser& parser, Axes& axes, int axis);
	// Read CollectionBlock keywords
	bool readCollectionBlock(LineParser& parser, Collection* collection);
	// Read DataSetBlock keywords
	bool readDataSetBlock(LineParser& parser, DataSet* dataSet, Collection* collection);
	// Read SettingsBlock keywords
	bool readSettingsBlock(LineParser& parser);
	// Read ViewBlock keywords
	bool readViewBlock(LineParser& parser);
	// Read ViewPaneBlock keywords
	bool readViewPaneBlock(LineParser& parser, ViewPane* pane);
	// Write AxisBlock keywords
	bool writeAxisBlock(LineParser& parser, Axes& axes, int axis);
	// Write CollectionBlock keywords
	bool writeCollectionBlock(LineParser& parser, Collection* collection, Collection::CollectionType type = Collection::MasterCollection, int indentLevel = 0);
	// Write DataSetBlock keywords
	bool writeDataSetBlock(LineParser& parser, DataSet* dataSet, int indentLevel = 0);
	// Write SettingsBlock keywords
	bool writeSettingsBlock(LineParser& parser);
	// Write ViewBlock keywords
	bool writeViewBlock(LineParser& parser);
	// Write ViewPaneBlock keywords
	bool writeViewPaneBlock(LineParser& parser, ViewPane* pane);
	
	public:
	// Load input from file specified
	bool loadInputFile(QString fileName);
	// Save current input to file specified
	bool saveInputFile(QString fileName);
	// Flag data as (not) modified and update titlebar
	void setAsModified(bool isModified = true);


	/*
	 * Data
	 */
	private:
	// Current image export filename
	QString imageExportFile_;
	// Format for exported image
	Viewer::ImageFormat imageExportFormat_;
	// Size of exported image
	int imageExportWidth_, imageExportHeight_;
	// Whether to maintain current aspect ratio on image export
	bool imageExportMaintainAspect_;
	// Font file to use for viewer
	QString viewerFont_;
	// List of Collections
	List<Collection> collections_;
	// Currently-selected Collection
	Collection* currentCollection_;

	private:
	// Flag all primitive data for regeneration
	void setRegeneratePrimitives();
	// Recalculate tick deltas for specified axis
	void calculateTickDeltas(int axis);

	public:
	// Add new collection
	Collection* addCollection(QString title = QString());
	// Remove existing collection
	void removeCollection(Collection* collection);
	// Return first collection in list
	Collection* collections();
	// Return nth collection in list
	Collection* collection(int index);
	// Find named collection
	Collection* findCollection(QString name);
	// Move collection focus to next in list
	void focusNextCollection();
	// Move collection focus to previous in list
	void focusPreviousCollection();
	// Return currently-selected Collection
	Collection* currentCollection();
	// Clear current data
	void clearData(bool resetLayout);


	/*
	 * View Layouts
	 */
	private:
	// List of available view layouts
	ParentList<ViewLayout,UChromaWindow> viewLayouts_;
	// Current view layout
	ViewLayout viewLayout_;
	// Current view pane
	ViewPane* currentViewPane_;

	public:
	// Return curreent view layout
	ViewLayout* viewLayout();
	// Recalculate layout pane positions / sizes (after context resize etc.)
	void recalculateViewLayout(int contextWidth, int contextHeight);
	// Set current view pane to the one under the specified screen coordinates
	bool setCurrentViewPane(int mouseX, int mouseY);
	// Return current view pane
	ViewPane* currentViewPane();


	/*
	 * Interaction
	 */
	private:
	// Current interaction mode
	InteractionMode::Mode interactionMode_;
	// Whether the user is currently interacting with the display
	bool interacting_;
	// Current axis target for interaction
	int interactionAxis_;
	// Starting (clicked) value when interacting
	double clickedInteractionValue_;
	// Starting (clicked) keyboard modifiers when interacting
	Qt::KeyboardModifiers clickedInteractionModifiers_;
	// Current interaction position on axis
	double currentInteractionValue_;
	// Current slice data
	DataSet currentSlice_;

	private:
	// Calculate selection axis coordinate from supplied screen coordinates
	double screenToAxis(int axis, int mouseX, int mouseY);
	// Return axis bin value of closest point to supplied value
	int closestBin(int axis, double value);
	// Extract slice based on specified axis and bin
	DataSet extractSlice(int axis, int bin);

	public:
	// Set interaction mode and target axis
	void setInteractionMode(InteractionMode::Mode mode, int axis);
	// Return interaction mode
	InteractionMode::Mode interactionMode();
	// Return current axis target for interaction
	int interactionAxis();
	// Return whether the user is currently interacting with the display
	bool interacting();
	// Start interaction at the specified screen coordinates
	void startInteraction(int mouseX, int mouseY, Qt::KeyboardModifiers modifiers);
	// Update current interaction value
	void updateInteractionPosition(int mouseX, int mouseY);
	// End interaction at the specified screen coordinates
	void endInteraction(int mouseX, int mouseY);
	// Return clicked interaction value on axis
	double clickedInteractionValue();
	// Return current interaction value on axis
	double currentInteractionValue();
	// Return clicked interaction coordinate on axis
	double clickedInteractionCoordinate();
	// Return current interaction coordinate on axis
	double currentInteractionCoordinate();
	// Return current slice data
	DataSet* currentSlice();

	signals:
	// Slice data has changed
	void currentSliceDataChanged();


	/*
	 * View
	 */
	private:
	// Whether axis text labels face the viewer automatically
	bool labelFaceViewer_;
	// Whether axis text labels are corrected for left-right / up readability
	bool labelCorrectOrientation_;

	public:
	// Set whether axis text labels face the viewer automatically
	void setLabelFaceViewer(bool b);
	// Return whether axis text labels face the viewer automatically
	bool labelFaceViewer();
	// Set whether axis text labels are corrected for left-right / up readability
	void setLabelCorrectOrientation(bool b);
	// Return whether axis text labels are corrected for left-right / up readability
	bool labelCorrectOrientation();
};

#endif
