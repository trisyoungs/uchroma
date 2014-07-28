/*
	*** Data Collection
	*** src/base/collection.cpp
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

#include "base/collection.h"
#include "base/viewpane.h"
#include <limits>

// Static Members
RefList<Collection,Collection::CollectionSignal> Collection::collectionSignals_;

// Constructor
Collection::Collection() : ListItem<Collection>()
{
	// Set variable defaults
	dataSets_.clear();
	dataFileDirectory_ = getenv("PWD");
	title_ = "Empty Collection";
	dataChanged_ = true;
	dataMin_.zero();
	dataMax_.set(10.0, 10.0, 10.0);

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
	colourScaleValid_ = false;

	// Associated data
	parent_ = NULL;
	type_ = Collection::MasterCollection;
	currentSlice_ = NULL;

	// Display
	visible_ = true;
	displayData_.clear();
	displayStyle_ = Collection::LineXYStyle;
	displayDataValid_ = false;
	displayPrimitivesValid_ = false;
	displayPane_ = NULL;
}

// Destructor
Collection::~Collection()
{
	// Ensure that the collection is removed from the pane it is being displayed in
	if (displayPane_) displayPane_->removeCollection(this);
}

// Copy constructor
Collection::Collection(const Collection& source)
{
	(*this) = source;
}

// Assignment operator
void Collection::operator=(const Collection& source)
{
	// Basic Data
	title_ = source.title_;
	dataSets_ = source.dataSets_;
	dataFileDirectory_ = source.dataFileDirectory_;
	dataMin_ = source.dataMin_;
	dataMax_ = source.dataMax_;
	dataChanged_ = source.dataChanged_;

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
	colourScaleValid_ = source.colourScaleValid_;
}

/*
 * Data
 */

// Set title
void Collection::setTitle(QString title)
{
	title_ = title;
}

// Return title
QString Collection::title()
{
	return title_;
}

// Add dataset
DataSet* Collection::addDataSet()
{
	// Create new dataset
	DataSet* dataSet = dataSets_.add();

	dataChanged_ = true;
	displayDataValid_ = false;
	return dataSet;
}

// Add dataset at specified z
DataSet* Collection::addDataSet(double z)
{
	DataSet* dataSet = dataSets_.add();
	setDataSetZ(dataSet, z);
	return dataSet;
}

// Add dataset, copying from supplied data
void Collection::addDataSet(DataSet* source)
{
	// Create new dataset
	DataSet* dataSet = dataSets_.add();
	(*dataSet) = (*source);

	dataChanged_ = true;
	displayDataValid_ = false;
	displayPrimitivesValid_ = false;
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
	displayDataValid_ = false;
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
	target->data().setZ(z);

	// Adjust position of the dataset in the list if necessary
	do
	{
		// Shift item if necessary
		if (target->prev && (target->prev->data().z() > target->data().z()))
		{
			// Shift specified target up the list
			dataSets_.shiftUp(target);
			displayDataValid_ = false;
			minBad = (target->prev ? (target->prev->data().z() > target->data().z()) : false);
		}
		else minBad = false;
		if (target->next && (target->next->data().z() < target->data().z()))
		{
			// Move specified target down the list
			dataSets_.shiftDown(target);
			displayDataValid_ = false;
			maxBad = (target->next ? (target->next->data().z() < target->data().z()) : false);
		}
		else maxBad = false;
		if (++dummy == 10) break;
		
	} while (minBad || maxBad);

	dataChanged_ = true;
	displayDataValid_ = false;
	displayPrimitivesValid_ = false;
}

// Set data of specified dataset
void Collection::setDataSetData(DataSet* target, Data2D* newData)
{
	// Check that this DataSet is owned by the collection
	if (!dataSets_.contains(target))
	{
		msg.print("Internal Error : Tried to set the data of a dataset using the wrong collection.\n");
		return;
	}

	target->data() = (*newData);

	dataChanged_ = true;
	displayDataValid_ = false;
	displayPrimitivesValid_ = false;
}

// Return first dataset in list
DataSet* Collection::dataSets() const
{
	return dataSets_.first();
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

	dataChanged_ = true;
	displayDataValid_ = false;
	displayPrimitivesValid_ = false;
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
}

// Return root directory for datafiles
QDir Collection::dataFileDirectory()
{
	return dataFileDirectory_;
}

// Load data for specified dataset index
bool Collection::loadDataSet(DataSet* dataSet)
{
	if (!dataSet) return false;
	dataChanged_ = true;
	return dataSet->loadData(dataFileDirectory_);
}

