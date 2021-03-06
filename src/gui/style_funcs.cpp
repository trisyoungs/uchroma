/*
	*** Style Window
	*** src/gui/style_funcs.cpp
	Copyright T. Youngs 2013-2015

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

#include "gui/style.h"
#include "gui/uchroma.h"
#include "session/session.h"
#include "templates/reflist.h"
#include <QColorDialog>

/*
 * Window Functions
 */

// Constructor
StyleWindow::StyleWindow(UChromaWindow& parent) : QWidget(&parent), uChroma_(parent)
{
	ui.setupUi(this);

	refreshing_ = true;

	// Add display styles to StyleCombo on Collection->Style tab
	for (int n=0; n<Collection::nDisplayStyles; ++n) ui.StyleCombo->addItem( Collection::displayStyle((Collection::DisplayStyle) n));

	// Create entries in LineStippleCombo
	for (int n=0; n<LineStipple::nStippleTypes; ++n) LineStipple::stipple[n].addStippleItem(ui.LineStippleCombo, 3);

	// Create a buttongroup for the main style RadioButtons
	QButtonGroup* butGroup = new QButtonGroup(this);
	butGroup->addButton(ui.ColourSingleColourRadio);
	butGroup->addButton(ui.ColourRGBGradientRadio);
	butGroup->addButton(ui.ColourHSVGradientRadio);
	butGroup->addButton(ui.ColourCustomGradientRadio);

	QWidget::setWindowFlags(Qt::Tool);

	refreshing_ = false;
}

// Destructor
StyleWindow::~StyleWindow()
{
}

// Window close event
void StyleWindow::closeEvent(QCloseEvent *event)
{
	emit(windowClosed(false));
}

/*
 * Convenience Functions
 */

/*
 * Slots
 */

// Style

void StyleWindow::on_StyleCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;

	// Check for invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_StyleCombo_currentIndexChanged")) return;

	currentCollection->setDisplayStyle( (Collection::DisplayStyle) index );
	updateControls();

	// Update display
	UChromaSession::setAsModified();
	uChroma_.updateDisplay();
}

// Colour - Single

void StyleWindow::on_ColourSingleColourRadio_clicked(bool checked)
{
	if (refreshing_) return;

	// Check for invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourSingleColourRadio_clicked")) return;

	currentCollection->setColourSource(Collection::SingleColourSource);

	// Update gradient bar and display
	updateGradientBar();
	UChromaSession::setAsModified();
	uChroma_.updateDisplay();
}

void StyleWindow::on_ColourSingleColourButton_clicked(bool checked)
{
	// Check for invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourSingleColourButton_clicked")) return;

	if (ui.ColourSingleColourButton->selectColour())
	{
		currentCollection->setColourScalePoint(Collection::SingleColourSource, ui.ColourSingleColourButton->colour());

		// Update gradient bar and display
		updateGradientBar();
		UChromaSession::setAsModified();
		uChroma_.updateDisplay();
	}
}

// Colour - RGB Gradient

void StyleWindow::on_ColourRGBGradientRadio_clicked(bool checked)
{
	if (refreshing_) return;

	// Check for invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourRGBGradientRadio_clicked")) return;

	currentCollection->setColourSource(Collection::RGBGradientSource);

	// Update gradient bar and display
	updateGradientBar();
	UChromaSession::setAsModified();
	uChroma_.updateDisplay();
}

void StyleWindow::on_ColourRGBGradientAButton_clicked(bool checked)
{
	// Check for invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourRGBGradientAButton_clicked")) return;

	if (ui.ColourRGBGradientAButton->selectColour())
	{
		currentCollection->setColourScalePoint(Collection::RGBGradientSource, ui.ColourRGBGradientAButton->colour(), ui.ColourRGBGradientASpin->value(), 0);

		// Update gradient bar and display
		updateGradientBar();
		UChromaSession::setAsModified();
		uChroma_.updateDisplay();
	}
}

void StyleWindow::on_ColourRGBGradientASpin_valueChanged(double value)
{
	if (refreshing_) return;

	// Check for invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourRGBGradientASpin_valueChanged")) return;

	currentCollection->setColourScalePoint(Collection::RGBGradientSource, ui.ColourRGBGradientAButton->colour(), ui.ColourRGBGradientASpin->value(), 0);

	// Update gradient bar and display
	updateGradientBar();
	UChromaSession::setAsModified();
	uChroma_.updateDisplay();
}

