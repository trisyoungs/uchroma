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
	ui.AxisXPositionYFractionalSpin->setRange(true, 0.0, true, 1.0);
	ui.AxisXPositionZFractionalSpin->setRange(true, 0.0, true, 1.0);
	ui.AxisYPositionXFractionalSpin->setRange(true, 0.0, true, 1.0);
	ui.AxisYPositionZFractionalSpin->setRange(true, 0.0, true, 1.0);
	ui.AxisZPositionYFractionalSpin->setRange(true, 0.0, true, 1.0);
	ui.AxisZPositionYFractionalSpin->setRange(true, 0.0, true, 1.0);
	ui.AxisXPositionYFractionalSpin->setSingleStep(0.1);
	ui.AxisXPositionZFractionalSpin->setSingleStep(0.1);
	ui.AxisYPositionXFractionalSpin->setSingleStep(0.1);
	ui.AxisYPositionZFractionalSpin->setSingleStep(0.1);
	ui.AxisZPositionYFractionalSpin->setSingleStep(0.1);
	ui.AxisZPositionYFractionalSpin->setSingleStep(0.1);

	// Add anchor types on to combo boxes
	for (int n=0; n<TextPrimitive::nTextAnchors; ++n)
	{
		ui.AxisXTitleAnchorCombo->addItem(TextPrimitive::textAnchor( (TextPrimitive::TextAnchor) n));
		ui.AxisXLabelAnchorCombo->addItem(TextPrimitive::textAnchor( (TextPrimitive::TextAnchor) n));
		ui.AxisYTitleAnchorCombo->addItem(TextPrimitive::textAnchor( (TextPrimitive::TextAnchor) n));
		ui.AxisYLabelAnchorCombo->addItem(TextPrimitive::textAnchor( (TextPrimitive::TextAnchor) n));
		ui.AxisZTitleAnchorCombo->addItem(TextPrimitive::textAnchor( (TextPrimitive::TextAnchor) n));
		ui.AxisZLabelAnchorCombo->addItem(TextPrimitive::textAnchor( (TextPrimitive::TextAnchor) n));
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
		ui.AxisXPositionYRealSpin->setDisabled(fractional);
		ui.AxisXPositionYFractionalSpin->setEnabled(fractional);
		ui.AxisXPositionZRealSpin->setDisabled(fractional);
		ui.AxisXPositionZFractionalSpin->setEnabled(fractional);
		ui.AxisXPositionYSetZeroButton->setIcon(fractional ? QIcon(":/uchroma/icons/tocentre.svg") : QIcon(":/uchroma/icons/tozero.svg"));
		ui.AxisXPositionZSetZeroButton->setIcon(fractional ? QIcon(":/uchroma/icons/tocentre.svg") : QIcon(":/uchroma/icons/tozero.svg"));
		ui.AxisXPositionYSetZeroButton->setToolTip(fractional ? "Set to midpoint" : "Set to zero");
		ui.AxisXPositionZSetZeroButton->setToolTip(fractional ? "Set to midpoint" : "Set to zero");
	}
	else if (axis == 1)
	{
		ui.AxisYPositionXRealSpin->setDisabled(fractional);
		ui.AxisYPositionXFractionalSpin->setEnabled(fractional);
		ui.AxisYPositionZRealSpin->setDisabled(fractional);
		ui.AxisYPositionZFractionalSpin->setEnabled(fractional);
		ui.AxisYPositionXSetZeroButton->setIcon(fractional ? QIcon(":/uchroma/icons/tocentre.svg") : QIcon(":/uchroma/icons/tozero.svg"));
		ui.AxisYPositionZSetZeroButton->setIcon(fractional ? QIcon(":/uchroma/icons/tocentre.svg") : QIcon(":/uchroma/icons/tozero.svg"));
		ui.AxisYPositionXSetZeroButton->setToolTip(fractional ? "Set to midpoint" : "Set to zero");
		ui.AxisYPositionZSetZeroButton->setToolTip(fractional ? "Set to midpoint" : "Set to zero");
	}
	else if (axis == 2)
	{
		ui.AxisZPositionXRealSpin->setDisabled(fractional);
		ui.AxisZPositionXFractionalSpin->setEnabled(fractional);
		ui.AxisZPositionYRealSpin->setDisabled(fractional);
		ui.AxisZPositionYFractionalSpin->setEnabled(fractional);
		ui.AxisZPositionXSetZeroButton->setIcon(fractional ? QIcon(":/uchroma/icons/tocentre.svg") : QIcon(":/uchroma/icons/tozero.svg"));
		ui.AxisZPositionYSetZeroButton->setIcon(fractional ? QIcon(":/uchroma/icons/tocentre.svg") : QIcon(":/uchroma/icons/tozero.svg"));
		ui.AxisZPositionXSetZeroButton->setToolTip(fractional ? "Set to midpoint" : "Set to zero");
		ui.AxisZPositionYSetZeroButton->setToolTip(fractional ? "Set to midpoint" : "Set to zero");
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

/*
 * Slots
 */

// X Axis - General

void AxesWindow::on_AxisXInvertCheck_clicked(bool checked)
{
	axisInvertChanged(0, checked);
}

void AxesWindow::on_AxisXLogarithmicCheck_clicked(bool checked)
{
	axisLogarithmicChanged(0, checked);
}

void AxesWindow::on_AxisXVisibleCheck_clicked(bool checked)
{
	axisVisibleChanged(0, checked);
}

void AxesWindow::on_AxisXTitleEdit_textChanged(QString text)
{
	axisTitleChanged(0, text);
}

void AxesWindow::on_AxisXStretchSpin_valueChanged(double value)
{
	axisStretchChanged(0, value);
}

void AxesWindow::on_AxisXMinSpin_valueChanged(double value)
{
	axisLimitChanged(0, true, value);
}

void AxesWindow::on_AxisXMinSetMinimumButton_clicked(bool checked)
{
	axisLimitSetExtreme(0, true);
}

void AxesWindow::on_AxisXMaxSpin_valueChanged(double value)
{
	axisLimitChanged(0, false, value);
}

void AxesWindow::on_AxisXMaxSetMaximumButton_clicked(bool checked)
{
	axisLimitSetExtreme(0, false);
}

void AxesWindow::on_AxisXPositionRealRadio_clicked(bool checked)
{
	axisPositionIsFractionalChanged(0, !checked);
}

void AxesWindow::on_AxisXPositionFractionalRadio_clicked(bool checked)
{
	axisPositionIsFractionalChanged(0, checked);
}

void AxesWindow::on_AxisXPositionYRealSpin_valueChanged(double value)
{
	axisPositionChanged(true, 0, 1, value);
}

void AxesWindow::on_AxisXPositionYFractionalSpin_valueChanged(double value)
{
	axisPositionChanged(false, 0, 1, value);
}

void AxesWindow::on_AxisXPositionZRealSpin_valueChanged(double value)
{
	axisPositionChanged(true, 0, 2, value);
}

void AxesWindow::on_AxisXPositionZFractionalSpin_valueChanged(double value)
{
	axisPositionChanged(false, 0, 2, value);
}

void AxesWindow::on_AxisXPositionYSetMinimumButton_clicked(bool checked)
{
	axisPositionSet(ui.AxisXPositionRealRadio->isChecked(), 0, 1, -1);
}

void AxesWindow::on_AxisXPositionYSetZeroButton_clicked(bool checked)
{
	axisPositionSet(ui.AxisXPositionRealRadio->isChecked(), 0, 1, 0);
}

void AxesWindow::on_AxisXPositionYSetMaximumButton_clicked(bool checked)
{
	axisPositionSet(ui.AxisXPositionRealRadio->isChecked(), 0, 1, 1);
}

void AxesWindow::on_AxisXPositionZSetMinimumButton_clicked(bool checked)
{
	axisPositionSet(ui.AxisXPositionRealRadio->isChecked(), 0, 2, -1);
}

void AxesWindow::on_AxisXPositionZSetZeroButton_clicked(bool checked)
{
	axisPositionSet(ui.AxisXPositionRealRadio->isChecked(), 0, 2, 0);
}

void AxesWindow::on_AxisXPositionZSetMaximumButton_clicked(bool checked)
{
	axisPositionSet(ui.AxisXPositionRealRadio->isChecked(), 0, 2, 1);
}

// X Axis - Ticks

void AxesWindow::on_AxisXAutoTicksCheck_clicked(bool checked)
{
	axisAutoTicksChanged(0, checked);
}

void AxesWindow::on_AxisXTicksStartSpin_valueChanged(double value)
{
	axisTicksChanged(0, true, value);
}

void AxesWindow::on_AxisXTicksDeltaSpin_valueChanged(double value)
{
	axisTicksChanged(0, false, value);
}

void AxesWindow::on_AxisXMinorTicksSpin_valueChanged(int value)
{
	axisMinorTicksChanged(0, value);
}

void AxesWindow::on_AxisXTickDirectionXSpin_valueChanged(double value)
{
	axisTickOrientationChanged(0, 0, value);
}

void AxesWindow::on_AxisXTickDirectionYSpin_valueChanged(double value)
{
	axisTickOrientationChanged(0, 1, value);
}

void AxesWindow::on_AxisXTickDirectionZSpin_valueChanged(double value)
{
	axisTickOrientationChanged(0, 2, value);
}

void AxesWindow::on_AxisXTickSizeSpin_valueChanged(double value)
{
	axisTickSizeChanged(0, value);
}

// X Axis - Labels

void AxesWindow::on_AxisXLabelAnchorCombo_currentIndexChanged(int index)
{
	axisAnchorChanged(0, false, (TextPrimitive::TextAnchor) index);
}

void AxesWindow::on_AxisXLabelAxialRotationSlider_valueChanged(int value)
{
	ui.AxisXLabelAxialRotationSpin->setValue(value);
	axisLabelOrientationChanged(0, 0, value);
}

void AxesWindow::on_AxisXLabelAxialRotationSpin_valueChanged(int value)
{
	ui.AxisXLabelAxialRotationSlider->setValue(value);
	axisLabelOrientationChanged(0, 0, value);
}

void AxesWindow::on_AxisXLabelInPlaneRotationSlider_valueChanged(int value)
{
	ui.AxisXLabelInPlaneRotationSpin->setValue(value);
	axisLabelOrientationChanged(0, 1, value);
}

void AxesWindow::on_AxisXLabelInPlaneRotationSpin_valueChanged(int value)
{
	ui.AxisXLabelInPlaneRotationSlider->setValue(value);
	axisLabelOrientationChanged(0, 1, value);
}

void AxesWindow::on_AxisXLabelDistanceSpin_valueChanged(double value)
{
	axisLabelOrientationChanged(0, 2, value);
}

void AxesWindow::on_AxisXTitleHOffsetSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(0, 3, double(value) / 1000.0);
}