// Reload data for all slices
int Collection::loadAllDataSets()
{
	int nFailed = 0;
	for (DataSet* dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next) if (!dataSet->loadData(dataFileDirectory_)) ++nFailed;
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
		dataChanged_ = true;
		updateLimitsAndTransforms();
	}

	displayDataValid_ = false;
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

	// Make sure limits and transform are up to date
	if (transforms_[axis].enabled())
	{
		dataChanged_ = true;
		updateLimitsAndTransforms();
	}

	displayDataValid_ = false;
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
}

// Return interpolation step size
double Collection::interpolationStep(int axis)
{
	return interpolationStep_[axis];
}

/*
 * Associated Data
 */

// Return axis bin value of closest point to supplied value
int Collection::closestBin(int axis, double value)
{
	if (dataSets_.nItems() == 0) return -1;

	if (axis == 0)
	{
		// Check X array of first slice
		Array<double>& x = dataSets_.first()->transformedData().arrayX();
		int midIndex, loIndex = 0, hiIndex = x.nItems() - 1;
		if (value < x.value(0)) return 0;
		if (value > x.value(hiIndex)) return hiIndex;
		// Binary... chop!
		while ((hiIndex - loIndex) > 1)
		{
			midIndex = (hiIndex + loIndex) / 2;
			if (x.value(midIndex) <= value) loIndex = midIndex;
			else hiIndex = midIndex;
		}
		if (fabs(x.value(loIndex) - value) < fabs(x.value(hiIndex) - value)) return loIndex;
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
		double delta, closestDelta = fabs(dataSets_.first()->transformedData().z() - value);
		for (DataSet* dataSet = dataSets_.first()->next; dataSet != NULL; dataSet = dataSet->next)
		{
			delta = fabs(dataSet->transformedData().z() - value);
			++n;
			if (delta < closestDelta)
			{
				closest = n;
				closestDelta = delta;
			}
		}
		return closest;
	}
	else return -1;
}

// Get slice at specified axis and bin
void Collection::getSlice(int axis, int bin)
{
	// Create the currentSlice_ collection if it doesn't already exist
	if (currentSlice_ == NULL)
	{
		currentSlice_ = new Collection;
		currentSlice_->parent_ = this;
		currentSlice_->type_ = Collection::CurrentSliceCollection;
		XXX Missing call to currentCollection_->displayPrimitives().setViewer(ui.MainView);
	}

	// Are supplied bin and axis valid?
	if ((bin == -1) || (axis == -1)) currentSlice_->clearDataSets();
	else if (axis == 0)
	{
		DataSet* newDataSet = currentSlice_->addDataSet();

		// Slice at fixed X, passing through closest point (if not interpolated) or actual value (if interpolated - TODO)
		for (DataSet* dataSet = dataSets_.first(); dataSet != NULL; dataSet = dataSet->next) newDataSet->data().addPoint(dataSet->transformedData().z(), dataSet->transformedData().y(bin));
		currentSlice_->setTitle("X = " + QString::number(dataSets_.first()->transformedData().x(bin)));
	}
	else if (axis == 1)
	{
		// TODO - Generate contour map?
	}
	else if (axis == 2)
	{
		// Slice through Z - i.e. original slice data
		currentSlice_->addDataSet(dataSets_[bin]);
		currentSlice_->setTitle("Z = " + QString::number(dataSets_[bin]->transformedData().z()));
	}
}

// Find collection with name specified
Collection* Collection::findCollection(QString name)
{
	// Does the name of this collection match?
	if (title_ == name) return this;

	// Check fit data
	for (Collection* fit = fitData_.first(); fit != NULL; fit = fit->next)
	{
		Collection* result = fit->findCollection(name);
		if (result) return result;
	}

	// Check extracted data
	for (Collection* extract = extractedData_.first(); extract != NULL; extract = extract->next)
	{
		Collection* result = extract->findCollection(name);
		if (result) return result;
	}

	// No match here, so return NULL
	return NULL;
}

// Return next logical collection in lists
Collection* Collection::nextCollection(bool descend)
{
	// If the current collection contains additional data, return that
	if (descend && fitData_.first()) return fitData_.first();
	else if (descend && extractedData_.first()) return extractedData_.first();

	// If there is a next item in the current collection's list (whatever it is) move on to that
	if (next) return next;

	// No additional data, and we are at the end of the current list, so check parent
	if (type_ == Collection::MasterCollection) return NULL;
	else if (type_ == Collection::FitCollection)
	{
		// No more fitData - check extracted data in parent. If nothing there, then we must ascend to the next item in the parent's list
		if (parent_->extractedData_.nItems() != 0) return parent_->extractedData();
		else return parent_->nextCollection(false);
	}
	else if (type_ == Collection::ExtractedCollection) return parent_->nextCollection(false);

	return NULL;
}

