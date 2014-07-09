/*
	*** Data 
	*** src/gui/data.cpp
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

// Flag all primitive data for regeneration
void UChromaWindow::setRegeneratePrimitives()
{
	for (Collection* collection = collections_.first(); collection != NULL; collection = collection->next) collection->setDisplayDataInvalid();
// 	TODO regenerateAxes_ = true;
}

// Add new collection
Collection* UChromaWindow::addCollection(QString title)
{
	static int collectionCount = 1;

	// Add an empty collection
	currentCollection_ = collections_.add();

	// Set link to MainView's primitive reflist
	currentCollection_->displayPrimitives().setViewer(ui.MainView);

	// Set the title
	if (title.isEmpty()) currentCollection_->setTitle("Empty Collection " + QString::number(collectionCount++));
	else currentCollection_->setTitle(title);

	setAsModified();

	return currentCollection_;
}

// Remove existing collection
void UChromaWindow::removeCollection(Collection* collection)
{
	if (!collection) return;

	// Check parent in order to work out where to delete the collection from
	if (collection->parent() == NULL)
	{
		// Set new currentCollection_
		if (collection->next) currentCollection_ = collection->next;
		else currentCollection_ = collection->prev;
		if (currentCollection_ == NULL) currentCollection_ = addCollection();

		// Remove master collection
		collections_.remove(collection);
	}
	else
	{
		// Need to check whether this is a Fit or an ExtractedData
		if (collection->type() == Collection::FitCollection)
		{
			// Set new currentCollection_
			if (collection->next) currentCollection_ = collection->next;
			else currentCollection_ = collection->prev;
			if (currentCollection_ == NULL) currentCollection_ = collection->parent();

			// Remove master collection
			collection->parent()->removeFitData(collection);
		}
		else
		{
			// Set new currentCollection_
			if (collection->next) currentCollection_ = collection->next;
			else currentCollection_ = collection->prev;
			if (currentCollection_ == NULL) currentCollection_ = collection->parent();

			// Remove master collection
			collection->parent()->removeExtractedData(collection);
		}
	}

	setAsModified();
}

// Move collection focus to next in list
void UChromaWindow::focusNextCollection()
{
	currentCollection_ = currentCollection_->nextCollection(true);

	// Safety check...
	if (currentCollection_ == NULL) currentCollection_ = collections_.first();

	updateGUI();
}

// Move collection focus to previous in list
void UChromaWindow::focusPreviousCollection()
{
	currentCollection_ = currentCollection_->previousCollection(false);

	// Safety check...
	if (currentCollection_ == NULL) currentCollection_ = collections_.last()->previousCollection(true);

	updateGUI();
}

// Return nth collection in list
Collection* UChromaWindow::collection(int index)
{
	return collections_[index];
}

// Find named collection
Collection* UChromaWindow::findCollection(QString name)
{
	// Loop over main collections
	for (Collection* collection = collections_.first(); collection != NULL; collection = collection->next)
	{
		if (collection->findCollection(name)) return collection;
	}
	return NULL;
}

// Return first collection in list
Collection* UChromaWindow::collections()
{
	return collections_.first();
}

// Return currently-selected Collection
Collection* UChromaWindow::currentCollection()
{
	return currentCollection_;
}

// Clear current data
void UChromaWindow::clearData(bool resetLayout)
{
	// Collections
	collections_.clear();

	// Data
	modified_ = false;
	inputFile_ = "";

	// Layout
	if (resetLayout)
	{
		viewLayout_.clear();
		currentViewPane_ = NULL;
	}

	// Interaction
	interactionMode_ = InteractionMode::ViewInteraction;
	interactionAxis_ = -1;
	clickedInteractionValue_ = 0.0;
	currentInteractionValue_ = 0.0;

	// View
	labelFaceViewer_ = false;
	labelCorrectOrientation_ = true;
	ui.actionViewPerspective->setChecked(false);
}