void AxesWindow::on_AxisXTitleHOffsetLeftButton_clicked(bool checked)
{
	ui.AxisXTitleHOffsetSlider->setValue(0);
	axisTitleOrientationChanged(0, 3, 0.0);
}

void AxesWindow::on_AxisXTitleHOffsetCentreButton_clicked(bool checked)
{
	ui.AxisXTitleHOffsetSlider->setValue(500);
	axisTitleOrientationChanged(0, 3, 0.5);
}

void AxesWindow::on_AxisXTitleHOffsetRightButton_clicked(bool checked)
{
	ui.AxisXTitleHOffsetSlider->setValue(1000);
	axisTitleOrientationChanged(0, 3, 1.0);
}

void AxesWindow::on_AxisXTitleAnchorCombo_currentIndexChanged(int index)
{
	axisAnchorChanged(0, true, (TextPrimitive::TextAnchor) index);
}

void AxesWindow::on_AxisXTitleAxialRotationSlider_valueChanged(int value)
{
	ui.AxisXTitleAxialRotationSpin->setValue(value);
	axisTitleOrientationChanged(0, 0, value);
}

void AxesWindow::on_AxisXTitleAxialRotationSpin_valueChanged(int value)
{
	ui.AxisXTitleAxialRotationSlider->setValue(value);
	axisTitleOrientationChanged(0, 0, value);
}