void StyleWindow::on_ColourRGBGradientASetMinimumButton_clicked(bool checked)
{
	// Check for invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourRGBGradientASetMinimumButton_clicked")) return;

	ui.ColourRGBGradientASpin->setValue(currentCollection->transformMin().y);
}

void StyleWindow::on_ColourRGBGradientASetMaximumButton_clicked(bool checked)
{
	// Check for invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourRGBGradientASetMaximumButton_clicked")) return;

	ui.ColourRGBGradientASpin->setValue(currentCollection->transformMax().y);
}

void StyleWindow::on_ColourRGBGradientBButton_clicked(bool checked)
{
	// Check for invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourRGBGradientBButton_clicked")) return;

	if (ui.ColourRGBGradientBButton->selectColour())
	{
		currentCollection->setColourScalePoint(Collection::RGBGradientSource, ui.ColourRGBGradientBButton->colour(), ui.ColourRGBGradientBSpin->value(), 1);

		// Update gradient bar and display
		updateGradientBar();
		UChromaSession::setAsModified();
		uChroma_.updateDisplay();
	}
}

void StyleWindow::on_ColourRGBGradientBSpin_valueChanged(double value)
{
	if (refreshing_) return;

	// Check for invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourRGBGradientBSpin_valueChanged")) return;

	currentCollection->setColourScalePoint(Collection::RGBGradientSource, ui.ColourRGBGradientBButton->colour(), ui.ColourRGBGradientBSpin->value(), 1);

	// Update gradient bar and display
	updateGradientBar();
	UChromaSession::setAsModified();
	uChroma_.updateDisplay();
}

void StyleWindow::on_ColourRGBGradientBSetMinimumButton_clicked(bool checked)
{
	if (refreshing_) return;

	// Check for invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourRGBGradientBSetMinimumButton_clicked")) return;

	ui.ColourRGBGradientBSpin->setValue(currentCollection->transformMin().y);
}

void StyleWindow::on_ColourRGBGradientBSetMaximumButton_clicked(bool checked)
{
	// Check for invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourRGBGradientBSetMaximumButton_clicked")) return;

	ui.ColourRGBGradientBSpin->setValue(currentCollection->transformMax().y);
}

// Colour - HSV Gradient

void StyleWindow::on_ColourHSVGradientRadio_clicked(bool checked)
{
	if (refreshing_) return;

	// Check for invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourHSVGradientRadio_clicked")) return;

	currentCollection->setColourSource(Collection::HSVGradientSource);

	// Update gradient bar and display
	updateGradientBar();
	UChromaSession::setAsModified();
	uChroma_.updateDisplay();
}

void StyleWindow::on_ColourHSVGradientAButton_clicked(bool checked)
{
	// Check for a current Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourHSVGradientAButton_clicked")) return;

	if (ui.ColourHSVGradientAButton->selectColour())
	{
		currentCollection->setColourScalePoint(Collection::HSVGradientSource, ui.ColourHSVGradientAButton->colour(), ui.ColourHSVGradientASpin->value(), 0);

		// Update gradient bar and display
		updateGradientBar();
		UChromaSession::setAsModified();
		uChroma_.updateDisplay();
	}
}

void StyleWindow::on_ColourHSVGradientASpin_valueChanged(double value)
{
	// Check for window refreshing or invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (refreshing_ || (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourHSVGradientASpin_valueChanged"))) return;

	currentCollection->setColourScalePoint(Collection::HSVGradientSource, ui.ColourHSVGradientAButton->colour(), ui.ColourHSVGradientASpin->value(), 0);

	// Update gradient bar and display
	updateGradientBar();
	UChromaSession::setAsModified();
	uChroma_.updateDisplay();
}

void StyleWindow::on_ColourHSVGradientASetMinimumButton_clicked(bool checked)
{
	// Check for a current Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourHSVGradientASetMinimumButton_clicked")) return;

	ui.ColourHSVGradientASpin->setValue(currentCollection->transformMin().y);
}

