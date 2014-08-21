/*
	*** uChroma View Window
	*** src/gui/view_funcs.cpp
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

#include "gui/view.h"
#include "uchroma.h"
#include "templates/reflist.h"

/*
 * Window Functions
 */

// Constructor
ViewWindow::ViewWindow(UChromaWindow& parent) : QWidget(&parent), uChroma_(parent)
{
	ui.setupUi(this);

	QWidget::setWindowFlags(Qt::Tool);

	refreshing_ = false;
}

// Destructor
ViewWindow::~ViewWindow()
{
}

// Window close event
void ViewWindow::closeEvent(QCloseEvent *event)
{
	emit(windowClosed(false));
}

/*
 * Convenience Functions
 */

/*
 * Slots
 */

void ViewWindow::on_ViewBoundingBoxNoneRadio_clicked(bool checked)
{
	if (refreshing_ || (!uChroma_.currentViewPane())) return;

	uChroma_.currentViewPane()->setBoundingBox(ViewPane::NoBox);

	// Update display
	uChroma_.setAsModified();
	uChroma_.updateDisplay();
}

void ViewWindow::on_ViewBoundingBoxPlaneRadio_clicked(bool checked)
{
	if (refreshing_ || (!uChroma_.currentViewPane())) return;

	uChroma_.currentViewPane()->setBoundingBox(ViewPane::PlaneBox);

	// Update display
	uChroma_.setAsModified();
	uChroma_.updateDisplay();
}

void ViewWindow::on_ViewBoundingBoxCubeRadio_clicked(bool checked)
{
	if (refreshing_ || (!uChroma_.currentViewPane())) return;

	uChroma_.currentViewPane()->setBoundingBox(ViewPane::CubeBox);

	// Update display
	uChroma_.setAsModified();
	uChroma_.updateDisplay();
}

void ViewWindow::on_ViewBoundingBoxPlaneYSpin_valueChanged(double value)
{
	if (refreshing_ || (!uChroma_.currentViewPane())) return;

	uChroma_.currentViewPane()->setBoundingBoxPlaneY(value);

	// Update display
	uChroma_.setAsModified();
	uChroma_.updateDisplay();
}

void ViewWindow::on_ViewBoundingBoxPlaneYSetMinimumButton_clicked(bool checked)
{
	// Check current display pane
	if (!uChroma_.currentViewPane()) return;

	ui.ViewBoundingBoxPlaneYSpin->setValue(uChroma_.currentViewPane()->axes().axisMin(1));
}

void ViewWindow::on_ViewBoundingBoxPlaneYSetZeroButton_clicked(bool checked)
{
	ui.ViewBoundingBoxPlaneYSpin->setValue(0.0);
}

void ViewWindow::on_ViewBoundingBoxPlaneYSetMaximumButton_clicked(bool checked)
{
	// Check current display pane
	if (!uChroma_.currentViewPane()) return;

	ui.ViewBoundingBoxPlaneYSpin->setValue(uChroma_.currentViewPane()->axes().axisMax(1));
}

// -- General Options
void ViewWindow::on_ViewLabelsFaceViewerCheck_clicked(bool checked)
{
	if (refreshing_) return;
	uChroma_.setLabelFaceViewer(checked);

	// Update display
	uChroma_.setAsModified();
	uChroma_.updateDisplay();
}

void ViewWindow::on_ViewLabelPointSizeSpin_valueChanged(double value)
{
	if (refreshing_ || (!uChroma_.currentViewPane())) return;
	
	uChroma_.currentViewPane()->setLabelPointSize(value);

	// Update display
	uChroma_.setAsModified();
	uChroma_.updateDisplay();
}

void ViewWindow::on_ViewTitlePointSizeSpin_valueChanged(double value)
{
	if (refreshing_ || (!uChroma_.currentViewPane())) return;
	
	uChroma_.currentViewPane()->setTitlePointSize(value);

	// Update display
	uChroma_.setAsModified();
	uChroma_.updateDisplay();
}

/*
 * Update
 */

// Update controls and show window
void ViewWindow::updateAndShow()
{
	updateControls();
	show();
	move(uChroma_.centrePos() - QPoint(width()/2, height()/2));
}

// Update controls
void ViewWindow::updateControls(bool force)
{
	// If the window isn't visible, do nothing...
	if ((!isVisible()) && (!force) ) return;

	if (!uChroma_.currentViewPane()) return;
	
	refreshing_ = true;

	// Bounding box
	if (uChroma_.currentViewPane()->boundingBox() == ViewPane::NoBox) ui.ViewBoundingBoxNoneRadio->setChecked(true);
	else if (uChroma_.currentViewPane()->boundingBox() == ViewPane::PlaneBox) ui.ViewBoundingBoxPlaneRadio->setChecked(true);
	else if (uChroma_.currentViewPane()->boundingBox() == ViewPane::CubeBox) ui.ViewBoundingBoxCubeRadio->setChecked(true);
	ui.ViewBoundingBoxPlaneYSpin->setValue(uChroma_.currentViewPane()->boundingBoxPlaneY());

	// Label Options
	ui.ViewLabelPointSizeSpin->setValue(uChroma_.currentViewPane()->labelPointSize());
	ui.ViewTitlePointSizeSpin->setValue(uChroma_.currentViewPane()->titlePointSize());
	ui.ViewLabelsFaceViewerCheck->setChecked(uChroma_.labelFaceViewer());

	refreshing_ = false;
}
