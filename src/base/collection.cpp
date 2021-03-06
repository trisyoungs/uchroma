/*
	*** Data Collection
	*** src/base/collection.cpp
	Copyright T. Youngs 2013-2015

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

#include "base/collection.h"
#include "base/viewpane.h"
#include "base/lineparser.h"
#include "session/session.h"
#include "kernels/fit.h"
#include <limits>

// Static Members
template<class Collection> RefList<Collection,int> ObjectStore<Collection>::objects_;
template<class Collection> int ObjectStore<Collection>::objectCount_ = 0;

// Constructor
Collection::Collection() : ListItem<Collection>(), ObjectStore<Collection>(this, ObjectTypes::CollectionObject)
{
	// Set variable defaults
	dataSets_.clear();
	dataFileDirectory_ = getenv("PWD");
	name_ = "Empty Collection";
	dataMin_.zero();
	dataMax_.set(10.0, 10.0, 10.0);
	dataVersion_ = 0;

	// Transform
	transformMin_.zero();
	transformMax_.set(10.0, 10.0, 10.0);
	transformMinPositive_.set(0.1, 0.1, 0.1);
	transformMaxPositive_.set(10.0, 10.0, 10.0);
	transforms_[0].setEnabled(false);
	transforms_[1].setEnabled(false);
	transforms_[2].setEnabled(false);
	transforms_[0].setEquation("x");
	transforms_[1].setEquation("y");
	transforms_[2].setEquation("z");
	interpolate_.set(false, false, false);
	interpolateConstrained_.set(false, false, false);
	interpolationStep_.set(1.0, 1.0, 1.0);

	// Colours
	colourScale_.clear();
	colourSinglePoint_.set(0.0, QColor(0,0,0));
	colourRGBGradientAPoint_.set(0.0, QColor(255,255,255));
	colourRGBGradientBPoint_.set(1.0, QColor(0,0,255));
	colourHSVGradientAPoint_.set(0.0, QColor(255,0,0));
	colourHSVGradientBPoint_.set(1.0, QColor(100,40,255));
	customColourScale_.clear();
	colourSource_ = SingleColourSource;
	alphaControl_ = Collection::OwnAlpha;
	fixedAlpha_ = 0.5;
	colourVersion_ = 0;
	colourScaleGeneratedAt_ = -1;

	// Associated data
	parent_ = NULL;
	type_ = Collection::MasterCollection;
	currentSlice_ = NULL;
	fitKernel_ = NULL;

	// Update
	limitsAndTransformsVersion_ = -1;

	// Display
	visible_ = true;
	displayData_.clear();
	displayDataGeneratedAt_ = -1;
	displayStyle_ = Collection::LineXYStyle;
	displaySurfaceShininess_ = 128.0;
	displayStyleVersion_ = 0;
}

// Destructor
Collection::~Collection()
{
	// Delete sub-collections from the bottom-up (in order to create the undo state correctly)
	while (fits_.last()) removeFit(fits_.last());
	while (slices_.last()) removeSlice(slices_.last());

	if (fitKernel_) delete fitKernel_;
}

// Copy constructor
Collection::Collection(const Collection& source) : ObjectStore<Collection>(NULL, ObjectTypes::CollectionObject)
{
	(*this) = source;
}

// Assignment operator
void Collection::operator=(const Collection& source)
{
	// Basic Data
	name_ = source.name_;
	dataSets_ = source.dataSets_;
	dataFileDirectory_ = source.dataFileDirectory_;
	dataMin_ = source.dataMin_;
	dataMax_ = source.dataMax_;
	dataVersion_ = 0;

	// Transforms
	transformMin_ = source.transformMin_;
	transformMax_ = source.transformMax_;
	transformMinPositive_ = source.transformMinPositive_;
	transformMaxPositive_ = source.transformMaxPositive_;
	transforms_[0] = source.transforms_[0];
	transforms_[1] = source.transforms_[1];
	transforms_[2] = source.transforms_[2];
	interpolate_ = source.interpolate_;
	interpolateConstrained_ = source.interpolateConstrained_;
	interpolationStep_ = source.interpolationStep_;
	limitsAndTransformsVersion_ = source.limitsAndTransformsVersion_;

	// Colours
	colourSource_ = source.colourSource_;
	colourSinglePoint_ = source.colourSinglePoint_;
	colourRGBGradientAPoint_ = source.colourRGBGradientAPoint_;
	colourRGBGradientBPoint_ = source.colourRGBGradientBPoint_;
	colourHSVGradientAPoint_ = source.colourHSVGradientAPoint_;
	colourHSVGradientBPoint_ = source.colourHSVGradientBPoint_;
	colourScale_ = source.colourScale_;
	customColourScale_ = source.customColourScale_;
	alphaControl_ = source.alphaControl_;
	fixedAlpha_ = source.fixedAlpha_;
	colourVersion_ = 0;
	colourScaleGeneratedAt_ = -1;

	// Associated data
	parent_ = source.parent_;
	type_ = source.type_;
	fits_.clear();
	for (Collection* fit = source.fits_.first(); fit != NULL; fit = fit->next)
	{
		Collection* newFit = fits_.add();
		(*newFit) = (*fit);
	}
	slices_.clear();
	for (Collection* slice = source.slices_.first(); slice != NULL; slice = slice->next)
	{
		Collection* newSlice = slices_.add();
		(*newSlice) = (*slice);
	}
	if (currentSlice_) delete currentSlice_;
	currentSlice_ = NULL;
	if (source.fitKernel_)
	{
		if (!fitKernel_) fitKernel_ = new FitKernel;
		(*fitKernel_) = (*source.fitKernel_);
	}
	else
	{
		if (fitKernel_) delete fitKernel_;
		fitKernel_ = NULL;
	}

	// Update
	limitsAndTransformsVersion_ = -1;

	// Display
	visible_ = source.visible_;
	displayData_.clear();
	displayDataGeneratedAt_ = -1;
	displayStyle_ = source.displayStyle_;
	displaySurfaceShininess_ = source.displaySurfaceShininess_;
	displayLineStyle_ = source.displayLineStyle_;
	displayStyleVersion_ = 0;
}

// Prepare for 

/*
 * Data
 */

