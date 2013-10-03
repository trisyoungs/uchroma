/*
	*** Main Window - Interactive Slicing / Fitting
	*** src/gui/uchroma_slice.cpp
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
#include "base/lineparser.h"

/*
 * Private Slots
 */

void UChromaWindow::on_AnalyseSliceNoneRadio_clicked(bool checked)
{
	ui.MainView->setSliceAxis(-1);
}

void UChromaWindow::on_AnalyseSliceXRadio_clicked(bool checked)
{
	ui.MainView->setSliceAxis(0);
}

void UChromaWindow::on_AnalyseSliceYRadio_clicked(bool checked)
{
	ui.MainView->setSliceAxis(1);
}

void UChromaWindow::on_AnalyseSliceZRadio_clicked(bool checked)
{
	ui.MainView->setSliceAxis(2);
}

void UChromaWindow::on_AnalyseSurfaceSliceMonitorCheck_clicked(bool checked)
{
}

void UChromaWindow::on_AnalyseSurfaceSliceShowLegendCheck_clicked(bool checked)
{
	ui.AnalyseSurfaceSliceGraph->setShowLegend(checked);
}

void UChromaWindow::on_AnalyseSurfaceSliceAutoScaleCheck_clicked(bool checked)
{
	ui.AnalyseSurfaceSliceGraph->setAutoScale(checked);
}

void UChromaWindow::on_AnalyseSurfaceSliceClearButton_clicked(bool checked)
{
	ui.AnalyseSurfaceSliceList->clear();
	ui.AnalyseSurfaceSliceGraph->removeAllDataSets();
}

void UChromaWindow::on_AnalyseSurfaceSliceSaveButton_clicked(bool checked)
{
	//if (ui.AnalyseSurfaceSliceList->currentIndex() == -1) return;
	
}

void UChromaWindow::on_AnalyseSurfaceSliceList_currentRowChanged(int index)
{
	
}

/*
 * Public Slots
 */

// Slice axis value changed
void UChromaWindow::surfaceSliceAxisValueChanged(int axis, double value)
{
	sliceAxis_ = axis;
	sliceAxisValue_ = value;
	if (sliceAxis_ == 0) ui.AnalyseSliceXRadio->setChecked(true);
	else if (sliceAxis_ == 1) ui.AnalyseSliceYRadio->setChecked(true);
	else if (sliceAxis_ == 2) ui.AnalyseSliceZRadio->setChecked(true);
	else ui.AnalyseSliceNoneRadio->setChecked(true);

	// Update label
	ui.SliceSelectorLabel->setEnabled(axis != -1);
	if (axis != -1)
	{
		// Update label
		ui.SliceSelectorLabel->setText( QString("Slice @ ") + QChar(88 + axis) + " = " + QString::number(value));
	}

	// If we are monitoring, construct a slice and add it as the static data of the graph
	if (ui.AnalyseSurfaceSliceMonitorCheck->isChecked()) updateSurfaceSliceData(true);
}

// Add slice to graph
void UChromaWindow::addSurfaceSlice(int axis, double value)
{
	updateSurfaceSliceData(false);
}

/*
 * Private Functions
 */

// Return axis bin value of closest point to supplied value
int UChromaWindow::closestBin(int axis, double value)
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

/*
 * Public Functions
 */

// Update/add slice data
void UChromaWindow::updateSurfaceSliceData(bool setStatic)
{
	if (setStatic && (!ui.AnalyseSurfaceSliceMonitorCheck->isChecked())) return;

	int bin = closestBin(sliceAxis_, sliceAxisValue_);
	QString title;

	// Grab slice data
	if (sliceAxis_ == 0)
	{
		// Slice at fixed X, passing through closest point (if not interpolated) or actual value (if interpolated)
		sliceData_.clear();
		for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next) sliceData_.addPoint(transformValue(slice->z(), 2), transformValue(slice->data().y(bin), 1));
		title = "X = " + QString::number(transformValue(slices_.first()->data().x(bin), 0));
	}
	else if (sliceAxis_ == 1)
	{
		return;
	}
	else if (sliceAxis_ == 2)
	{
		// Slice through Z - i.e. original slice data
		sliceData_.clear();
		sliceData_ = slices_[bin]->data();
		title = "Z = " + QString::number(slices_[bin]->z());
	}

	// Set static data, or add other dataset?
	if (setStatic) ui.AnalyseSurfaceSliceGraph->setStaticData(sliceData_, title);
	else
	{
		ui.AnalyseSurfaceSliceGraph->addDataSet(sliceData_, title, "Data");
		ui.AnalyseSurfaceSliceList->addItem(title);
	}
}