void AxesWindow::on_AxisXTitleInPlaneRotationSlider_valueChanged(int value)
{
	ui.AxisXTitleInPlaneRotationSpin->setValue(value);
	axisTitleOrientationChanged(0, 1, value);
}

void AxesWindow::on_AxisXTitleInPlaneRotationSpin_valueChanged(int value)
{
	ui.AxisXTitleInPlaneRotationSlider->setValue(value);
	axisTitleOrientationChanged(0, 1, value);
}

void AxesWindow::on_AxisXTitleDistanceSpin_valueChanged(double value)
{
	axisTitleOrientationChanged(0, 2, value);
}

// X Axis - GridLines

void AxesWindow::on_AxisXGridLineMajorCheck_clicked(bool checked)
{
	axisGridLineChanged(0, true, checked);
}

void AxesWindow::on_AxisXGridLineMinorCheck_clicked(bool checked)
{
	axisGridLineChanged(0, false, checked);
}

void AxesWindow::on_AxisXGridLineFullCheck_clicked(bool checked)
{
	axisGridFullChanged(0, checked);
}

void AxesWindow::on_AxisXGridLineMajorStyleButton_clicked(bool checked)
{
	axisGridStyleClicked(0, true);
}

void AxesWindow::on_AxisXGridLineMinorStyleButton_clicked(bool checked)
{
	axisGridStyleClicked(0, false);
}

void AxesWindow::on_AxisXGridLineApplyStyleButton_clicked(bool checked)
{
	axisGridStyleApplyClicked(0);
}

// Y Axis - General

void AxesWindow::on_AxisYInvertCheck_clicked(bool checked)
{
	axisInvertChanged(1, checked);
}

void AxesWindow::on_AxisYLogarithmicCheck_clicked(bool checked)
{
	axisLogarithmicChanged(1, checked);
}

void AxesWindow::on_AxisYVisibleCheck_clicked(bool checked)
{
	axisVisibleChanged(1, checked);
}

void AxesWindow::on_AxisYTitleEdit_textChanged(QString text)
{
	axisTitleChanged(1, text);
}

void AxesWindow::on_AxisYStretchSpin_valueChanged(double value)
{
	axisStretchChanged(1, value);
}

void AxesWindow::on_AxisYMinSpin_valueChanged(double value)
{
	axisLimitChanged(1, true, value);
}

void AxesWindow::on_AxisYMinSetMinimumButton_clicked(bool checked)
{
	axisLimitSetExtreme(1, true);
}

void AxesWindow::on_AxisYMaxSpin_valueChanged(double value)
{
	axisLimitChanged(1, false, value);
}

void AxesWindow::on_AxisYMaxSetMaximumButton_clicked(bool checked)
{
	axisLimitSetExtreme(1, false);
}

void AxesWindow::on_AxisYPositionRealRadio_clicked(bool checked)
{
	axisPositionIsFractionalChanged(1, !checked);
}

void AxesWindow::on_AxisYPositionFractionalRadio_clicked(bool checked)
{
	axisPositionIsFractionalChanged(1, checked);
}

void AxesWindow::on_AxisYPositionXRealSpin_valueChanged(double value)
{
	axisPositionChanged(true, 1, 0, value);
}

void AxesWindow::on_AxisYPositionXFractionalSpin_valueChanged(double value)
{
	axisPositionChanged(false, 1, 0, value);
}

void AxesWindow::on_AxisYPositionZRealSpin_valueChanged(double value)
{
	axisPositionChanged(true, 1, 2, value);
}

void AxesWindow::on_AxisYPositionZFractionalSpin_valueChanged(double value)
{
	axisPositionChanged(false, 1, 2, value);
}

void AxesWindow::on_AxisYPositionXSetMinimumButton_clicked(bool checked)
{
	axisPositionSet(ui.AxisYPositionRealRadio->isChecked(), 1, 0, -1);
}

void AxesWindow::on_AxisYPositionXSetZeroButton_clicked(bool checked)
{
	axisPositionSet(ui.AxisYPositionRealRadio->isChecked(), 1, 0, 0);
}

void AxesWindow::on_AxisYPositionXSetMaximumButton_clicked(bool checked)
{
	axisPositionSet(ui.AxisYPositionRealRadio->isChecked(), 1, 0, 1);
}

void AxesWindow::on_AxisYPositionZSetMinimumButton_clicked(bool checked)
{
	axisPositionSet(ui.AxisYPositionRealRadio->isChecked(), 1, 2, -1);
}

void AxesWindow::on_AxisYPositionZSetZeroButton_clicked(bool checked)
{
	axisPositionSet(ui.AxisYPositionRealRadio->isChecked(), 1, 2, 0);
}

void AxesWindow::on_AxisYPositionZSetMaximumButton_clicked(bool checked)
{
	axisPositionSet(ui.AxisYPositionRealRadio->isChecked(), 1, 2, 1);
}

// Y Axis - Ticks

