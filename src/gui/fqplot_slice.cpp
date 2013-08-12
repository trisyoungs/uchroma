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
		ui.SliceSelectorLabel->setText( QString("Slice @ ") + QChar(80 + axis) + " = " + QString::number(value));
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
		double delta, lastDelta = (x[1] - x[0])*1.0e9;
		for (int n=0; n<x.nItems(); ++n)
		{
			delta = value - x[n];
			XXX
		}
	}
	else if (sliceAxis_ == 1)
	{
		// ???
	}
	else if (sliceAxis_ == 2)
	{
		// Slice through Z - i.e. original slice data
		//
	}
	else return -1;
}

// Update slice data
void FQPlotWindow::updateSliceData()
{
	// Grab slice data
	if (sliceAxis_ == 0)
	{
		// Slice at fixed X, passing through closest point (if not interpolated) or actual value (if interpolated)
	}
	else if (sliceAxis_ == 1)
	{
		// ???
	}
	else if (sliceAxis_ == 2)
	{
		// Slice through Z - i.e. original slice data
		//
	}
}
