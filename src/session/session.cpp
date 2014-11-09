/*
	*** Session Statics
	*** src/io/session.cpp
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

#include "session/session.h"
#include "gui/uchroma.h"
#include "version.h"

// Static variables
// -- Main window link
UChromaWindow* UChromaSession::uChroma_ = NULL;
bool UChromaSession::lastTitleBarModificationStatus_ = false;
// -- Modification flag
bool UChromaSession::modified_ = false;
// -- Image export and viewer font
QString UChromaSession::imageExportFileName_ = "image.png";
int UChromaSession::imageExportWidth_ = 800;
int UChromaSession::imageExportHeight_ = 600;
bool UChromaSession::imageExportMaintainAspect_ = true;
UChromaSession::ImageFormat UChromaSession::imageExportFormat_ = UChromaSession::PNGFormat;
#ifdef WIN32
	QString UChromaSession::viewerFontFileName_ = QDir::current().absoluteFilePath("bin/wright.ttf");
#elif __APPLE__
	QString UChromaSession::viewerFontFileName_ = QCoreApplication::applicationDirPath() + "/../SharedSupport/resources/wright.ttf";
#else
	QString UChromaSession::viewerFontFileName_ = QDir::current().absoluteFilePath("wright.ttf");
#endif
// -- Collections
List<Collection> UChromaSession::collections_;
Collection* UChromaSession::currentCollection_ = NULL;
// -- View layout
ViewLayout UChromaSession::viewLayout_;
ViewPane* UChromaSession::currentViewPane_ = NULL;
// -- Data
QDir UChromaSession::sessionFileDirectory_;
QString UChromaSession::inputFile_;
bool UChromaSession::hardIOFail_ = false;

/*
 * Main Window Pointer
 */

// Set main window pointer
void UChromaSession::setMainWindow(UChromaWindow* ptr)
{
	uChroma_ = ptr;

	updateTitleBar();
}

// Update title bar of main window
void UChromaSession::updateTitleBar()
{
	if (!uChroma_) return;

	// Do nothing if the modification status is the same as it was last time we were called
	if (modified_ == lastTitleBarModificationStatus_) return;

	if (modified_) uChroma_->setWindowTitle("uChroma v" + QString(UCHROMAREVISION) + " - " + inputFile_ + " (modified) ");
	else uChroma_->setWindowTitle("uChroma v" + QString(UCHROMAREVISION) + " - " + inputFile_);

	lastTitleBarModificationStatus_ = modified_;
}

/*
 * Input File
 */

// Set name of input file
void UChromaSession::setInputFile(QString fileName)
{
	inputFile_ = fileName;

	updateTitleBar();
}

// Return name of input file
QString UChromaSession::inputFile()
{
	return inputFile_;
}

/*
 * Modification Flag
 */

// Set as modified
void UChromaSession::setAsModified()
{
	modified_ = true;

	updateTitleBar();
}

// Set as not modified
void UChromaSession::setAsNotModified()
{
	modified_ = false;

	updateTitleBar();
}

// Return status of flag
bool UChromaSession::isModified()
{
	return modified_;
}

/*
 * General Data
 */

// Available image export formats
const char* imageFormatFilters[UChromaSession::nImageFormats] = { "Windows Image (*.bmp)", "Joint Photographic Experts Group (*.jpg)", "Portable Network Graphics (*.png)", "Portable Pixmap (*.ppm)", "X11 Bitmap (*.xbm)", "X11 Pixmap (*.xpm)" };
const char* imageFormatExtensions[UChromaSession::nImageFormats] = { "bmp", "jpg", "png", "ppm", "xbm", "xpm" };
UChromaSession::ImageFormat UChromaSession::imageFormat(QString s)
{
	for (int n=0; n<UChromaSession::nImageFormats; ++n) if (s == imageFormatExtensions[n]) return (UChromaSession::ImageFormat) n;
	return UChromaSession::nImageFormats;
}
UChromaSession::ImageFormat UChromaSession::imageFormatFromFilter(QString s)
{
	for (int n=0; n<UChromaSession::nImageFormats; ++n) if (s == imageFormatFilters[n]) return (UChromaSession::ImageFormat) n;
	return UChromaSession::nImageFormats;
}
const char* UChromaSession::imageFormatFilter(UChromaSession::ImageFormat bf)
{
        return imageFormatFilters[bf];
}
const char* UChromaSession::imageFormatExtension(UChromaSession::ImageFormat bf)
{
        return imageFormatExtensions[bf];
}

// Set current image export filename
void UChromaSession::setImageExportFileName(QString fileName)
{
	imageExportFileName_ = fileName;
}

// Return current image export filename
QString UChromaSession::imageExportFileName()
{
	return imageExportFileName_;
}

// Set format for exported image
void UChromaSession::setImageExportFormat(UChromaSession::ImageFormat fmt)
{
	imageExportFormat_ = fmt;
}

// Return format for exported image
UChromaSession::ImageFormat UChromaSession::imageExportFormat()
{
	return imageExportFormat_;
}

