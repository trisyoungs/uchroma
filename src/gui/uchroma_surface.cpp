/*
	*** Main Window - Surface 
	*** src/gui/uchroma_surface.cpp
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
 * Private Slots
 */

void UChromaWindow::on_SurfaceSliceNoneRadio_clicked(bool checked)
{
	if (refreshing_) return;
	sliceAxis_ = -1;

	// Update GUI
	sliceMonitorWindow_.update();
	updateDisplay();
}

void UChromaWindow::on_SurfaceSliceXRadio_clicked(bool checked)
{
	if (refreshing_) return;
	sliceAxis_ = 0;

	// Update GUI
	sliceMonitorWindow_.update();
	updateDisplay();
}

void UChromaWindow::on_SurfaceSliceYRadio_clicked(bool checked)
{
	if (refreshing_) return;
	sliceAxis_ = 1;

	// Update GUI
	sliceMonitorWindow_.update();
	updateDisplay();
}

void UChromaWindow::on_SurfaceSliceZRadio_clicked(bool checked)
{
	if (refreshing_) return;
	sliceAxis_ = 2;

	// Update GUI
	sliceMonitorWindow_.update();
	updateDisplay();
}
