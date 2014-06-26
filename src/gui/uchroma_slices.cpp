/*
	*** Slice Acquisition / Fitting
	*** src/gui/slices.cpp
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

/*
 * Public Functions
 */

// Return first extracted slice group
ExtractedSliceGroup* UChromaWindow::extractedSliceGroups()
{
	return extractedSliceGroups_.first();
}

// Set slice axis
void UChromaWindow::setSliceAxis(int axis)
{
	sliceAxis_ = axis;
}

// Return current axis target for slice selection
int UChromaWindow::sliceAxis()
{
	return sliceAxis_;
}

// Calculate axis slice value at supplied mouse position
bool UChromaWindow::updateSliceValue(int mouseX, int mouseY)
{
	if (sliceAxis_ != -1)
	{
// 		printf("Test: min=%f, max=%f\n", axisMin_[0], axisMax_[0]);
// 		rMouseLast_.print();
// 		axisCoordMin_[0].print();
		// Project axis coordinates to get a screen-based yardstick
		Vec4<double> axmin = ui.MainView->modelToScreen(axisCoordMin_[sliceAxis_]);
		Vec4<double> axmax = ui.MainView->modelToScreen(axisCoordMax_[sliceAxis_]);
// 		axmin.print();
// 		axmax.print();

		// Calculate vectors between axis minimum and mouse position (AM) and axis maximum (AB)
		Vec3<double> ab(axmax.x - axmin.x, axmax.y - axmin.y, 0.0);
		Vec3<double> am(mouseX - axmin.x, mouseY - axmin.y, 0.0);
		Vec3<double> amNorm = am, abNorm = ab;
		double ratio = am.magnitude() / ab.magnitude();
		abNorm.normalise();
		amNorm.normalise();
		double angle = acos(abNorm.dp(amNorm)) ;
// 		printf("Angle = %f, %f\n", angle, angle * DEGRAD);

		// Calculate slice axis value - no need to account for inverted axes here, since this is accounted for in the vectors axmin and axmax
		if (axisLogarithmic_[sliceAxis_]) sliceValue_ = pow(10, abNorm.dp(amNorm)*ratio * (log10(axisMax_[sliceAxis_]) - log10(axisMin_[sliceAxis_])) + log10(axisMin_[sliceAxis_]));
		else sliceValue_ = abNorm.dp(amNorm)*ratio * (axisMax_[sliceAxis_] - axisMin_[sliceAxis_]) + axisMin_[sliceAxis_];
// 		printf("slicevalue = %f (%f)\n", sliceValue_, abNorm.dp(amNorm)*ratio);

		// Clamp value to data range
		if (sliceValue_ < axisMin_[sliceAxis_]) sliceValue_ = axisMin_[sliceAxis_];
		else if (sliceValue_ > axisMax_[sliceAxis_]) sliceValue_ = axisMax_[sliceAxis_];
// 		printf("ACMAG = %f, X = %f\n", ratio, sliceValue_);

		// Extract slice from data
		int bin = closestBin(sliceAxis_, sliceValue_);
		currentSlice_.data().clear();

		// Grab slice data
		if (bin != -1)
		{
			if (sliceAxis_ == 0)
			{
				// Slice at fixed X, passing through closest point (if not interpolated) or actual value (if interpolated - TODO)
				currentSlice_.data().clear();
				for (Slice* slice = currentCollection_->slices(); slice != NULL; slice = slice->next) currentSlice_.data().addPoint(slice->transformedData().z(), slice->transformedData().y(bin));
				currentSlice_.setTitle("X = " + QString::number(currentCollection_->slices()->transformedData().x(bin)));
			}
			else if (sliceAxis_ == 1)
			{
				return false;
			}
			else if (sliceAxis_ == 2)
			{
				// Slice through Z - i.e. original slice data
				currentSlice_.data().clear();
				currentSlice_.data() = currentCollection_->slice(bin)->data();
				currentSlice_.setTitle("Z = " + QString::number(currentCollection_->slice(bin)->transformedData().z()));
			}
		}

		emit(sliceDataChanged());
		updateCoordinateInfo();
		sliceMonitorWindow_.updateControls();
		return true;
	}

	sliceMonitorWindow_.updateControls();

	return false;
}

// Return current axis value for slice selection
double UChromaWindow::sliceValue()
{
	return sliceValue_;
}

// Return current slice coordinate along axis
double UChromaWindow::sliceCoordinate()
{
	if (sliceAxis_ == -1) return 0.0;

	if (axisLogarithmic_[sliceAxis_]) return (axisInverted_[sliceAxis_] ? log10(axisMax_[sliceAxis_]/sliceValue_) : log10(sliceValue_));
	else return (axisInverted_[sliceAxis_] ? axisMax_[sliceAxis_] - sliceValue_ : sliceValue_);
}

// Return axis bin value of closest point to supplied value
int UChromaWindow::closestBin(int axis, double value)
{
	if (!currentCollection_) return -1;
	if (currentCollection_->nSlices() == 0) return -1;

	if (axis == 0)
	{
		// Check X array of first slice
		Array<double>& x = currentCollection_->slices()->transformedData().arrayX();
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
		double delta, closestDelta = fabs(currentCollection_->slices()->transformedData().z() - value);
		for (Slice* slice = currentCollection_->slices()->next; slice != NULL; slice = slice->next)
		{
			delta = fabs(slice->transformedData().z() - value);
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
