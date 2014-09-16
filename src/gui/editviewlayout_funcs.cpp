/*
	*** Edit ViewLayout Dialog
	*** src/gui/editviewlayout_funcs.cpp
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

#include "gui/editviewlayout.h"
#include "gui/uchroma.h"
#include "gui/selecttarget.h"
#include "templates/variantpointer.h"

/*
 * Window Functions
 */

// Constructor
EditViewLayoutDialog::EditViewLayoutDialog(UChromaWindow& parent) : QDialog(&parent), uChroma_(parent), layout_(parent)
{
	ui.setupUi(this);

	currentPane_ = NULL;

	refreshing_ = true;

	// Set items in PaneRoleCombo
	for (int n=0; n<ViewPane::nPaneRoles; ++n) ui.PaneRoleCombo->addItem(ViewPane::paneRole((ViewPane::PaneRole) n));

	refreshing_ = false;
}

// Destructor
EditViewLayoutDialog::~EditViewLayoutDialog()
{
}

// Window close event
void EditViewLayoutDialog::closeEvent(QCloseEvent* event)
{
	reject();
}

void EditViewLayoutDialog::reject()
{
	setResult(QDialog::Rejected);
	hide();
}

void EditViewLayoutDialog::on_OKButton_clicked(bool checked)
{
	accept();
}

void EditViewLayoutDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}

// Call dialog to edit specified ViewLayout
bool EditViewLayoutDialog::call(ViewLayout* target)
{
	// Check supplied var
	if (!target)
	{
		msg.print("Internal Error: NULL ViewLayout given to EditViewLayoutDialog::call().\n");
		return false;
	}

	// Copy supplied data
	layout_ = (*target);

	// Tell the Organiser widget which layout it's editing
	ui.Organiser->setViewLayout(&layout_);

	updateControls();


	// Call the dialog
	if (exec()) return true;
	else return false;

}

// Return ViewLayout stored in dialog
ViewLayout& EditViewLayoutDialog::viewLayout()
{
	return layout_;
}

/*
 * Convenience Functions
 */

/*
 * Slots - Pane Layout
 */

void EditViewLayoutDialog::on_GridNColumnsSpin_valueChanged(int value)
{
	if (refreshing_) return;

	layout_.setGrid(value, ui.GridNRowsSpin->value());
	updateControls();
}

void EditViewLayoutDialog::on_GridNRowsSpin_valueChanged(int value)
{
	if (refreshing_) return;

	layout_.setGrid(ui.GridNColumnsSpin->value(), value);
	updateControls();
}

void EditViewLayoutDialog::on_PaneAddButton_clicked(bool checked)
{
	if (refreshing_) return;

	currentPane_ = layout_.addPane("New Pane");

	updateControls();
}

void EditViewLayoutDialog::on_PaneRemoveButton_clicked(bool checked)
{
	if (refreshing_) return;

	layout_.removePane(currentPane_);

	updateControls();
}

void EditViewLayoutDialog::on_PaneNextButton_clicked(bool checked)
{
	if (refreshing_) return;

	if (currentPane_) currentPane_ = currentPane_->next;
	if (!currentPane_) currentPane_ = layout_.panes();

	updateControls();
}

void EditViewLayoutDialog::on_PanePreviousButton_clicked(bool checked)
{
	if (refreshing_) return;

	if (currentPane_) currentPane_ = currentPane_->prev;
	if (!currentPane_) currentPane_ = layout_.lastPane();

	updateControls();
}

void EditViewLayoutDialog::on_Organiser_currentPaneChanged(int gridX, int gridY)
{
	if (refreshing_) return;

	// Get pane at the specified grid coordinates
	currentPane_ = layout_.paneAtGrid(gridX, gridY);

	updateControls();
}

/*
 * Slots - Pane Basic Info
 */

void EditViewLayoutDialog::on_PaneNameEdit_textChanged(QString text)
{
	if (refreshing_) return;

	if (currentPane_) currentPane_->setName(text);
	ui.Organiser->update();
}

