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
	else if (axis == 1)
	{
		// ???
	}
	else if (axis == 2)
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

/*
 * Public Functions
 */

// Return current slice data
ExtractedSlice UChromaWindow::currentSlice()
{
	return currentSlice_;
}
