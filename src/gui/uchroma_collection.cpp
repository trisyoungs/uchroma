/*
	*** UChroma - Collection 
	*** src/gui/uchroma_collection.cpp
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
#include "templates/reflist.h"
#include "templates/variantpointer.h"

void UChromaWindow::on_CollectionListButton_clicked(bool checked)
{
	if (refreshing_) return;
	ui.LeftWidgetsWidget->setVisible(checked);
}

void UChromaWindow::on_CollectionFocusNextButton_clicked(bool checked)
{
	focusNextCollection();
}

void UChromaWindow::on_CollectionFocusPreviousButton_clicked(bool checked)
{
	focusPreviousCollection();
}

void UChromaWindow::on_CollectionTree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	// Check current item
	if (current == NULL) return;

	// Get collection from item
	Collection* collection = VariantPointer<Collection>(current->data(0, Qt::UserRole));
	if (!collection) return;

	// Set new current collection
	currentCollection_ = collection;

	// Update other parts of the GUI
	updateSubWindows();
	updateCollectionInfo();
	updateDisplay();
}

void UChromaWindow::on_CollectionTree_itemClicked(QTreeWidgetItem* item, int column)
{
	// Get collection from item
	Collection* collection = VariantPointer<Collection>(item->data(0, Qt::UserRole));
	if (!collection) return;

	// Set the checkstate of the collection
	collection->setVisible(item->checkState(0) == Qt::Checked);

	// Update other parts of the GUI
	updateSubWindows();
	updateDisplay();
}

void UChromaWindow::on_CollectionTree_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
	// Get collection from item
	Collection* collection = VariantPointer<Collection>(item->data(0, Qt::UserRole));
	if (!collection) return;

	// TODO
}

void UChromaWindow::on_CollectionTree_itemChanged(QTreeWidgetItem* item, int column)
{
	// Check refreshing status
	if (refreshing_) return;

	// Get collection from item
	Collection* collection = VariantPointer<Collection>(item->data(0, Qt::UserRole));
	if (!collection) return;

	// Set name of collection
	collection->setName(item->text(0));

	updateCollectionInfo();
}

void UChromaWindow::on_CollectionAddButton_clicked(bool checked)
{
	addCollection();

	updateGUI();
}

void UChromaWindow::on_CollectionRemoveButton_clicked(bool checked)
{
	if (!currentCollection_) return;
	removeCollection(currentCollection_);

	updateGUI();
}

// Update item in collections tree
void UChromaWindow::updateCollectionTreeItem(QTreeWidgetItem* item)
{
	if (item == NULL) return;

	// First, get Collection from item data
	Collection* collection = VariantPointer<Collection>(item->data(0, Qt::UserRole));
	if (collection == NULL) return;

	// Set basic information
	item->setText(0, collection->name());
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	item->setCheckState(0, collection->visible() ? Qt::Checked : Qt::Unchecked);

	// Set icon
	item->setIcon(0, QIcon(collection->iconString(viewLayout_.collectionUsed(collection, ViewPane::StandardRole))));

	// If this is the current collection, select it
	if (collection == currentCollection_) item->setSelected(true);
}

// Add collection data to CollectionTree under specified item
void UChromaWindow::addCollectionsToTree(Collection* collection, QTreeWidgetItem* parent)
{
	// Add on collection item
	QTreeWidgetItem* item;
	if (parent) item = new QTreeWidgetItem(parent, 0);
	else item = new QTreeWidgetItem(ui.CollectionTree, 0);

	// Set item information
	item->setData(0, Qt::UserRole, VariantPointer<Collection>(collection));
	updateCollectionTreeItem(item);
	item->setExpanded(true);

	// Add any additional data
	for (Collection* fit = collection->fits(); fit != NULL; fit = fit->next) addCollectionsToTree(fit, item);
	for (Collection* extract = collection->slices(); extract != NULL; extract = extract->next) addCollectionsToTree(extract, item);
}

/*
 * Slots
 */

