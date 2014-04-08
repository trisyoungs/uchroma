/*
	*** Main Window - Collection Style Functions 
	*** src/gui/uchroma_collection_style.cpp
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
#include "templates/reflist.h"

// Update Gradient Bar
void UChromaWindow::updateGradientBar()
{
	// Check for a current Collection
	if (!currentCollection_) return;
	
	ui.ColourGradient->setColourScale(currentCollection_->colourScale());
}

/*
 * Style
 */

void UChromaWindow::on_StyleCombo_currentIndexChanged(int index)
{
	// Check for window refreshing or invalid Collection
	if (refreshing_ || (!currentCollection_)) return;

	currentCollection_->setDisplayStyle( (Collection::DisplayStyle) index );
	updateDisplay();
}

/*
 * Single Colour
 */

void UChromaWindow::on_ColourSingleColourRadio_clicked(bool checked)
{
	// Check for window refreshing or invalid Collection
	if (refreshing_ || (!currentCollection_)) return;

	currentCollection_->setColourSource(Collection::SingleColourSource);
	updateGradientBar();
	updateDisplay();
}

void UChromaWindow::on_ColourSingleColourButton_clicked(bool checked)
{
	// Check for window refreshing or invalid Collection
	if (refreshing_ || (!currentCollection_)) return;

	if (ui.ColourSingleColourButton->selectColour())
	{
		currentCollection_->setColourScalePoint(Collection::SingleColourSource, ui.ColourSingleColourButton->colour());
		updateGradientBar();
		updateDisplay();
	}
}

/*
 * RGB Gradient
 */

void UChromaWindow::on_ColourRGBGradientRadio_clicked(bool checked)
{
	// Check for a current Collection
	if (!currentCollection_) return;

	currentCollection_->setColourSource(Collection::RGBGradientSource);
	updateGradientBar();
	updateDisplay();
}

void UChromaWindow::on_ColourRGBGradientAButton_clicked(bool checked)
{
	// Check for a current Collection
	if (!currentCollection_) return;

	if (ui.ColourRGBGradientAButton->selectColour())
	{
		currentCollection_->setColourScalePoint(Collection::RGBGradientSource, ui.ColourRGBGradientAButton->colour(), ui.ColourRGBGradientASpin->value(), 0);
		updateGradientBar();
		updateDisplay();
	}
}

void UChromaWindow::on_ColourRGBGradientASpin_valueChanged(double value)
{
	// Check for a current Collection
	if (!currentCollection_) return;

	currentCollection_->setColourScalePoint(Collection::RGBGradientSource, ui.ColourRGBGradientAButton->colour(), ui.ColourRGBGradientASpin->value(), 0);
	updateGradientBar();
	updateDisplay();
}

void UChromaWindow::on_ColourRGBGradientASetMinimumButton_clicked(bool checked)
{
	// Check for a current Collection
	if (!currentCollection_) return;

	ui.ColourRGBGradientASpin->setValue(axisLimitMin_.y);
}

void UChromaWindow::on_ColourRGBGradientASetMaximumButton_clicked(bool checked)
{
	// Check for a current Collection
	if (!currentCollection_) return;

	ui.ColourRGBGradientASpin->setValue(axisLimitMax_.y);
}

void UChromaWindow::on_ColourRGBGradientBButton_clicked(bool checked)
{
	// Check for a current Collection
	if (!currentCollection_) return;

	if (ui.ColourRGBGradientBButton->selectColour())
	{
		currentCollection_->setColourScalePoint(Collection::RGBGradientSource, ui.ColourRGBGradientBButton->colour(), ui.ColourRGBGradientASpin->value(), 1);
		updateGradientBar();
		updateDisplay();
	}
}

void UChromaWindow::on_ColourRGBGradientBSpin_valueChanged(double value)
{
	// Check for window refreshing or invalid Collection
	if (refreshing_ || (!currentCollection_)) return;

	currentCollection_->setColourScalePoint(Collection::RGBGradientSource, ui.ColourRGBGradientBButton->colour(), ui.ColourRGBGradientASpin->value(), 1);
	updateGradientBar();
	updateDisplay();
}