// Set name
void Collection::setName(QString name)
{
	UChromaSession::addEditState(objectInfo(), EditState::CollectionTitleQuantity, name_, name);

	name_ = name;
}

// Return name
QString Collection::name()
{
	return name_;
}

// Return collection locator
QString Collection::locator(QString currentPath)
{
	// Check parent - if it is NULL then we are at the 'top' and can start setting the path
	if (!parent_) return name_;

	// We are in the 'middle' of the chain, so get the parent's contribution to the path first...
	QString result = parent_->locator(currentPath);

	// Add our info on to the path...
	result += "//" + name_;

	return result;
}

// Add dataset
DataSet* Collection::addDataSet()
{
	// Create new dataset
	DataSet* dataSet = dataSets_.add();
	dataSet->setParent(this);

	++dataVersion_;

	UChromaSession::setAsModified();

	return dataSet;
}

// Add dataset at specified z
DataSet* Collection::addDataSet(double z)
{
	DataSet* dataSet = dataSets_.add();
	dataSet->setParent(this);
	setDataSetZ(dataSet, z);

	UChromaSession::setAsModified();

	return dataSet;
}

// Add dataset, copying from supplied DataSet
void Collection::addDataSet(DataSet* source)
{
	// Create new dataset
	DataSet* dataSet = addDataSet();

	// Copy existing dataset data, overwriting parent just to make sure
	(*dataSet) = (*source);
	dataSet->setParent(this);

	++dataVersion_;

	UChromaSession::setAsModified();
}

// Add dataset, copying from supplied DisplayDataSet
void Collection::addDataSet(const Array<double>& abscissa, DisplayDataSet* source, bool pruneEmpty)
{
	// Create new dataset
	DataSet* dataSet = addDataSet();

	// Grab references
	const Array<double>& y = source->y();
	const Array<DisplayDataSet::DataPointType>& yType = source->yType();

	// Check sizes
	if (abscissa.nItems() != y.nItems())
	{
		msg.print("Collection::addDataSet() : Abscissa and y arrays are of different sizes.\n");
		return;
	}

	// Loop over points, adding points as we go (and pruning truly empty points if necessary)
	for (int n=0; n<abscissa.nItems(); ++n)
	{
		if (pruneEmpty && (yType.value(n) == DisplayDataSet::NoPoint)) continue;
		dataSet->addPoint(abscissa.value(n), y.value(n));
	}

	++dataVersion_;

	UChromaSession::setAsModified();
}

// Copy datasets from specified source collection
void Collection::copyDataSets(Collection* source)
{
	for (DataSet* dataSet = source->dataSets(); dataSet != NULL; dataSet = dataSet->next) addDataSet(dataSet);
}

// Return number of datasets
int Collection::nDataSets()
{
	return dataSets_.nItems();
}

// Remove dataset
void Collection::removeDataSet(DataSet* dataSet)
{
	dataSets_.remove(dataSet);

	++dataVersion_;

	UChromaSession::setAsModified();
}

// Set z value of specified dataset
void Collection::setDataSetZ(DataSet* target, double z)
{
	bool minBad = true, maxBad = true;
	int dummy = 0;

	// Check that this DataSet is owned by the collection
	if (!dataSets_.contains(target))
	{
		msg.print("Internal Error : Tried to set the Z value of a dataset using the wrong collection.\n");
		return;
	}
	target->setZ(z);

	// Adjust position of the dataset in the list if necessary
	do
	{
		// Shift item if necessary
		if (target->prev && (target->prev->data().z() > target->data().z()))
		{
			// Shift specified target up the list
			dataSets_.shiftUp(target);
			minBad = (target->prev ? (target->prev->data().z() > target->data().z()) : false);
		}
		else minBad = false;
		if (target->next && (target->next->data().z() < target->data().z()))
		{
			// Move specified target down the list
			dataSets_.shiftDown(target);
			maxBad = (target->next ? (target->next->data().z() < target->data().z()) : false);
		}
		else maxBad = false;
		if (++dummy == 10) break;
		
	} while (minBad || maxBad);

	++dataVersion_;

	UChromaSession::setAsModified();
}

// Set data for specified dataste (from source DataSet)
void Collection::setDataSetData(DataSet* target, DataSet& source)
{
	// Check that this DataSet is owned by the collection
	if (!dataSets_.contains(target))
	{
		msg.print("Internal Error : Tried to set the data of a dataset using the wrong collection.\n");
		return;
	}

	(*target) = source;

	++dataVersion_;

	UChromaSession::setAsModified();
}

// Return first dataset in list
DataSet* Collection::dataSets() const
{
	return dataSets_.first();
}

// Return named dataset
DataSet* Collection::dataSet(QString name)
{
	for (DataSet* dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next) if (dataSet->name() == name) return dataSet;
	return NULL;
}

// Return last dataset in list
DataSet* Collection::lastDataSet()
{
	return dataSets_.last();
}

// Return nth dataset in list
DataSet* Collection::dataSet(int index)
{
	return dataSets_[index];
}

// Return index of specified dataset
int Collection::dataSetIndex(const char* name)
{
	int index = 0;
	for (DataSet* dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next, ++index) if (dataSet->name() == name) return index;
	return -1;
}

// Return unique name based on supplied basename
QString Collection::uniqueDataSetName(QString baseName)
{
	QString testName = baseName, suffix;
	int index = 0;
	do
	{
		// Add on suffix (if index > 0)
		if (index > 0) testName = baseName + " ("+QString::number(index)+")";
		++index;
	} while (dataSet(testName));

	return testName;
}

// Return number of slices with no data present
int Collection::nEmptyDataSets()
{
	int count = 0;
	for (DataSet* dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next) if (dataSet->data().nPoints() < 2) ++count;
	return count;
}

