/*
	*** Data - SetZ Functions
	*** src/gui/data_setz_funcs.cpp
	Copyright T. Youngs 2012-2014

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

#include "gui/operate_bgsub.h"
#include "base/collection.h"
#include "base/lineparser.h"
#include "expression/variable.h"
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

// Constructor
OperateBGSubDialog::OperateBGSubDialog(QWidget* parent, Collection* targetCollection) : QDialog(parent)
{
	ui.setupUi(this);

	targetCollection_ = targetCollection;

	updateControls();
}

// Destructor
OperateBGSubDialog::~OperateBGSubDialog()
{
}

/*
 * Slots
 */


void OperateBGSubDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}

void OperateBGSubDialog::on_ApplyButton_clicked(bool checked)
{
	// Set some z's!
	bool result;
// 	if (ui.FromEquationRadio->isChecked()) result = setZFromEquation();
// 	else if (ui.FromTimeStampsRadio->isChecked()) result = setZFromTimeStamps();
// 	else if (ui.FromSourceFilesRadio->isChecked()) result = setZFromSourceFiles();
	if (result) accept();
	else reject();
}

/*
 * Functions
 */

/*
 * Update
 */

// Update controls
void OperateBGSubDialog::updateControls()
{

}