void StyleWindow::on_ColourHSVGradientASetMaximumButton_clicked(bool checked)
{
	// Check for a current Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourHSVGradientASetMaximumButton_clicked")) return;

	ui.ColourHSVGradientASpin->setValue(currentCollection->transformMax().y);
}

void StyleWindow::on_ColourHSVGradientBButton_clicked(bool checked)
{
	// Check for a current Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourHSVGradientBButton_clicked")) return;

	if (ui.ColourHSVGradientBButton->selectColour())
	{
		currentCollection->setColourScalePoint(Collection::HSVGradientSource, ui.ColourHSVGradientBButton->colour(), ui.ColourHSVGradientBSpin->value(), 1);

		// Update gradient bar and display
		updateGradientBar();
		UChromaSession::setAsModified();
		uChroma_.updateDisplay();
	}
}

void StyleWindow::on_ColourHSVGradientBSpin_valueChanged(double value)
{
	// Check for window refreshing or invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (refreshing_ || (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourHSVGradientBSpin_valueChanged"))) return;

	currentCollection->setColourScalePoint(Collection::HSVGradientSource, ui.ColourHSVGradientBButton->colour(), ui.ColourHSVGradientBSpin->value(), 1);

	// Update gradient bar and display
	updateGradientBar();
	UChromaSession::setAsModified();
	uChroma_.updateDisplay();
}

void StyleWindow::on_ColourHSVGradientBSetMinimumButton_clicked(bool checked)
{
	// Check for a current Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourHSVGradientBSetMinimumButton_clicked")) return;

	ui.ColourHSVGradientBSpin->setValue(currentCollection->transformMin().y);
}

void StyleWindow::on_ColourHSVGradientBSetMaximumButton_clicked(bool checked)
{
	// Check for a current Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourHSVGradientBSetMaximumButton_clicked")) return;

	ui.ColourHSVGradientBSpin->setValue(currentCollection->transformMax().y);
}

// Colour - Custom Gradient

void StyleWindow::on_ColourCustomGradientRadio_clicked(bool checked)
{
	// Check for window refreshing or invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (refreshing_ || (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourCustomGradientRadio_clicked"))) return;

	currentCollection->setColourSource(Collection::CustomGradientSource);

	// Update gradient bar and display
	updateGradientBar();
	UChromaSession::setAsModified();
	uChroma_.updateDisplay();
}

void StyleWindow::on_ColourCustomGradientTable_itemSelectionChanged()
{
	bool selected = (ui.ColourCustomGradientTable->selectedItems().count() != 0);
	ui.ColourCustomGradientRemoveButton->setEnabled(selected);
}

void StyleWindow::on_ColourCustomGradientTable_cellDoubleClicked(int row, int column)
{
	// Check for window refreshing or invalid Collection or invalid table column
	Collection* currentCollection = UChromaSession::currentCollection();
	if (refreshing_ || (column == 0)) return;
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourCustomGradientTable_cellDoubleClicked")) return;

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
	currentCollection->setColourScalePoint(Collection::CustomGradientSource, newcol, valueItem->text().toDouble(), cspId);

	// Update table and refresh surface
	item->setBackgroundColor(newcol);

	// Update gradient bar and display
	updateGradientBar();
	UChromaSession::setAsModified();
	uChroma_.updateDisplay();
}

void StyleWindow::on_ColourCustomGradientTable_cellChanged(int row, int column)
{
	// Check for window refreshing or invalid Collection or invalid table column
	Collection* currentCollection = UChromaSession::currentCollection();
	if (refreshing_ || (column == 1)) return;
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourCustomGradientTable_cellChanged")) return;

	// Get colour item data (also contains ColourScalePoint index)
	QTableWidgetItem* item = ui.ColourCustomGradientTable->item(row, 1);
	if (!item) return;
	int cspId = item->data(Qt::UserRole).toInt();

	// Get point value
	QTableWidgetItem* valueItem = ui.ColourCustomGradientTable->item(row, 0);
	if (!valueItem) return;
	
	// Value of this point has changed, so must update the list and refresh the surface and necessary widgets
	currentCollection->setColourScalePoint(Collection::CustomGradientSource, item->backgroundColor(), valueItem->text().toDouble(), cspId);

	// Update gradient bar and display
	updateGradientBar();
	UChromaSession::setAsModified();
	uChroma_.updateDisplay();
}