// Clear dataset data from collection
void Collection::clearDataSets()
{
	dataSets_.clear();
	displayData_.clear();

	++dataVersion_;

	UChromaSession::setAsModified();
}

// Return total number of points across all datasets
int Collection::nDataPoints()
{
	int nPoints = 0;
	for (DataSet* dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next) nPoints += dataSet->data().nPoints();
	return nPoints;
}

// Set root directory for datafiles
void Collection::setDataFileDirectory(QDir directory)
{
	dataFileDirectory_ = directory;

	UChromaSession::setAsModified();
}

// Return root directory for datafiles
QDir Collection::dataFileDirectory()
{
	return dataFileDirectory_;
}

// Append dataset to collection
bool Collection::appendDataSet(QString fileName)
{
	// Get fileinfo to extract pure filename part
	QFileInfo fileInfo(fileName);
	
	double z = 0.0;
	if (nDataSets() > 0) z = lastDataSet()->data().z() + 1.0;

	DataSet* dataSet = addDataSet(z);
	dataSet->setName(fileInfo.fileName());
	dataSet->setDataSource(DataSet::FileSource);
	dataSet->setSourceFileName(dataFileDirectory().relativeFilePath(fileName));

	return loadDataSet(dataSet);
}

// Load data for specified dataset index
bool Collection::loadDataSet(DataSet* dataSet)
{
	if (!dataSet) return false;

	UChromaSession::setAsModified();

	return dataSet->loadData(dataFileDirectory_);
}

// Reload data for all slices
int Collection::loadAllDataSets()
{
	int nFailed = 0;
	for (DataSet* dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next) if (!dataSet->loadData(dataFileDirectory_)) ++nFailed;

	UChromaSession::setAsModified();

	return nFailed;
}

// Return data minima, calculating if necessary
Vec3<double> Collection::dataMin()
{
	// Make sure limits and transform are up to date
	updateLimitsAndTransforms();

	return dataMin_;
}

// Return data maxima, calculating if necessary
Vec3<double> Collection::dataMax()
{
	// Make sure limits and transform are up to date
	updateLimitsAndTransforms();

	return dataMax_;
}

// Increase data version (i.e. notify that data has been changed)
void Collection::notifyDataChanged()
{
	++dataVersion_;
}

// Return version counter for changes to data
int Collection::dataVersion()
{
	return dataVersion_;
}

/*
 * Transforms
 */

// Return transformed data minima, calculating if necessary
Vec3<double> Collection::transformMin()
{
	// Make sure limits and transform are up to date
	updateLimitsAndTransforms();

	return transformMin_;
}

// Return transformed data maxima, calculating if necessary
Vec3<double> Collection::transformMax()
{
	// Make sure limits and transform are up to date
	updateLimitsAndTransforms();

	return transformMax_;
}

// Return transformed positive data minima, calculating if necessary
Vec3<double> Collection::transformMinPositive()
{
	// Make sure limits and transform are up to date
	updateLimitsAndTransforms();

	return transformMinPositive_;
}

// Return transformed positive data maxima, calculating if necessary
Vec3<double> Collection::transformMaxPositive()
{
	// Make sure limits and transform are up to date
	updateLimitsAndTransforms();

	return transformMaxPositive_;
}

// Set transform equation for data
void Collection::setTransformEquation(int axis, QString transformEquation)
{
	transforms_[axis].setEquation(transformEquation);

	// Make sure limits and transform are up to date
	if (transforms_[axis].enabled())
	{
		++dataVersion_;
		updateLimitsAndTransforms();
	}

	UChromaSession::setAsModified();
}

// Return transform equation for data
QString Collection::transformEquation(int axis)
{
	return transforms_[axis].text();
}

// Return whether specified transform equation is valid
bool Collection::transformEquationValid(int axis)
{
	return transforms_[axis].valid();
}

// Set whether specified transform is enabled
void Collection::setTransformEnabled(int axis, bool enabled)
{
	transforms_[axis].setEnabled(enabled);

	++dataVersion_;

	// Make sure limits and transform are up to date
	updateLimitsAndTransforms();

	UChromaSession::setAsModified();
}

// Return whether specified transform is enabled
bool Collection::transformEnabled(int axis)
{
	return transforms_[axis].enabled();
}

// Set whether interpolation is enabled
void Collection::setInterpolate(int axis, bool enabled)
{
	interpolate_[axis] = enabled;

	UChromaSession::setAsModified();
}

// Return whether interpolation is enabled
bool Collection::interpolate(int axis)
{
	return interpolate_[axis];
}

// Set whether interpolation is constrained
void Collection::setInterpolateConstrained(int axis, bool enabled)
{
	interpolateConstrained_[axis] = enabled;

	UChromaSession::setAsModified();
}

// Return whether interpolation is constrained
bool Collection::interpolateConstrained(int axis)
{
	return interpolateConstrained_[axis];
}

// Set interpolation step size
void Collection::setInterpolationStep(int axis, double step)
{
	interpolationStep_[axis] = step;

	UChromaSession::setAsModified();
}

// Return interpolation step size
double Collection::interpolationStep(int axis)
{
	return interpolationStep_[axis];
}

/*
 * Data Operations
 */

// Add to specified axis value`
void Collection::addConstantValue(int axis, double value)
{
	// Loop over datasets
	for (DataSet* dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next) dataSet->addConstantValue(axis, value);

	++dataVersion_;

	updateLimitsAndTransforms();
}

/*
 * Associated Data
 */

