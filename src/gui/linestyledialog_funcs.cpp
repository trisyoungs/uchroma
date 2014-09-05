/*
	*** Line Style Dialog
	*** src/gui/linestyledialog_funcs.cpp
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

#include "gui/linestyledialog.h"
#include "base/messenger.h"

// Constructor
LineStyleDialog::LineStyleDialog(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
}

// Destructor
LineStyleDialog::~LineStyleDialog()
{
}

/*
 * Window Functions
 */

// Window close event
void LineStyleDialog::closeEvent(QCloseEvent* event)
{
	reject();
}

void LineStyleDialog::reject()
{
	// Revert to saved data
	(*lineStyleTarget_) = lineStyleBackup_;

	setResult(QDialog::Rejected);
	hide();
}

void LineStyleDialog::on_OKButton_clicked(bool checked)
{
	accept();
}

void LineStyleDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}

// Call dialog to edit specified reference
bool LineStyleDialog::call(LineStyle* target)
{
	// Check supplied var
	if (!target)
	{
		msg.print("Internal Error: NULL LineStyle given to LineStyleDialog::call().\n");
		return false;
	}

	// Take copy of supplied data
	lineStyleTarget_ = target;
	lineStyleBackup_ = (*lineStyleTarget_);

	refreshing_ = true;

	// Create entries in LineStippleCombo



	// Update widgets
	ui.LineWidthSpin->setValue(lineStyleTarget_->width());
	ui.LineColourButton->setColour(lineStyleTarget_->colour());
	ui.LineStippleCombo->setCurrentIndex(lineStyleTarget_->stipple());

	refreshing_ = false;

	// Call the dialog
	if (exec()) return true;
	else return false;
}

/*
 * Slots
 */

void LineStyleDialog::on_LineColourButton_clicked(bool checked)
{
	if (ui.LineColourButton->selectColour()) lineStyleTarget_->setColour(ui.LineColourButton->colour());
}

void LineStyleDialog::on_LineWidthSpin_valueChanged(double value)
{
	lineStyleTarget_->setWidth(value);
}

void LineStyleDialog::on_LineStippleCombo_currentIndexChanged(int currentIndex)
{
	lineStyleTarget_->setStipple( (LineStipple::StippleType) currentIndex);
}