void EditViewLayoutDialog::on_PaneRoleCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;

	if (currentPane_) currentPane_->setRole((ViewPane::PaneRole) index);
	updateControls();
}

void EditViewLayoutDialog::on_Pane2DCheck_clicked(bool checked)
{
	if (refreshing_) return;

	if (currentPane_) currentPane_->setTwoDimensional(checked);
}

/*
 * Slots - Pane Targets
 */

void EditViewLayoutDialog::on_PaneTargetsList_currentRowChanged(int index)
{
	ui.PaneRemoveTargetButton->setEnabled(index != -1);
}

void EditViewLayoutDialog::on_PaneAddTargetButton_clicked(bool checked)
{
	SelectTargetDialog targetDialog(this);
	targetDialog.populateLists(currentPane_, layout_.panes(), uChroma_.collections());
	if (targetDialog.exec())
	{
		// Get lists of panes and collections, and add them to the targets list
		RefList<ViewPane,bool> panes = targetDialog.selectedPanes();
		for (RefListItem<ViewPane,bool>* ri = panes.first(); ri != NULL; ri = ri->next) currentPane_->addRoleTargetPane(ri->item);
		RefList<Collection,bool> collections = targetDialog.selectedCollections();
		for (RefListItem<Collection,bool>* rj = collections.first(); rj != NULL; rj = rj->next) currentPane_->addRoleTargetCollection(rj->item);
	}

	updateControls();
}

void EditViewLayoutDialog::on_PaneRemoveTargetButton_clicked(bool checked)
{
	// Get list of current items
	QList<QListWidgetItem*> items = ui.PaneTargetsList->selectedItems();
	for (int n=0; n<items.count(); ++n)
	{
		if (items[n]->text().startsWith("(C)")) currentPane_->removeRoleTargetCollection(VariantPointer<Collection>(items[n]->data(Qt::UserRole)));
		else if (items[n]->text().startsWith("(P)")) currentPane_->removeRoleTargetPane(VariantPointer<ViewPane>(items[n]->data(Qt::UserRole)));
	}

	updateControls();
}

/*
 * Slots - Pane Role Options
 */

/*
 * Update
 */

// Update controls
void EditViewLayoutDialog::updateControls()
{
	refreshing_ = true;

	// Make a check on the currentPane_ to see if it still exists in the layout...
	if (currentPane_ && (!layout_.containsPane(currentPane_))) currentPane_ = layout_.panes();

	ui.Organiser->update();

	ui.GridNColumnsSpin->setValue(layout_.nColumns());
	ui.GridNRowsSpin->setValue(layout_.nRows());

	// Check that a valid pane is selected in the organiser
	ui.PaneNameEdit->setEnabled(currentPane_);
	ui.PaneRoleCombo->setEnabled(currentPane_);
	ui.PaneRemoveButton->setEnabled(currentPane_);
	if (currentPane_)
	{
		ui.PaneNameEdit->setText(currentPane_->name());
		ui.PaneRoleCombo->setCurrentIndex(currentPane_->role());
		ui.Pane2DCheck->setChecked(currentPane_->twoDimensional());

		// Update targets list
		ui.PaneTargetsList->clear();
		for (RefListItem<ViewPane,bool>* ri = currentPane_->roleTargetPanes(); ri != NULL; ri = ri->next)
		{
			QListWidgetItem* item;
			item = new QListWidgetItem(ui.PaneTargetsList);
			item->setText("(P) " + ri->item->name());
			item->setData(Qt::UserRole, VariantPointer<ViewPane>(ri->item));
		}
		for (RefListItem<Collection,TargetData>* ri = currentPane_->roleTargetCollections(); ri != NULL; ri = ri->next)
		{
			QListWidgetItem* item;
			item = new QListWidgetItem(ui.PaneTargetsList);
			item->setText("(C) " + ri->item->name());
			item->setData(Qt::UserRole, VariantPointer<Collection>(ri->item));
		}

		// Set the correct stack page for the selected role
		ui.PaneRoleStack->setCurrentIndex(currentPane_->role());
		// -- Set stack page controls
// 		ui.
	}

	refreshing_ = false;
}