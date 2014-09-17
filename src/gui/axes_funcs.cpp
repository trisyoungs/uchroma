/*
	*** Main Window - Axes Functions 
	*** src/gui/axes_funcs.cpp
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

#include "gui/axes.h"
#include "gui/uchroma.h"
#include "gui/editlinestyle.h"
#include "gui/editnumberformat.h"
#include "templates/reflist.h"
#include <limits>

/*
 * Window Functions
 */

// Constructor
AxesWindow::AxesWindow(UChromaWindow& parent) : QWidget(&parent), uChroma_(parent)
{
	ui.setupUi(this);

	QWidget::setWindowFlags(Qt::Tool);

	refreshing_ = true;

	// Set limits / deltas for fractional axis position
	ui.XPositionYFractionalSpin->setRange(true, 0.0, true, 1.0);
	ui.XPositionZFractionalSpin->setRange(true, 0.0, true, 1.0);
	ui.YPositionXFractionalSpin->setRange(true, 0.0, true, 1.0);
	ui.YPositionZFractionalSpin->setRange(true, 0.0, true, 1.0);
	ui.ZPositionYFractionalSpin->setRange(true, 0.0, true, 1.0);
	ui.ZPositionYFractionalSpin->setRange(true, 0.0, true, 1.0);
	ui.XPositionYFractionalSpin->setSingleStep(0.1);
	ui.XPositionZFractionalSpin->setSingleStep(0.1);
	ui.YPositionXFractionalSpin->setSingleStep(0.1);
	ui.YPositionZFractionalSpin->setSingleStep(0.1);
	ui.ZPositionYFractionalSpin->setSingleStep(0.1);
	ui.ZPositionYFractionalSpin->setSingleStep(0.1);

	// Add anchor types on to combo boxes
	for (int n=0; n<TextPrimitive::nTextAnchors; ++n)
	{
		ui.XTitleAnchorCombo->addItem(TextPrimitive::textAnchor( (TextPrimitive::TextAnchor) n));
		ui.XLabelAnchorCombo->addItem(TextPrimitive::textAnchor( (TextPrimitive::TextAnchor) n));
		ui.YTitleAnchorCombo->addItem(TextPrimitive::textAnchor( (TextPrimitive::TextAnchor) n));
		ui.YLabelAnchorCombo->addItem(TextPrimitive::textAnchor( (TextPrimitive::TextAnchor) n));
		ui.ZTitleAnchorCombo->addItem(TextPrimitive::textAnchor( (TextPrimitive::TextAnchor) n));
		ui.ZLabelAnchorCombo->addItem(TextPrimitive::textAnchor( (TextPrimitive::TextAnchor) n));
	}
	
	refreshing_ = false;
}

// Destructor
AxesWindow::~AxesWindow()
{
}

// Window close event
void AxesWindow::closeEvent(QCloseEvent* event)
{
	emit(windowClosed(false));
}

/*
 * Convenience Functions
 */

bool AxesWindow::haveCurrentAxes()
{
	// Get current view pane
	ViewPane* pane = uChroma_.currentViewPane();
	return (pane != NULL);
}

Axes& AxesWindow::currentAxes()
{
	// Get current view pane
	ViewPane* pane = uChroma_.currentViewPane();
	return pane->axes();
}

