/*
	*** uChroma Layout Window
	*** src/gui/layout_funcs.cpp
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

#include "gui/layout.h"
#include "gui/uchroma.h"

/*
 * Window Functions
 */

// Constructor
LayoutWindow::LayoutWindow(UChromaWindow& parent) : QWidget(&parent), uChroma_(parent)
{
	ui.setupUi(this);

	QWidget::setWindowFlags(Qt::Tool);

	refreshing_ = true;

	currentPane_ = NULL;
	// Set items in PaneRoleCombo
	for (int n=0; n<ViewPane::nPaneRoles; ++n) ui.PaneRoleCombo->addItem(ViewPane::paneRole((ViewPane::PaneRole) n));

	refreshing_ = false;
}

// Destructor
LayoutWindow::~LayoutWindow()
{
}

// Window close event
void LayoutWindow::closeEvent(QCloseEvent *event)
{
	emit(windowClosed(false));
}

/*
 * Convenience Functions
 */

/*
 * Slots
 */

void LayoutWindow::on_GridNColumnsSpin_valueChanged(int value)
{
	if (refreshing_) return;

	uChroma_.viewLayout()->setGrid(value, ui.GridNRowsSpin->value());
	updateControls();
	emit(updateMainDisplay());
}

void LayoutWindow::on_GridNRowsSpin_valueChanged(int value)
{
	if (refreshing_) return;

	uChroma_.viewLayout()->setGrid(ui.GridNColumnsSpin->value(), value);
	updateControls();
	emit(updateMainDisplay());
}

void LayoutWindow::on_PaneAddButton_clicked(bool checked)
{
	if (refreshing_) return;

	currentPane_ = uChroma_.viewLayout()->addPane("New Pane");

	updateControls();
}

void LayoutWindow::on_PaneRemoveButton_clicked(bool checked)
{
	if (refreshing_) return;
}

void LayoutWindow::on_PaneNextButton_clicked(bool checked)
{
	if (refreshing_) return;

	if (currentPane_) currentPane_ = currentPane_->next;
	if (!currentPane_) currentPane_ = uChroma_.viewLayout()->panes();

	updateControls();
}

void LayoutWindow::on_PanePreviousButton_clicked(bool checked)
{
	if (refreshing_) return;

	if (currentPane_) currentPane_ = currentPane_->prev;
	if (!currentPane_) currentPane_ = uChroma_.viewLayout()->lastPane();

	updateControls();
}

void LayoutWindow::on_PaneNameEdit_textChanged(QString text)
{
	if (refreshing_) return;

	if (currentPane_) currentPane_->setName(text);
	ui.Organiser->update();
}

void LayoutWindow::on_PaneRoleCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;

	if (currentPane_) currentPane_->setRole((ViewPane::PaneRole) index);
	ui.Organiser->update();
}

void LayoutWindow::on_Organiser_currentPaneChanged(int gridX, int gridY)
{
	if (refreshing_) return;

	// Get pane at the specified grid coordinates
	currentPane_ = uChroma_.viewLayout()->paneAtGrid(gridX, gridY);

	updateControls();
}

/*
 * Update
 */

// Update controls and show window
void LayoutWindow::updateAndShow()
{
	updateControls(true);
	show();
	move(uChroma_.centrePos() - QPoint(width()/2, height()/2));
}

// Update controls
void LayoutWindow::updateControls(bool force)
{
	// If the window isn't visible, do nothing...
	if ((!isVisible()) && (!force) ) return;

	refreshing_ = true;

	// Make a check on the currentPane_ to see if it still exists in the layout...
	if (currentPane_ && (!uChroma_.viewLayout()->containsPane(currentPane_))) currentPane_ = uChroma_.viewLayout()->panes();

	ui.Organiser->update();

	ui.GridNColumnsSpin->setValue(uChroma_.viewLayout()->nColumns());
	ui.GridNRowsSpin->setValue(uChroma_.viewLayout()->nRows());

	// Check that a valid pane is selected in the organiser
	ui.PaneNameEdit->setEnabled(currentPane_);
	ui.PaneRoleCombo->setEnabled(currentPane_);
	ui.PaneRemoveButton->setEnabled(currentPane_);
	if (currentPane_)
	{
		ui.PaneNameEdit->setText(currentPane_->name());
		ui.PaneRoleCombo->setCurrentIndex(currentPane_->role());
	}

	refreshing_ = false;
}