// Return previous logical collection in lists
Collection* Collection::previousCollection(bool descend)
{
	// If the current collection contains additional data, return that
	if (descend && extractedData_.last()) return extractedData_.last()->previousCollection(true);
	else if (descend && fitData_.last()) return fitData_.last()->previousCollection(true);
	else if (descend) return this;

	// If there is a previous item in the current collection's list (whatever it is) move on to that
	if (prev) return prev;

	// No additional data, and we are at the beginning of the current list, so check parent
	if (type_ == Collection::MasterCollection) return NULL;
	else if (type_ == Collection::ExtractedCollection)
	{
		// No more extractedData - check fit data in parent. If nothing there, then we must ascend to the previous item in the parent's list
		if (parent_->fitData_.nItems() != 0) return parent_->fitData_.last()->previousCollection(true);
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

// Add fit data to Collection
Collection* Collection::addFitData(QString title)
{
	Collection* newFit = fitData_.add();
	newFit->setTitle(title);
	newFit->type_ = Collection::FitCollection;
	newFit->setParent(this);

	// Set link to MainView's primitive reflist (copy pointer set in parent collection)
	newFit->displayPrimitives_.setViewer(displayPrimitives_.viewer());

	return newFit;
}

// Remove specified fit data from list
void Collection::removeFitData(Collection* collection)
{
	fitData_.remove(collection);
}

// Return fit data in Collection
Collection* Collection::fitData()
{
	return fitData_.first();
}

// Add extracted data to Collection
Collection* Collection::addExtractedData(QString title)
{
	Collection* newExtract = extractedData_.add();
	newExtract->setTitle(title);
	newExtract->type_ = Collection::ExtractedCollection;
	newExtract->setParent(this);

	// Set link to MainView's primitive reflist (copy pointer set in parent collection)
	newExtract->displayPrimitives_.setViewer(displayPrimitives_.viewer());

	return newExtract;
}

// Remove specified extracted data from list
void Collection::removeExtractedData(Collection* collection)
{
	extractedData_.remove(collection);
}

// Return extracted data in Collection
Collection* Collection::extractedData()
{
	return extractedData_.first();
}

// Update current slice based on specified axis and value
void Collection::updateCurrentSlice(int axis, double axisValue)
{
	getSlice(axis, closestBin(axis, axisValue));

	notifyDependents(this, Collection::CurrentSliceChangedSignal);
}

// Extract current slice based on specified axis and value
void Collection::extractCurrentSlice(int axis, double axisValue)
{
	getSlice(axis, closestBin(axis, axisValue));

	Collection* newExtractedData = addExtractedData(currentSlice_->title());
	newExtractedData->addDataSet(currentSlice_->dataSets());

	notifyDependents(newExtractedData, Collection::CollectionCreatedSignal);
	notifyDependents(this, Collection::ExtractedDataAddedSignal);
}

// Return current slice
Collection* Collection::currentSlice()
{
	return currentSlice_;
}
	
/*
 * Dependent Data / Signalling
 */

// Notify dependents that something in this collection has changed
void Collection::notifyDependents(Collection* source, Collection::CollectionSignal signal)
{
	// Add signal to list
	collectionSignals_.add(source, signal);
}

// Return first signal in lists
RefListItem<Collection,Collection::CollectionSignal>* Collection::collectionSignals()
{
	return collectionSignals_.first();
}

// Delete specified signal and return next
RefListItem<Collection,Collection::CollectionSignal>* Collection::deleteCollectionSignal(RefListItem<Collection,Collection::CollectionSignal>* collectionSignal)
{
	RefListItem<Collection,Collection::CollectionSignal>* nextSignal = collectionSignal->next;
	collectionSignals_.remove(collectionSignal);
	return nextSignal;
}

/*
 * Update
 */

// Update data limits and transform data
void Collection::updateLimitsAndTransforms()
{
	if (!dataChanged_) return;
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

	// Reset flag
	dataChanged_ = false;
}

// Flag that one or more datasets have changed
void Collection::setDataChanged()
{
	dataChanged_ = true;
}

/*
 * Colours
 */

// Update colour scale
void Collection::updateColourScale()
{
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

	colourScaleValid_ = true;

	// Primitive will now need to be regenerated...
	displayPrimitivesValid_ = false;
}

// Set colourscale source to use
void Collection::setColourSource(ColourSource source)
{
	colourSource_ = source;
	colourScaleValid_ = false;
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
	}

	// Update colourscale?
	if (source == colourSource_) colourScaleValid_ = false;
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
	}
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
	if (colourSource_ == Collection::CustomGradientSource) colourScaleValid_ = false;
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
	if (colourSource_ == Collection::CustomGradientSource) colourScaleValid_ = false;
}