// Context menu requested for CollectionTree
void UChromaWindow::collectionTreeContextMenuRequested(const QPoint& point)
{
	// Build the context menu to display
	QMenu contextMenu;
	QAction* action;
	// -- Main 'edit' functions
	QAction* deleteAction = contextMenu.addAction("&Delete");
	QAction* duplicateAction = contextMenu.addAction("Du&plicate");
	// -- "Display..." pane menu
	contextMenu.addSeparator();
	QMenu paneMenu, removeFromPaneMenu, addToPaneMenu;
	RefList<ViewPane,bool> displayPanes = viewLayout_.panes(ViewPane::StandardRole);
	RefList<ViewPane,bool> currentPanes = viewLayout_.panes(currentCollection_, ViewPane::StandardRole);
	QList<QAction*> addToPaneActions, removeFromPaneActions;
	// -- ... Populate removeFromPaneMenu first...
	for (RefListItem<ViewPane,bool>* ri = currentPanes.first(); ri != NULL; ri = ri->next)
	{
		action = removeFromPaneMenu.addAction(ri->item->name());
		action->setData(VariantPointer<ViewPane>(ri->item));
		removeFromPaneActions << action;
	}
	action = paneMenu.addMenu(&removeFromPaneMenu);
	action->setText("Remove from pane...");
	// -- ... Now populate addToPaneMenu...
	for (RefListItem<ViewPane,bool>* ri = displayPanes.first(); ri != NULL; ri = ri->next)
	{
		if (currentPanes.contains(ri->item)) continue;
		action = addToPaneMenu.addAction(ri->item->name());
		action->setData(VariantPointer<ViewPane>(ri->item));
		addToPaneActions << action;
	}
	action = paneMenu.addMenu(&addToPaneMenu);
	action->setText("Add to pane...");
	action = contextMenu.addMenu(&paneMenu);
	action->setText("Display...");
	// -- Analysis
	contextMenu.addSeparator();
	QAction* fitAction = contextMenu.addAction("New &Fit Equation...");
	QAction* editFitAction = contextMenu.addAction("&Edit Fit E&quation...");
	QAction* updateFitAction = contextMenu.addAction("&Update/Continue Fit");

	// Show it
	QAction* menuResult = contextMenu.exec(QCursor::pos());

	// What was clicked?
	if (menuResult == deleteAction) on_actionCollectionDelete_triggered(false);
	else if (menuResult == duplicateAction) on_actionCollectionDuplicate_triggered(false);
	else if (removeFromPaneActions.contains(menuResult))
	{
		ViewPane* pane = VariantPointer<ViewPane>(menuResult->data());
		if (pane) pane->removeCollectionTarget(currentCollection_);

		// Update the item
		updateCollectionTreeItem(ui.CollectionTree->currentItem());

		updateDisplay();
	}
	else if (addToPaneActions.contains(menuResult))
	{
		ViewPane* pane = VariantPointer<ViewPane>(menuResult->data());
		if (pane) pane->addCollectionTarget(currentCollection_);

		// Update the item
		updateCollectionTreeItem(ui.CollectionTree->currentItem());

		updateDisplay();
	}
	else if (menuResult == fitAction) on_actionAnalyseNewFit_triggered(false);
	else if (menuResult == editFitAction) on_actionAnalyseEditFit_triggered(false);
	else if (menuResult == updateFitAction) on_actionAnalyseUpdateFit_triggered(false);
}

// Refresh collection list
void UChromaWindow::refreshCollections()
{
	refreshing_ = true;

	ui.CollectionTree->clear();
	for (Collection* collection = collections_.first(); collection != NULL; collection = collection->next) addCollectionsToTree(collection, NULL);

	refreshing_ = false;

	// Update associated info
	updateCollectionInfo();
}

// Update displayed info for current collection
void UChromaWindow::updateCollectionInfo()
{
	// Update collection stats underneath main Collection list
	if (currentCollection_)
	{
		ui.CollectionNSlicesLabel->setText(QString::number(currentCollection_->nDataSets()));
		ui.CollectionNPointsLabel->setText(QString::number(currentCollection_->nDataPoints()));
	}
	else
	{
		ui.CollectionNSlicesLabel->setText("0");
		ui.CollectionNPointsLabel->setText("0");
	}

	// Set number of collections in the current session (can use Collection's ObjectList to get number)
	ui.CollectionNCollectionsLabel->setText("("+QString::number(collections_.nItems())+")");

	// Update collection info label
	if (currentCollection_)
	{
		ui.InfoCurrentCollectionLabel->setText(currentCollection_->name());
		ui.InfoCurrentCollectionIconLabel->setPixmap(QPixmap(currentCollection_->iconString(viewLayout_.collectionUsed(currentCollection_, ViewPane::StandardRole))));
	}
	else ui.InfoCurrentCollectionLabel->setText("<No Current Collection>");
}