void AxesWindow::on_AxisYAutoTicksCheck_clicked(bool checked)
{
	axisAutoTicksChanged(1, checked);
}

void AxesWindow::on_AxisYTicksStartSpin_valueChanged(double value)
{
	axisTicksChanged(1, true, value);
}

void AxesWindow::on_AxisYTicksDeltaSpin_valueChanged(double value)
{
	axisTicksChanged(1, false, value);
}

void AxesWindow::on_AxisYMinorTicksSpin_valueChanged(int value)
{
	axisMinorTicksChanged(1, value);
}

void AxesWindow::on_AxisYTickDirectionXSpin_valueChanged(double value)
{
	axisTickOrientationChanged(1, 0, value);
}

void AxesWindow::on_AxisYTickDirectionYSpin_valueChanged(double value)
{
	axisTickOrientationChanged(1, 1, value);
}

void AxesWindow::on_AxisYTickDirectionZSpin_valueChanged(double value)
{
	axisTickOrientationChanged(1, 2, value);
}

void AxesWindow::on_AxisYTickSizeSpin_valueChanged(double value)
{
	axisTickSizeChanged(1, value);
}

// Y Axis - Labels

void AxesWindow::on_AxisYLabelAnchorCombo_currentIndexChanged(int index)
{
	axisAnchorChanged(1, false, (TextPrimitive::TextAnchor) index);
}

void AxesWindow::on_AxisYLabelAxialRotationSlider_valueChanged(int value)
{
	ui.AxisYLabelAxialRotationSpin->setValue(value);
	axisLabelOrientationChanged(1, 0, value);
}

void AxesWindow::on_AxisYLabelAxialRotationSpin_valueChanged(int value)
{
	ui.AxisYLabelAxialRotationSlider->setValue(value);
	axisLabelOrientationChanged(1, 0, value);
}

void AxesWindow::on_AxisYLabelInPlaneRotationSlider_valueChanged(int value)
{
	ui.AxisYLabelInPlaneRotationSpin->setValue(value);
	axisLabelOrientationChanged(1, 1, value);
}

void AxesWindow::on_AxisYLabelInPlaneRotationSpin_valueChanged(int value)
{
	ui.AxisYLabelInPlaneRotationSlider->setValue(value);
	axisLabelOrientationChanged(1, 1, value);
}

void AxesWindow::on_AxisYLabelDistanceSpin_valueChanged(double value)
{
	axisLabelOrientationChanged(1, 2, value);
}

void AxesWindow::on_AxisYTitleHOffsetSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(1, 3, double(value) / 1000.0);
}

void AxesWindow::on_AxisYTitleHOffsetLeftButton_clicked(bool checked)
{
	ui.AxisYTitleHOffsetSlider->setValue(0);
	axisTitleOrientationChanged(1, 3, 0.0);
}

void AxesWindow::on_AxisYTitleHOffsetCentreButton_clicked(bool checked)
{
	ui.AxisYTitleHOffsetSlider->setValue(500);
	axisTitleOrientationChanged(1, 3, 0.5);
}

void AxesWindow::on_AxisYTitleHOffsetRightButton_clicked(bool checked)
{
	ui.AxisYTitleHOffsetSlider->setValue(1000);
	axisTitleOrientationChanged(1, 3, 1.0);
}

void AxesWindow::on_AxisYTitleAnchorCombo_currentIndexChanged(int index)
{
	axisAnchorChanged(1, true, (TextPrimitive::TextAnchor) index);
}

void AxesWindow::on_AxisYTitleAxialRotationSlider_valueChanged(int value)
{
	ui.AxisYTitleAxialRotationSpin->setValue(value);
	axisTitleOrientationChanged(1, 0, value);
}

void AxesWindow::on_AxisYTitleAxialRotationSpin_valueChanged(int value)
{
	ui.AxisYTitleAxialRotationSlider->setValue(value);
	axisTitleOrientationChanged(1, 0, value);
}

void AxesWindow::on_AxisYTitleInPlaneRotationSlider_valueChanged(int value)
{
	ui.AxisYTitleInPlaneRotationSpin->setValue(value);
	axisTitleOrientationChanged(1, 1, value);
}

void AxesWindow::on_AxisYTitleInPlaneRotationSpin_valueChanged(int value)
{
	ui.AxisYTitleInPlaneRotationSlider->setValue(value);
	axisTitleOrientationChanged(1, 1, value);
}

void AxesWindow::on_AxisYTitleDistanceSpin_valueChanged(double value)
{
	axisTitleOrientationChanged(1, 2, value);
}

// Y Axis - Grid

void AxesWindow::on_AxisYGridLineMajorCheck_clicked(bool checked)
{
	axisGridLineChanged(1, true, checked);
}

void AxesWindow::on_AxisYGridLineMinorCheck_clicked(bool checked)
{
	axisGridLineChanged(1, false, checked);
}

void AxesWindow::on_AxisYGridLineFullCheck_clicked(bool checked)
{
	axisGridFullChanged(1, checked);
}

void AxesWindow::on_AxisYGridLineMajorStyleButton_clicked(bool checked)
{
	axisGridStyleClicked(1, true);
}

void AxesWindow::on_AxisYGridLineMinorStyleButton_clicked(bool checked)
{
	axisGridStyleClicked(1, false);
}

void AxesWindow::on_AxisYGridLineApplyStyleButton_clicked(bool checked)
{
	axisGridStyleApplyClicked(1);
}

// Z Axis - General

void AxesWindow::on_AxisZInvertCheck_clicked(bool checked)
{
	axisInvertChanged(2, checked);
}

void AxesWindow::on_AxisZLogarithmicCheck_clicked(bool checked)
{
	axisLogarithmicChanged(2, checked);
}

void AxesWindow::on_AxisZVisibleCheck_clicked(bool checked)
{
	axisVisibleChanged(2, checked);
}