// Set width of exported image
void UChromaSession::setImageExportWidth(int width)
{
	imageExportWidth_ = width;
}

// Return width of exported image
int UChromaSession::imageExportWidth()
{
	return imageExportWidth_;
}

// Set height of exported image
void UChromaSession::setImageExportHeight(int height)
{
	imageExportHeight_ = height;
}

// Return height of exported image
int UChromaSession::imageExportHeight()
{
	return imageExportHeight_;
}

// Set whether to maintain current aspect ratio on image export
void UChromaSession::setImageExportMaintainAspect(bool b)
{
	imageExportMaintainAspect_ = b;
}

// Return whether to maintain current aspect ratio on image export
bool UChromaSession::imageExportMaintainAspect()
{
	return imageExportMaintainAspect_;
}

// Set font filename to use for viewer
void UChromaSession::setViewerFontFileName(QString fileName)
{
	viewerFontFileName_ = fileName;
}

// Return font file to use for viewer
QString UChromaSession::viewerFontFileName()
{
	return viewerFontFileName_;
}

/*
 * Collections
 */

// Return unique name based on supplied baseName
QString UChromaSession::uniqueCollectionName(QString baseName)
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
void UChromaSession::startNewSession(bool createDefaults)
{
	// Clear collections
	collections_.clear();

	// Clear layout
	viewLayout_.clear();
	currentViewPane_ = NULL;

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
	setAsNotModified();
	inputFile_ = QString();
}

// Add new collection
Collection* UChromaSession::addCollection(QString name)
{
	// Add an empty collection
	currentCollection_ = collections_.add();

	// Set the title
	if (name.isEmpty()) currentCollection_->setName( uniqueCollectionName("Empty Collection") );
	else currentCollection_->setName(uniqueCollectionName(name));

	setAsModified();

	return currentCollection_;
}

// Remove existing collection
void UChromaSession::removeCollection(Collection* collection)
{
	// Check current Collection
	if (!Collection::objectValid(collection, "collection in UChromaSession::removeCollection()")) return;

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

			// Remove collection
			collection->parent()->removeFit(collection);
		}
		else
		{
			// Set new currentCollection_
			if (collection->next) currentCollection_ = collection->next;
			else currentCollection_ = collection->prev;
			if (currentCollection_ == NULL) currentCollection_ = collection->parent();

			// Remove collection
			collection->parent()->removeSlice(collection);
		}
	}

	setAsModified();
}

// Remove current collection
void UChromaSession::removeCurrentCollection()
{
	removeCollection(currentCollection_);
}

// Return number of collections
int UChromaSession::nCollections()
{
	return collections_.nItems();
}

// Move collection focus to next in list
void UChromaSession::focusNextCollection()
{
	currentCollection_ = currentCollection_->nextCollection(true);

	// Safety check...
	if (currentCollection_ == NULL) currentCollection_ = collections_.first();

	uChroma_->updateGUI();
}

// Move collection focus to previous in list
void UChromaSession::focusPreviousCollection()
{
	currentCollection_ = currentCollection_->previousCollection(false);

	// Safety check...
	if (currentCollection_ == NULL) currentCollection_ = collections_.last()->previousCollection(true);

	uChroma_->updateGUI();
}

// Set current Collection
void UChromaSession::setCurrentCollection(Collection* collection)
{
	if (!Collection::objectValid(collection, "collection in UChromaSession::setCurrentCollection()")) currentCollection_ = collection;
}

// Return nth collection in list
Collection* UChromaSession::collection(int index)
{
	return collections_[index];
}

// Locate collection
Collection* UChromaSession::locateCollection(QString locator)
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
Collection* UChromaSession::collections()
{
	return collections_.first();
}

// Return currently-selected Collection
Collection* UChromaSession::currentCollection()
{
	return currentCollection_;
}

// Return whether specified collection is the current collection
bool UChromaSession::isCurrentCollection(Collection* collection)
{
	return (currentCollection_ == collection);
}

/*
 * View Layout
 */

// Return curreent view layout
ViewLayout& UChromaSession::viewLayout()
{
	return viewLayout_;
}

// Set view layout
void UChromaSession::setViewLayout(ViewLayout& source)
{
	viewLayout_ = source;
	currentViewPane_ = viewLayout_.panes();
}

// Recalculate layout pane positions / sizes (after context resize etc.)
void UChromaSession::recalculateViewLayout(int contextWidth, int contextHeight)
{
	viewLayout_.recalculate(contextWidth, contextHeight);

	uChroma_->updateDisplay();
}

// Set current view pane to the one under the specified screen coordinates
bool UChromaSession::setCurrentViewPane(int layoutX, int layoutY)
{
	ViewPane* newCurrentPane = viewLayout_.paneAt(layoutX, layoutY);
	if (newCurrentPane == NULL) return false;

	currentViewPane_ = newCurrentPane;

	// Toolbars and subwindows now need updating
	uChroma_->updateToolBars();

	return true;
}

// Return current view pane
ViewPane* UChromaSession::currentViewPane()
{
	return currentViewPane_;
}