void UChromaWindow::on_ColourRGBGradientBSetMinimumButton_clicked(bool checked)
{
	// Check for a current Collection
	if (!currentCollection_) return;

	ui.ColourRGBGradientBSpin->setValue(axisLimitMin_.y);
}

void UChromaWindow::on_ColourRGBGradientBSetMaximumButton_clicked(bool checked)
{
	// Check for a current Collection
	if (!currentCollection_) return;

	ui.ColourRGBGradientBSpin->setValue(axisLimitMax_.y);
}

/*
 * HSV Gradient
 */

void UChromaWindow::on_ColourHSVGradientRadio_clicked(bool checked)
{
	// Check for window refreshing or invalid Collection
	if (refreshing_ || (!currentCollection_)) return;

	currentCollection_->setColourSource(Collection::HSVGradientSource);
	updateGradientBar();
	updateDisplay();
}

void UChromaWindow::on_ColourHSVGradientAButton_clicked(bool checked)
{
	// Check for a current Collection
	if (!currentCollection_) return;

	if (ui.ColourHSVGradientAButton->selectColour())
	{
		currentCollection_->setColourScalePoint(Collection::HSVGradientSource, ui.ColourHSVGradientAButton->colour(), ui.ColourHSVGradientASpin->value(), 0);
		updateGradientBar();
		updateDisplay();
	}
}

void UChromaWindow::on_ColourHSVGradientASpin_valueChanged(double value)
{
	// Check for window refreshing or invalid Collection
	if (refreshing_ || (!currentCollection_)) return;

	currentCollection_->setColourScalePoint(Collection::HSVGradientSource, ui.ColourHSVGradientAButton->colour(), ui.ColourHSVGradientASpin->value(), 0);
	updateGradientBar();
	updateDisplay();
}

void UChromaWindow::on_ColourHSVGradientASetMinimumButton_clicked(bool checked)
{
	// Check for a current Collection
	if (!currentCollection_) return;

	ui.ColourHSVGradientASpin->setValue(axisLimitMin_.y);
}

void UChromaWindow::on_ColourHSVGradientASetMaximumButton_clicked(bool checked)
{
	// Check for a current Collection
	if (!currentCollection_) return;

	ui.ColourHSVGradientASpin->setValue(axisLimitMax_.y);
}

void UChromaWindow::on_ColourHSVGradientBButton_clicked(bool checked)
{
	// Check for a current Collection
	if (!currentCollection_) return;

	if (ui.ColourHSVGradientBButton->selectColour())
	{
		currentCollection_->setColourScalePoint(Collection::HSVGradientSource, ui.ColourHSVGradientBButton->colour(), ui.ColourHSVGradientBSpin->value(), 1);
		updateGradientBar();
		updateDisplay();
	}
}

void UChromaWindow::on_ColourHSVGradientBSpin_valueChanged(double value)
{
	// Check for window refreshing or invalid Collection
	if (refreshing_ || (!currentCollection_)) return;

	currentCollection_->setColourScalePoint(Collection::HSVGradientSource, ui.ColourHSVGradientBButton->colour(), ui.ColourHSVGradientBSpin->value(), 1);
	updateGradientBar();
	updateDisplay();
}

void UChromaWindow::on_ColourHSVGradientBSetMinimumButton_clicked(bool checked)
{
	// Check for a current Collection
	if (!currentCollection_) return;

	ui.ColourHSVGradientBSpin->setValue(axisLimitMin_.y);
}

void UChromaWindow::on_ColourHSVGradientBSetMaximumButton_clicked(bool checked)
{
	// Check for a current Collection
	if (!currentCollection_) return;

	ui.ColourHSVGradientBSpin->setValue(axisLimitMax_.y);
}

/*
 * Custom Gradient
 */

void UChromaWindow::on_ColourCustomGradientRadio_clicked(bool checked)
{
	// Check for window refreshing or invalid Collection
	if (refreshing_ || (!currentCollection_)) return;

	currentCollection_->setColourSource(Collection::CustomGradientSource);
	updateGradientBar();
	updateDisplay();
}