// Set alpha control
void Collection::setAlphaControl(Collection::AlphaControl alpha)
{

	alphaControl_ = alpha;
	colourScaleValid_ = false;
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
	colourScaleValid_ = false;
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
	if (!colourScaleValid_) updateColourScale();
	
	return colourScale_;
}

// Return whether colourscale is valid
bool Collection::colourScaleValid()
{
	return colourScaleValid_;
}

/*
 * Surface
 */

// Suface Style Keywords
const char* SurfaceStyleKeywords[] = { "LineXY", "LineZY", "Grid", "Surface" };

// Convert text string to DisplayStyle
Collection::DisplayStyle Collection::displayStyle(const char* s)
{
	for (int n=0; n<nDisplayStyles; ++n) if (strcmp(s,SurfaceStyleKeywords[n]) == 0) return (Collection::DisplayStyle) n;
	return nDisplayStyles;
}

// Convert DisplayStyle to text string
const char* Collection::displayStyle(Collection::DisplayStyle kwd)
{
	return SurfaceStyleKeywords[kwd];
}

// Set whether data is visible
bool Collection::setVisible(bool visible)
{
	visible_ = visible;
}

// Return hether data is visible
bool Collection::visible()
{
	return visible_;
}

// Return transformed display abscissa for data
const Array<double>& Collection::displayAbscissa() const
{
	return displayAbscissa_;
}

// Return transformed data to display
List<DisplayDataSet>& Collection::displayData()
{
	return displayData_;
}

// Set display style of data
void Collection::setDisplayStyle(DisplayStyle style)
{
	displayStyle_ = style;

	displayPrimitivesValid_ = false;
}

// Return display style of data
Collection::DisplayStyle Collection::displayStyle()
{
	return displayStyle_;
}

// Set line width (for line styles)
void Collection::setDisplayLineWidth(double width)
{
	displayLineWidth_ = width;
}

// Return Line width (for line styles)
double Collection::displayLineWidth()
{
	return displayLineWidth_;
}

// Manually set the flag to force regeneration of surface data
void Collection::setDisplayDataInvalid()
{
	// Update subcollections first
	// -- Fit data
	for (Collection* fit = fitData_.first(); fit != NULL; fit = fit->next) fit->setDisplayDataInvalid();
	// -- Extracted data
	for (Collection* extract = extractedData_.first(); extract != NULL; extract = extract->next) extract->setDisplayDataInvalid();
	// -- Current slice
	if (currentSlice_) currentSlice_->setDisplayDataInvalid();

	displayDataValid_ = false;
}

// Flag that the primitive has been updated
void Collection::setDisplayPrimitiveValid()
{
	displayPrimitivesValid_ = true;
}

// Return whether surface primitives are valid
bool Collection::displayPrimitivesValid()
{
	return displayPrimitivesValid_;
}

// Return surface primitives list
PrimitiveList& Collection::displayPrimitives()
{
	return displayPrimitives_;
}

