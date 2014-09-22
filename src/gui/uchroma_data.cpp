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

#include "base/session.h"
#include "gui/uchroma.h"

// Return unique name based on supplied baseName
QString UChromaWindow::uniqueCollectionName(QString baseName)
{
	QString testName = baseName;
	int index = 0;
	Collection* collection;
	do
	{
		// Add on suffix (if index > 0)
		if (index > 0) testName = baseName + " "+QString::number(index);
		++index;
		for (collection = collections_.first(); collection != NULL; collection = collection->next) if (collection->name() == testName) break;
	} while (collection);

	return testName;
}

// Setup new, empty session
void UChromaWindow::startNewSession(bool createDefaults)
{
	// Clear collections
	collections_.clear();

	// Clear layout
	viewLayout_.clear();
	currentViewPane_ = NULL;

	// Interaction
	interacting_ = false;
	interactionMode_ = InteractionMode::ViewInteraction;
	interactionAxis_ = -1;
	clickedInteractionValue_ = 0.0;
	currentInteractionValue_ = 0.0;

	// View
	labelFaceViewer_ = false;
	labelCorrectOrientation_ = true;

	// Create defaults if requested
	if (createDefaults)
	{
		// Set basic view layout
		currentViewPane_ = viewLayout_.setDefault();

		// Add an empty collection, and add it to the current view pane
		currentViewPane_->addCollectionTarget(addCollection());
		currentViewPane_->translateView(0.0, 0.0, -15.0);
	}

	// Set current project data
	Session::setAsNotModified();
	Session::setInputFile(QString());
}

// Add new collection
Collection* UChromaWindow::addCollection(QString name)
{
	// Add an empty collection
	currentCollection_ = collections_.add();

	// Set the title
	if (name.isEmpty()) currentCollection_->setName( uniqueCollectionName("Empty Collection") );
	else currentCollection_->setName(uniqueCollectionName(name));

	Session::setAsModified();

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
			collection->parent()->removeFit(collection);
		}
		else
		{
			// Set new currentCollection_
			if (collection->next) currentCollection_ = collection->next;
			else currentCollection_ = collection->prev;
			if (currentCollection_ == NULL) currentCollection_ = collection->parent();

			// Remove master collection
			collection->parent()->removeSlice(collection);
		}
	}

	Session::setAsModified();
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

// Locate collection
Collection* UChromaWindow::locateCollection(QString locator)
{
	// First, split up the supplied string by the delimiter '//'
	QStringList locatorParts = locator.split("//");

	// Loop over main collections, passing parts list
	for (Collection* collection = collections_.first(); collection != NULL; collection = collection->next)
	{
		Collection* foundCollection = collection->locateCollection(locatorParts, 0);
		if (foundCollection) return foundCollection;
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
