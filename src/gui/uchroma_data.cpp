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
	regenerateAxes_ = true;
}

// Recalculate tick deltas for specified axis
void UChromaWindow::calculateTickDeltas(int axis)
{
	const int nBaseValues = 5, maxIterations = 10, maxTicks = 10;
	int power = 1, baseValues[nBaseValues] = { 1, 2, 3, 4, 5 }, baseValueIndex = 0, nTicks, iteration, minTicks = maxTicks/2;
	double clampedStartValue;

	baseValueIndex = 0;
	power = int(log10((axisMax_[axis]-axisMin_[axis]) / maxTicks) - 1);
	iteration = 0;

	if ((axisMax_[axis] - axisMin_[axis]) > 1.0e-10)
	{
		do
		{
			// Calculate current tickDelta
			axisTickDelta_[axis] = baseValues[baseValueIndex]*pow(10.0,power);

			// Get first tickmark value
			axisFirstTick_[axis] = int(axisMin_[axis] / axisTickDelta_[axis]) * axisTickDelta_[axis];
			clampedStartValue = (axisFirstTick_[axis] < axisMin_[axis] ? axisFirstTick_[axis] + axisTickDelta_[axis] : axisFirstTick_[axis]);

			// How many ticks now fit between the firstTick and max value?
			// Add 1 to get total ticks for this delta (i.e. including firstTick)
			nTicks = int((axisMax_[axis]-axisMin_[axis]) / axisTickDelta_[axis]);
			++nTicks;

			// Check n...
			if (nTicks > maxTicks)
			{
				++baseValueIndex;
				if (baseValueIndex == nBaseValues) ++power;
				baseValueIndex = baseValueIndex%nBaseValues;
			}
			else if (nTicks < minTicks)
			{
				--baseValueIndex;
				if (baseValueIndex == -1)
				{
					--power;
					baseValueIndex += nBaseValues;
				}
			}

			++iteration;
			if (iteration == maxIterations) break;

		} while ((nTicks > maxTicks) || (nTicks < minTicks));
	}
	else
	{
		axisFirstTick_[axis] = axisMin_[axis];
		axisTickDelta_[axis] = 1.0;
	}
}

// Return absolute minimum transformed values over all collections
Vec3<double> UChromaWindow::transformedDataMinima()
{
	if (collections_.nItems() == 0) return Vec3<double>(0.0,0.0,0.0);

	// Set starting values from first collection
	Vec3<double> v, minima = collections_.first()->transformMin();
	for (Collection* collection = collections_.first()->next; collection != NULL; collection = collection->next)
	{
		v = collection->transformMin();
		if (v.x < minima.x) minima.x = v.x;
		if (v.y < minima.y) minima.y = v.y;
		if (v.z < minima.z) minima.z = v.z;
	}
	return minima;
}

// Return absolute maximum transformed values over all collections
Vec3<double> UChromaWindow::transformedDataMaxima()
{
	if (collections_.nItems() == 0) return Vec3<double>(0.0,0.0,0.0);

	// Set starting values from first collection
	Vec3<double> v, maxima = collections_.first()->transformMax();
	for (Collection* collection = collections_.first()->next; collection != NULL; collection = collection->next)
	{
		v = collection->transformMax();
		if (v.x > maxima.x) maxima.x = v.x;
		if (v.y > maxima.y) maxima.y = v.y;
		if (v.z > maxima.z) maxima.z = v.z;
	}
	return maxima;
}

// Return absolute minimum positive transformed values over all collections
Vec3<double> UChromaWindow::transformedDataPositiveMinima()
{
	if (collections_.nItems() == 0) return Vec3<double>(0.1,0.1,0.1);

	// Set starting values from first collection
	Vec3<double> v, minima = collections_.first()->transformMinPositive();
	for (Collection* collection = collections_.first()->next; collection != NULL; collection = collection->next)
	{
		v = collection->transformMinPositive();
		if (v.x < minima.x) minima.x = v.x;
		if (v.y < minima.y) minima.y = v.y;
		if (v.z < minima.z) minima.z = v.z;
	}
	return minima;
}

// Return absolute maximum positive transformed values over all collections
Vec3<double> UChromaWindow::transformedDataPositiveMaxima()
{
	if (collections_.nItems() == 0) return Vec3<double>(1.0, 1.0, 1.0);

	// Set starting values from first collection
	Vec3<double> v, maxima = collections_.first()->transformMaxPositive();
	for (Collection* collection = collections_.first()->next; collection != NULL; collection = collection->next)
	{
		v = collection->transformMaxPositive();
		if (v.x > maxima.x) maxima.x = v.x;
		if (v.y > maxima.y) maxima.y = v.y;
		if (v.z > maxima.z) maxima.z = v.z;
	}
	return maxima;
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

	// Set new currentCollection_
	if (collection->next) currentCollection_ = collection->next;
	else currentCollection_ = collection->prev;
	if (currentCollection_ == NULL) currentCollection_ = addCollection();

	setAsModified();

	// Finally, remove old collection
	collections_.remove(collection);
}