// Return axis bin value of closest point to supplied value
int Collection::closestBin(int axis, double value)
{
	if (dataSets_.nItems() == 0) return -1;

	// Make sure that display data is up-to-date
	updateDisplayData();

	if (axis == 0)
	{
		// Check display data abscissa for closest value
		int midIndex, loIndex = 0, hiIndex = displayAbscissa_.nItems() - 1;
		if (value < displayAbscissa_.value(0)) return 0;
		if (value > displayAbscissa_.value(hiIndex)) return hiIndex;
		// Binary... chop!
		while ((hiIndex - loIndex) > 1)
		{
			midIndex = (hiIndex + loIndex) / 2;
			if (displayAbscissa_.value(midIndex) <= value) loIndex = midIndex;
			else hiIndex = midIndex;
		}
		if (fabs(displayAbscissa_.value(loIndex) - value) < fabs(displayAbscissa_.value(hiIndex) - value)) return loIndex;
		else return hiIndex;
	}
	else if (axis == 1)
	{
		// ???
	}
	else if (axis == 2)
	{
		// Check z-values
		int closest = 0, n = 0;
		double delta, closestDelta = fabs(displayData_.first()->z() - value);
		for (DisplayDataSet* displayDataSet = displayData_.first()->next; displayDataSet != NULL; displayDataSet = displayDataSet->next)
		{
			delta = fabs(displayDataSet->z() - value);
			++n;
			if (delta < closestDelta)
			{
				closest = n;
				closestDelta = delta;
			}
		}
		return closest;
	}

	return -1;
}

// Get slice at specified axis and bin
void Collection::getSlice(int axis, int bin)
{
	// Call currentSlice() so we generate a collection if one does not yet exist
	currentSlice();
	currentSlice_->clearDataSets();

	// Are supplied bin and axis valid?
	if ((bin == -1) || (axis == -1)) return;
	else if (axis == 0)
	{
		DataSet* newDataSet = currentSlice_->addDataSet();

		// Slice at fixed X - the bin will be an index from the displayAbscissa_ / displayData_ data.
		for (DisplayDataSet* displayDataSet = displayData_.first(); displayDataSet != NULL; displayDataSet = displayDataSet->next) newDataSet->addPoint(displayDataSet->z(), displayDataSet->y().value(bin));
		currentSlice_->setName("X = " + QString::number(displayAbscissa_.value(bin)));
	}
	else if (axis == 1)
	{
		// TODO - Generate contour map?
	}
	else if (axis == 2)
	{
		// Slice through Z
		DisplayDataSet* displayDataSet = displayData_[bin];
		if (displayDataSet)
		{
			currentSlice_->addDataSet(displayAbscissa_, displayDataSet);
			currentSlice_->setName("Z = " + QString::number(displayDataSet->z()));
		}
	}
}

// Locate collection using parts specified
Collection* Collection::locateCollection(const QStringList& parts, int offset)
{
	// If we don't match the first part (including offset) in the list, return now...
	if (name_ != parts.at(offset)) return NULL;

	// If this is the last part in the list, then we are the match. If not, check fit and slice data
	++offset;
	if (parts.size() == offset) return this;

	// Check fit data
	for (Collection* fit = fits_.first(); fit != NULL; fit = fit->next)
	{
		Collection* result = fit->locateCollection(parts, offset);
		if (result) return result;
	}

	// Check extracted data
	for (Collection* extract = slices_.first(); extract != NULL; extract = extract->next)
	{
		Collection* result = extract->locateCollection(parts, offset);
		if (result) return result;
	}

	// No match here, so return NULL
	return NULL;
}

// Return next logical collection in lists
Collection* Collection::nextCollection(bool descend)
{
	// If the current collection contains additional data, return that
	if (descend && fits_.first()) return fits_.first();
	else if (descend && slices_.first()) return slices_.first();

	// If there is a next item in the current collection's list (whatever it is) move on to that
	if (next) return next;

	// No additional data, and we are at the end of the current list, so check parent
	if (type_ == Collection::MasterCollection) return NULL;
	else if (type_ == Collection::FitCollection)
	{
		// No more fitData - check extracted data in parent. If nothing there, then we must ascend to the next item in the parent's list
		if (parent_->slices_.nItems() != 0) return parent_->slices();
		else return parent_->nextCollection(false);
	}
	else if (type_ == Collection::ExtractedCollection) return parent_->nextCollection(false);

	return NULL;
}

// Return previous logical collection in lists
Collection* Collection::previousCollection(bool descend)
{
	// If the current collection contains additional data, return that
	if (descend && slices_.last()) return slices_.last()->previousCollection(true);
	else if (descend && fits_.last()) return fits_.last()->previousCollection(true);
	else if (descend) return this;

	// If there is a previous item in the current collection's list (whatever it is) move on to that
	if (prev) return prev;

	// No additional data, and we are at the beginning of the current list, so check parent
	if (type_ == Collection::MasterCollection) return NULL;
	else if (type_ == Collection::ExtractedCollection)
	{
		// No more extractedData - check fit data in parent. If nothing there, then we must ascend to the previous item in the parent's list
		if (parent_->fits_.nItems() != 0) return parent_->fits_.last()->previousCollection(true);
		else return parent_->previousCollection(true);
	}
	else if (type_ == Collection::FitCollection) return parent_;

	return NULL;
}

// Set parent Collection
void Collection::setParent(Collection* parent)
{
	parent_ = parent;
}

// Parent Collection
Collection* Collection::parent()
{
	return parent_;
}

// Return type of this collection
Collection::CollectionType Collection::type()
{
	return type_;
}

// Return icon string reflecting this Collection's type / status
QString Collection::iconString(bool isUsed)
{
	QString iconName;

	// Get base icon name
	if (type_ == Collection::MasterCollection) iconName = ":/uchroma/icons/collection_collection";
	else if (type_ == Collection::FitCollection) iconName = ":/uchroma/icons/collection_fit";
	else if (type_ == Collection::ExtractedCollection) iconName = ":/uchroma/icons/collection_extracted";

	// If display pane is invalid, tweak icon name
	if (!isUsed) iconName += "_nopane";
	iconName += ".svg";
	
	return iconName;
}

// Return locally-unique fit name based on basename provided
QString Collection::uniqueFitName(QString baseName)
{
	QString testName = baseName, suffix;
	int index = 0;
	do
	{
		// Add on suffix (if index > 0)
		if (index > 0) testName = baseName + " ("+QString::number(index)+")";
		++index;
	} while (fit(testName));

	return testName;
}