void StyleWindow::on_ColourCustomGradientAddButton_clicked(bool checked)
{
	// Check for invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourCustomGradientAddButton_clicked")) return;

	currentCollection->addCustomColourScalePoint();

	// Update gradient bar and display
	updateGradientBar();
	UChromaSession::setAsModified();
	uChroma_.updateDisplay();
}

void StyleWindow::on_ColourCustomGradientRemoveButton_clicked(bool checked)
{
	// Check for invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourCustomGradientRemoveButton_clicked")) return;

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
		toDelete.addUnique(currentCollection->customColourScalePoint(cspId));
	}

	// Now delete the points
	for (RefListItem<ColourScalePoint,int>* ri = toDelete.first(); ri != NULL; ri = ri->next) currentCollection->removeCustomColourScalePoint(ri->item);

	// Update gradient bar and display
	updateGradientBar();
	UChromaSession::setAsModified();
	uChroma_.updateDisplay();
}

// Alpha Control

void StyleWindow::on_ColourAlphaOwnAlphaRadio_clicked(bool checked)
{
	// Check for window refreshing or invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (refreshing_ || (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourAlphaOwnAlphaRadio_clicked"))) return;

	currentCollection->setAlphaControl(Collection::OwnAlpha);

	// Update display
	UChromaSession::setAsModified();
	uChroma_.updateDisplay();
}

void StyleWindow::on_ColourAlphaFixedAlphaRadio_clicked(bool checked)
{
	// Check for window refreshing or invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (refreshing_ || (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourAlphaFixedAlphaRadio_clicked"))) return;

	currentCollection->setAlphaControl(Collection::FixedAlpha);

	// Update display
	UChromaSession::setAsModified();
	uChroma_.updateDisplay();
}

void StyleWindow::on_ColourAlphaFixedAlphaSpin_valueChanged(double value)
{
	// Check for window refreshing or invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (refreshing_ || (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_ColourAlphaFixedAlphaSpin_valueChanged"))) return;

	currentCollection->setFixedAlpha(value);

	// Update display
	UChromaSession::setAsModified();
	uChroma_.updateDisplay();
}

// Surface Options
void StyleWindow::on_SurfaceShininessSpin_valueChanged(double value)
{
	// Check for window refreshing or invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (refreshing_ || (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_SurfaceShininessSpin_valueChanged"))) return;

	currentCollection->setDisplaySurfaceShininess(value);

	// Update display
	UChromaSession::setAsModified();
	uChroma_.updateDisplay();
}

// -- Line Options
void StyleWindow::on_LineStippleCombo_currentIndexChanged(int index)
{
	// Check for window refreshing or invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (refreshing_ || (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_SurfaceShininessSpin_valueChanged"))) return;

	currentCollection->displayLineStyle().setStipple( (LineStipple::StippleType) index );

	// Update display
	UChromaSession::setAsModified();
	uChroma_.updateDisplay();
}

void StyleWindow::on_LineWidthSpin_valueChanged(double value)
{
	// Check for window refreshing or invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (refreshing_ || (!Collection::objectValid(UChromaSession::currentCollection(), "current collection in StyleWindow::on_SurfaceShininessSpin_valueChanged"))) return;

	currentCollection->displayLineStyle().setWidth(value);

	// Update display
	UChromaSession::setAsModified();
	uChroma_.updateDisplay();
}

/*
 * Update
 */

// Update Gradient Bar
void StyleWindow::updateGradientBar()
{
	// Check for a current Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!currentCollection) return;
	
	ui.ColourGradient->setColourScale(currentCollection->colourScale());
}

// Update controls and show window
void StyleWindow::updateAndShow()
{
	updateControls(true);
	show();
	move(uChroma_.centrePos() - QPoint(width()/2, height()/2));
}