bool AxesWindow::axisInvertChanged(int axis, bool checked)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;
	
	if (haveCurrentAxes()) currentAxes().setInverted(axis, checked);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisLogarithmicChanged(int axis, bool checked)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;
	
	currentAxes().setLogarithmic(axis, checked);

	// Update relevant parts of gui
	updateControls();
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisVisibleChanged(int axis, bool checked)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;
	
	currentAxes().setVisible(axis, checked);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisStretchChanged(int axis, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	currentAxes().setStretch(axis, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisLimitChanged(int axis, bool minLim, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;
	
	if (minLim) currentAxes().setMin(axis, value);
	else currentAxes().setMax(axis, value);

	// Update relevant parts of gui
	updateControls();
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisLimitSetExtreme(int axis, bool minLim)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;
	
	currentAxes().setToLimit(axis, minLim);

	// Update relevant parts of gui
	updateControls();
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisPositionIsFractionalChanged(int axis, bool fractional)
{
	// Disable / enable relevant widgets, and set button graphics to reflect choice
	if (axis == 0)
	{
		ui.XPositionYRealSpin->setDisabled(fractional);
		ui.XPositionYFractionalSpin->setEnabled(fractional);
		ui.XPositionZRealSpin->setDisabled(fractional);
		ui.XPositionZFractionalSpin->setEnabled(fractional);
		ui.XPositionYSetZeroButton->setIcon(fractional ? QIcon(":/uchroma/icons/tocentre.svg") : QIcon(":/uchroma/icons/tozero.svg"));
		ui.XPositionZSetZeroButton->setIcon(fractional ? QIcon(":/uchroma/icons/tocentre.svg") : QIcon(":/uchroma/icons/tozero.svg"));
		ui.XPositionYSetZeroButton->setToolTip(fractional ? "Set to midpoint" : "Set to zero");
		ui.XPositionZSetZeroButton->setToolTip(fractional ? "Set to midpoint" : "Set to zero");
	}
	else if (axis == 1)
	{
		ui.YPositionXRealSpin->setDisabled(fractional);
		ui.YPositionXFractionalSpin->setEnabled(fractional);
		ui.YPositionZRealSpin->setDisabled(fractional);
		ui.YPositionZFractionalSpin->setEnabled(fractional);
		ui.YPositionXSetZeroButton->setIcon(fractional ? QIcon(":/uchroma/icons/tocentre.svg") : QIcon(":/uchroma/icons/tozero.svg"));
		ui.YPositionZSetZeroButton->setIcon(fractional ? QIcon(":/uchroma/icons/tocentre.svg") : QIcon(":/uchroma/icons/tozero.svg"));
		ui.YPositionXSetZeroButton->setToolTip(fractional ? "Set to midpoint" : "Set to zero");
		ui.YPositionZSetZeroButton->setToolTip(fractional ? "Set to midpoint" : "Set to zero");
	}
	else if (axis == 2)
	{
		ui.ZPositionXRealSpin->setDisabled(fractional);
		ui.ZPositionXFractionalSpin->setEnabled(fractional);
		ui.ZPositionYRealSpin->setDisabled(fractional);
		ui.ZPositionYFractionalSpin->setEnabled(fractional);
		ui.ZPositionXSetZeroButton->setIcon(fractional ? QIcon(":/uchroma/icons/tocentre.svg") : QIcon(":/uchroma/icons/tozero.svg"));
		ui.ZPositionYSetZeroButton->setIcon(fractional ? QIcon(":/uchroma/icons/tocentre.svg") : QIcon(":/uchroma/icons/tozero.svg"));
		ui.ZPositionXSetZeroButton->setToolTip(fractional ? "Set to midpoint" : "Set to zero");
		ui.ZPositionYSetZeroButton->setToolTip(fractional ? "Set to midpoint" : "Set to zero");
	}

	// Don't do anything else if we are currently refreshing
	if (refreshing_ || (!haveCurrentAxes())) return false;

	currentAxes().setPositionIsFractional(axis, fractional);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisPositionChanged(bool real, int axis, int dir, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	if (real) currentAxes().setPositionReal(axis, dir, value);
	else currentAxes().setPositionFractional(axis, dir, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisPositionSet(bool real, int axis, int dir, int type)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	if (real)
	{
		if (type == -1) currentAxes().setPositionRealToLimit(axis, dir, true);
		else if (type == 0) currentAxes().setPositionReal(axis, dir, 0.0);
		else if (type == 1) currentAxes().setPositionRealToLimit(axis, dir, false);
		else return false;
	}
	else
	{
		if (type == -1) currentAxes().setPositionFractional(axis, dir, 0.0);
		else if (type == 0) currentAxes().setPositionFractional(axis, dir, 0.5);
		else if (type == 1) currentAxes().setPositionFractional(axis, dir, 1.0);
		else return false;
	}

	// Update relevant parts of gui
	updateControls();
	uChroma_.updateDisplay();
	
	return true;
}

bool AxesWindow::axisAutoTicksChanged(int axis, bool enabled)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	currentAxes().setAutoTicks(axis, enabled);

	// Update relevant parts of gui
	updateControls();
	uChroma_.updateDisplay();
	
	return true;
}

bool AxesWindow::axisTicksChanged(int axis, bool start, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	if (start) currentAxes().setFirstTick(axis, value);
	else currentAxes().setTickDelta(axis, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}


bool AxesWindow::axisTickOrientationChanged(int axis, int dir, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	currentAxes().setTickDirection(axis, dir, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisLabelOrientationChanged(int axis, int component, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	currentAxes().setLabelOrientation(axis, component, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisTickSizeChanged(int axis, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	currentAxes().setTickSize(axis, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisTitleOrientationChanged(int axis, int component, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	currentAxes().setTitleOrientation(axis, component, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisMinorTicksChanged(int axis, int value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	currentAxes().setMinorTicks(axis, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisTitleChanged(int axis, QString& title)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	currentAxes().setTitle(axis, title);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisAnchorChanged(int axis, bool titleAnchor, TextPrimitive::TextAnchor anchor)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	if (titleAnchor) currentAxes().setTitleAnchor(axis, anchor);
	else currentAxes().setLabelAnchor(axis, anchor);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisGridLineChanged(int axis, bool major, bool on)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	if (major) currentAxes().setGridLinesMajor(axis, on);
	else currentAxes().setGridLinesMinor(axis, on);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisGridFullChanged(int axis, bool full)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	currentAxes().setGridLinesFull(axis, full);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisGridStyleClicked(int axis, bool major)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	EditLineStyleDialog dialog(this);
	bool success = dialog.call(major ? &currentAxes().gridLineMajorStyle(axis) : &currentAxes().gridLineMinorStyle(axis));

	// Update relevant parts of gui
	if (success)
	{
		if (major) currentAxes().gridLineMajorStyle(axis) = dialog.lineStyle();
		else currentAxes().gridLineMinorStyle(axis) = dialog.lineStyle();
		currentAxes().setPrimitivesInvalid();
		uChroma_.updateDisplay();
	}

	return success;
}

bool AxesWindow::axisGridStyleApplyClicked(int axis)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	for (int n=0; n<3; ++n)
	{
		if (n == axis) continue;
		currentAxes().gridLineMajorStyle(n) = currentAxes().gridLineMajorStyle(axis);
		currentAxes().gridLineMinorStyle(n) = currentAxes().gridLineMinorStyle(axis);
	}

	// Update relevant parts of gui
	currentAxes().setPrimitivesInvalid();
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisNumberFormatChangeClicked(int axis)
{
	EditNumberFormatDialog numberDialog(this);
	if (numberDialog.call(&currentAxes().numberFormat(axis)))
	{
		currentAxes().numberFormat(axis) = numberDialog.numberFormat();

		// Update relevant parts of gui
		currentAxes().setPrimitivesInvalid();
		uChroma_.updateDisplay();
	}

	return true;
}

/*
 * Slots
 */

// X Axis - General

void AxesWindow::on_XInvertCheck_clicked(bool checked)
{
	axisInvertChanged(0, checked);
}

void AxesWindow::on_XLogarithmicCheck_clicked(bool checked)
{
	axisLogarithmicChanged(0, checked);
}

void AxesWindow::on_XVisibleCheck_clicked(bool checked)
{
	axisVisibleChanged(0, checked);
}

void AxesWindow::on_XTitleEdit_textChanged(QString text)
{
	axisTitleChanged(0, text);
}

void AxesWindow::on_XStretchSpin_valueChanged(double value)
{
	axisStretchChanged(0, value);
}

void AxesWindow::on_XMinSpin_valueChanged(double value)
{
	axisLimitChanged(0, true, value);
}

void AxesWindow::on_XMinSetMinimumButton_clicked(bool checked)
{
	axisLimitSetExtreme(0, true);
}

void AxesWindow::on_XMaxSpin_valueChanged(double value)
{
	axisLimitChanged(0, false, value);
}

void AxesWindow::on_XMaxSetMaximumButton_clicked(bool checked)
{
	axisLimitSetExtreme(0, false);
}

void AxesWindow::on_XPositionRealRadio_clicked(bool checked)
{
	axisPositionIsFractionalChanged(0, !checked);
}

void AxesWindow::on_XPositionFractionalRadio_clicked(bool checked)
{
	axisPositionIsFractionalChanged(0, checked);
}

void AxesWindow::on_XPositionYRealSpin_valueChanged(double value)
{
	axisPositionChanged(true, 0, 1, value);
}

void AxesWindow::on_XPositionYFractionalSpin_valueChanged(double value)
{
	axisPositionChanged(false, 0, 1, value);
}

void AxesWindow::on_XPositionZRealSpin_valueChanged(double value)
{
	axisPositionChanged(true, 0, 2, value);
}

void AxesWindow::on_XPositionZFractionalSpin_valueChanged(double value)
{
	axisPositionChanged(false, 0, 2, value);
}

void AxesWindow::on_XPositionYSetMinimumButton_clicked(bool checked)
{
	axisPositionSet(ui.XPositionRealRadio->isChecked(), 0, 1, -1);
}

void AxesWindow::on_XPositionYSetZeroButton_clicked(bool checked)
{
	axisPositionSet(ui.XPositionRealRadio->isChecked(), 0, 1, 0);
}

void AxesWindow::on_XPositionYSetMaximumButton_clicked(bool checked)
{
	axisPositionSet(ui.XPositionRealRadio->isChecked(), 0, 1, 1);
}

void AxesWindow::on_XPositionZSetMinimumButton_clicked(bool checked)
{
	axisPositionSet(ui.XPositionRealRadio->isChecked(), 0, 2, -1);
}

void AxesWindow::on_XPositionZSetZeroButton_clicked(bool checked)
{
	axisPositionSet(ui.XPositionRealRadio->isChecked(), 0, 2, 0);
}

void AxesWindow::on_XPositionZSetMaximumButton_clicked(bool checked)
{
	axisPositionSet(ui.XPositionRealRadio->isChecked(), 0, 2, 1);
}

// X Axis - Ticks

void AxesWindow::on_XAutoTicksCheck_clicked(bool checked)
{
	axisAutoTicksChanged(0, checked);
}

void AxesWindow::on_XTicksStartSpin_valueChanged(double value)
{
	axisTicksChanged(0, true, value);
}

void AxesWindow::on_XTicksDeltaSpin_valueChanged(double value)
{
	axisTicksChanged(0, false, value);
}

void AxesWindow::on_XMinorTicksSpin_valueChanged(int value)
{
	axisMinorTicksChanged(0, value);
}

void AxesWindow::on_XTickDirectionXSpin_valueChanged(double value)
{
	axisTickOrientationChanged(0, 0, value);
}

void AxesWindow::on_XTickDirectionYSpin_valueChanged(double value)
{
	axisTickOrientationChanged(0, 1, value);
}

void AxesWindow::on_XTickDirectionZSpin_valueChanged(double value)
{
	axisTickOrientationChanged(0, 2, value);
}

void AxesWindow::on_XTickSizeSpin_valueChanged(double value)
{
	axisTickSizeChanged(0, value);
}

// X Axis - Labels

void AxesWindow::on_XNumberFormatChangeButton_clicked(bool checked)
{
	axisNumberFormatChangeClicked(0);
}

void AxesWindow::on_XLabelAnchorCombo_currentIndexChanged(int index)
{
	axisAnchorChanged(0, false, (TextPrimitive::TextAnchor) index);
}

void AxesWindow::on_XLabelAxialRotationSlider_valueChanged(int value)
{
	ui.XLabelAxialRotationSpin->setValue(value);
	axisLabelOrientationChanged(0, 0, value);
}

void AxesWindow::on_XLabelAxialRotationSpin_valueChanged(int value)
{
	ui.XLabelAxialRotationSlider->setValue(value);
	axisLabelOrientationChanged(0, 0, value);
}

void AxesWindow::on_XLabelInPlaneRotationSlider_valueChanged(int value)
{
	ui.XLabelInPlaneRotationSpin->setValue(value);
	axisLabelOrientationChanged(0, 1, value);
}

void AxesWindow::on_XLabelInPlaneRotationSpin_valueChanged(int value)
{
	ui.XLabelInPlaneRotationSlider->setValue(value);
	axisLabelOrientationChanged(0, 1, value);
}

void AxesWindow::on_XLabelDistanceSpin_valueChanged(double value)
{
	axisLabelOrientationChanged(0, 2, value);
}

void AxesWindow::on_XTitleHOffsetSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(0, 3, double(value) / 1000.0);
}

void AxesWindow::on_XTitleHOffsetLeftButton_clicked(bool checked)
{
	ui.XTitleHOffsetSlider->setValue(0);
	axisTitleOrientationChanged(0, 3, 0.0);
}

void AxesWindow::on_XTitleHOffsetCentreButton_clicked(bool checked)
{
	ui.XTitleHOffsetSlider->setValue(500);
	axisTitleOrientationChanged(0, 3, 0.5);
}

void AxesWindow::on_XTitleHOffsetRightButton_clicked(bool checked)
{
	ui.XTitleHOffsetSlider->setValue(1000);
	axisTitleOrientationChanged(0, 3, 1.0);
}

void AxesWindow::on_XTitleAnchorCombo_currentIndexChanged(int index)
{
	axisAnchorChanged(0, true, (TextPrimitive::TextAnchor) index);
}

void AxesWindow::on_XTitleAxialRotationSlider_valueChanged(int value)
{
	ui.XTitleAxialRotationSpin->setValue(value);
	axisTitleOrientationChanged(0, 0, value);
}

void AxesWindow::on_XTitleAxialRotationSpin_valueChanged(int value)
{
	ui.XTitleAxialRotationSlider->setValue(value);
	axisTitleOrientationChanged(0, 0, value);
}

void AxesWindow::on_XTitleInPlaneRotationSlider_valueChanged(int value)
{
	ui.XTitleInPlaneRotationSpin->setValue(value);
	axisTitleOrientationChanged(0, 1, value);
}

void AxesWindow::on_XTitleInPlaneRotationSpin_valueChanged(int value)
{
	ui.XTitleInPlaneRotationSlider->setValue(value);
	axisTitleOrientationChanged(0, 1, value);
}

void AxesWindow::on_XTitleDistanceSpin_valueChanged(double value)
{
	axisTitleOrientationChanged(0, 2, value);
}

// X Axis - GridLines

void AxesWindow::on_XGridLineMajorCheck_clicked(bool checked)
{
	axisGridLineChanged(0, true, checked);
}

void AxesWindow::on_XGridLineMinorCheck_clicked(bool checked)
{
	axisGridLineChanged(0, false, checked);
}

void AxesWindow::on_XGridLineFullCheck_clicked(bool checked)
{
	axisGridFullChanged(0, checked);
}

void AxesWindow::on_XGridLineMajorStyleButton_clicked(bool checked)
{
	axisGridStyleClicked(0, true);
}

void AxesWindow::on_XGridLineMinorStyleButton_clicked(bool checked)
{
	axisGridStyleClicked(0, false);
}

void AxesWindow::on_XGridLineApplyStyleButton_clicked(bool checked)
{
	axisGridStyleApplyClicked(0);
}

// Y Axis - General

void AxesWindow::on_YInvertCheck_clicked(bool checked)
{
	axisInvertChanged(1, checked);
}

void AxesWindow::on_YLogarithmicCheck_clicked(bool checked)
{
	axisLogarithmicChanged(1, checked);
}

void AxesWindow::on_YVisibleCheck_clicked(bool checked)
{
	axisVisibleChanged(1, checked);
}

void AxesWindow::on_YTitleEdit_textChanged(QString text)
{
	axisTitleChanged(1, text);
}

void AxesWindow::on_YStretchSpin_valueChanged(double value)
{
	axisStretchChanged(1, value);
}

void AxesWindow::on_YMinSpin_valueChanged(double value)
{
	axisLimitChanged(1, true, value);
}

void AxesWindow::on_YMinSetMinimumButton_clicked(bool checked)
{
	axisLimitSetExtreme(1, true);
}

void AxesWindow::on_YMaxSpin_valueChanged(double value)
{
	axisLimitChanged(1, false, value);
}

void AxesWindow::on_YMaxSetMaximumButton_clicked(bool checked)
{
	axisLimitSetExtreme(1, false);
}

void AxesWindow::on_YPositionRealRadio_clicked(bool checked)
{
	axisPositionIsFractionalChanged(1, !checked);
}

void AxesWindow::on_YPositionFractionalRadio_clicked(bool checked)
{
	axisPositionIsFractionalChanged(1, checked);
}

void AxesWindow::on_YPositionXRealSpin_valueChanged(double value)
{
	axisPositionChanged(true, 1, 0, value);
}

void AxesWindow::on_YPositionXFractionalSpin_valueChanged(double value)
{
	axisPositionChanged(false, 1, 0, value);
}

void AxesWindow::on_YPositionZRealSpin_valueChanged(double value)
{
	axisPositionChanged(true, 1, 2, value);
}

void AxesWindow::on_YPositionZFractionalSpin_valueChanged(double value)
{
	axisPositionChanged(false, 1, 2, value);
}

void AxesWindow::on_YPositionXSetMinimumButton_clicked(bool checked)
{
	axisPositionSet(ui.YPositionRealRadio->isChecked(), 1, 0, -1);
}

void AxesWindow::on_YPositionXSetZeroButton_clicked(bool checked)
{
	axisPositionSet(ui.YPositionRealRadio->isChecked(), 1, 0, 0);
}

void AxesWindow::on_YPositionXSetMaximumButton_clicked(bool checked)
{
	axisPositionSet(ui.YPositionRealRadio->isChecked(), 1, 0, 1);
}

void AxesWindow::on_YPositionZSetMinimumButton_clicked(bool checked)
{
	axisPositionSet(ui.YPositionRealRadio->isChecked(), 1, 2, -1);
}

void AxesWindow::on_YPositionZSetZeroButton_clicked(bool checked)
{
	axisPositionSet(ui.YPositionRealRadio->isChecked(), 1, 2, 0);
}

void AxesWindow::on_YPositionZSetMaximumButton_clicked(bool checked)
{
	axisPositionSet(ui.YPositionRealRadio->isChecked(), 1, 2, 1);
}

// Y Axis - Ticks

void AxesWindow::on_YAutoTicksCheck_clicked(bool checked)
{
	axisAutoTicksChanged(1, checked);
}

void AxesWindow::on_YTicksStartSpin_valueChanged(double value)
{
	axisTicksChanged(1, true, value);
}

void AxesWindow::on_YTicksDeltaSpin_valueChanged(double value)
{
	axisTicksChanged(1, false, value);
}

void AxesWindow::on_YMinorTicksSpin_valueChanged(int value)
{
	axisMinorTicksChanged(1, value);
}

void AxesWindow::on_YTickDirectionXSpin_valueChanged(double value)
{
	axisTickOrientationChanged(1, 0, value);
}

void AxesWindow::on_YTickDirectionYSpin_valueChanged(double value)
{
	axisTickOrientationChanged(1, 1, value);
}

void AxesWindow::on_YTickDirectionZSpin_valueChanged(double value)
{
	axisTickOrientationChanged(1, 2, value);
}

void AxesWindow::on_YTickSizeSpin_valueChanged(double value)
{
	axisTickSizeChanged(1, value);
}

// Y Axis - Labels

void AxesWindow::on_YNumberFormatChangeButton_clicked(bool checked)
{
	axisNumberFormatChangeClicked(1);
}

void AxesWindow::on_YLabelAnchorCombo_currentIndexChanged(int index)
{
	axisAnchorChanged(1, false, (TextPrimitive::TextAnchor) index);
}

void AxesWindow::on_YLabelAxialRotationSlider_valueChanged(int value)
{
	ui.YLabelAxialRotationSpin->setValue(value);
	axisLabelOrientationChanged(1, 0, value);
}

void AxesWindow::on_YLabelAxialRotationSpin_valueChanged(int value)
{
	ui.YLabelAxialRotationSlider->setValue(value);
	axisLabelOrientationChanged(1, 0, value);
}

void AxesWindow::on_YLabelInPlaneRotationSlider_valueChanged(int value)
{
	ui.YLabelInPlaneRotationSpin->setValue(value);
	axisLabelOrientationChanged(1, 1, value);
}

void AxesWindow::on_YLabelInPlaneRotationSpin_valueChanged(int value)
{
	ui.YLabelInPlaneRotationSlider->setValue(value);
	axisLabelOrientationChanged(1, 1, value);
}

void AxesWindow::on_YLabelDistanceSpin_valueChanged(double value)
{
	axisLabelOrientationChanged(1, 2, value);
}

void AxesWindow::on_YTitleHOffsetSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(1, 3, double(value) / 1000.0);
}

void AxesWindow::on_YTitleHOffsetLeftButton_clicked(bool checked)
{
	ui.YTitleHOffsetSlider->setValue(0);
	axisTitleOrientationChanged(1, 3, 0.0);
}

void AxesWindow::on_YTitleHOffsetCentreButton_clicked(bool checked)
{
	ui.YTitleHOffsetSlider->setValue(500);
	axisTitleOrientationChanged(1, 3, 0.5);
}

void AxesWindow::on_YTitleHOffsetRightButton_clicked(bool checked)
{
	ui.YTitleHOffsetSlider->setValue(1000);
	axisTitleOrientationChanged(1, 3, 1.0);
}

void AxesWindow::on_YTitleAnchorCombo_currentIndexChanged(int index)
{
	axisAnchorChanged(1, true, (TextPrimitive::TextAnchor) index);
}

void AxesWindow::on_YTitleAxialRotationSlider_valueChanged(int value)
{
	ui.YTitleAxialRotationSpin->setValue(value);
	axisTitleOrientationChanged(1, 0, value);
}

void AxesWindow::on_YTitleAxialRotationSpin_valueChanged(int value)
{
	ui.YTitleAxialRotationSlider->setValue(value);
	axisTitleOrientationChanged(1, 0, value);
}

void AxesWindow::on_YTitleInPlaneRotationSlider_valueChanged(int value)
{
	ui.YTitleInPlaneRotationSpin->setValue(value);
	axisTitleOrientationChanged(1, 1, value);
}

void AxesWindow::on_YTitleInPlaneRotationSpin_valueChanged(int value)
{
	ui.YTitleInPlaneRotationSlider->setValue(value);
	axisTitleOrientationChanged(1, 1, value);
}

void AxesWindow::on_YTitleDistanceSpin_valueChanged(double value)
{
	axisTitleOrientationChanged(1, 2, value);
}

// Y Axis - Grid

void AxesWindow::on_YGridLineMajorCheck_clicked(bool checked)
{
	axisGridLineChanged(1, true, checked);
}

void AxesWindow::on_YGridLineMinorCheck_clicked(bool checked)
{
	axisGridLineChanged(1, false, checked);
}

void AxesWindow::on_YGridLineFullCheck_clicked(bool checked)
{
	axisGridFullChanged(1, checked);
}

void AxesWindow::on_YGridLineMajorStyleButton_clicked(bool checked)
{
	axisGridStyleClicked(1, true);
}

void AxesWindow::on_YGridLineMinorStyleButton_clicked(bool checked)
{
	axisGridStyleClicked(1, false);
}

void AxesWindow::on_YGridLineApplyStyleButton_clicked(bool checked)
{
	axisGridStyleApplyClicked(1);
}

// Z Axis - General

void AxesWindow::on_ZInvertCheck_clicked(bool checked)
{
	axisInvertChanged(2, checked);
}

void AxesWindow::on_ZLogarithmicCheck_clicked(bool checked)
{
	axisLogarithmicChanged(2, checked);
}

void AxesWindow::on_ZVisibleCheck_clicked(bool checked)
{
	axisVisibleChanged(2, checked);
}

void AxesWindow::on_ZTitleEdit_textChanged(QString text)
{
	axisTitleChanged(2, text);
}

void AxesWindow::on_ZStretchSpin_valueChanged(double value)
{
	axisStretchChanged(2, value);
}

void AxesWindow::on_ZMinSpin_valueChanged(double value)
{
	axisLimitChanged(2, true, value);
}

void AxesWindow::on_ZMinSetMinimumButton_clicked(bool checked)
{
	axisLimitSetExtreme(2, true);
}

void AxesWindow::on_ZMaxSpin_valueChanged(double value)
{
	axisLimitChanged(2, false, value);
}

void AxesWindow::on_ZMaxSetMaximumButton_clicked(bool checked)
{
	axisLimitSetExtreme(2, false);
}

void AxesWindow::on_ZPositionRealRadio_clicked(bool checked)
{
	axisPositionIsFractionalChanged(2, !checked);
}

void AxesWindow::on_ZPositionFractionalRadio_clicked(bool checked)
{
	axisPositionIsFractionalChanged(2, checked);
}

void AxesWindow::on_ZPositionXRealSpin_valueChanged(double value)
{
	axisPositionChanged(true, 2, 0, value);
}

void AxesWindow::on_ZPositionXFractionalSpin_valueChanged(double value)
{
	axisPositionChanged(false, 2, 0, value);
}

void AxesWindow::on_ZPositionYRealSpin_valueChanged(double value)
{
	axisPositionChanged(true, 2, 1, value);
}

void AxesWindow::on_ZPositionYFractionalSpin_valueChanged(double value)
{
	axisPositionChanged(false, 2, 1, value);
}

void AxesWindow::on_ZPositionXSetMinimumButton_clicked(bool checked)
{
	axisPositionSet(ui.ZPositionRealRadio->isChecked(), 2, 0, -1);
}

void AxesWindow::on_ZPositionXSetZeroButton_clicked(bool checked)
{
	axisPositionSet(ui.ZPositionRealRadio->isChecked(), 2, 0, 0);
}

void AxesWindow::on_ZPositionXSetMaximumButton_clicked(bool checked)
{
	axisPositionSet(ui.ZPositionRealRadio->isChecked(), 2, 0, 1);
}

void AxesWindow::on_ZPositionYSetMinimumButton_clicked(bool checked)
{
	axisPositionSet(ui.ZPositionRealRadio->isChecked(), 2, 1, -1);
}

void AxesWindow::on_ZPositionYSetZeroButton_clicked(bool checked)
{
	axisPositionSet(ui.ZPositionRealRadio->isChecked(), 2, 1, 0);
}

void AxesWindow::on_ZPositionYSetMaximumButton_clicked(bool checked)
{
	axisPositionSet(ui.ZPositionRealRadio->isChecked(), 2, 1, 1);
}

// Z Axis - Ticks

void AxesWindow::on_ZAutoTicksCheck_clicked(bool checked)
{
	axisAutoTicksChanged(2, checked);
}

void AxesWindow::on_ZTicksStartSpin_valueChanged(double value)
{
	axisTicksChanged(2, true, value);
}

void AxesWindow::on_ZTicksDeltaSpin_valueChanged(double value)
{
	axisTicksChanged(2, false, value);
}

void AxesWindow::on_ZMinorTicksSpin_valueChanged(int value)
{
	axisMinorTicksChanged(2, value);
}

void AxesWindow::on_ZTickDirectionXSpin_valueChanged(double value)
{
	axisTickOrientationChanged(2, 0, value);
}

void AxesWindow::on_ZTickDirectionYSpin_valueChanged(double value)
{
	axisTickOrientationChanged(2, 1, value);
}

void AxesWindow::on_ZTickDirectionZSpin_valueChanged(double value)
{
	axisTickOrientationChanged(2, 2, value);
}

void AxesWindow::on_ZTickSizeSpin_valueChanged(double value)
{
	axisTickSizeChanged(2, value);
}

// Z Axis - Labels

void AxesWindow::on_ZNumberFormatChangeButton_clicked(bool checked)
{
	axisNumberFormatChangeClicked(2);
}

void AxesWindow::on_ZLabelAnchorCombo_currentIndexChanged(int index)
{
	axisAnchorChanged(2, false, (TextPrimitive::TextAnchor) index);
}

void AxesWindow::on_ZLabelAxialRotationSlider_valueChanged(int value)
{
	ui.ZLabelAxialRotationSpin->setValue(value);
	axisLabelOrientationChanged(2, 0, value);
}

void AxesWindow::on_ZLabelAxialRotationSpin_valueChanged(int value)
{
	ui.ZLabelAxialRotationSlider->setValue(value);
	axisLabelOrientationChanged(2, 0, value);
}

void AxesWindow::on_ZLabelInPlaneRotationSlider_valueChanged(int value)
{
	ui.ZLabelInPlaneRotationSpin->setValue(value);
	axisLabelOrientationChanged(2, 1, value);
}

void AxesWindow::on_ZLabelInPlaneRotationSpin_valueChanged(int value)
{
	ui.ZLabelInPlaneRotationSlider->setValue(value);
	axisLabelOrientationChanged(2, 1, value);
}

void AxesWindow::on_ZLabelDistanceSpin_valueChanged(double value)
{
	axisLabelOrientationChanged(2, 2, value);
}

void AxesWindow::on_ZTitleHOffsetSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(2, 3, double(value) / 1000.0);
}

void AxesWindow::on_ZTitleHOffsetLeftButton_clicked(bool checked)
{
	ui.ZTitleHOffsetSlider->setValue(0);
	axisTitleOrientationChanged(2, 3, 0.0);
}

void AxesWindow::on_ZTitleHOffsetCentreButton_clicked(bool checked)
{
	ui.ZTitleHOffsetSlider->setValue(500);
	axisTitleOrientationChanged(2, 3, 0.5);
}

void AxesWindow::on_ZTitleHOffsetRightButton_clicked(bool checked)
{
	ui.ZTitleHOffsetSlider->setValue(1000);
	axisTitleOrientationChanged(2, 3, 1.0);
}

void AxesWindow::on_ZTitleAnchorCombo_currentIndexChanged(int index)
{
	axisAnchorChanged(2, true, (TextPrimitive::TextAnchor) index);
}

void AxesWindow::on_ZTitleAxialRotationSlider_valueChanged(int value)
{
	ui.ZTitleAxialRotationSpin->setValue(value);
	axisTitleOrientationChanged(2, 0, value);
}

void AxesWindow::on_ZTitleAxialRotationSpin_valueChanged(int value)
{
	ui.ZTitleAxialRotationSlider->setValue(value);
	axisTitleOrientationChanged(2, 0, value);
}

void AxesWindow::on_ZTitleInPlaneRotationSlider_valueChanged(int value)
{
	ui.ZTitleInPlaneRotationSpin->setValue(value);
	axisTitleOrientationChanged(2, 1, value);
}

void AxesWindow::on_ZTitleInPlaneRotationSpin_valueChanged(int value)
{
	ui.ZTitleInPlaneRotationSlider->setValue(value);
	axisTitleOrientationChanged(2, 1, value);
}

void AxesWindow::on_ZTitleDistanceSpin_valueChanged(double value)
{
	axisTitleOrientationChanged(2, 2, value);
}

// Z Axis - Grid

void AxesWindow::on_ZGridLineMajorCheck_clicked(bool checked)
{
	axisGridLineChanged(2, true, checked);
}

void AxesWindow::on_ZGridLineMinorCheck_clicked(bool checked)
{
	axisGridLineChanged(2, false, checked);
}

void AxesWindow::on_ZGridLineFullCheck_clicked(bool checked)
{
	axisGridFullChanged(2, checked);
}

void AxesWindow::on_ZGridLineMajorStyleButton_clicked(bool checked)
{
	axisGridStyleClicked(2, true);
}

void AxesWindow::on_ZGridLineMinorStyleButton_clicked(bool checked)
{
	axisGridStyleClicked(2, false);
}

void AxesWindow::on_ZGridLineApplyStyleButton_clicked(bool checked)
{
	axisGridStyleApplyClicked(2);
}

/*
 * Update
 */

// Update controls and show window
void AxesWindow::updateAndShow()
{
	updateControls(true);
	show();
	move(uChroma_.centrePos() - QPoint(width()/2, height()/2));
}

// Update controls
void AxesWindow::updateControls(bool force)
{
	// If the window isn't visible, do nothing...
	if ((!isVisible()) && (!force) ) return;

	// Get current view pane
	ViewPane* pane = uChroma_.currentViewPane();
	if (pane == NULL) return;
	Axes& axes = pane->axes();

	refreshing_ = true;

	// Invert / Visible / Logarithmic
	ui.XInvertCheck->setChecked(axes.inverted(0));
	ui.YInvertCheck->setChecked(axes.inverted(1));
	ui.ZInvertCheck->setChecked(axes.inverted(2));
	ui.XVisibleCheck->setChecked(axes.visible(0));
	ui.YVisibleCheck->setChecked(axes.visible(1));
	ui.ZVisibleCheck->setChecked(axes.visible(2));
	ui.XLogarithmicCheck->setChecked(axes.logarithmic(0));
	ui.YLogarithmicCheck->setChecked(axes.logarithmic(1));
	ui.ZLogarithmicCheck->setChecked(axes.logarithmic(2));
	ui.XTicksDeltaSpin->setDisabled(axes.logarithmic(0));
	ui.YTicksDeltaSpin->setDisabled(axes.logarithmic(1));
	ui.ZTicksDeltaSpin->setDisabled(axes.logarithmic(2));
	ui.XAutoTicksCheck->setDisabled(axes.logarithmic(0));
	ui.YAutoTicksCheck->setDisabled(axes.logarithmic(1));
	ui.ZAutoTicksCheck->setDisabled(axes.logarithmic(2));

	// Axis titles
	ui.XTitleEdit->setText(axes.title(0));
	ui.YTitleEdit->setText(axes.title(1));
	ui.ZTitleEdit->setText(axes.title(2));

	// Axis Stretch factors
	bool stretchOff = (pane->twoDimensional() || pane->autoStretch3D());
	ui.XStretchSpin->setValue(axes.stretch(0));
	ui.XStretchSpin->setDisabled(stretchOff);
	ui.YStretchSpin->setValue(axes.stretch(1));
	ui.YStretchSpin->setDisabled(stretchOff);
	ui.ZStretchSpin->setValue(axes.stretch(2));
	ui.ZStretchSpin->setDisabled(stretchOff);

	// Axis Min/Max Limits
	ui.XMinSpin->setRange(axes.logarithmic(0), axes.limitMin(0), false, 0.0);
	ui.YMinSpin->setRange(axes.logarithmic(1), axes.limitMin(1), false, 0.0);
	ui.ZMinSpin->setRange(axes.logarithmic(2), axes.limitMin(2), false, 0.0);
	ui.XMinSpin->setSingleStep(std::max((axes.limitMax(0)-axes.limitMin(0))*0.01, 1.0));
	ui.YMinSpin->setSingleStep(std::max((axes.limitMax(1)-axes.limitMin(1))*0.01, 1.0));
	ui.ZMinSpin->setSingleStep(std::max((axes.limitMax(2)-axes.limitMin(2))*0.01, 1.0));

	ui.XMaxSpin->setRange(axes.logarithmic(0), axes.limitMin(0), false, 0.0);
	ui.YMaxSpin->setRange(axes.logarithmic(1), axes.limitMin(1), false, 0.0);
	ui.ZMaxSpin->setRange(axes.logarithmic(2), axes.limitMin(2), false, 0.0);
	ui.XMaxSpin->setSingleStep(std::max((axes.limitMax(0)-axes.limitMin(0))*0.01, 1.0));
	ui.YMaxSpin->setSingleStep(std::max((axes.limitMax(1)-axes.limitMin(1))*0.01, 1.0));
	ui.ZMaxSpin->setSingleStep(std::max((axes.limitMax(2)-axes.limitMin(2))*0.01, 1.0));
	ui.XMinSpin->setValue(axes.min(0));
	ui.YMinSpin->setValue(axes.min(1));
	ui.ZMinSpin->setValue(axes.min(2));
	ui.XMaxSpin->setValue(axes.max(0));
	ui.YMaxSpin->setValue(axes.max(1));
	ui.ZMaxSpin->setValue(axes.max(2));
	ui.XMinLabel->setText("DataMin: " + QString::number(axes.limitMin(0)));
	ui.XMaxLabel->setText("DataMax: " + QString::number(axes.limitMax(0)));
	ui.YMinLabel->setText("DataMin: " + QString::number(axes.limitMin(1)));
	ui.YMaxLabel->setText("DataMax: " + QString::number(axes.limitMax(1)));
	ui.ZMinLabel->setText("DataMin: " + QString::number(axes.limitMin(2)));
	ui.ZMaxLabel->setText("DataMax: " + QString::number(axes.limitMax(2)));

	// Axis positions
	// -- X
	ui.XPositionFractionalRadio->setChecked(axes.positionIsFractional(0));
	axisPositionIsFractionalChanged(0, axes.positionIsFractional(0));
	ui.XPositionYRealSpin->setRange(axes.logarithmic(1), axes.limitMin(1), false, 0.0);
	ui.XPositionYRealSpin->setValue(axes.positionReal(0).y);
	ui.XPositionZRealSpin->setRange(axes.logarithmic(2), axes.limitMin(2), false, 0.0);
	ui.XPositionZRealSpin->setValue(axes.positionReal(0).z);
	ui.XPositionYFractionalSpin->setValue(axes.positionFractional(0).y);
	ui.XPositionZFractionalSpin->setValue(axes.positionFractional(0).z);
	// -- Y
	ui.YPositionFractionalRadio->setChecked(axes.positionIsFractional(1));
	axisPositionIsFractionalChanged(1, axes.positionIsFractional(1));
	ui.YPositionXRealSpin->setRange(axes.logarithmic(0), axes.limitMin(0), false, 0.0);
	ui.YPositionXRealSpin->setValue(axes.positionReal(1).x);
	ui.YPositionZRealSpin->setRange(axes.logarithmic(2), axes.limitMin(2), false, 0.0);
	ui.YPositionZRealSpin->setValue(axes.positionReal(1).z);
	ui.YPositionXFractionalSpin->setValue(axes.positionFractional(1).x);
	ui.YPositionZFractionalSpin->setValue(axes.positionFractional(1).z);
	// -- Z
	ui.ZPositionFractionalRadio->setChecked(axes.positionIsFractional(2));
	axisPositionIsFractionalChanged(2, axes.positionIsFractional(2));
	ui.ZPositionXRealSpin->setRange(axes.logarithmic(0), axes.limitMin(0), false, 0.0);
	ui.ZPositionXRealSpin->setValue(axes.positionReal(2).x);
	ui.ZPositionYRealSpin->setRange(axes.logarithmic(1), axes.limitMin(1), false, 0.0);
	ui.ZPositionYRealSpin->setValue(axes.positionReal(2).y);
	ui.ZPositionXFractionalSpin->setValue(axes.positionFractional(2).x);
	ui.ZPositionYFractionalSpin->setValue(axes.positionFractional(2).y);

	// Axis Ticks
	// -- X
	ui.XAutoTicksCheck->setChecked(axes.autoTicks(0));
	ui.XTicksStartSpin->setValue(axes.tickFirst(0));
	ui.XTicksStartSpin->setEnabled(!axes.autoTicks(0));
	ui.XTicksDeltaSpin->setValue(axes.tickDelta(0));
	ui.XTicksDeltaSpin->setEnabled(!axes.autoTicks(0));
	ui.XMinorTicksSpin->setValue(axes.minorTicks(0));
	ui.XTickDirectionXSpin->setValue(axes.tickDirection(0).x);
	ui.XTickDirectionYSpin->setValue(axes.tickDirection(0).y);
	ui.XTickDirectionZSpin->setValue(axes.tickDirection(0).z);
	ui.XTickSizeSpin->setValue(axes.tickSize(0));
	// -- Y
	ui.YAutoTicksCheck->setChecked(axes.autoTicks(1));
	ui.YTicksStartSpin->setValue(axes.tickFirst(1));
	ui.YTicksStartSpin->setEnabled(!axes.autoTicks(1));
	ui.YTicksDeltaSpin->setValue(axes.tickDelta(1));
	ui.YTicksDeltaSpin->setEnabled(!axes.autoTicks(1));
	ui.YMinorTicksSpin->setValue(axes.minorTicks(1));
	ui.YTickDirectionXSpin->setValue(axes.tickDirection(1).x);
	ui.YTickDirectionYSpin->setValue(axes.tickDirection(1).y);
	ui.YTickDirectionZSpin->setValue(axes.tickDirection(1).z);
	// -- Z
	ui.ZAutoTicksCheck->setChecked(axes.autoTicks(2));
	ui.ZTicksStartSpin->setValue(axes.tickFirst(2));
	ui.ZTicksStartSpin->setEnabled(!axes.autoTicks(2));
	ui.ZTicksDeltaSpin->setValue(axes.tickDelta(2));
	ui.ZTicksDeltaSpin->setEnabled(!axes.autoTicks(2));
	ui.ZMinorTicksSpin->setValue(axes.minorTicks(2));
	ui.ZTickDirectionXSpin->setValue(axes.tickDirection(2).x);
	ui.ZTickDirectionYSpin->setValue(axes.tickDirection(2).y);
	ui.ZTickDirectionZSpin->setValue(axes.tickDirection(2).z);

	// Labels
	// -- X
	ui.XNumberFormatTestLabel->setText(axes.numberFormat(0).format(1.23456789));
	ui.XLabelAnchorCombo->setCurrentIndex(axes.labelAnchor(0));
	ui.XLabelAxialRotationSlider->setValue(axes.labelOrientation(0).x);
	ui.XLabelAxialRotationSpin->setValue(axes.labelOrientation(0).x);
	ui.XLabelInPlaneRotationSlider->setValue(axes.labelOrientation(0).y);
	ui.XLabelInPlaneRotationSpin->setValue(axes.labelOrientation(0).y);
	ui.XLabelDistanceSpin->setValue(axes.labelOrientation(0).z);
	ui.YTitleHOffsetSlider->setValue(axes.titleOrientation(0).w*1000);
	ui.XTitleAnchorCombo->setCurrentIndex(axes.titleAnchor(0));
	ui.XTitleAxialRotationSlider->setValue(axes.titleOrientation(0).x);
	ui.XTitleAxialRotationSpin->setValue(axes.titleOrientation(0).x);
	ui.XTitleInPlaneRotationSlider->setValue(axes.titleOrientation(0).y);
	ui.XTitleInPlaneRotationSpin->setValue(axes.titleOrientation(0).y);
	ui.XTitleDistanceSpin->setValue(axes.titleOrientation(0).z);
	// -- Y
	ui.YNumberFormatTestLabel->setText(axes.numberFormat(1).format(1.23456789));
	ui.YLabelAnchorCombo->setCurrentIndex(axes.labelAnchor(1));
	ui.YLabelAxialRotationSlider->setValue(axes.labelOrientation(1).x);
	ui.YLabelAxialRotationSpin->setValue(axes.labelOrientation(1).x);
	ui.YLabelInPlaneRotationSlider->setValue(axes.labelOrientation(1).y);
	ui.YLabelInPlaneRotationSpin->setValue(axes.labelOrientation(1).y);
	ui.YLabelDistanceSpin->setValue(axes.labelOrientation(1).z);
	ui.YTitleHOffsetSlider->setValue(axes.titleOrientation(1).w*1000);
	ui.YTitleAnchorCombo->setCurrentIndex(axes.titleAnchor(1));
	ui.YTitleAxialRotationSlider->setValue(axes.titleOrientation(1).x);
	ui.YTitleAxialRotationSpin->setValue(axes.titleOrientation(1).x);
	ui.YTitleInPlaneRotationSlider->setValue(axes.titleOrientation(1).y);
	ui.YTitleInPlaneRotationSpin->setValue(axes.titleOrientation(1).y);
	ui.YTitleDistanceSpin->setValue(axes.titleOrientation(1).z);
	// -- Z
	ui.ZNumberFormatTestLabel->setText(axes.numberFormat(2).format(1.23456789));
	ui.ZLabelAnchorCombo->setCurrentIndex(axes.labelAnchor(2));
	ui.ZLabelAxialRotationSlider->setValue(axes.labelOrientation(2).x);
	ui.ZLabelAxialRotationSpin->setValue(axes.labelOrientation(2).x);
	ui.ZLabelInPlaneRotationSlider->setValue(axes.labelOrientation(2).y);
	ui.ZLabelInPlaneRotationSpin->setValue(axes.labelOrientation(2).y);
	ui.ZLabelDistanceSpin->setValue(axes.labelOrientation(2).z);
	ui.ZTitleHOffsetSlider->setValue(axes.titleOrientation(2).w*1000);
	ui.ZTitleAnchorCombo->setCurrentIndex(axes.titleAnchor(2));
	ui.ZTitleAxialRotationSlider->setValue(axes.titleOrientation(2).x);
	ui.ZTitleAxialRotationSpin->setValue(axes.titleOrientation(2).x);
	ui.ZTitleInPlaneRotationSlider->setValue(axes.titleOrientation(2).y);
	ui.ZTitleInPlaneRotationSpin->setValue(axes.titleOrientation(2).y);
	ui.ZTitleDistanceSpin->setValue(axes.titleOrientation(2).z);

	// Grid
	// -- X
	ui.XGridLineMajorCheck->setChecked(axes.gridLinesMajor(0));
	ui.XGridLineMinorCheck->setChecked(axes.gridLinesMinor(0));
	ui.XGridLineFullCheck->setChecked(axes.gridLinesFull(0));
	// -- Y
	ui.YGridLineMajorCheck->setChecked(axes.gridLinesMajor(1));
	ui.YGridLineMinorCheck->setChecked(axes.gridLinesMinor(1));
	ui.YGridLineFullCheck->setChecked(axes.gridLinesFull(1));
	// -- Z
	ui.ZGridLineMajorCheck->setChecked(axes.gridLinesMajor(2));
	ui.ZGridLineMinorCheck->setChecked(axes.gridLinesMinor(2));
	ui.ZGridLineFullCheck->setChecked(axes.gridLinesFull(2));

	refreshing_ = false;
}
