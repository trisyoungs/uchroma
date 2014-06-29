/*
	*** Slice Acquisition
	*** src/gui/uchroma_slices.cpp
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

/*
 * Private Functions
 */

// Add / retrieve group
ExtractedSliceGroup* UChromaWindow::addOrRetrieveGroup(QString name)
{
	// Loop over current groups
	ExtractedSliceGroup* group = NULL;
	for (group = extractedSliceGroups_.first(); group != NULL; group = group->next) if (group->name() == name) break;
	if (group == NULL)
	{
		group = extractedSliceGroups_.add();
		group->setName(name);
	}
	return group;
}

// Extract slice based on specified axis and bin
void UChromaWindow::extractSlice(int axis, int bin)
{
	if (bin != -1)
	{
		currentSlice_.data().clear();
		return;
	}

	if (interactionAxis_ == 0)
	{
		// Slice at fixed X, passing through closest point (if not interpolated) or actual value (if interpolated - TODO)
		currentSlice_.data().clear();
		for (DataSet* dataSet = currentCollection_->dataSets(); dataSet != NULL; dataSet = dataSet->next) currentSlice_.data().addPoint(dataSet->transformedData().z(), dataSet->transformedData().y(bin));
		currentSlice_.setTitle("X = " + QString::number(currentCollection_->dataSets()->transformedData().x(bin)));
	}
	else if (interactionAxis_ == 1)
	{
		currentSlice_.data().clear();
		// TODO - Generate contour map?
	}
	else if (interactionAxis_ == 2)
	{
		// Slice through Z - i.e. original slice data
		currentSlice_.data().clear();
		currentSlice_.data() = currentCollection_->dataSet(bin)->data();
		currentSlice_.setTitle("Z = " + QString::number(currentCollection_->dataSet(bin)->transformedData().z()));
	}

	emit(sliceDataChanged());
	sliceMonitorWindow_.updateControls();
}

/*
 * Public Functions
 */

// Return first extracted slice group
ExtractedSliceGroup* UChromaWindow::extractedSliceGroups()
{
	return extractedSliceGroups_.first();
}

// Return current slice data
ExtractedSlice* UChromaWindow::currentSlice()
{
	return &currentSlice_;
}

// Add current slice data to extracted slices list
void UChromaWindow::addSurfaceSlice()
{
	// The new slice will go into the Uncategorised group
	ExtractedSliceGroup* group = addOrRetrieveGroup("Uncategorised");
	ExtractedSlice* slice = group->addSlice(currentSlice_);
	slicesWindow_.addSlice(slice);
}