// Add fit to Collection
Collection* Collection::addFit(QString name, int position)
{
	Collection* newFit = fits_.add();
	newFit->type_ = Collection::FitCollection;
	newFit->setParent(this);

	// Create editstate data
	if (UChromaSession::addEditState(newFit->objectInfo(), EditState::CollectionAddQuantity))
	{
		UChromaSession::addEditStateData(true, "collection", newFit);
		UChromaSession::addEditStateData(true, "locator", newFit->locator());
		UChromaSession::addEditStateData(true, "position", newFit->listIndex());
	}

	newFit->addFitKernel();

	// Set name, and add fit kernel
	newFit->setName(uniqueFitName(name));

	// Send a signal out
	UChromaSignal::send(UChromaSignal::CollectionCreatedSignal, newFit);

	UChromaSession::setAsModified();

	return newFit;
}

// Remove specified fit from list
void Collection::removeFit(Collection* collection)
{
	// Send a signal out before we finalise deletion
	UChromaSignal::send(UChromaSignal::CollectionDeletedSignal, collection);

	// Create editstate data
	if (UChromaSession::addEditState(collection->objectInfo(), EditState::CollectionRemoveQuantity))
	{
		UChromaSession::addEditStateData(false, "collection", collection);
		UChromaSession::addEditStateData(false, "locator", collection->locator());
		UChromaSession::addEditStateData(false, "position", collection->listIndex());
	}

	fits_.remove(collection);

	UChromaSession::setAsModified();
}

// Return fits in Collection
Collection* Collection::fits()
{
	return fits_.first();
}
	
// Return fit with name specified
Collection* Collection::fit(QString name)
{
	for (Collection* fit = fits_.first(); fit != NULL; fit = fit->next) if (fit->name() == name) return fit;
	return NULL;
}

// Return locally-unique slice name based on basename provided
QString Collection::uniqueSliceName(QString baseName)
{
	QString testName = baseName, suffix;
	int index = 0;
	do
	{
		// Add on suffix (if index > 0)
		if (index > 0) testName = baseName + " ("+QString::number(index)+")";
		++index;
	} while (slice(testName));

	return testName;
}

// Add slice to Collection
Collection* Collection::addSlice(QString name, int position)
{
	Collection* newSlice = slices_.addAt(position);
	newSlice->type_ = Collection::ExtractedCollection;
	newSlice->setParent(this);

	// Create editstate data
	if (UChromaSession::addEditState(newSlice->objectInfo(), EditState::CollectionAddQuantity))
	{
		UChromaSession::addEditStateData(true, "collection", newSlice);
		UChromaSession::addEditStateData(true, "locator", newSlice->locator());
		UChromaSession::addEditStateData(true, "position", newSlice->listIndex());
	}

	// Set name
	newSlice->setName(uniqueSliceName(name));

	// Send a signal out
	UChromaSignal::send(UChromaSignal::CollectionCreatedSignal, newSlice);

	UChromaSession::setAsModified();

	return newSlice;
}

// Remove specified slice from list
void Collection::removeSlice(Collection* collection)
{
	// Send a signal out before we finalise deletion
	UChromaSignal::send(UChromaSignal::CollectionDeletedSignal, collection);

	// Create editstate data
	if (UChromaSession::addEditState(collection->objectInfo(), EditState::CollectionRemoveQuantity))
	{
		UChromaSession::addEditStateData(false, "collection", collection);
		UChromaSession::addEditStateData(false, "locator", collection->locator());
		UChromaSession::addEditStateData(false, "position", collection->listIndex());
	}

	slices_.remove(collection);

	UChromaSession::setAsModified();
}

// Return slices in Collection
Collection* Collection::slices()
{
	return slices_.first();
}

// Return slice with name specified
Collection* Collection::slice(QString name)
{
	for (Collection* slice = slices_.first(); slice != NULL; slice = slice->next) if (slice->name() == name) return slice;
	return NULL;
}

// Update current slice based on specified axis and value
void Collection::updateCurrentSlice(int axis, double axisValue)
{
	getSlice(axis, closestBin(axis, axisValue));

	UChromaSignal::send(UChromaSignal::CollectionSliceChangedSignal, this);
}

// Extract current slice based on specified axis and value
void Collection::extractCurrentSlice(int axis, double axisValue)
{
	getSlice(axis, closestBin(axis, axisValue));

	Collection* newSlice = addSlice(currentSlice_->name());
	newSlice->addDataSet(currentSlice_->dataSets());

	UChromaSignal::send(UChromaSignal::CollectionSliceExtractedSignal, newSlice);
}

// Return current slice
Collection* Collection::currentSlice()
{
	// Create the currentSlice_ collection if it doesn't already exist
	if (currentSlice_ == NULL)
	{
		currentSlice_ = new Collection;
		currentSlice_->parent_ = this;
		currentSlice_->type_ = Collection::CurrentSliceCollection;
	}

	return currentSlice_;
}

// Add FitKernel, if one does not exist
void Collection::addFitKernel()
{
	if (fitKernel_)	msg.print("Warning: Attempted to add a new FitKernel to collection '%s', but one already exists.\n", qPrintable(name_));
	else fitKernel_ = new FitKernel;
	fitKernel_->setSourceCollection(parent_);
	fitKernel_->setDestinationCollection(this);

	UChromaSession::setAsModified();
}

// Return FitKernel
FitKernel* Collection::fitKernel()
{
	return fitKernel_;
}

/*
 * Update
 */

