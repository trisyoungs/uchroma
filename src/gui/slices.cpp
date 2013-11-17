/*
	*** Slice Acquisition / Fitting
	*** src/gui/slices.cpp
	Copyright T. Youngs 2013

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

// Add slice to graph
void UChromaWindow::addSurfaceSlice()
{
	
// 	updateSurfaceSliceData(false);
// 	QTableWidgetItem* item = new QTableWidgetItem(ui.SlicesTable);
// 	printf("Here.\n");
// 	ui.SlicesTable->setRowCount(ui.SlicesTable->rowCount()+1);
// 	GraphWidget* widget = new GraphWidget(ui.SlicesTable);
// 	ui.SlicesTable->setCellWidget(0, 0, widget);
}

/*
 * Private Functions
 */

// Return axis bin value of closest point to supplied value
int UChromaWindow::closestBin(int axis, double value)
{
	if (slices_.nItems() == 0) return -1;

	if (axis == 0)
	{
		// Check X array of first slice
		Array<double>& x = slices_.first()->transformedData().arrayX();
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
		double delta, closestDelta = fabs(slices_.first()->transformedData().z() - value);
		for (Slice* slice = slices_.first()->next; slice != NULL; slice = slice->next)
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
		if (axisLogarithmic_[sliceAxis_]) sliceValue_ = pow(10, abNorm.dp(amNorm)*ratio * (log10(limitMax_[sliceAxis_]) - log10(limitMin_[sliceAxis_])) + log10(limitMin_[sliceAxis_]));
		else sliceValue_ = abNorm.dp(amNorm)*ratio * (limitMax_[sliceAxis_] - limitMin_[sliceAxis_]) + limitMin_[sliceAxis_];
		printf("slicevalue = %f (%f)\n", sliceValue_, abNorm.dp(amNorm)*ratio);

		// Clamp value to data range
		if (sliceValue_ < limitMin_[sliceAxis_]) sliceValue_ = limitMin_[sliceAxis_];
		else if (sliceValue_ > limitMax_[sliceAxis_]) sliceValue_ = limitMax_[sliceAxis_];
		printf("ACMAG = %f, X = %f\n", ratio, sliceValue_);

		// Extract slice from data
		int bin = closestBin(sliceAxis_, sliceValue_);
		QString title;

		// Grab slice data
		if (sliceAxis_ == 0)
		{
			// Slice at fixed X, passing through closest point (if not interpolated) or actual value (if interpolated)
			currentSlice_.data().clear();
			for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next) currentSlice_.data().addPoint(slice->transformedData().z(), slice->transformedData().y(bin));
			currentSlice_.data().setName("X = " + QString::number(slices_.first()->transformedData().x(bin)));
		}
		else if (sliceAxis_ == 1)
		{
			return false;
		}
		else if (sliceAxis_ == 2)
		{
			// Slice through Z - i.e. original slice data
			currentSlice_.data().clear();
			currentSlice_.data() = slices_[bin]->data();
			title = "Z = " + QString::number(slices_[bin]->transformedData().z());
		}

		emit(sliceDataChanged());
		updateSurfaceTab();
		return true;
	}

	updateSurfaceTab();
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

	if (axisLogarithmic_[sliceAxis_]) return (axisInverted_[sliceAxis_] ? log10(limitMax_[sliceAxis_]/sliceValue_) : log10(sliceValue_));
	else return (axisInverted_[sliceAxis_] ? limitMax_[sliceAxis_] - sliceValue_ : sliceValue_);
}


/*
 * Public Functions
 */

// Return current slice data
ExtractedSlice UChromaWindow::currentSlice()
{
	return currentSlice_;
}