// Update controls
void StyleWindow::updateControls(bool force)
{
	// If the window isn't visible, do nothing...
	if ((!isVisible()) && (!force) ) return;

	// Check for invalid Collection
	Collection* currentCollection = UChromaSession::currentCollection();
	if (!currentCollection) return;

	refreshing_ = true;

	// Style Combo
	ui.StyleCombo->setCurrentIndex(currentCollection->displayStyle());

	// Single Colour
	ui.ColourSingleColourButton->setColour(currentCollection->colourScalePointColour(Collection::SingleColourSource));

	// RGB Gradient
	ui.ColourRGBGradientAButton->setColour(currentCollection->colourScalePointColour(Collection::RGBGradientSource, 0));
	ui.ColourRGBGradientASpin->setValue(currentCollection->colourScalePointValue(Collection::RGBGradientSource, 0));
	ui.ColourRGBGradientBButton->setColour(currentCollection->colourScalePointColour(Collection::RGBGradientSource, 1));
	ui.ColourRGBGradientBSpin->setValue(currentCollection->colourScalePointValue(Collection::RGBGradientSource, 1));

	// HSV Gradient
	ui.ColourHSVGradientAButton->setColour(currentCollection->colourScalePointColour(Collection::HSVGradientSource, 0));
	ui.ColourHSVGradientASpin->setValue(currentCollection->colourScalePointValue(Collection::HSVGradientSource, 0));
	ui.ColourHSVGradientBButton->setColour(currentCollection->colourScalePointColour(Collection::HSVGradientSource, 1));
	ui.ColourHSVGradientBSpin->setValue(currentCollection->colourScalePointValue(Collection::HSVGradientSource, 1));

	// Custom Gradient - add points in reverse order so higher values appear at the top of the list
	ui.ColourCustomGradientTable->clearContents();
	ui.ColourCustomGradientTable->setRowCount(currentCollection->nCustomColourScalePoints());
	QTableWidgetItem *item;
	int count = 0;
	for (int n = currentCollection->nCustomColourScalePoints()-1; n>=0; --n)
	{
		item = new QTableWidgetItem(QString::number(currentCollection->colourScalePointValue(Collection::CustomGradientSource, n)));
		item->setData(Qt::UserRole, n);
		ui.ColourCustomGradientTable->setItem(count, 0, item);
		item = new QTableWidgetItem();
		item->setBackgroundColor(currentCollection->colourScalePointColour(Collection::CustomGradientSource, n));
		item->setData(Qt::UserRole, n);
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable	);
		ui.ColourCustomGradientTable->setItem(count, 1, item);
		++count;
	}
	// -- Select first item in list
	ui.ColourCustomGradientTable->setCurrentItem(0);
	
	// Set current colour source
	if (currentCollection->colourSource() == Collection::SingleColourSource) ui.ColourSingleColourRadio->setChecked(true);
	else if (currentCollection->colourSource() == Collection::RGBGradientSource) ui.ColourRGBGradientRadio->setChecked(true);
	else if (currentCollection->colourSource() == Collection::HSVGradientSource) ui.ColourHSVGradientRadio->setChecked(true);
	else if (currentCollection->colourSource() == Collection::CustomGradientSource) ui.ColourCustomGradientRadio->setChecked(true);

	// Update gradient bar
	updateGradientBar();

	// Set current alpha control
	if (currentCollection->alphaControl() == Collection::OwnAlpha) ui.ColourAlphaOwnAlphaRadio->setChecked(true);
	else if (currentCollection->alphaControl() == Collection::FixedAlpha) ui.ColourAlphaFixedAlphaRadio->setChecked(true);
	ui.ColourAlphaFixedAlphaSpin->setValue(currentCollection->fixedAlpha());

	// Set surface options
	ui.SurfaceShininessSpin->setValue(currentCollection->displaySurfaceShininess());

	// Set line options
	ui.LineWidthSpin->setValue(currentCollection->displayLineStyle().width());
	ui.LineStippleCombo->setCurrentIndex(currentCollection->displayLineStyle().stipple());

	// Enable/disable groups specific to one style
	bool isSurface = ((currentCollection->displayStyle() == Collection::SurfaceStyle) || (currentCollection->displayStyle() == Collection::UnlitSurfaceStyle));
	ui.SurfaceOptionsGroup->setEnabled(isSurface);
	ui.LineOptionsGroup->setEnabled(!isSurface);

	refreshing_ = false;
}