void AxesWindow::on_AxisZTitleEdit_textChanged(QString text)
{
	axisTitleChanged(2, text);
}

void AxesWindow::on_AxisZStretchSpin_valueChanged(double value)
{
	axisStretchChanged(2, value);
}

void AxesWindow::on_AxisZMinSpin_valueChanged(double value)
{
	axisLimitChanged(2, true, value);
}

void AxesWindow::on_AxisZMinSetMinimumButton_clicked(bool checked)
{
	axisLimitSetExtreme(2, true);
}

void AxesWindow::on_AxisZMaxSpin_valueChanged(double value)
{
	axisLimitChanged(2, false, value);
}

void AxesWindow::on_AxisZMaxSetMaximumButton_clicked(bool checked)
{
	axisLimitSetExtreme(2, false);
}

void AxesWindow::on_AxisZPositionRealRadio_clicked(bool checked)
{
	axisPositionIsFractionalChanged(2, !checked);
}

void AxesWindow::on_AxisZPositionFractionalRadio_clicked(bool checked)
{
	axisPositionIsFractionalChanged(2, checked);
}

void AxesWindow::on_AxisZPositionXRealSpin_valueChanged(double value)
{
	axisPositionChanged(true, 2, 0, value);
}

void AxesWindow::on_AxisZPositionXFractionalSpin_valueChanged(double value)
{
	axisPositionChanged(false, 2, 0, value);
}

void AxesWindow::on_AxisZPositionYRealSpin_valueChanged(double value)
{
	axisPositionChanged(true, 2, 1, value);
}

void AxesWindow::on_AxisZPositionYFractionalSpin_valueChanged(double value)
{
	axisPositionChanged(false, 2, 1, value);
}

void AxesWindow::on_AxisZPositionXSetMinimumButton_clicked(bool checked)
{
	axisPositionSet(ui.AxisZPositionRealRadio->isChecked(), 2, 0, -1);
}

void AxesWindow::on_AxisZPositionXSetZeroButton_clicked(bool checked)
{
	axisPositionSet(ui.AxisZPositionRealRadio->isChecked(), 2, 0, 0);
}

void AxesWindow::on_AxisZPositionXSetMaximumButton_clicked(bool checked)
{
	axisPositionSet(ui.AxisZPositionRealRadio->isChecked(), 2, 0, 1);
}

void AxesWindow::on_AxisZPositionYSetMinimumButton_clicked(bool checked)
{
	axisPositionSet(ui.AxisZPositionRealRadio->isChecked(), 2, 1, -1);
}

void AxesWindow::on_AxisZPositionYSetZeroButton_clicked(bool checked)
{
	axisPositionSet(ui.AxisZPositionRealRadio->isChecked(), 2, 1, 0);
}

void AxesWindow::on_AxisZPositionYSetMaximumButton_clicked(bool checked)
{
	axisPositionSet(ui.AxisZPositionRealRadio->isChecked(), 2, 1, 1);
}

// Z Axis - Ticks

void AxesWindow::on_AxisZAutoTicksCheck_clicked(bool checked)
{
	axisAutoTicksChanged(2, checked);
}

void AxesWindow::on_AxisZTicksStartSpin_valueChanged(double value)
{
	axisTicksChanged(2, true, value);
}

void AxesWindow::on_AxisZTicksDeltaSpin_valueChanged(double value)
{
	axisTicksChanged(2, false, value);
}

void AxesWindow::on_AxisZMinorTicksSpin_valueChanged(int value)
{
	axisMinorTicksChanged(2, value);
}

void AxesWindow::on_AxisZTickDirectionXSpin_valueChanged(double value)
{
	axisTickOrientationChanged(2, 0, value);
}

void AxesWindow::on_AxisZTickDirectionYSpin_valueChanged(double value)
{
	axisTickOrientationChanged(2, 1, value);
}

void AxesWindow::on_AxisZTickDirectionZSpin_valueChanged(double value)
{
	axisTickOrientationChanged(2, 2, value);
}

void AxesWindow::on_AxisZTickSizeSpin_valueChanged(double value)
{
	axisTickSizeChanged(2, value);
}

// Z Axis - Labels

void AxesWindow::on_AxisZLabelAnchorCombo_currentIndexChanged(int index)
{
	axisAnchorChanged(2, false, (TextPrimitive::TextAnchor) index);
}

void AxesWindow::on_AxisZLabelAxialRotationSlider_valueChanged(int value)
{
	ui.AxisZLabelAxialRotationSpin->setValue(value);
	axisLabelOrientationChanged(2, 0, value);
}

void AxesWindow::on_AxisZLabelAxialRotationSpin_valueChanged(int value)
{
	ui.AxisZLabelAxialRotationSlider->setValue(value);
	axisLabelOrientationChanged(2, 0, value);
}

void AxesWindow::on_AxisZLabelInPlaneRotationSlider_valueChanged(int value)
{
	ui.AxisZLabelInPlaneRotationSpin->setValue(value);
	axisLabelOrientationChanged(2, 1, value);
}

void AxesWindow::on_AxisZLabelInPlaneRotationSpin_valueChanged(int value)
{
	ui.AxisZLabelInPlaneRotationSlider->setValue(value);
	axisLabelOrientationChanged(2, 1, value);
}

void AxesWindow::on_AxisZLabelDistanceSpin_valueChanged(double value)
{
	axisLabelOrientationChanged(2, 2, value);
}

void AxesWindow::on_AxisZTitleHOffsetSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(2, 3, double(value) / 1000.0);
}

void AxesWindow::on_AxisZTitleHOffsetLeftButton_clicked(bool checked)
{
	ui.AxisZTitleHOffsetSlider->setValue(0);
	axisTitleOrientationChanged(2, 3, 0.0);
}

