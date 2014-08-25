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

	// Set title of collection
	collection->setTitle(item->text(0));
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
	item->setText(0, collection->title());
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	item->setCheckState(0, collection->visible() ? Qt::Checked : Qt::Unchecked);

	// Set icon
	QString iconName;
	// -- Get base name
	if (collection->type() == Collection::MasterCollection) iconName = ":/uchroma/icons/collection_collection";
	else if (collection->type() == Collection::FitCollection) iconName = ":/uchroma/icons/collection_fit";
	else if (collection->type() == Collection::ExtractedCollection) iconName = ":/uchroma/icons/collection_extracted";
	// -- If display pane is invalid, tweak icon name
	if (collection->displayPane() == NULL) iconName += "_nopane";
	item->setIcon(0, QIcon(iconName+".svg"));

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
	// -- Main 'edit' functions
	QAction* deleteAction = contextMenu.addAction("&Delete");
	QAction* duplicateAction = contextMenu.addAction("Du&plicate");
	// -- "Move to..." pane menu
	contextMenu.addSeparator();
	QMenu paneMenu;
	QList<QAction*> paneActions;
	for (ViewPane* pane = viewLayout_.panes(); pane != NULL; pane = pane->next)
	{
		QAction* action = paneMenu.addAction(pane->name());
		action->setData(VariantPointer<ViewPane>(pane));
		paneActions << action;
	}
	QAction* sendToPaneMenuAction = contextMenu.addMenu(&paneMenu);
	sendToPaneMenuAction->setText("Move to...");
	// -- Analysis
	contextMenu.addSeparator();
	QAction* fitAction = contextMenu.addAction("New &Fit Equation");
	QAction* reFitAction = contextMenu.addAction("Edit Fit E&quation");

	// Show it
	QAction* menuResult = contextMenu.exec(QCursor::pos());

	// What was clicked?
	if (menuResult == deleteAction)
	{
		if (QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete the collection '" + currentCollection_->title() + "'?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) removeCollection(currentCollection_);
	}
	else if (menuResult == duplicateAction)
	{
	}
	else if (paneActions.contains(menuResult))
	{
		// Check current display pane - if it is non-null we must remove it from that pane first
		if (currentCollection_->displayPane()) currentCollection_->displayPane()->removeCollection(currentCollection_);
		ViewPane* pane = VariantPointer<ViewPane>(menuResult->data());
		if (pane) pane->addCollection(currentCollection_);

		// Update the item
		updateCollectionTreeItem(ui.CollectionTree->currentItem());

		updateDisplay();
	}
	else if (menuResult == fitAction)
	{
		on_actionAnalyseFit_triggered(false);
	}
	else if (menuResult == reFitAction)
	{
	}
}

// Refresh collection list
void UChromaWindow::refreshCollections()
{
	ui.CollectionTree->clear();
	for (Collection* collection = collections_.first(); collection != NULL; collection = collection->next) addCollectionsToTree(collection, NULL);

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
		ui.CollectionNVerticesLabel->setText(QString::number(currentCollection_->displayPrimitives().nDefinedVertices()));
		ui.CollectionNIndicesLabel->setText(QString::number(currentCollection_->displayPrimitives().nDefinedIndices()));
		int nPrimitives = currentCollection_->displayPrimitives().nDefinedIndices() / (currentCollection_->displayStyle() == Collection::SurfaceStyle ? 3 : 2);
		ui.CollectionNPrimitivesLabel->setText(QString::number(nPrimitives));
	}
	else
	{
		ui.CollectionNSlicesLabel->setText("0");
		ui.CollectionNPointsLabel->setText("0");
		ui.CollectionNVerticesLabel->setText("0");
		ui.CollectionNIndicesLabel->setText("0");
		ui.CollectionNPrimitivesLabel->setText("0");
	}

	// Update collection info label
	if (currentCollection_) ui.InfoCurrentCollectionLabel->setText(currentCollection_->title());
	else ui.InfoCurrentCollectionLabel->setText("<No Current Collection>");
}