// Update data limits and transform data
void Collection::updateLimitsAndTransforms()
{
	if (dataVersion_ == limitsAndTransformsVersion_) return;

	dataMin_ = 0.0;
	dataMax_ = 0.0;
	if (dataSets_.nItems() > 0)
	{
		// Grab first dataset and set initial values
		DataSet* dataSet = dataSets_.first();
		dataMin_.set(dataSet->data().xMin(), dataSet->data().yMin(), dataSet->data().z());
		dataMax_.set(dataSet->data().xMax(), dataSet->data().yMax(), dataSet->data().z());
		double mmin, mmax;
		for (dataSet = dataSet->next; dataSet != NULL; dataSet = dataSet->next)
		{
// 			printf("Z = %f\n", dataSet->data().z());
			mmin = dataSet->data().xMin();
			mmax = dataSet->data().xMax();
			if (mmin < dataMin_.x) dataMin_.x = mmin;
			if (mmax > dataMax_.x) dataMax_.x = mmax;
			mmin = dataSet->data().yMin();
			mmax = dataSet->data().yMax();
			if (mmin < dataMin_.y) dataMin_.y = mmin;
			if (mmax > dataMax_.y) dataMax_.y = mmax;
			if (dataSet->data().z() < dataMin_.z) dataMin_.z = dataSet->data().z();
			else if (dataSet->data().z() > dataMax_.z) dataMax_.z = dataSet->data().z();
		}
	}

	// Loop over dataSets_ list, updating transforms as we go
	for (DataSet* dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next) dataSet->transform(transforms_[0], transforms_[1], transforms_[2]);

	transformMin_ = 0.0;
	transformMax_ = 0.0;
	transformMinPositive_ = 0.1;
	transformMaxPositive_ = -1.0;
	if (dataSets_.nItems() == 0) return;
	
	// Grab first dataset and set initial values
	DataSet* dataSet = dataSets_.first();
	transformMin_.set(dataSet->transformedData().xMin(), dataSet->transformedData().yMin(), dataSet->transformedData().z());
	transformMax_.set(dataSet->transformedData().xMax(), dataSet->transformedData().yMax(), dataSet->transformedData().z());
	double mmin, mmax;
	for (dataSet = dataSet->next; dataSet != NULL; dataSet = dataSet->next)
	{
		mmin = dataSet->transformedData().xMin();
		mmax = dataSet->transformedData().xMax();
		if (mmin < transformMin_.x) transformMin_.x = mmin;
		if (mmax > transformMax_.x) transformMax_.x = mmax;
		mmin = dataSet->transformedData().yMin();
		mmax = dataSet->transformedData().yMax();
		if (mmin < transformMin_.y) transformMin_.y = mmin;
		if (mmax > transformMax_.y) transformMax_.y = mmax;
		if (dataSet->transformedData().z() < transformMin_.z) transformMin_.z = dataSet->transformedData().z();
		else if (dataSet->transformedData().z() > transformMax_.z) transformMax_.z = dataSet->transformedData().z();
	}

	// Now determine minimum positive limits
	for (dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next)
	{
		// Loop over XY points in data, searching for first positive, non-zero value
		Data2D& data = dataSet->transformedData();
		for (int n=0; n<data.nPoints(); ++n)
		{
			// X
			if (data.x(n) > 0.0)
			{
				if (data.x(n) < transformMinPositive_.x) transformMinPositive_.x = data.x(n);
				if (data.x(n) > transformMaxPositive_.x) transformMaxPositive_.x = data.x(n);
			}
			// Y
			if (data.y(n) > 0.0)
			{
				if (data.y(n) < transformMinPositive_.y) transformMinPositive_.y = data.y(n);
				if (data.y(n) > transformMaxPositive_.y) transformMaxPositive_.y = data.y(n);
			}
		}
		
		// Z
		if (data.z() > 0.0)
		{
			if (data.z() < transformMinPositive_.z) transformMinPositive_.z = data.z();
			if (data.z() > transformMaxPositive_.z) transformMaxPositive_.z = data.z();
		}
	}

	// Check maximum positive values (since all datapoints might have been negative
	if (transformMaxPositive_.x < 0.0) transformMaxPositive_.x = 1.0;
	if (transformMaxPositive_.y < 0.0) transformMaxPositive_.y = 1.0;
	if (transformMaxPositive_.z < 0.0) transformMaxPositive_.z = 1.0;

	// Update version
	limitsAndTransformsVersion_ = dataVersion_;
}

/*
 * Colours
 */

// ColourSource Keywords
const char* ColourSourceKeywords[] = { "SingleColour", "RGBGradient", "HSVGradient", "CustomGradient" };

// Convert text string to ColourSource
Collection::ColourSource Collection::colourSource(QString s)
{
	for (int n=0; n<Collection::nColourSources; ++n) if (s == ColourSourceKeywords[n]) return (Collection::ColourSource) n;
	return Collection::nColourSources;
}

// Convert ColourSource to text string
const char* Collection::colourSource(ColourSource cs)
{
	return ColourSourceKeywords[cs];
}

// AlphaControl keywords
const char* AlphaControlKeywords[] = { "OwnAlpha", "FixedAlpha" };

// Convert text string to AlphaControl
Collection::AlphaControl Collection::alphaControl(QString s)
{
	for (int n=0; n<Collection::nAlphaControls; ++n) if (s == AlphaControlKeywords[n]) return (Collection::AlphaControl) n;
	return Collection::nAlphaControls;
}

// Convert AlphaControl to text string
const char* Collection::alphaControl(Collection::AlphaControl as)
{
	return AlphaControlKeywords[as];
}

// Update colour scale
void Collection::updateColourScale()
{
	if (colourVersion_ == colourScaleGeneratedAt_) return;

	colourScale_.clear();
	if (colourSource_ == Collection::SingleColourSource) colourScale_.addPoint(0.0, colourSinglePoint_.colour());
	else if (colourSource_ == Collection::RGBGradientSource)
	{
		colourScale_.setUseHSV(false);
		colourScale_.addPoint(colourRGBGradientAPoint_.value(), colourRGBGradientAPoint_.colour());
		colourScale_.addPoint(colourRGBGradientBPoint_.value(), colourRGBGradientBPoint_.colour());
	}
	else if (colourSource_ == Collection::HSVGradientSource)
	{
		colourScale_.setUseHSV(true);
		colourScale_.addPoint(colourHSVGradientAPoint_.value(), colourHSVGradientAPoint_.colour());
		colourScale_.addPoint(colourHSVGradientBPoint_.value(), colourHSVGradientBPoint_.colour());
	}
	else if (colourSource_ == Collection::CustomGradientSource) colourScale_ = customColourScale_;

	// Set alpha value for all points if alphaControl_ == FixedAlpha
	if (alphaControl_ == Collection::FixedAlpha) colourScale_.setAllAlpha(fixedAlpha_);

	colourScaleGeneratedAt_ = colourVersion_;
}