void AxesWindow::on_AxisZTitleHOffsetCentreButton_clicked(bool checked)
{
	ui.AxisZTitleHOffsetSlider->setValue(500);
	axisTitleOrientationChanged(2, 3, 0.5);
}

void AxesWindow::on_AxisZTitleHOffsetRightButton_clicked(bool checked)
{
	ui.AxisZTitleHOffsetSlider->setValue(1000);
	axisTitleOrientationChanged(2, 3, 1.0);
}

void AxesWindow::on_AxisZTitleAnchorCombo_currentIndexChanged(int index)
{
	axisAnchorChanged(2, true, (TextPrimitive::TextAnchor) index);
}

void AxesWindow::on_AxisZTitleAxialRotationSlider_valueChanged(int value)
{
	ui.AxisZTitleAxialRotationSpin->setValue(value);
	axisTitleOrientationChanged(2, 0, value);
}

void AxesWindow::on_AxisZTitleAxialRotationSpin_valueChanged(int value)
{
	ui.AxisZTitleAxialRotationSlider->setValue(value);
	axisTitleOrientationChanged(2, 0, value);
}

void AxesWindow::on_AxisZTitleInPlaneRotationSlider_valueChanged(int value)
{
	ui.AxisZTitleInPlaneRotationSpin->setValue(value);
	axisTitleOrientationChanged(2, 1, value);
}

void AxesWindow::on_AxisZTitleInPlaneRotationSpin_valueChanged(int value)
{
	ui.AxisZTitleInPlaneRotationSlider->setValue(value);
	axisTitleOrientationChanged(2, 1, value);
}

void AxesWindow::on_AxisZTitleDistanceSpin_valueChanged(double value)
{
	axisTitleOrientationChanged(2, 2, value);
}

// Z Axis - Grid

void AxesWindow::on_AxisZGridLineMajorCheck_clicked(bool checked)
{
	axisGridLineChanged(2, true, checked);
}

void AxesWindow::on_AxisZGridLineMinorCheck_clicked(bool checked)
{
	axisGridLineChanged(2, false, checked);
}

void AxesWindow::on_AxisZGridLineFullCheck_clicked(bool checked)
{
	axisGridFullChanged(2, checked);
}

void AxesWindow::on_AxisZGridLineMajorStyleButton_clicked(bool checked)
{
	axisGridStyleClicked(2, true);
}

void AxesWindow::on_AxisZGridLineMinorStyleButton_clicked(bool checked)
{
	axisGridStyleClicked(2, false);
}

