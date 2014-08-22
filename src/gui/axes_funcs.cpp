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
	
	if (haveCurrentAxes()) currentAxes().setAxisInverted(axis, checked);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisLogarithmicChanged(int axis, bool checked)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;
	
	currentAxes().setAxisLogarithmic(axis, checked);

	// Update relevant parts of gui
	updateControls();
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisVisibleChanged(int axis, bool checked)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;
	
	currentAxes().setAxisVisible(axis, checked);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisStretchChanged(int axis, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	currentAxes().setAxisStretch(axis, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisLimitChanged(int axis, bool minLim, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;
	
	if (minLim) currentAxes().setAxisMin(axis, value);
	else currentAxes().setAxisMax(axis, value);

	// Update relevant parts of gui
	updateControls();
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisLimitSetExtreme(int axis, bool minLim)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;
	
	currentAxes().setAxisToLimit(axis, minLim);

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

	currentAxes().setAxisPositionIsFractional(axis, fractional);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisPositionChanged(bool real, int axis, int dir, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	if (real) currentAxes().setAxisPositionReal(axis, dir, value);
	else currentAxes().setAxisPositionFractional(axis, dir, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisPositionSet(bool real, int axis, int dir, int type)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	if (real)
	{
		if (type == -1) currentAxes().setAxisPositionRealToLimit(axis, dir, true);
		else if (type == 0) currentAxes().setAxisPositionReal(axis, dir, 0.0);
		else if (type == 1) currentAxes().setAxisPositionRealToLimit(axis, dir, false);
		else return false;
	}
	else
	{
		if (type == -1) currentAxes().setAxisPositionFractional(axis, dir, 0.0);
		else if (type == 0) currentAxes().setAxisPositionFractional(axis, dir, 0.5);
		else if (type == 1) currentAxes().setAxisPositionFractional(axis, dir, 1.0);
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

	currentAxes().setAxisAutoTicks(axis, enabled);

	// Update relevant parts of gui
	updateControls();
	uChroma_.updateDisplay();
	
	return true;
}

bool AxesWindow::axisTicksChanged(int axis, bool start, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	if (start) currentAxes().setAxisFirstTick(axis, value);
	else currentAxes().setAxisTickDelta(axis, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisTickOrientationChanged(int axis, int dir, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	currentAxes().setAxisTickDirection(axis, dir, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisLabelOrientationChanged(int axis, int component, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	currentAxes().setAxisLabelOrientation(axis, component, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisTitleOrientationChanged(int axis, int component, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	currentAxes().setAxisTitleOrientation(axis, component, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisMinorTicksChanged(int axis, int value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	currentAxes().setAxisMinorTicks(axis, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisTitleChanged(int axis, QString& title)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	currentAxes().setAxisTitle(axis, title);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisTitleAlignmentChanged(int axis, Axes::AxisAnchor anchor)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	currentAxes().setAxisTitleAnchor(axis, anchor);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

/*
 * Slots
 */

// X Axis

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

void AxesWindow::on_AxisXLabelAxialRotationSlider_valueChanged(int value)
{
	axisLabelOrientationChanged(0, 0, value);
}

void AxesWindow::on_AxisXLabelAxialRotationSpin_valueChanged(int value)
{
	axisLabelOrientationChanged(0, 0, value);
}

void AxesWindow::on_AxisXLabelInPlaneRotationSlider_valueChanged(int value)
{
	axisLabelOrientationChanged(0, 1, value);
}

void AxesWindow::on_AxisXLabelInPlaneRotationSpin_valueChanged(int value)
{
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
	axisTitleAlignmentChanged(0, (Axes::AxisAnchor) index);
}

void AxesWindow::on_AxisXTitleAxialRotationSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(0, 0, value);
}

void AxesWindow::on_AxisXTitleAxialRotationSpin_valueChanged(int value)
{
	axisTitleOrientationChanged(0, 0, value);
}

void AxesWindow::on_AxisXTitleInPlaneRotationSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(0, 1, value);
}

void AxesWindow::on_AxisXTitleInPlaneRotationSpin_valueChanged(int value)
{
	axisTitleOrientationChanged(0, 1, value);
}

void AxesWindow::on_AxisXTitleDistanceSpin_valueChanged(double value)
{
	axisTitleOrientationChanged(0, 2, value);
}

// Y Axis

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

void AxesWindow::on_AxisYLabelAxialRotationSlider_valueChanged(int value)
{
	axisLabelOrientationChanged(1, 0, value);
}

void AxesWindow::on_AxisYLabelAxialRotationSpin_valueChanged(int value)
{
	axisLabelOrientationChanged(1, 0, value);
}

void AxesWindow::on_AxisYLabelInPlaneRotationSlider_valueChanged(int value)
{
	axisLabelOrientationChanged(1, 1, value);
}

void AxesWindow::on_AxisYLabelInPlaneRotationSpin_valueChanged(int value)
{
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
	axisTitleAlignmentChanged(1, (Axes::AxisAnchor) index);
}

void AxesWindow::on_AxisYTitleAxialRotationSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(1, 0, value);
}

void AxesWindow::on_AxisYTitleAxialRotationSpin_valueChanged(int value)
{
	axisTitleOrientationChanged(1, 0, value);
}

void AxesWindow::on_AxisYTitleInPlaneRotationSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(1, 1, value);
}

void AxesWindow::on_AxisYTitleInPlaneRotationSpin_valueChanged(int value)
{
	axisTitleOrientationChanged(1, 1, value);
}

void AxesWindow::on_AxisYTitleDistanceSpin_valueChanged(double value)
{
	axisTitleOrientationChanged(1, 2, value);
}

// Z Axis

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

void AxesWindow::on_AxisZLabelAxialRotationSlider_valueChanged(int value)
{
	axisLabelOrientationChanged(2, 0, value);
}

void AxesWindow::on_AxisZLabelAxialRotationSpin_valueChanged(int value)
{
	axisLabelOrientationChanged(2, 0, value);
}

void AxesWindow::on_AxisZLabelInPlaneRotationSlider_valueChanged(int value)
{
	axisLabelOrientationChanged(2, 1, value);
}

void AxesWindow::on_AxisZLabelInPlaneRotationSpin_valueChanged(int value)
{
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
	axisTitleAlignmentChanged(2, (Axes::AxisAnchor) index);
}

void AxesWindow::on_AxisZTitleAxialRotationSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(2, 0, value);
}

void AxesWindow::on_AxisZTitleAxialRotationSpin_valueChanged(int value)
{
	axisTitleOrientationChanged(2, 0, value);
}

void AxesWindow::on_AxisZTitleInPlaneRotationSlider_valueChanged(int value)
{
	axisTitleOrientationChanged(2, 1, value);
}

void AxesWindow::on_AxisZTitleInPlaneRotationSpin_valueChanged(int value)
{
	axisTitleOrientationChanged(2, 1, value);
}

void AxesWindow::on_AxisZTitleDistanceSpin_valueChanged(double value)
{
	axisTitleOrientationChanged(2, 2, value);
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
	ui.AxisXInvertCheck->setChecked(axes.axisInverted(0));
	ui.AxisYInvertCheck->setChecked(axes.axisInverted(1));
	ui.AxisZInvertCheck->setChecked(axes.axisInverted(2));
	ui.AxisXVisibleCheck->setChecked(axes.axisVisible(0));
	ui.AxisYVisibleCheck->setChecked(axes.axisVisible(1));
	ui.AxisZVisibleCheck->setChecked(axes.axisVisible(2));
	ui.AxisXLogarithmicCheck->setChecked(axes.axisLogarithmic(0));
	ui.AxisYLogarithmicCheck->setChecked(axes.axisLogarithmic(1));
	ui.AxisZLogarithmicCheck->setChecked(axes.axisLogarithmic(2));
	ui.AxisXTicksDeltaSpin->setDisabled(axes.axisLogarithmic(0));
	ui.AxisYTicksDeltaSpin->setDisabled(axes.axisLogarithmic(1));
	ui.AxisZTicksDeltaSpin->setDisabled(axes.axisLogarithmic(2));
	ui.AxisXAutoTicksCheck->setDisabled(axes.axisLogarithmic(0));
	ui.AxisYAutoTicksCheck->setDisabled(axes.axisLogarithmic(1));
	ui.AxisZAutoTicksCheck->setDisabled(axes.axisLogarithmic(2));

	// Axis titles
	ui.AxisXTitleEdit->setText(axes.axisTitle(0));
	ui.AxisYTitleEdit->setText(axes.axisTitle(1));
	ui.AxisZTitleEdit->setText(axes.axisTitle(2));

	// Axis Stretch factors
	ui.AxisXStretchSpin->setValue(axes.axisStretch(0));
	ui.AxisYStretchSpin->setValue(axes.axisStretch(1));
	ui.AxisZStretchSpin->setValue(axes.axisStretch(2));

	// Axis Min/Max Limits
	ui.AxisXMinSpin->setRange(axes.axisLogarithmic(0), axes.axisLimitMin(0), false, 0.0);
	ui.AxisYMinSpin->setRange(axes.axisLogarithmic(1), axes.axisLimitMin(1), false, 0.0);
	ui.AxisZMinSpin->setRange(axes.axisLogarithmic(2), axes.axisLimitMin(2), false, 0.0);
	ui.AxisXMinSpin->setSingleStep(max((axes.axisLimitMax(0)-axes.axisLimitMin(0))*0.01, 1.0));
	ui.AxisYMinSpin->setSingleStep(max((axes.axisLimitMax(1)-axes.axisLimitMin(1))*0.01, 1.0));
	ui.AxisZMinSpin->setSingleStep(max((axes.axisLimitMax(2)-axes.axisLimitMin(2))*0.01, 1.0));

	ui.AxisXMaxSpin->setRange(axes.axisLogarithmic(0), axes.axisLimitMin(0), false, 0.0);
	ui.AxisYMaxSpin->setRange(axes.axisLogarithmic(1), axes.axisLimitMin(1), false, 0.0);
	ui.AxisZMaxSpin->setRange(axes.axisLogarithmic(2), axes.axisLimitMin(2), false, 0.0);
	ui.AxisXMaxSpin->setSingleStep(max((axes.axisLimitMax(0)-axes.axisLimitMin(0))*0.01, 1.0));
	ui.AxisYMaxSpin->setSingleStep(max((axes.axisLimitMax(1)-axes.axisLimitMin(1))*0.01, 1.0));
	ui.AxisZMaxSpin->setSingleStep(max((axes.axisLimitMax(2)-axes.axisLimitMin(2))*0.01, 1.0));
	ui.AxisXMinSpin->setValue(axes.axisMin(0));
	ui.AxisYMinSpin->setValue(axes.axisMin(1));
	ui.AxisZMinSpin->setValue(axes.axisMin(2));
	ui.AxisXMaxSpin->setValue(axes.axisMax(0));
	ui.AxisYMaxSpin->setValue(axes.axisMax(1));
	ui.AxisZMaxSpin->setValue(axes.axisMax(2));
	ui.AxisXMinLabel->setText("DataMin: " + QString::number(axes.axisLimitMin(0)));
	ui.AxisXMaxLabel->setText("DataMax: " + QString::number(axes.axisLimitMax(0)));
	ui.AxisYMinLabel->setText("DataMin: " + QString::number(axes.axisLimitMin(1)));
	ui.AxisYMaxLabel->setText("DataMax: " + QString::number(axes.axisLimitMax(1)));
	ui.AxisZMinLabel->setText("DataMin: " + QString::number(axes.axisLimitMin(2)));
	ui.AxisZMaxLabel->setText("DataMax: " + QString::number(axes.axisLimitMax(2)));

	// Axis positions
	// -- X
	ui.AxisXPositionFractionalRadio->setChecked(axes.axisPositionIsFractional(0));
	axisPositionIsFractionalChanged(0, axes.axisPositionIsFractional(0));
	ui.AxisXPositionYRealSpin->setRange(axes.axisLogarithmic(1), axes.axisLimitMin(1), false, 0.0);
	ui.AxisXPositionYRealSpin->setValue(axes.axisPositionReal(0).y);
	ui.AxisXPositionZRealSpin->setRange(axes.axisLogarithmic(2), axes.axisLimitMin(2), false, 0.0);
	ui.AxisXPositionZRealSpin->setValue(axes.axisPositionReal(0).z);
	ui.AxisXPositionYFractionalSpin->setValue(axes.axisPositionFractional(0).y);
	ui.AxisXPositionZFractionalSpin->setValue(axes.axisPositionFractional(0).z);
	// -- Y
	ui.AxisYPositionFractionalRadio->setChecked(axes.axisPositionIsFractional(1));
	axisPositionIsFractionalChanged(1, axes.axisPositionIsFractional(1));
	ui.AxisYPositionXRealSpin->setRange(axes.axisLogarithmic(0), axes.axisLimitMin(0), false, 0.0);
	ui.AxisYPositionXRealSpin->setValue(axes.axisPositionReal(1).x);
	ui.AxisYPositionZRealSpin->setRange(axes.axisLogarithmic(2), axes.axisLimitMin(2), false, 0.0);
	ui.AxisYPositionZRealSpin->setValue(axes.axisPositionReal(1).z);
	ui.AxisYPositionXFractionalSpin->setValue(axes.axisPositionFractional(1).x);
	ui.AxisYPositionZFractionalSpin->setValue(axes.axisPositionFractional(1).z);
	// -- Z
	ui.AxisZPositionFractionalRadio->setChecked(axes.axisPositionIsFractional(2));
	axisPositionIsFractionalChanged(2, axes.axisPositionIsFractional(2));
	ui.AxisZPositionXRealSpin->setRange(axes.axisLogarithmic(0), axes.axisLimitMin(0), false, 0.0);
	ui.AxisZPositionXRealSpin->setValue(axes.axisPositionReal(2).x);
	ui.AxisZPositionYRealSpin->setRange(axes.axisLogarithmic(1), axes.axisLimitMin(1), false, 0.0);
	ui.AxisZPositionYRealSpin->setValue(axes.axisPositionReal(2).y);
	ui.AxisZPositionXFractionalSpin->setValue(axes.axisPositionFractional(2).x);
	ui.AxisZPositionYFractionalSpin->setValue(axes.axisPositionFractional(2).y);

	// Axis Ticks
	// -- X
	ui.AxisXAutoTicksCheck->setChecked(axes.axisAutoTicks(0));
	ui.AxisXTicksStartSpin->setValue(axes.axisFirstTick(0));
	ui.AxisXTicksStartSpin->setEnabled(!axes.axisAutoTicks(0));
	ui.AxisXTicksDeltaSpin->setValue(axes.axisTickDelta(0));
	ui.AxisXTicksDeltaSpin->setEnabled(!axes.axisAutoTicks(0));
	ui.AxisXMinorTicksSpin->setValue(axes.axisMinorTicks(0));
	ui.AxisXTickDirectionXSpin->setValue(axes.axisTickDirection(0).x);
	ui.AxisXTickDirectionYSpin->setValue(axes.axisTickDirection(0).y);
	ui.AxisXTickDirectionZSpin->setValue(axes.axisTickDirection(0).z);
	// -- Y
	ui.AxisYAutoTicksCheck->setChecked(axes.axisAutoTicks(1));
	ui.AxisYTicksStartSpin->setValue(axes.axisFirstTick(1));
	ui.AxisYTicksStartSpin->setEnabled(!axes.axisAutoTicks(1));
	ui.AxisYTicksDeltaSpin->setValue(axes.axisTickDelta(1));
	ui.AxisYTicksDeltaSpin->setEnabled(!axes.axisAutoTicks(1));
	ui.AxisYMinorTicksSpin->setValue(axes.axisMinorTicks(1));
	ui.AxisYTickDirectionXSpin->setValue(axes.axisTickDirection(1).x);
	ui.AxisYTickDirectionYSpin->setValue(axes.axisTickDirection(1).y);
	ui.AxisYTickDirectionZSpin->setValue(axes.axisTickDirection(1).z);
	// -- Z
	ui.AxisZAutoTicksCheck->setChecked(axes.axisAutoTicks(2));
	ui.AxisZTicksStartSpin->setValue(axes.axisFirstTick(2));
	ui.AxisZTicksStartSpin->setEnabled(!axes.axisAutoTicks(2));
	ui.AxisZTicksDeltaSpin->setValue(axes.axisTickDelta(2));
	ui.AxisZTicksDeltaSpin->setEnabled(!axes.axisAutoTicks(2));
	ui.AxisZMinorTicksSpin->setValue(axes.axisMinorTicks(2));
	ui.AxisZTickDirectionXSpin->setValue(axes.axisTickDirection(2).x);
	ui.AxisZTickDirectionYSpin->setValue(axes.axisTickDirection(2).y);
	ui.AxisZTickDirectionZSpin->setValue(axes.axisTickDirection(2).z);

	// Text Orientation
	// -- X
	ui.AxisXLabelAxialRotationSlider->setValue(axes.axisLabelOrientation(0).x);
	ui.AxisXLabelAxialRotationSpin->setValue(axes.axisLabelOrientation(0).x);
	ui.AxisXLabelInPlaneRotationSlider->setValue(axes.axisLabelOrientation(0).y);
	ui.AxisXLabelInPlaneRotationSpin->setValue(axes.axisLabelOrientation(0).y);
	ui.AxisXLabelDistanceSpin->setValue(axes.axisLabelOrientation(0).z);
	ui.AxisYTitleHOffsetSlider->setValue(axes.axisTitleOrientation(0).w*1000);
	ui.AxisXTitleAnchorCombo->setCurrentIndex(axes.axisTitleAnchor(0));
	ui.AxisXTitleAxialRotationSlider->setValue(axes.axisTitleOrientation(0).x);
	ui.AxisXTitleAxialRotationSpin->setValue(axes.axisTitleOrientation(0).x);
	ui.AxisXTitleInPlaneRotationSlider->setValue(axes.axisTitleOrientation(0).y);
	ui.AxisXTitleInPlaneRotationSpin->setValue(axes.axisTitleOrientation(0).y);
	ui.AxisXTitleDistanceSpin->setValue(axes.axisTitleOrientation(0).z);
	// -- Y
	ui.AxisYLabelAxialRotationSlider->setValue(axes.axisLabelOrientation(1).x);
	ui.AxisYLabelAxialRotationSpin->setValue(axes.axisLabelOrientation(1).x);
	ui.AxisYLabelInPlaneRotationSlider->setValue(axes.axisLabelOrientation(1).y);
	ui.AxisYLabelInPlaneRotationSpin->setValue(axes.axisLabelOrientation(1).y);
	ui.AxisYLabelDistanceSpin->setValue(axes.axisLabelOrientation(1).z);
	ui.AxisYTitleHOffsetSlider->setValue(axes.axisTitleOrientation(1).w*1000);
	ui.AxisYTitleAnchorCombo->setCurrentIndex(axes.axisTitleAnchor(1));
	ui.AxisYTitleAxialRotationSlider->setValue(axes.axisTitleOrientation(1).x);
	ui.AxisYTitleAxialRotationSpin->setValue(axes.axisTitleOrientation(1).x);
	ui.AxisYTitleInPlaneRotationSlider->setValue(axes.axisTitleOrientation(1).y);
	ui.AxisYTitleInPlaneRotationSpin->setValue(axes.axisTitleOrientation(1).y);
	ui.AxisYTitleDistanceSpin->setValue(axes.axisTitleOrientation(1).z);
	// -- Z
	ui.AxisZLabelAxialRotationSlider->setValue(axes.axisLabelOrientation(2).x);
	ui.AxisZLabelAxialRotationSpin->setValue(axes.axisLabelOrientation(2).x);
	ui.AxisZLabelInPlaneRotationSlider->setValue(axes.axisLabelOrientation(2).y);
	ui.AxisZLabelInPlaneRotationSpin->setValue(axes.axisLabelOrientation(2).y);
	ui.AxisZLabelDistanceSpin->setValue(axes.axisLabelOrientation(2).z);
	ui.AxisZTitleHOffsetSlider->setValue(axes.axisTitleOrientation(2).w*1000);
	ui.AxisZTitleAnchorCombo->setCurrentIndex(axes.axisTitleAnchor(2));
	ui.AxisZTitleAxialRotationSlider->setValue(axes.axisTitleOrientation(2).x);
	ui.AxisZTitleAxialRotationSpin->setValue(axes.axisTitleOrientation(2).x);
	ui.AxisZTitleInPlaneRotationSlider->setValue(axes.axisTitleOrientation(2).y);
	ui.AxisZTitleInPlaneRotationSpin->setValue(axes.axisTitleOrientation(2).y);
	ui.AxisZTitleDistanceSpin->setValue(axes.axisTitleOrientation(2).z);

	refreshing_ = false;
}