void UChromaWindow::on_ColourCustomGradientTable_itemSelectionChanged()
{
	bool selected = (ui.ColourCustomGradientTable->selectedItems().count() != 0);
	ui.ColourCustomGradientRemoveButton->setEnabled(selected);
}

void UChromaWindow::on_ColourCustomGradientTable_cellDoubleClicked(int row, int column)
{
	// Check for window refreshing or invalid Collection or invalid table column
	if (refreshing_ || (!currentCollection_) || (column == 0)) return;

	// Get colour item data (also contains ColourScalePoint index)
	QTableWidgetItem* item = ui.ColourCustomGradientTable->item(row, 1);
	if (!item) return;
	int cspId = item->data(Qt::UserRole).toInt();

        // Request a colour dialog
        QColor newcol = QColorDialog::getColor(item->backgroundColor().rgba(), this, "Select new colour", QColorDialog::ShowAlphaChannel);
        if (!newcol.isValid()) return;

	// Get point value
	QTableWidgetItem* valueItem = ui.ColourCustomGradientTable->item(row, 0);
	if (!valueItem) return;

	// Update the colourscale point
	currentCollection_->setColourScalePoint(Collection::CustomGradientSource, newcol, valueItem->text().toDouble(), cspId);

	// Update table and refresh surface
	item->setBackgroundColor(newcol);
	updateGradientBar();
	setAsModified();
}

void UChromaWindow::on_ColourCustomGradientTable_cellChanged(int row, int column)
{
	// Check for window refreshing or invalid Collection or invalid table column
	if (refreshing_ || (!currentCollection_) || (column == 1)) return;

	// Get colour item data (also contains ColourScalePoint index)
	QTableWidgetItem* item = ui.ColourCustomGradientTable->item(row, 1);
	if (!item) return;
	int cspId = item->data(Qt::UserRole).toInt();

	// Get point value
	QTableWidgetItem* valueItem = ui.ColourCustomGradientTable->item(row, 0);
	if (!valueItem) return;
	
	// Value of this point has changed, so must update the list and refresh the surface and necessary widgets
	currentCollection_->setColourScalePoint(Collection::CustomGradientSource, item->backgroundColor(), valueItem->text().toDouble(), cspId);

	// Refresh table
	updateCollectionColourTab();
	setAsModified();
}

void UChromaWindow::on_ColourCustomGradientAddButton_clicked(bool checked)
{
	// Check for invalid Collection
	if (!currentCollection_) return;

	currentCollection_->addCustomColourScalePoint();

	// Refresh table and surface
	updateCollectionColourTab();
	updateDisplay();
	setAsModified();
}

void UChromaWindow::on_ColourCustomGradientRemoveButton_clicked(bool checked)
{
	// Check for invalid Collection
	if (!currentCollection_) return;

	if (ui.ColourCustomGradientTable->selectedItems().count() == 0) return;

	// Get list of items to delete
	int lastId = -1;
	RefList<ColourScalePoint,int> toDelete;
	foreach(QTableWidgetItem* item, ui.ColourCustomGradientTable->selectedItems())
	{
		// Get point id from item
		int cspId = item->data(Qt::UserRole).toInt();
		if (lastId == cspId) continue;
		lastId = cspId;
		toDelete.addUnique(currentCollection_->customColourScalePoint(cspId));
	}

	// Now delete the points
	for (RefListItem<ColourScalePoint,int>* ri = toDelete.first(); ri != NULL; ri = ri->next) currentCollection_->removeCustomColourScalePoint(ri->item);

	// Refresh table and surface
	updateCollectionColourTab();
	updateDisplay();
	setAsModified();
}

/*
 * Alpha Control
 */

void UChromaWindow::on_ColourAlphaOwnAlphaRadio_clicked(bool checked)
{
	// Check for window refreshing or invalid Collection
	if (refreshing_ || (!currentCollection_)) return;

	currentCollection_->setAlphaControl(Collection::OwnAlpha);
	updateDisplay();
}

void UChromaWindow::on_ColourAlphaFixedAlphaRadio_clicked(bool checked)
{
	// Check for window refreshing or invalid Collection
	if (refreshing_ || (!currentCollection_)) return;

	currentCollection_->setAlphaControl(Collection::FixedAlpha);
	updateDisplay();
}