void AxesWindow::on_AxisZGridLineApplyStyleButton_clicked(bool checked)
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
	ui.AxisXInvertCheck->setChecked(axes.inverted(0));
	ui.AxisYInvertCheck->setChecked(axes.inverted(1));
	ui.AxisZInvertCheck->setChecked(axes.inverted(2));
	ui.AxisXVisibleCheck->setChecked(axes.visible(0));
	ui.AxisYVisibleCheck->setChecked(axes.visible(1));
	ui.AxisZVisibleCheck->setChecked(axes.visible(2));
	ui.AxisXLogarithmicCheck->setChecked(axes.logarithmic(0));
	ui.AxisYLogarithmicCheck->setChecked(axes.logarithmic(1));
	ui.AxisZLogarithmicCheck->setChecked(axes.logarithmic(2));
	ui.AxisXTicksDeltaSpin->setDisabled(axes.logarithmic(0));
	ui.AxisYTicksDeltaSpin->setDisabled(axes.logarithmic(1));
	ui.AxisZTicksDeltaSpin->setDisabled(axes.logarithmic(2));
	ui.AxisXAutoTicksCheck->setDisabled(axes.logarithmic(0));
	ui.AxisYAutoTicksCheck->setDisabled(axes.logarithmic(1));
	ui.AxisZAutoTicksCheck->setDisabled(axes.logarithmic(2));

	// Axis titles
	ui.AxisXTitleEdit->setText(axes.title(0));
	ui.AxisYTitleEdit->setText(axes.title(1));
	ui.AxisZTitleEdit->setText(axes.title(2));

	// Axis Stretch factors
	bool stretchOff = (pane->twoDimensional() || pane->autoStretch3D());
	ui.AxisXStretchSpin->setValue(axes.stretch(0));
	ui.AxisXStretchSpin->setDisabled(stretchOff);
	ui.AxisYStretchSpin->setValue(axes.stretch(1));
	ui.AxisYStretchSpin->setDisabled(stretchOff);
	ui.AxisZStretchSpin->setValue(axes.stretch(2));
	ui.AxisZStretchSpin->setDisabled(stretchOff);

	// Axis Min/Max Limits
	ui.AxisXMinSpin->setRange(axes.logarithmic(0), axes.limitMin(0), false, 0.0);
	ui.AxisYMinSpin->setRange(axes.logarithmic(1), axes.limitMin(1), false, 0.0);
	ui.AxisZMinSpin->setRange(axes.logarithmic(2), axes.limitMin(2), false, 0.0);
	ui.AxisXMinSpin->setSingleStep(max((axes.limitMax(0)-axes.limitMin(0))*0.01, 1.0));
	ui.AxisYMinSpin->setSingleStep(max((axes.limitMax(1)-axes.limitMin(1))*0.01, 1.0));
	ui.AxisZMinSpin->setSingleStep(max((axes.limitMax(2)-axes.limitMin(2))*0.01, 1.0));

	ui.AxisXMaxSpin->setRange(axes.logarithmic(0), axes.limitMin(0), false, 0.0);
	ui.AxisYMaxSpin->setRange(axes.logarithmic(1), axes.limitMin(1), false, 0.0);
	ui.AxisZMaxSpin->setRange(axes.logarithmic(2), axes.limitMin(2), false, 0.0);
	ui.AxisXMaxSpin->setSingleStep(max((axes.limitMax(0)-axes.limitMin(0))*0.01, 1.0));
	ui.AxisYMaxSpin->setSingleStep(max((axes.limitMax(1)-axes.limitMin(1))*0.01, 1.0));
	ui.AxisZMaxSpin->setSingleStep(max((axes.limitMax(2)-axes.limitMin(2))*0.01, 1.0));
	ui.AxisXMinSpin->setValue(axes.min(0));
	ui.AxisYMinSpin->setValue(axes.min(1));
	ui.AxisZMinSpin->setValue(axes.min(2));
	ui.AxisXMaxSpin->setValue(axes.max(0));
	ui.AxisYMaxSpin->setValue(axes.max(1));
	ui.AxisZMaxSpin->setValue(axes.max(2));
	ui.AxisXMinLabel->setText("DataMin: " + QString::number(axes.limitMin(0)));
	ui.AxisXMaxLabel->setText("DataMax: " + QString::number(axes.limitMax(0)));
	ui.AxisYMinLabel->setText("DataMin: " + QString::number(axes.limitMin(1)));
	ui.AxisYMaxLabel->setText("DataMax: " + QString::number(axes.limitMax(1)));
	ui.AxisZMinLabel->setText("DataMin: " + QString::number(axes.limitMin(2)));
	ui.AxisZMaxLabel->setText("DataMax: " + QString::number(axes.limitMax(2)));

	// Axis positions
	// -- X
	ui.AxisXPositionFractionalRadio->setChecked(axes.positionIsFractional(0));
	axisPositionIsFractionalChanged(0, axes.positionIsFractional(0));
	ui.AxisXPositionYRealSpin->setRange(axes.logarithmic(1), axes.limitMin(1), false, 0.0);
	ui.AxisXPositionYRealSpin->setValue(axes.positionReal(0).y);
	ui.AxisXPositionZRealSpin->setRange(axes.logarithmic(2), axes.limitMin(2), false, 0.0);
	ui.AxisXPositionZRealSpin->setValue(axes.positionReal(0).z);
	ui.AxisXPositionYFractionalSpin->setValue(axes.positionFractional(0).y);
	ui.AxisXPositionZFractionalSpin->setValue(axes.positionFractional(0).z);
	// -- Y
	ui.AxisYPositionFractionalRadio->setChecked(axes.positionIsFractional(1));
	axisPositionIsFractionalChanged(1, axes.positionIsFractional(1));
	ui.AxisYPositionXRealSpin->setRange(axes.logarithmic(0), axes.limitMin(0), false, 0.0);
	ui.AxisYPositionXRealSpin->setValue(axes.positionReal(1).x);
	ui.AxisYPositionZRealSpin->setRange(axes.logarithmic(2), axes.limitMin(2), false, 0.0);
	ui.AxisYPositionZRealSpin->setValue(axes.positionReal(1).z);
	ui.AxisYPositionXFractionalSpin->setValue(axes.positionFractional(1).x);
	ui.AxisYPositionZFractionalSpin->setValue(axes.positionFractional(1).z);
	// -- Z
	ui.AxisZPositionFractionalRadio->setChecked(axes.positionIsFractional(2));
	axisPositionIsFractionalChanged(2, axes.positionIsFractional(2));
	ui.AxisZPositionXRealSpin->setRange(axes.logarithmic(0), axes.limitMin(0), false, 0.0);
	ui.AxisZPositionXRealSpin->setValue(axes.positionReal(2).x);
	ui.AxisZPositionYRealSpin->setRange(axes.logarithmic(1), axes.limitMin(1), false, 0.0);
	ui.AxisZPositionYRealSpin->setValue(axes.positionReal(2).y);
	ui.AxisZPositionXFractionalSpin->setValue(axes.positionFractional(2).x);
	ui.AxisZPositionYFractionalSpin->setValue(axes.positionFractional(2).y);

	// Axis Ticks
	// -- X
	ui.AxisXAutoTicksCheck->setChecked(axes.autoTicks(0));
	ui.AxisXTicksStartSpin->setValue(axes.tickFirst(0));
	ui.AxisXTicksStartSpin->setEnabled(!axes.autoTicks(0));
	ui.AxisXTicksDeltaSpin->setValue(axes.tickDelta(0));
	ui.AxisXTicksDeltaSpin->setEnabled(!axes.autoTicks(0));
	ui.AxisXMinorTicksSpin->setValue(axes.minorTicks(0));
	ui.AxisXTickDirectionXSpin->setValue(axes.tickDirection(0).x);
	ui.AxisXTickDirectionYSpin->setValue(axes.tickDirection(0).y);
	ui.AxisXTickDirectionZSpin->setValue(axes.tickDirection(0).z);
	ui.AxisXTickSizeSpin->setValue(axes.tickSize(0));
	// -- Y
	ui.AxisYAutoTicksCheck->setChecked(axes.autoTicks(1));
	ui.AxisYTicksStartSpin->setValue(axes.tickFirst(1));
	ui.AxisYTicksStartSpin->setEnabled(!axes.autoTicks(1));
	ui.AxisYTicksDeltaSpin->setValue(axes.tickDelta(1));
	ui.AxisYTicksDeltaSpin->setEnabled(!axes.autoTicks(1));
	ui.AxisYMinorTicksSpin->setValue(axes.minorTicks(1));
	ui.AxisYTickDirectionXSpin->setValue(axes.tickDirection(1).x);
	ui.AxisYTickDirectionYSpin->setValue(axes.tickDirection(1).y);
	ui.AxisYTickDirectionZSpin->setValue(axes.tickDirection(1).z);
	// -- Z
	ui.AxisZAutoTicksCheck->setChecked(axes.autoTicks(2));
	ui.AxisZTicksStartSpin->setValue(axes.tickFirst(2));
	ui.AxisZTicksStartSpin->setEnabled(!axes.autoTicks(2));
	ui.AxisZTicksDeltaSpin->setValue(axes.tickDelta(2));
	ui.AxisZTicksDeltaSpin->setEnabled(!axes.autoTicks(2));
	ui.AxisZMinorTicksSpin->setValue(axes.minorTicks(2));
	ui.AxisZTickDirectionXSpin->setValue(axes.tickDirection(2).x);
	ui.AxisZTickDirectionYSpin->setValue(axes.tickDirection(2).y);
	ui.AxisZTickDirectionZSpin->setValue(axes.tickDirection(2).z);

	// Text Orientation
	// -- X
	ui.AxisXLabelAnchorCombo->setCurrentIndex(axes.labelAnchor(0));
	ui.AxisXLabelAxialRotationSlider->setValue(axes.labelOrientation(0).x);
	ui.AxisXLabelAxialRotationSpin->setValue(axes.labelOrientation(0).x);
	ui.AxisXLabelInPlaneRotationSlider->setValue(axes.labelOrientation(0).y);
	ui.AxisXLabelInPlaneRotationSpin->setValue(axes.labelOrientation(0).y);
	ui.AxisXLabelDistanceSpin->setValue(axes.labelOrientation(0).z);
	ui.AxisYTitleHOffsetSlider->setValue(axes.titleOrientation(0).w*1000);
	ui.AxisXTitleAnchorCombo->setCurrentIndex(axes.titleAnchor(0));
	ui.AxisXTitleAxialRotationSlider->setValue(axes.titleOrientation(0).x);
	ui.AxisXTitleAxialRotationSpin->setValue(axes.titleOrientation(0).x);
	ui.AxisXTitleInPlaneRotationSlider->setValue(axes.titleOrientation(0).y);
	ui.AxisXTitleInPlaneRotationSpin->setValue(axes.titleOrientation(0).y);
	ui.AxisXTitleDistanceSpin->setValue(axes.titleOrientation(0).z);
	// -- Y
	ui.AxisYLabelAnchorCombo->setCurrentIndex(axes.labelAnchor(1));
	ui.AxisYLabelAxialRotationSlider->setValue(axes.labelOrientation(1).x);
	ui.AxisYLabelAxialRotationSpin->setValue(axes.labelOrientation(1).x);
	ui.AxisYLabelInPlaneRotationSlider->setValue(axes.labelOrientation(1).y);
	ui.AxisYLabelInPlaneRotationSpin->setValue(axes.labelOrientation(1).y);
	ui.AxisYLabelDistanceSpin->setValue(axes.labelOrientation(1).z);
	ui.AxisYTitleHOffsetSlider->setValue(axes.titleOrientation(1).w*1000);
	ui.AxisYTitleAnchorCombo->setCurrentIndex(axes.titleAnchor(1));
	ui.AxisYTitleAxialRotationSlider->setValue(axes.titleOrientation(1).x);
	ui.AxisYTitleAxialRotationSpin->setValue(axes.titleOrientation(1).x);
	ui.AxisYTitleInPlaneRotationSlider->setValue(axes.titleOrientation(1).y);
	ui.AxisYTitleInPlaneRotationSpin->setValue(axes.titleOrientation(1).y);
	ui.AxisYTitleDistanceSpin->setValue(axes.titleOrientation(1).z);
	// -- Z
	ui.AxisZLabelAnchorCombo->setCurrentIndex(axes.labelAnchor(2));
	ui.AxisZLabelAxialRotationSlider->setValue(axes.labelOrientation(2).x);
	ui.AxisZLabelAxialRotationSpin->setValue(axes.labelOrientation(2).x);
	ui.AxisZLabelInPlaneRotationSlider->setValue(axes.labelOrientation(2).y);
	ui.AxisZLabelInPlaneRotationSpin->setValue(axes.labelOrientation(2).y);
	ui.AxisZLabelDistanceSpin->setValue(axes.labelOrientation(2).z);
	ui.AxisZTitleHOffsetSlider->setValue(axes.titleOrientation(2).w*1000);
	ui.AxisZTitleAnchorCombo->setCurrentIndex(axes.titleAnchor(2));
	ui.AxisZTitleAxialRotationSlider->setValue(axes.titleOrientation(2).x);
	ui.AxisZTitleAxialRotationSpin->setValue(axes.titleOrientation(2).x);
	ui.AxisZTitleInPlaneRotationSlider->setValue(axes.titleOrientation(2).y);
	ui.AxisZTitleInPlaneRotationSpin->setValue(axes.titleOrientation(2).y);
	ui.AxisZTitleDistanceSpin->setValue(axes.titleOrientation(2).z);

	// Grid
	// -- X
	ui.AxisXGridLineMajorCheck->setChecked(axes.gridLinesMajor(0));
	ui.AxisXGridLineMinorCheck->setChecked(axes.gridLinesMinor(0));
	ui.AxisXGridLineFullCheck->setChecked(axes.gridLinesFull(0));
	// -- Y
	ui.AxisYGridLineMajorCheck->setChecked(axes.gridLinesMajor(1));
	ui.AxisYGridLineMinorCheck->setChecked(axes.gridLinesMinor(1));
	ui.AxisYGridLineFullCheck->setChecked(axes.gridLinesFull(1));
	// -- Z
	ui.AxisZGridLineMajorCheck->setChecked(axes.gridLinesMajor(2));
	ui.AxisZGridLineMinorCheck->setChecked(axes.gridLinesMinor(2));
	ui.AxisZGridLineFullCheck->setChecked(axes.gridLinesFull(2));

	refreshing_ = false;
}
