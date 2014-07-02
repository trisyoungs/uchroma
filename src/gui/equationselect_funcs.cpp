/*
	*** Fit Widget Functions
	*** src/gui/fit_funcs.cpp
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

#include "gui/equationselect.h"
#include "templates/variantpointer.h"

// Static list of fit equations
Equation Equation::equations[] = {
	{ "Exponential1",		"A1*exp(-x/B1)",
		"Single exponential with prefactor" },
	{ "Exponential2",		"A1*exp(-x/B1) + A2*exp(-x/B2)",
		"Double, summed exponential" },
	{ "Exponential3",		"A1*exp(-x/B1) + A2*exp(-x/B2) + A3*exp(-x/B3)",
		"Triple, summed exponential" },
};

// Constructor
EquationSelectDialog::EquationSelectDialog(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);

	selectedEquation_ = NULL;
	// Populate the equations list
	QListWidgetItem* item;
	for (int n=0; n<Equation::nEquations; ++n)
	{
		item = new QListWidgetItem(ui.EquationList);
		item->setText(Equation::equations[n].name);
		item->setData(Qt::UserRole, VariantPointer<Equation>(&Equation::equations[n]));
	}
}

// Destructor
EquationSelectDialog::~EquationSelectDialog()
{
}

/*
 * Equation
 */

// Return selected equation
Equation EquationSelectDialog::selectedEquation()
{
	return (*selectedEquation_);
}

/*
 * Slots
 */

void EquationSelectDialog::on_EquationList_currentRowChanged(int index)
{
	selectedEquation_ = &Equation::equations[index];

	// Update labels
	ui.EquationNameLabel->setText(selectedEquation_->name);
	ui.EquationTextLabel->setText(selectedEquation_->equationText);
	ui.EquationDescriptionLabel->setText(selectedEquation_->description);
}

void EquationSelectDialog::on_EquationList_itemDoubleClicked(QListWidgetItem* item)
{
	selectedEquation_ = VariantPointer<Equation>(item->data(Qt::UserRole));
	accept();
}

void EquationSelectDialog::on_CloseButton_clicked(bool checked)
{
	reject();
}

void EquationSelectDialog::on_SelectButton_clicked(bool checked)
{
	accept();
}