// Set colourscale source to use
void Collection::setColourSource(ColourSource source)
{
	colourSource_ = source;

	++colourVersion_;
}

// Return colourscale source to use
Collection::ColourSource Collection::colourSource()
{
	return colourSource_;
}

// Set colourscale point colour
void Collection::setColourScalePoint(ColourSource source, QColor colour, double value, int index)
{
	switch (source)
	{
		case (Collection::SingleColourSource):
			colourSinglePoint_.setColour(colour);
			break;
		case (Collection::RGBGradientSource):
			if (index == 0) colourRGBGradientAPoint_.set(value, colour);
			else colourRGBGradientBPoint_.set(value, colour);
			break;
		case (Collection::HSVGradientSource):
			if (index == 0) colourHSVGradientAPoint_.set(value, colour);
			else colourHSVGradientBPoint_.set(value, colour);
			break;
		case (Collection::CustomGradientSource):
			customColourScale_.setPoint(index, value, colour);
			break;
		default:
			msg.print("Unhandled ColourSoruce in Collection::setColourScalePoint().\n");
	}

	// Update colourscale?
	if (source == colourSource_) ++colourVersion_;
}

// Return colourscale point specified
const ColourScalePoint* Collection::colourScalePoint(ColourSource source, int index)
{
	switch (source)
	{
		case (Collection::SingleColourSource):
			return &colourSinglePoint_;
			break;
		case (Collection::RGBGradientSource):
			return (index == 0 ? &colourRGBGradientAPoint_ : &colourRGBGradientBPoint_);
			break;
		case (Collection::HSVGradientSource):
			return (index == 0 ? &colourHSVGradientAPoint_ : &colourHSVGradientBPoint_);
			break;
		case (Collection::CustomGradientSource):
			return customColourScale_.point(index);
			break;
		default:
			msg.print("Unhandled ColourSoruce in Collection::colourScalePoint().\n");
	}

	return NULL;
}

// Return colour of point specified
QColor Collection::colourScalePointColour(ColourSource source, int index)
{
	const ColourScalePoint* point = colourScalePoint(source, index);
	if (point) return point->colour();
	else return Qt::black;
}

// Return value of point specified
double Collection::colourScalePointValue(ColourSource source, int index)
{
	const ColourScalePoint* point = colourScalePoint(source, index);
	if (point) return point->value();
	else return 0.0;
}

// Add point to custom colour scale
void Collection::addCustomColourScalePoint()
{
	customColourScale_.addPoint(customColourScale_.lastPoint() ? customColourScale_.lastPoint()->value() + 1.0 : 0.0, Qt::white);

	// Update colourscale?
	if (colourSource_ == Collection::CustomGradientSource) ++colourVersion_;
}

// Add point to custom colourscale
void Collection::addCustomColourScalePoint(double value, QColor colour)
{
	customColourScale_.addPoint(value, colour);
}

// Return number of custom colourscale points
int Collection::nCustomColourScalePoints()
{
	return customColourScale_.nPoints();
}

// Return first custom colourscale point in list
ColourScalePoint* Collection::customColourScalePoints()
{
	return customColourScale_.firstPoint();
}

// Return custom colourscale point with index specified
ColourScalePoint* Collection::customColourScalePoint(int id)
{
	return customColourScale_.point(id);
}

// Remove specified colourscale point
void Collection::removeCustomColourScalePoint(ColourScalePoint* point)
{
	customColourScale_.removePoint(point);

	// Update colourscale?
	if (colourSource_ == Collection::CustomGradientSource) ++colourVersion_;
}

// Set alpha control
void Collection::setAlphaControl(Collection::AlphaControl alpha)
{

	alphaControl_ = alpha;

	++colourVersion_;
}

// Return current alpha control
Collection::AlphaControl Collection::alphaControl()
{
	return alphaControl_;
}

// Set fixed alpha value
void Collection::setFixedAlpha(double alpha)
{
	fixedAlpha_ = alpha;

	++colourVersion_;
}

// Return fixed alpha value
double Collection::fixedAlpha()
{
	return fixedAlpha_;
}

// Return current colourscale
const ColourScale& Collection::colourScale()
{
	// Does the colourscale need updating first?
	updateColourScale();
	
	return colourScale_;
}

// Return colour version
int Collection::colourVersion()
{
	return colourVersion_;
}

/*
 * Display
 */

// Suface Style Keywords
const char* SurfaceStyleKeywords[] = { "LineXY", "LineZY", "Grid", "Surface", "UnlitSurface" };

// Convert text string to DisplayStyle
Collection::DisplayStyle Collection::displayStyle(QString s)
{
	for (int n=0; n<nDisplayStyles; ++n) if (s == SurfaceStyleKeywords[n]) return (Collection::DisplayStyle) n;
	return nDisplayStyles;
}

// Convert DisplayStyle to text string
const char* Collection::displayStyle(Collection::DisplayStyle kwd)
{
	return SurfaceStyleKeywords[kwd];
}