void UChromaWindow::on_ColourAlphaFixedAlphaSpin_valueChanged(int value)
{
	// Check for window refreshing or invalid Collection
	if (refreshing_ || (!currentCollection_)) return;

	currentCollection_->setFixedAlpha(value);
	updateDisplay();
}

/*
 * Update
 */
void UChromaWindow::updateCollectionColourTab()
{
	// Check for invalid Collection
	if (!currentCollection_) return;

	refreshing_ = true;

	// Style Combo
	ui.StyleCombo->setCurrentIndex(currentCollection_->displayStyle());

	// Single Colour
	ui.ColourSingleColourButton->setColour(currentCollection_->colourScalePointColour(Collection::SingleColourSource));

	// RGB Gradient
	ui.ColourRGBGradientAButton->setColour(currentCollection_->colourScalePointColour(Collection::RGBGradientSource, 0));
	ui.ColourRGBGradientASpin->setValue(currentCollection_->colourScalePointValue(Collection::RGBGradientSource, 0));
	ui.ColourRGBGradientBButton->setColour(currentCollection_->colourScalePointColour(Collection::RGBGradientSource, 1));
	ui.ColourRGBGradientBSpin->setValue(currentCollection_->colourScalePointValue(Collection::RGBGradientSource, 1));

	// HSV Gradient
	ui.ColourHSVGradientAButton->setColour(currentCollection_->colourScalePointColour(Collection::HSVGradientSource, 0));
	ui.ColourHSVGradientASpin->setValue(currentCollection_->colourScalePointValue(Collection::HSVGradientSource, 0));
	ui.ColourHSVGradientBButton->setColour(currentCollection_->colourScalePointColour(Collection::HSVGradientSource, 1));
	ui.ColourHSVGradientBSpin->setValue(currentCollection_->colourScalePointValue(Collection::HSVGradientSource, 1));

	// Custom Gradient - add points in reverse order so higher values appear at the top of the list
	ui.ColourCustomGradientTable->clearContents();
	ui.ColourCustomGradientTable->setRowCount(currentCollection_->nCustomColourScalePoints());
	QTableWidgetItem *item;
	int count = 0;
	for (int n = currentCollection_->nCustomColourScalePoints()-1; n>=0; --n)
	{
		item = new QTableWidgetItem(QString::number(currentCollection_->colourScalePointValue(Collection::CustomGradientSource, n)));
		item->setData(Qt::UserRole, n);
		ui.ColourCustomGradientTable->setItem(count, 0, item);
		item = new QTableWidgetItem();
		item->setBackgroundColor(currentCollection_->colourScalePointColour(Collection::CustomGradientSource, n));
		item->setData(Qt::UserRole, n);
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable	);
		ui.ColourCustomGradientTable->setItem(count, 1, item);
		++count;
	}
	// -- Select first item in list
	ui.ColourCustomGradientTable->setCurrentItem(0);
	
	// Set current colour source
	if (currentCollection_->colourSource() == Collection::SingleColourSource) ui.ColourSingleColourRadio->setChecked(true);
	else if (currentCollection_->colourSource() == Collection::RGBGradientSource) ui.ColourRGBGradientRadio->setChecked(true);
	else if (currentCollection_->colourSource() == Collection::HSVGradientSource) ui.ColourHSVGradientRadio->setChecked(true);
	else if (currentCollection_->colourSource() == Collection::CustomGradientSource) ui.ColourCustomGradientRadio->setChecked(true);

	// Update gradient bar
	updateGradientBar();

	// Set current alpha control
	if (currentCollection_->alphaControl() == Collection::OwnAlpha) ui.ColourAlphaOwnAlphaRadio->setChecked(true);
	else if (currentCollection_->alphaControl() == Collection::FixedAlpha) ui.ColourAlphaFixedAlphaRadio->setChecked(true);
	ui.ColourAlphaFixedAlphaSpin->setValue(currentCollection_->fixedAlpha());

	refreshing_ = false;
}
