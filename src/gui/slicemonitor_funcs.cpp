/*
	*** Slice Monitor Functions
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

/*!
 * \brief Constructor
 */
SliceMonitorWindow::SliceMonitorWindow(UChromaWindow& parent) : QWidget(&parent), uChroma_(parent)
{
	ui.setupUi(this);

	QWidget::setWindowFlags(Qt::Tool);

	refreshing_ = false;
}

/*!
 * \brief Destructor
 */
SliceMonitorWindow::~SliceMonitorWindow()
{
}

// Window close event
void SliceMonitorWindow::closeEvent(QCloseEvent* event)
{
	emit(windowClosed(false));
}

/*
 * Slots
 */

/*
 * Update
 */

// Update controls and show window
void SliceMonitorWindow::updateAndShow()
{
	updateControls();
	show();
	move(uChroma_.centrePos() - QPoint(width()/2, height()/2));
}

// Update controls
void SliceMonitorWindow::updateControls(bool force)
{
	// If the window isn't visible, do nothing...
	if ((!isVisible()) && (!force) ) return;

	if (uChroma_.sliceAxis() == -1)
	{
		ui.SurfaceSliceNoneRadio->setChecked(true);
		ui.SliceSelectorLabel->setText("");
		ui.SliceSelectorLabel->setEnabled(false);
	}
	else
	{
		if (uChroma_.sliceAxis()== 0) ui.SurfaceSliceXRadio->setChecked(true);
		else if (uChroma_.sliceAxis() == 1) ui.SurfaceSliceYRadio->setChecked(true);
		else if (uChroma_.sliceAxis() == 2) ui.SurfaceSliceZRadio->setChecked(true);
		Dnchar s(-1, "%c = %f", char(88+uChroma_.sliceAxis()), uChroma_.sliceValue());
		ui.SliceSelectorLabel->setText(s.get());
		ui.SliceSelectorLabel->setEnabled(true);
	}
}