// Generate display data
void Collection::updateDisplayData()
{
	if (dataVersion_ == displayDataGeneratedAt_) return;

	// Make sure transforms are up to date
	updateLimitsAndTransforms();

	// Clear old displayData_ and create temporary Data2D list for display data construction
	List<Data2D> transformedData;
	displayData_.clear();

	// Loop over slices, apply any transforms (X or Y) and check limits
	for (DataSet* dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next)
	{
		// Check for slice with no points...
		if (dataSet->data().nPoints() == 0) continue;

		// Z
		double z = dataSet->transformedData().z();

		// Add new item to transformedData and displayData_ arrays
		Data2D* surfaceDataSet = transformedData.add();
		DisplayDataSet* displayDataSet = displayData_.add();
		displayDataSet->setZ(z);

		// Copy / interpolate raw data arrays
		Array<double> array[2];
		if (interpolate_.x)
		{
			dataSet->transformedData().interpolate(interpolateConstrained_.x);
			double x = dataSet->transformedData().arrayX().first();
			while (x <= dataSet->transformedData().arrayX().last())
			{
				array[0].add(x);
				array[1].add(dataSet->transformedData().interpolated(x));
				x += interpolationStep_.x;
			}
		}
		else
		{
			array[0] = dataSet->transformedData().arrayX();
			array[1] = dataSet->transformedData().arrayY();
		}

		// Now add data to surfaceDataSet
		for (int n=0; n<array[0].nItems(); ++n) surfaceDataSet->addPoint(array[0].value(n), array[1].value(n));
	}

	// Construct common x scale for data, and create y value data
	// We have already pruned out those slices with no data points, so no checks for this are necessary
	int n, nFinished = 0, nTransformedDataSets = transformedData.nItems();
	Data2D** data = transformedData.array();
	double lowest;

	// -- Set up initial array indices and nFinished count
	Array<int> i(nTransformedDataSets);
	for (n=0; n<nTransformedDataSets; ++n)
	{
		if (data[n]->nPoints() == 0)
		{
			i[n] = -1;
			++nFinished;
		}
		else i[n] = 0;
	}
	DisplayDataSet** slices = displayData_.array();
	displayAbscissa_.clear();

	// -- Loop over all datasets simultaneously, seeking next lowest point in their x data
	while (nFinished != nTransformedDataSets)
	{
		// Find lowest point of current x values
		lowest = std::numeric_limits<double>::max();
		for (n = 0; n< nTransformedDataSets; ++n)
		{
			// If we have exhausted this slice's data, move on
			if (i[n] == -1) continue;
			if (data[n]->x(i[n]) <= lowest) lowest = data[n]->x(i[n]);
		}

		// Now have lowest x value, so add new x point to abscissa...
		displayAbscissa_.add(lowest);

		// ...and add y values/flags from slice data to new displayData_
		for (n = 0; n< nTransformedDataSets; ++n)
		{
			// If we have exhausted this slice's data, add a dummy value.
			// Otherwise, check how close the X-value is to 'lowest'
			if (i[n] == -1) slices[n]->addDummy();
			else if (fabs(data[n]->x(i[n]) - lowest) < 1.0e-5)
			{
				slices[n]->add(data[n]->y(i[n]), DisplayDataSet::RealPoint);
				++i[n];
				if (i[n] == data[n]->nPoints())
				{
					i[n] = -1;
					++nFinished;
				}
			}
			else slices[n]->addDummy();
		}
	}

	// Finally, interpolate values over dummy points (where the dummy points are surrounded by actual values)
	int lastReal, m, o;
	double yWidth, xWidth, position;
	for (n = 0; n < nTransformedDataSets; ++n)
	{
		lastReal = -1;
		const Array<DisplayDataSet::DataPointType>& yType = slices[n]->yType();
		const Array<double>& y = slices[n]->y();
		for (m = 0; m < displayAbscissa_.nItems(); ++m)
		{
			// If this point is a real value, interpolate up to here from the last real point (if one exists and it is more than one element away)
			if (yType.value(m) == DisplayDataSet::RealPoint)
			{
				if (lastReal == -1) lastReal = m;
				else if ((m - lastReal) == 1) lastReal = m;
				else
				{
					// Interpolate from 'lastReal' index up to here
					xWidth = displayAbscissa_[m] - displayAbscissa_[lastReal]; 
					yWidth = y.value(m) - y.value(lastReal);
					for (o = lastReal+1; o<=m; ++o)
					{
						position = (displayAbscissa_[o] - displayAbscissa_[lastReal]) / xWidth;
						slices[n]->setY(o, y.value(lastReal) + yWidth*position, DisplayDataSet::InterpolatedPoint);
					}
				}
				lastReal = m;
			}
		}	
	}
	
	// Store new version 
	displayDataGeneratedAt_ = dataVersion_;
}

// Set whether data is visible
void Collection::setVisible(bool visible)
{
	visible_ = visible;
}

// Return hether data is visible
bool Collection::visible()
{
	return visible_;
}

// Return transformed display abscissa for data
const Array<double>& Collection::displayAbscissa()
{
	updateDisplayData();

	return displayAbscissa_;
}

// Return transformed data to display
List<DisplayDataSet>& Collection::displayData()
{
	updateDisplayData();

	return displayData_;
}

// Set display style of data
void Collection::setDisplayStyle(DisplayStyle style)
{
	displayStyle_ = style;

	++displayStyleVersion_;
}

// Return display style of data
Collection::DisplayStyle Collection::displayStyle()
{
	return displayStyle_;
}

// Return line style
LineStyle& Collection::displayLineStyle()
{
	return displayLineStyle_;
}

// Set surface shininess
void Collection::setDisplaySurfaceShininess(double shininess)
{
	displaySurfaceShininess_ = shininess;
}

// Return surface shininess
double Collection::displaySurfaceShininess()
{
	return displaySurfaceShininess_;
}

// Return style version
int Collection::displayStyleVersion()
{
	return displayStyleVersion_;
}

/*
 * File
 */

// Export data to plain text file
bool Collection::exportData(QString fileName)
{
	LineParser parser(fileName, true);

	if (!parser.ready()) return false;

	parser.writeLineF("# Exported data: '%s'\n", qPrintable(name_));

	for (DataSet* dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next)
	{
		parser.writeLineF("# Z = %e\n", dataSet->z());
		for (int n=0; n<dataSet->data().nPoints(); ++n)
		{
			const Array<double>& x = dataSet->data().constArrayX();
			const Array<double>& y = dataSet->data().constArrayY();
			parser.writeLineF("%e  %e\n", x.value(n), y.value(n));
		}
		parser.writeLineF("\n");
	}

	parser.closeFiles();

	return true;
}
