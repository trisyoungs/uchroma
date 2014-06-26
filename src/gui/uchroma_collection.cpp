/*
	*** Main Window - Collection 
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

void UChromaWindow::on_CollectionTree_itemClicked(QTreeWidgetItem* item, int column)
{
	// Get collection from item
	Collection* collection = VariantPointer<Collection>(item->data(0, Qt::UserRole));
	if (!collection) return;

	// Set new current collection
	currentCollection_ = collection;

	// Set the checkstate of the collection
	collection->setVisible(item->checkState(0) == Qt::Checked);
	updateGUI();
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
	collection->setTitle(item->text(1));
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

// Refresh collection list
void UChromaWindow::refreshCollections()
{
	// Clear and repopulate list
	ui.CollectionTree->clear();
	for (Collection* collection = collections_.first(); collection != NULL; collection = collection->next)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem(ui.CollectionTree, 0);
		item->setText(0, collection->title());
		item->setData(0, Qt::UserRole, VariantPointer<Collection>(collection));
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		item->setCheckState(0, collection->visible() ? Qt::Checked : Qt::Unchecked);

		// If this is the current collection, select it
		if (collection == currentCollection_) item->setSelected(true);
	}

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
	ui.InfoCurrentCollectionLabel->setText("<No Current Collection>");
}
