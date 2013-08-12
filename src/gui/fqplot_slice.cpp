/*
	*** Main Window - Interactive Slicing / Fitting
	*** src/gui/fqplot_slice.cpp
	Copyright T. Youngs 2013

	This file is part of FQPlot.

	FQPlot is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	FQPlot is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FQPlot.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/fqplot.h"
#include "base/lineparser.h"

void FQPlotWindow::sliceAxisValueChanged(int axis, double value)
{
	sliceAxis_ = axis;
	sliceAxisValue_ = value;

	// Update label
	ui.SliceSelectorLabel->setEnabled(axis != -1);
	if (axis != -1)
	{
		// Update label
		ui.SliceSelectorLabel->setText( QString("Slice @ ") + QChar(88 + axis) + " = " + QString::number(value));
	}
	updateSliceData();
}

// Return axis bin value of closest point to supplied value
int FQPlotWindow::closestBin(int axis, double value)
{
	if (slices_.nItems() == 0) return -1;

	if (sliceAxis_ == 0)
	{
		// Check X array of first slice
		Array<double>& x = slices_.first()->data().arrayX();
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
	else if (sliceAxis_ == 1)
	{
		// ???
	}
	else if (sliceAxis_ == 2)
	{
		// Check z-values
		int closest = 0, n = 0;
		double delta, closestDelta = fabs(slices_.first()->z() - value);
		for (Slice* slice = slices_.first()->next; slice != NULL; slice = slice->next)
		{
			delta = fabs(slice->z() - value);
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

// Update slice data
void FQPlotWindow::updateSliceData()
{
	int bin = closestBin(sliceAxis_, sliceAxisValue_);
	// Grab slice data
	if (sliceAxis_ == 0)
	{
		// Slice at fixed X, passing through closest point (if not interpolated) or actual value (if interpolated)
		sliceData_.clear();
		for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next) sliceData_.addPoint(slice->z(), slice->data().y(bin));
		ui.SliceGraph->setStaticData(sliceData_, "X = " + QString::number(sliceData_.x(bin)));
	}
	else if (sliceAxis_ == 1)
	{
		// ???
	}
	else if (sliceAxis_ == 2)
	{
		// Slice through Z - i.e. original slice data
		sliceData_.clear();
		sliceData_ = slices_[bin]->data();
		ui.SliceGraph->setStaticData(sliceData_, "Z = " + QString::number(slices_[bin]->z()));
	}
}
