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

void UChromaWindow::refreshCollections()
{
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
}
