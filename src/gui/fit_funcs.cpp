/*
	*** SliceMonitor Functions
	*** src/slicemonitor_funcs.cpp
	Copyright T. Youngs 2012-2013

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

#include "gui/slicemonitor.h"
#include "uchroma.h"

// Static singletons
UChromaWindow* SliceMonitorDialog::uChroma_ = NULL;

/*!
 * \brief Constructor
 */
SliceMonitorDialog::SliceMonitorDialog(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);
}

/*!
 * \brief Destructor
 */
SliceMonitorDialog::~SliceMonitorDialog()
{
}

/*
 * Link to UChroma
 */

// Set UChromaWindow pointer
void SliceMonitorDialog::setUChroma(UChromaWindow* ptr)
{
	uChroma_ = ptr;
}

/*
// Widgets / Slots / Reimplementations
*/
