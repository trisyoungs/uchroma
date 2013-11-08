/*
	*** Main Window - Slices
	*** src/gui/uchroma_slices.cpp
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

/*
 * Private Slots
 */

void UChromaWindow::on_SlicesClearButton_clicked(bool checked)
{
	ui.SlicesTree->clear();
// 	ui.AnalyseSurfaceSliceGraph->removeAllDataSets();
}

void UChromaWindow::on_SlicesSaveButton_clicked(bool checked)
{
	//if (ui.AnalyseSurfaceSliceList->currentIndex() == -1) return;
	
}

void UChromaWindow::on_SlicesList_currentRowChanged(int index)
{
}
