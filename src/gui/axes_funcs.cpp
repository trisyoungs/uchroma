/*
	*** Main Window - Axes Functions 
	*** src/gui/uchroma_axes.cpp
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
#include "uchroma.h"
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
}

// Destructor
AxesWindow::~AxesWindow()
{
}

// Window close event
void AxesWindow::closeEvent(QCloseEvent *event)
{
	// TODO
}

/*
 * Convenience Functions
 */

bool AxesWindow::axisInvertChanged(int axis, bool checked)
{
	if (refreshing_) return false;
	uChroma_.setAxisInverted(axis, checked);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisLogarithmicChanged(int axis, bool checked)
{
	if (refreshing_) return false;
	uChroma_.setAxisLogarithmic(axis, checked);

	// Update relevant parts of gui
	updateControls();
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisVisibleChanged(int axis, bool checked)
{
	if (refreshing_) return false;
	uChroma_.setAxisVisible(axis, checked);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisStretchChanged(int axis, double value)
{
	if (refreshing_) return false;
	uChroma_.setAxisStretch(axis, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisLimitChanged(int axis, bool minLim, double value)
{
	if (refreshing_) return false;
	if (minLim) uChroma_.setAxisMin(axis, value);
	else uChroma_.setAxisMax(axis, value);

	// Update relevant parts of gui
	updateControls();
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisLimitSetExtreme(int axis, bool minLim)
{
	if (refreshing_) return false;
	uChroma_.setAxisToLimit(axis, minLim);

	// Update relevant parts of gui
	updateControls();
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisCrossChanged(int axis, int dir, double value)
{
	if (refreshing_) return false;
	uChroma_.setAxisPosition(axis, dir, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisCrossSet(int axis, int dir, int type)
{
	if (refreshing_) return false;
	if (type == -1) uChroma_.setAxisPositionToLimit(axis, dir, true);
	else if (type == 0) uChroma_.setAxisPosition(axis, dir, 0.0);
	else if (type == 1) uChroma_.setAxisPositionToLimit(axis, dir, false);
	else return false;

	// Update relevant parts of gui
	updateControls();
	uChroma_.updateDisplay();
	
	return true;
}

bool AxesWindow::axisAutoTicksChanged(int axis, bool enabled)
{
	if (refreshing_) return false;
	uChroma_.setAxisAutoTicks(axis, enabled);

	// Update relevant parts of gui
	updateControls();
	uChroma_.updateDisplay();
	
	return true;
}

bool AxesWindow::axisTicksChanged(int axis, bool start, double value)
{
	if (refreshing_) return false;
	if (start) uChroma_.setAxisFirstTick(axis, value);
	else uChroma_.setAxisTickDelta(axis, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisTickOrientationChanged(int axis, int dir, double value)
{
	if (refreshing_) return false;
	uChroma_.setAxisTickDirection(axis, dir, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisLabelOrientationChanged(int axis, int component, double value)
{
	if (refreshing_) return false;
	uChroma_.setAxisLabelOrientation(axis, component, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisTitleOrientationChanged(int axis, int component, double value)
{
	if (refreshing_) return false;
	uChroma_.setAxisTitleOrientation(axis, component, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisMinorTicksChanged(int axis, int value)
{
	if (refreshing_) return false;
	uChroma_.setAxisMinorTicks(axis, value);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisTitleChanged(int axis, QString& title)
{
	if (refreshing_) return false;
	uChroma_.setAxisTitle(axis, title);

	// Update relevant parts of gui
	uChroma_.updateDisplay();

	return true;
}

bool AxesWindow::axisTitleAlignmentChanged(int axis, TextPrimitive::HorizontalAnchor anchor)
{
	if (refreshing_) return false;
	uChroma_.setAxisTitleAnchor(axis, anchor);

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

void AxesWindow::on_AxisYMinSpin_valueChanged(double value)
{
	axisLimitChanged(1, true, value);
}

void AxesWindow::on_AxisZMinSpin_valueChanged(double value)
{
	axisLimitChanged(2, true, value);
}

void AxesWindow::on_AxisXMinSetMinimumButton_clicked(bool checked)
{
	axisLimitSetExtreme(0, true);
}

void AxesWindow::on_AxisYMinSetMinimumButton_clicked(bool checked)
{
	axisLimitSetExtreme(1, true);
}

void AxesWindow::on_AxisZMinSetMinimumButton_clicked(bool checked)
{
	axisLimitSetExtreme(2, true);
}

void AxesWindow::on_AxisXMaxSpin_valueChanged(double value)
{
	axisLimitChanged(0, false, value);
}

void AxesWindow::on_AxisYMaxSpin_valueChanged(double value)
{
	axisLimitChanged(1, false, value);
}

void AxesWindow::on_AxisZMaxSpin_valueChanged(double value)
{
	axisLimitChanged(2, false, value);
}

void AxesWindow::on_AxisXMaxSetMaximumButton_clicked(bool checked)
{
	axisLimitSetExtreme(0, false);
}

void AxesWindow::on_AxisYMaxSetMaximumButton_clicked(bool checked)
{
	axisLimitSetExtreme(1, false);
}

void AxesWindow::on_AxisZMaxSetMaximumButton_clicked(bool checked)
{
	axisLimitSetExtreme(2, false);
}

void AxesWindow::on_AxisXCrossAtYSpin_valueChanged(double value)
{
	axisCrossChanged(0, 1, value);
}

void AxesWindow::on_AxisXCrossAtZSpin_valueChanged(double value)
{
	axisCrossChanged(0, 2, value);
}

void AxesWindow::on_AxisXCrossAtYSetMinimumButton_clicked(bool checked)
{
	axisCrossSet(0, 1, -1);
}

void AxesWindow::on_AxisXCrossAtYSetZeroButton_clicked(bool checked)
{
	axisCrossSet(0, 1, 0);
}

void AxesWindow::on_AxisXCrossAtYSetMaximumButton_clicked(bool checked)
{
	axisCrossSet(0, 1, 1);
}

void AxesWindow::on_AxisXCrossAtZSetMinimumButton_clicked(bool checked)
{
	axisCrossSet(0, 2, -1);
}

void AxesWindow::on_AxisXCrossAtZSetZeroButton_clicked(bool checked)
{
	axisCrossSet(0, 2, 0);
}

void AxesWindow::on_AxisXCrossAtZSetMaximumButton_clicked(bool checked)
{
	axisCrossSet(0, 2, 1);
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
	axisTitleAlignmentChanged(0, (TextPrimitive::HorizontalAnchor) index);
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

void AxesWindow::on_AxisYCrossAtXSpin_valueChanged(double value)
{
	axisCrossChanged(1, 0, value);
}

void AxesWindow::on_AxisYCrossAtZSpin_valueChanged(double value)
{
	axisCrossChanged(1, 2, value);
}

void AxesWindow::on_AxisYCrossAtXSetMinimumButton_clicked(bool checked)
{
	axisCrossSet(1, 0, -1);
}

void AxesWindow::on_AxisYCrossAtXSetZeroButton_clicked(bool checked)
{
	axisCrossSet(1, 0, 0);
}

void AxesWindow::on_AxisYCrossAtXSetMaximumButton_clicked(bool checked)
{
	axisCrossSet(1, 0, 1);
}

void AxesWindow::on_AxisYCrossAtZSetMinimumButton_clicked(bool checked)
{
	axisCrossSet(1, 2, -1);
}

void AxesWindow::on_AxisYCrossAtZSetZeroButton_clicked(bool checked)
{
	axisCrossSet(1, 2, 0);
}

void AxesWindow::on_AxisYCrossAtZSetMaximumButton_clicked(bool checked)
{
	axisCrossSet(1, 2, 1);
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
	axisTitleAlignmentChanged(1, (TextPrimitive::HorizontalAnchor) index);
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

void AxesWindow::on_AxisZCrossAtXSpin_valueChanged(double value)
{
	axisCrossChanged(2, 0, value);
}

void AxesWindow::on_AxisZCrossAtYSpin_valueChanged(double value)
{
	axisCrossChanged(2, 1, value);
}

void AxesWindow::on_AxisZCrossAtXSetMinimumButton_clicked(bool checked)
{
	axisCrossSet(2, 0, -1);
}

void AxesWindow::on_AxisZCrossAtXSetZeroButton_clicked(bool checked)
{
	axisCrossSet(2, 0, 0);
}

void AxesWindow::on_AxisZCrossAtXSetMaximumButton_clicked(bool checked)
{
	axisCrossSet(2, 0, 1);
}

void AxesWindow::on_AxisZCrossAtYSetMinimumButton_clicked(bool checked)
{
	axisCrossSet(2, 1, -1);
}

void AxesWindow::on_AxisZCrossAtYSetZeroButton_clicked(bool checked)
{
	axisCrossSet(2, 1, 0);
}

void AxesWindow::on_AxisZCrossAtYSetMaximumButton_clicked(bool checked)
{
	axisCrossSet(2, 1, 1);
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
	axisTitleAlignmentChanged(2, (TextPrimitive::HorizontalAnchor) index);
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
	updateControls();
	show();
}

// Update controls
void AxesWindow::updateControls()
{
	refreshing_ = true;

	// Invert / Visible / Logarithmic
	ui.AxisXInvertCheck->setChecked(uChroma_.axisInverted(0));
	ui.AxisYInvertCheck->setChecked(uChroma_.axisInverted(1));
	ui.AxisZInvertCheck->setChecked(uChroma_.axisInverted(2));
	ui.AxisXVisibleCheck->setChecked(uChroma_.axisVisible(0));
	ui.AxisYVisibleCheck->setChecked(uChroma_.axisVisible(1));
	ui.AxisZVisibleCheck->setChecked(uChroma_.axisVisible(2));
	ui.AxisXLogarithmicCheck->setChecked(uChroma_.axisLogarithmic(0));
	ui.AxisYLogarithmicCheck->setChecked(uChroma_.axisLogarithmic(1));
	ui.AxisZLogarithmicCheck->setChecked(uChroma_.axisLogarithmic(2));
	ui.AxisXTicksStartDeltaAutoWidget->setDisabled(uChroma_.axisLogarithmic(0));
	ui.AxisYTicksStartDeltaAutoWidget->setDisabled(uChroma_.axisLogarithmic(1));
	ui.AxisZTicksStartDeltaAutoWidget->setDisabled(uChroma_.axisLogarithmic(2));

	// Axis titles
	ui.AxisXTitleEdit->setText(uChroma_.axisTitle(0));
	ui.AxisYTitleEdit->setText(uChroma_.axisTitle(1));
	ui.AxisZTitleEdit->setText(uChroma_.axisTitle(2));

	// Axis Stretch factors
	ui.AxisXStretchSpin->setValue(uChroma_.axisStretch(0));
	ui.AxisYStretchSpin->setValue(uChroma_.axisStretch(1));
	ui.AxisZStretchSpin->setValue(uChroma_.axisStretch(2));

	// Axis Min/Max Limits
	ui.AxisXMinSpin->setRange(uChroma_.axisLogarithmic(0), uChroma_.axisLimitMin(0), false, 0.0);
	ui.AxisYMinSpin->setRange(uChroma_.axisLogarithmic(1), uChroma_.axisLimitMin(1), false, 0.0);
	ui.AxisZMinSpin->setRange(uChroma_.axisLogarithmic(2), uChroma_.axisLimitMin(2), false, 0.0);
	ui.AxisXMinSpin->setSingleStep(max((uChroma_.axisLimitMax(0)-uChroma_.axisLimitMin(0))*0.01, 1.0));
	ui.AxisYMinSpin->setSingleStep(max((uChroma_.axisLimitMax(1)-uChroma_.axisLimitMin(1))*0.01, 1.0));
	ui.AxisZMinSpin->setSingleStep(max((uChroma_.axisLimitMax(2)-uChroma_.axisLimitMin(2))*0.01, 1.0));

	ui.AxisXMaxSpin->setRange(uChroma_.axisLogarithmic(0), uChroma_.axisLimitMin(0), false, 0.0);
	ui.AxisYMaxSpin->setRange(uChroma_.axisLogarithmic(1), uChroma_.axisLimitMin(1), false, 0.0);
	ui.AxisZMaxSpin->setRange(uChroma_.axisLogarithmic(2), uChroma_.axisLimitMin(2), false, 0.0);
	ui.AxisXMaxSpin->setSingleStep(max((uChroma_.axisLimitMax(0)-uChroma_.axisLimitMin(0))*0.01, 1.0));
	ui.AxisYMaxSpin->setSingleStep(max((uChroma_.axisLimitMax(1)-uChroma_.axisLimitMin(1))*0.01, 1.0));
	ui.AxisZMaxSpin->setSingleStep(max((uChroma_.axisLimitMax(2)-uChroma_.axisLimitMin(2))*0.01, 1.0));
	ui.AxisXMinSpin->setValue(uChroma_.axisMin(0));
	ui.AxisYMinSpin->setValue(uChroma_.axisMin(1));
	ui.AxisZMinSpin->setValue(uChroma_.axisMin(2));
	ui.AxisXMaxSpin->setValue(uChroma_.axisMax(0));
	ui.AxisYMaxSpin->setValue(uChroma_.axisMax(1));
	ui.AxisZMaxSpin->setValue(uChroma_.axisMax(2));
	ui.AxisXMinLabel->setText("DataMin: " + QString::number(uChroma_.axisLimitMin(0)));
	ui.AxisXMaxLabel->setText("DataMax: " + QString::number(uChroma_.axisLimitMax(0)));
	ui.AxisYMinLabel->setText("DataMin: " + QString::number(uChroma_.axisLimitMin(1)));
	ui.AxisYMaxLabel->setText("DataMax: " + QString::number(uChroma_.axisLimitMax(1)));
	ui.AxisZMinLabel->setText("DataMin: " + QString::number(uChroma_.axisLimitMin(2)));
	ui.AxisZMaxLabel->setText("DataMax: " + QString::number(uChroma_.axisLimitMax(2)));

	// Axis positions
	// -- X
	ui.AxisXCrossAtYSpin->setRange(uChroma_.axisLogarithmic(1), uChroma_.axisLimitMin(1), false, 0.0);
	ui.AxisXCrossAtYSpin->setValue(uChroma_.axisPosition(0).y);
	ui.AxisXCrossAtZSpin->setRange(uChroma_.axisLogarithmic(2), uChroma_.axisLimitMin(2), false, 0.0);
	ui.AxisXCrossAtZSpin->setValue(uChroma_.axisPosition(0).z);
	// -- Y
	ui.AxisYCrossAtXSpin->setValue(uChroma_.axisPosition(1).x);
	ui.AxisYCrossAtXSpin->setRange(uChroma_.axisLogarithmic(0), uChroma_.axisLimitMin(0), false, 0.0);
	ui.AxisYCrossAtZSpin->setValue(uChroma_.axisPosition(1).z);
	ui.AxisYCrossAtZSpin->setRange(uChroma_.axisLogarithmic(2), uChroma_.axisLimitMin(2), false, 0.0);
	// -- Z
	ui.AxisZCrossAtXSpin->setValue(uChroma_.axisPosition(2).x);
	ui.AxisZCrossAtXSpin->setRange(uChroma_.axisLogarithmic(0), uChroma_.axisLimitMin(0), false, 0.0);
	ui.AxisZCrossAtYSpin->setValue(uChroma_.axisPosition(2).y);
	ui.AxisZCrossAtYSpin->setRange(uChroma_.axisLogarithmic(1), uChroma_.axisLimitMin(1), false, 0.0);

	// Axis Ticks
	// -- X
	ui.AxisXAutoTicksCheck->setChecked(uChroma_.axisAutoTicks(0));
	ui.AxisXTicksStartSpin->setValue(uChroma_.axisFirstTick(0));
	ui.AxisXTicksStartSpin->setEnabled(!uChroma_.axisAutoTicks(0));
	ui.AxisXTicksDeltaSpin->setValue(uChroma_.axisTickDelta(0));
	ui.AxisXTicksDeltaSpin->setEnabled(!uChroma_.axisAutoTicks(0));
	ui.AxisXMinorTicksSpin->setValue(uChroma_.axisMinorTicks(0));
	ui.AxisXTickDirectionXSpin->setValue(uChroma_.axisTickDirection(0).x);
	ui.AxisXTickDirectionYSpin->setValue(uChroma_.axisTickDirection(0).y);
	ui.AxisXTickDirectionZSpin->setValue(uChroma_.axisTickDirection(0).z);
	// -- Y
	ui.AxisYAutoTicksCheck->setChecked(uChroma_.axisAutoTicks(1));
	ui.AxisYTicksStartSpin->setValue(uChroma_.axisFirstTick(1));
	ui.AxisYTicksStartSpin->setEnabled(!uChroma_.axisAutoTicks(1));
	ui.AxisYTicksDeltaSpin->setValue(uChroma_.axisTickDelta(1));
	ui.AxisYTicksDeltaSpin->setEnabled(!uChroma_.axisAutoTicks(1));
	ui.AxisYMinorTicksSpin->setValue(uChroma_.axisMinorTicks(1));
	ui.AxisYTickDirectionXSpin->setValue(uChroma_.axisTickDirection(1).x);
	ui.AxisYTickDirectionYSpin->setValue(uChroma_.axisTickDirection(1).y);
	ui.AxisYTickDirectionZSpin->setValue(uChroma_.axisTickDirection(1).z);
	// -- Z
	ui.AxisZAutoTicksCheck->setChecked(uChroma_.axisAutoTicks(2));
	ui.AxisZTicksStartSpin->setValue(uChroma_.axisFirstTick(2));
	ui.AxisZTicksStartSpin->setEnabled(!uChroma_.axisAutoTicks(2));
	ui.AxisZTicksDeltaSpin->setValue(uChroma_.axisTickDelta(2));
	ui.AxisZTicksDeltaSpin->setEnabled(!uChroma_.axisAutoTicks(2));
	ui.AxisZMinorTicksSpin->setValue(uChroma_.axisMinorTicks(2));
	ui.AxisZTickDirectionXSpin->setValue(uChroma_.axisTickDirection(2).x);
	ui.AxisZTickDirectionYSpin->setValue(uChroma_.axisTickDirection(2).y);
	ui.AxisZTickDirectionZSpin->setValue(uChroma_.axisTickDirection(2).z);

	// Text Orientation
	// -- X
	ui.AxisXLabelAxialRotationSlider->setValue(uChroma_.axisLabelOrientation(0).x);
	ui.AxisXLabelAxialRotationSpin->setValue(uChroma_.axisLabelOrientation(0).x);
	ui.AxisXLabelInPlaneRotationSlider->setValue(uChroma_.axisLabelOrientation(0).y);
	ui.AxisXLabelInPlaneRotationSpin->setValue(uChroma_.axisLabelOrientation(0).y);
	ui.AxisXLabelDistanceSpin->setValue(uChroma_.axisLabelOrientation(0).z);
	ui.AxisYTitleHOffsetSlider->setValue(uChroma_.axisTitleOrientation(0).w*1000);
	ui.AxisXTitleAnchorCombo->setCurrentIndex(uChroma_.axisTitleAnchor(0));
	ui.AxisXTitleAxialRotationSlider->setValue(uChroma_.axisTitleOrientation(0).x);
	ui.AxisXTitleAxialRotationSpin->setValue(uChroma_.axisTitleOrientation(0).x);
	ui.AxisXTitleInPlaneRotationSlider->setValue(uChroma_.axisTitleOrientation(0).y);
	ui.AxisXTitleInPlaneRotationSpin->setValue(uChroma_.axisTitleOrientation(0).y);
	ui.AxisXTitleDistanceSpin->setValue(uChroma_.axisTitleOrientation(0).z);
	// -- Y
	ui.AxisYLabelAxialRotationSlider->setValue(uChroma_.axisLabelOrientation(1).x);
	ui.AxisYLabelAxialRotationSpin->setValue(uChroma_.axisLabelOrientation(1).x);
	ui.AxisYLabelInPlaneRotationSlider->setValue(uChroma_.axisLabelOrientation(1).y);
	ui.AxisYLabelInPlaneRotationSpin->setValue(uChroma_.axisLabelOrientation(1).y);
	ui.AxisYLabelDistanceSpin->setValue(uChroma_.axisLabelOrientation(1).z);
	ui.AxisYTitleHOffsetSlider->setValue(uChroma_.axisTitleOrientation(1).w*1000);
	ui.AxisYTitleAnchorCombo->setCurrentIndex(uChroma_.axisTitleAnchor(1));
	ui.AxisYTitleAxialRotationSlider->setValue(uChroma_.axisTitleOrientation(1).x);
	ui.AxisYTitleAxialRotationSpin->setValue(uChroma_.axisTitleOrientation(1).x);
	ui.AxisYTitleInPlaneRotationSlider->setValue(uChroma_.axisTitleOrientation(1).y);
	ui.AxisYTitleInPlaneRotationSpin->setValue(uChroma_.axisTitleOrientation(1).y);
	ui.AxisYTitleDistanceSpin->setValue(uChroma_.axisTitleOrientation(1).z);
	// -- Z
	ui.AxisZLabelAxialRotationSlider->setValue(uChroma_.axisLabelOrientation(2).x);
	ui.AxisZLabelAxialRotationSpin->setValue(uChroma_.axisLabelOrientation(2).x);
	ui.AxisZLabelInPlaneRotationSlider->setValue(uChroma_.axisLabelOrientation(2).y);
	ui.AxisZLabelInPlaneRotationSpin->setValue(uChroma_.axisLabelOrientation(2).y);
	ui.AxisZLabelDistanceSpin->setValue(uChroma_.axisLabelOrientation(2).z);
	ui.AxisZTitleHOffsetSlider->setValue(uChroma_.axisTitleOrientation(2).w*1000);
	ui.AxisZTitleAnchorCombo->setCurrentIndex(uChroma_.axisTitleAnchor(2));
	ui.AxisZTitleAxialRotationSlider->setValue(uChroma_.axisTitleOrientation(2).x);
	ui.AxisZTitleAxialRotationSpin->setValue(uChroma_.axisTitleOrientation(2).x);
	ui.AxisZTitleInPlaneRotationSlider->setValue(uChroma_.axisTitleOrientation(2).y);
	ui.AxisZTitleInPlaneRotationSpin->setValue(uChroma_.axisTitleOrientation(2).y);
	ui.AxisZTitleDistanceSpin->setValue(uChroma_.axisTitleOrientation(2).z);

	refreshing_ = false;
}