// Update surface data if necessary
void Collection::updateDisplayData()
{
	// Update subcollections first
	// -- Fit data
	for (Collection* fit = fitData_.first(); fit != NULL; fit = fit->next) fit->updateDisplayData();
	// -- Extracted data
	for (Collection* extract = extractedData_.first(); extract != NULL; extract = extract->next) extract->updateDisplayData();
	// -- Current slice
	if (currentSlice_) currentSlice_->updateDisplayData();

	// Is surface reconstruction necessary?
	if (displayDataValid_) return;

	// Check view pane for this collection
	if (displayPane_ == NULL) return;

	// Make sure transforms are up to date
	updateLimitsAndTransforms();

	// Grab some stuff from the pane's axes
	Vec3<double> axisMin(displayPane_->axes().axisMin(0), displayPane_->axes().axisMin(1), displayPane_->axes().axisMin(2));
	Vec3<double> axisMax(displayPane_->axes().axisMax(0), displayPane_->axes().axisMax(1), displayPane_->axes().axisMax(2));

	// Clear old displayData_ and create temporary Data2D list for display data construction
	List<Data2D> transformedData;
	displayData_.clear();
	double x, y;

	// Loop over slices, apply any transforms (X or Y) and check limits
	DataSet* dataSet = displayPane_->axes().axisInverted(2) ? dataSets_.last() : dataSets_.first();
	while (dataSet)
	{
		// Check for slice with no points...
		if (dataSet->data().nPoints() == 0) continue;

		// Z
		double z = dataSet->transformedData().z();
		// -- Is the transformed Z value within range?
		// -- If the pane is 2D, display all data regardless of z
		if ((!displayPane_->twoDimensional()) && ((z < axisMin.z) || (z > axisMax.z)))
		{
			dataSet = displayPane_->axes().axisInverted(2) ? dataSet->prev : dataSet->next;
			continue;
		}
		if (displayPane_->axes().axisInverted(2) && displayPane_->axes().axisLogarithmic(2)) z = log10(axisMax.z/z);   // XXX TODO Changed from 'log10(axisMax[n]/z)' which was a bug???
		else if (displayPane_->axes().axisInverted(2)) z = (axisMax.z - z) + displayPane_->axes().axisMin(2);
		else if (displayPane_->axes().axisLogarithmic(2)) z = log10(z);

		// Add new item to transformedData and displayData_ arrays
		Data2D* surfaceDataSet = transformedData.add();
		DisplayDataSet* displayDataSet = displayData_.add();
		displayDataSet->setZ(z * displayPane_->axes().axisStretch(2));

		// Copy / interpolate raw data arrays
		Array<double> array[2];
		if (interpolate_.x)
		{
			dataSet->transformedData().interpolate(interpolateConstrained_.x);
			double x = axisMin.x;
			while (x <= axisMax.x)
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

		// Now add data to surfaceDataSet, obeying defined x-limits
		if (displayPane_->axes().axisInverted(0)) for (int n=array[0].nItems()-1; n >= 0; --n)
		{
			x = array[0].value(n);
			if ((x < axisMin.x) || (x > axisMax.x)) continue;
			if (displayPane_->axes().axisLogarithmic(0)) x = log10(axisMax.x / x);
			else x = (axisMax.x - x) + axisMin.x;
			x *= displayPane_->axes().axisStretch(0);
			y = array[1].value(n);
			if (displayPane_->axes().axisLogarithmic(1)) y = (displayPane_->axes().axisInverted(1) ? log10(axisMax.y / y) : log10(y));
			else if (displayPane_->axes().axisInverted(1)) y = (axisMax.y - y) + axisMin.y;
			y *= displayPane_->axes().axisStretch(1);
			surfaceDataSet->addPoint(x, y);
		}
		else for (int n=0; n<array[0].nItems(); ++n)
		{
			x = array[0].value(n);
			if ((x < axisMin.x) || (x > axisMax.x)) continue;
			if (displayPane_->axes().axisLogarithmic(0)) x = log10(x);
			x *= displayPane_->axes().axisStretch(0);
			y = array[1].value(n);
			if (displayPane_->axes().axisLogarithmic(1))
			{
				if (y < 0.0) y = 1.0e-10;
				else y = (displayPane_->axes().axisInverted(1) ? log10(axisMax.y / y) : log10(y));
			}
			else if (displayPane_->axes().axisInverted(1)) y = (axisMax.y - y) + axisMin.y;
			y *= displayPane_->axes().axisStretch(1);
			surfaceDataSet->addPoint(x, y);
		}

		// Move to next Z slice
		dataSet = displayPane_->axes().axisInverted(2) ? dataSet->prev : dataSet->next;
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
	int test = 0;
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

	// Data has been updated - surface primitive will need to be reconstructed
	displayPrimitivesValid_ = false;
	displayDataValid_ = true;
}

// Set view pane on which this data is being displayed
void Collection::setDisplayPane(ViewPane* pane)
{
	displayPane_ = pane;

	// Axes limits are likely to be different, so must flag current display data as invalid
	displayDataValid_ = false;
	displayPrimitivesValid_ = false;
}

// Return view pane on which this data is being displayed
ViewPane* Collection::displayPane()
{
	return displayPane_;
}

// Send collection data to GL, including any associated fit and extracted data
void Collection::sendToGL()
{
	// If this collection is not visible return now
	if (!visible_) return;

	if (displayStyle_ == Collection::SurfaceStyle)
	{
		glEnable(GL_LIGHTING);
		glDisable(GL_LINE_SMOOTH);
	}
	else
	{
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(displayLineWidth_);
		glDisable(GL_LIGHTING);
	}

	// Send Primitives to display
	displayPrimitives_.sendToGL();
}