// Move collection focus to next in list
void UChromaWindow::focusNextCollection()
{
	currentCollection_ = currentCollection_->next;
	if (currentCollection_ == NULL) currentCollection_ = collections_.first();

	updateGUI();
}

// Move collection focus to previous in list
void UChromaWindow::focusPreviousCollection()
{
	currentCollection_ = currentCollection_->prev;
	if (currentCollection_ == NULL) currentCollection_ = collections_.last();

	updateGUI();
}

// Return nth collection in list
Collection* UChromaWindow::collection(int index)
{
	return collections_[index];
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
void UChromaWindow::clearData()
{
	// Collections
	collections_.clear();

	// Data
	modified_ = false;
	inputFile_ = "";
	imageExportFile_ = "image.png";
	imageExportWidth_ = 800;
	imageExportHeight_ = 600;
	imageExportMaintainAspect_ = true;
	imageExportFormat_ = Viewer::PNGFormat;
	// viewerFont_ = ??

	// Axes
	axisMin_.zero();
	axisMax_.set(10.0, 10.0, 10.0);
	axisInverted_.set(false, false, false);
	axisLogarithmic_.set(false, false, false);
	axisVisible_.set(true, true, true);
	axisPositionIsFractional_.set(true, true, true);
	axisPositionReal_[0].zero();
	axisPositionReal_[1].zero();
	axisPositionReal_[2].zero();
	axisPositionFractional_[0].zero();
	axisPositionFractional_[1].zero();
	axisPositionFractional_[2].zero();
	axisTickDirection_[0].set(0.0, -1.0, 0.0);
	axisTickDirection_[1].set(-1.0, 0.0, 0.0);
	axisTickDirection_[2].set(1.0, 0.0, 0.0);
	axisFirstTick_.zero();
	axisTickDelta_.set(1.0,1.0,1.0);
	axisAutoTicks_.set(true, true, true);
	axisMinorTicks_.set(1,1,1);
	axisLabelOrientation_[0].set(0.0, 0.0, 0.2);
	axisLabelOrientation_[1].set(0.0, 0.0, 0.2);
	axisLabelOrientation_[2].set(0.0, 180.0, 0.2);
	axisTitle_[0] = "X Axis";
	axisTitle_[1] = "Y Axis";
	axisTitle_[2] = "Z Axis";
	axisTitleOrientation_[0].set(0.0, 0.0, 0.2, 0.5);
	axisTitleOrientation_[1].set(0.0, 0.0, 0.2, 0.5);
	axisTitleOrientation_[2].set(0.0, 0.0, 0.2, 0.5);
	axisTitleAnchor_[0] = TextPrimitive::AnchorCentre;
	axisTitleAnchor_[1] = TextPrimitive::AnchorCentre;
	axisTitleAnchor_[2] = TextPrimitive::AnchorCentre;
	axisStretch_.set(1.0, 1.0, 1.0);
	axisCoordMin_[0].zero();
	axisCoordMin_[1].zero();
	axisCoordMin_[2].zero();
	axisCoordMax_[0].zero();
	axisCoordMax_[1].zero();
	axisCoordMax_[2].zero();
	regenerateAxes_ = true;

	// Interaction
	interactionMode_ = UChromaWindow::NoInteraction;
	interactionAxis_ = -1;
	clickedInteractionValue_ = 0.0;
	currentInteractionValue_ = 0.0;

	// Slices
	extractedSliceGroups_.clear();
	extractedSliceGroups_.add();

	// View
	labelScale_ = 0.1;
	titleScale_ = 0.12;
	boundingBox_ = UChromaWindow::NoBox;
	boundingBoxPlaneY_ = 0.0;
	labelFaceViewer_ = false;
	labelCorrectOrientation_ = true;
	graphMode_ = false;
	ui.actionViewPerspective->setChecked(false);
}

// Set display limits to show all available data
void UChromaWindow::showAllData()
{
	updateAxisLimits();

	axisMin_ = axisLimitMin_;
	axisMax_ = axisLimitMax_;

	// Flag that all surfaces and axes should be regenerated (data and primitive)
	setRegeneratePrimitives();
}
