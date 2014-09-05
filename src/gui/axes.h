/*
	*** uChroma Axes Window
	*** src/gui/axes.h
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

#ifndef UCHROMA_AXESWINDOW_H
#define UCHROMA_AXESWINDOW_H

#include "gui/ui_axes.h"
#include "base/axes.h"

// Forward Declarations
class UChromaWindow;

class AxesWindow : public QWidget
{
	// All Qt declarations must include this macro
	Q_OBJECT


	/*
	 * Window Functions
	 */
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;

	public:
	// Constructor / Destructor
	AxesWindow(UChromaWindow& parent);
	~AxesWindow();
	// Main form declaration
	Ui::AxesWindow ui;
	// UChromaWindow reference
	UChromaWindow& uChroma_;

	protected:
	// Window close event
	void closeEvent(QCloseEvent* event);

	signals:
	// Window closed signal
	void windowClosed(bool);


	/*
	 * Convenience Functions
	 */
	private:
	bool haveCurrentAxes();
	Axes& currentAxes();
	bool axisInvertChanged(int axis, bool checked);
	bool axisLogarithmicChanged(int axis, bool checked);
	bool axisVisibleChanged(int axis, bool checked);
	bool axisStretchChanged(int axis, double value);
	bool axisLimitChanged(int axis, bool minLim, double value);
	bool axisLimitSetExtreme(int axis, bool minLim);
	bool axisPositionIsFractionalChanged(int axis, bool real);
	bool axisPositionChanged(bool real, int axis, int dir, double value);
	bool axisPositionSet(bool real, int axis, int dir, int type);
	bool axisAutoTicksChanged(int axis, bool enabled);
	bool axisTicksChanged(int axis, bool start, double value);
	bool axisTickOrientationChanged(int axis, int dir, double value);
	bool axisTickSizeChanged(int axis, double value);
	bool axisLabelOrientationChanged(int axis, int component, double value);
	bool axisMinorTicksChanged(int axis, int value);
	bool axisTitleChanged(int axis, QString& title);
	bool axisTitleOrientationChanged(int axis, int component, double value);
	bool axisAnchorChanged(int axis, bool titleAnchor, TextPrimitive::TextAnchor anchor);
	bool axisGridLineChanged(int axis, bool major, bool on);
	bool axisGridFullChanged(int axis, bool full);
	bool axisGridStyleClicked(int axis, bool major);
	bool axisGridStyleApplyClicked(int axis);


	/*
	 * Slots
	 */
	private slots:
	// -- X Axis Tab
	// -- -- General
	void on_AxisXInvertCheck_clicked(bool checked);
	void on_AxisXLogarithmicCheck_clicked(bool checked);
	void on_AxisXVisibleCheck_clicked(bool checked);
	void on_AxisXTitleEdit_textChanged(QString text);
	void on_AxisXStretchSpin_valueChanged(double value);
	void on_AxisXMinSpin_valueChanged(double value);
	void on_AxisXMaxSpin_valueChanged(double value);
	void on_AxisXMinSetMinimumButton_clicked(bool checked);
	void on_AxisXMaxSetMaximumButton_clicked(bool checked);
	void on_AxisXPositionRealRadio_clicked(bool checked);
	void on_AxisXPositionFractionalRadio_clicked(bool checked);
	void on_AxisXPositionYRealSpin_valueChanged(double value);
	void on_AxisXPositionYFractionalSpin_valueChanged(double value);
	void on_AxisXPositionZRealSpin_valueChanged(double value);
	void on_AxisXPositionZFractionalSpin_valueChanged(double value);
	void on_AxisXPositionYSetMinimumButton_clicked(bool checked);
	void on_AxisXPositionYSetZeroButton_clicked(bool checked);
	void on_AxisXPositionYSetMaximumButton_clicked(bool checked);
	void on_AxisXPositionZSetMinimumButton_clicked(bool checked);
	void on_AxisXPositionZSetZeroButton_clicked(bool checked);
	void on_AxisXPositionZSetMaximumButton_clicked(bool checked);
	// -- -- Ticks
	void on_AxisXAutoTicksCheck_clicked(bool checked);
	void on_AxisXTicksStartSpin_valueChanged(double value);
	void on_AxisXTicksDeltaSpin_valueChanged(double value);
	void on_AxisXMinorTicksSpin_valueChanged(int value);
	void on_AxisXTickDirectionXSpin_valueChanged(double value);
	void on_AxisXTickDirectionYSpin_valueChanged(double value);
	void on_AxisXTickDirectionZSpin_valueChanged(double value);
	void on_AxisXTickSizeSpin_valueChanged(double value);
	// -- -- Labels
	void on_AxisXLabelAnchorCombo_currentIndexChanged(int index);
	void on_AxisXLabelAxialRotationSlider_valueChanged(int value);
	void on_AxisXLabelAxialRotationSpin_valueChanged(int value);
	void on_AxisXLabelInPlaneRotationSlider_valueChanged(int value);
	void on_AxisXLabelInPlaneRotationSpin_valueChanged(int value);
	void on_AxisXLabelDistanceSpin_valueChanged(double value);
	void on_AxisXTitleHOffsetSlider_valueChanged(int value);
	void on_AxisXTitleHOffsetLeftButton_clicked(bool checked);
	void on_AxisXTitleHOffsetCentreButton_clicked(bool checked);
	void on_AxisXTitleHOffsetRightButton_clicked(bool checked);
	void on_AxisXTitleAnchorCombo_currentIndexChanged(int index);
	void on_AxisXTitleAxialRotationSlider_valueChanged(int value);
	void on_AxisXTitleAxialRotationSpin_valueChanged(int value);
	void on_AxisXTitleInPlaneRotationSlider_valueChanged(int value);
	void on_AxisXTitleInPlaneRotationSpin_valueChanged(int value);
	void on_AxisXTitleDistanceSpin_valueChanged(double value);
	// -- -- GridLines
	void on_AxisXGridLineMajorCheck_clicked(bool checked);
	void on_AxisXGridLineMinorCheck_clicked(bool checked);
	void on_AxisXGridLineFullCheck_clicked(bool checked);
	void on_AxisXGridLineMajorStyleButton_clicked(bool checked);
	void on_AxisXGridLineMinorStyleButton_clicked(bool checked);
	void on_AxisXGridLineApplyStyleButton_clicked(bool checked);
	// -- Y Axis Tab
	// -- -- General
	void on_AxisYInvertCheck_clicked(bool checked);
	void on_AxisYLogarithmicCheck_clicked(bool checked);
	void on_AxisYVisibleCheck_clicked(bool checked);
	void on_AxisYTitleEdit_textChanged(QString text);
	void on_AxisYStretchSpin_valueChanged(double value);
	void on_AxisYMinSpin_valueChanged(double value);
	void on_AxisYMinSetMinimumButton_clicked(bool checked);
	void on_AxisYMaxSpin_valueChanged(double value);
	void on_AxisYMaxSetMaximumButton_clicked(bool checked);
	void on_AxisYPositionRealRadio_clicked(bool checked);
	void on_AxisYPositionFractionalRadio_clicked(bool checked);
	void on_AxisYPositionXRealSpin_valueChanged(double value);
	void on_AxisYPositionXFractionalSpin_valueChanged(double value);
	void on_AxisYPositionZRealSpin_valueChanged(double value);
	void on_AxisYPositionZFractionalSpin_valueChanged(double value);
	void on_AxisYPositionXSetMinimumButton_clicked(bool checked);
	void on_AxisYPositionXSetZeroButton_clicked(bool checked);
	void on_AxisYPositionXSetMaximumButton_clicked(bool checked);
	void on_AxisYPositionZSetMinimumButton_clicked(bool checked);
	void on_AxisYPositionZSetZeroButton_clicked(bool checked);
	void on_AxisYPositionZSetMaximumButton_clicked(bool checked);
	// -- -- Ticks
	void on_AxisYAutoTicksCheck_clicked(bool checked);
	void on_AxisYTicksStartSpin_valueChanged(double value);
	void on_AxisYTicksDeltaSpin_valueChanged(double value);
	void on_AxisYMinorTicksSpin_valueChanged(int value);
	void on_AxisYTickDirectionXSpin_valueChanged(double value);
	void on_AxisYTickDirectionYSpin_valueChanged(double value);
	void on_AxisYTickDirectionZSpin_valueChanged(double value);
	void on_AxisYTickSizeSpin_valueChanged(double value);
	// -- -- Labels
	void on_AxisYLabelAnchorCombo_currentIndexChanged(int index);
	void on_AxisYLabelAxialRotationSlider_valueChanged(int value);
	void on_AxisYLabelAxialRotationSpin_valueChanged(int value);
	void on_AxisYLabelInPlaneRotationSlider_valueChanged(int value);
	void on_AxisYLabelInPlaneRotationSpin_valueChanged(int value);
	void on_AxisYLabelDistanceSpin_valueChanged(double value);
	void on_AxisYTitleHOffsetSlider_valueChanged(int value);
	void on_AxisYTitleHOffsetLeftButton_clicked(bool checked);
	void on_AxisYTitleHOffsetCentreButton_clicked(bool checked);
	void on_AxisYTitleHOffsetRightButton_clicked(bool checked);
	void on_AxisYTitleAnchorCombo_currentIndexChanged(int index);
	void on_AxisYTitleAxialRotationSlider_valueChanged(int value);
	void on_AxisYTitleAxialRotationSpin_valueChanged(int value);
	void on_AxisYTitleInPlaneRotationSlider_valueChanged(int value);
	void on_AxisYTitleInPlaneRotationSpin_valueChanged(int value);
	void on_AxisYTitleDistanceSpin_valueChanged(double value);
	// -- -- Grid
	void on_AxisYGridLineMajorCheck_clicked(bool checked);
	void on_AxisYGridLineMinorCheck_clicked(bool checked);
	void on_AxisYGridLineFullCheck_clicked(bool checked);
	void on_AxisYGridLineMajorStyleButton_clicked(bool checked);
	void on_AxisYGridLineMinorStyleButton_clicked(bool checked);
	void on_AxisYGridLineApplyStyleButton_clicked(bool checked);
	// -- Z Axis Tab
	// -- -- General
	void on_AxisZInvertCheck_clicked(bool checked);
	void on_AxisZLogarithmicCheck_clicked(bool checked);
	void on_AxisZVisibleCheck_clicked(bool checked);
	void on_AxisZTitleEdit_textChanged(QString text);
	void on_AxisZStretchSpin_valueChanged(double value);
	void on_AxisZMinSpin_valueChanged(double value);
	void on_AxisZMinSetMinimumButton_clicked(bool checked);
	void on_AxisZMaxSpin_valueChanged(double value);
	void on_AxisZMaxSetMaximumButton_clicked(bool checked);
	void on_AxisZPositionRealRadio_clicked(bool checked);
	void on_AxisZPositionFractionalRadio_clicked(bool checked);
	void on_AxisZPositionXRealSpin_valueChanged(double value);
	void on_AxisZPositionYRealSpin_valueChanged(double value);
	void on_AxisZPositionXFractionalSpin_valueChanged(double value);
	void on_AxisZPositionYFractionalSpin_valueChanged(double value);
	void on_AxisZPositionXSetMinimumButton_clicked(bool checked);
	void on_AxisZPositionXSetZeroButton_clicked(bool checked);
	void on_AxisZPositionXSetMaximumButton_clicked(bool checked);
	void on_AxisZPositionYSetMinimumButton_clicked(bool checked);
	void on_AxisZPositionYSetZeroButton_clicked(bool checked);
	void on_AxisZPositionYSetMaximumButton_clicked(bool checked);
	// -- -- Ticks
	void on_AxisZAutoTicksCheck_clicked(bool checked);
	void on_AxisZTicksStartSpin_valueChanged(double value);
	void on_AxisZTicksDeltaSpin_valueChanged(double value);
	void on_AxisZMinorTicksSpin_valueChanged(int value);
	void on_AxisZTickDirectionXSpin_valueChanged(double value);
	void on_AxisZTickDirectionYSpin_valueChanged(double value);
	void on_AxisZTickDirectionZSpin_valueChanged(double value);
	void on_AxisZTickSizeSpin_valueChanged(double value);
	// -- -- Labels
	void on_AxisZLabelAnchorCombo_currentIndexChanged(int index);
	void on_AxisZLabelAxialRotationSlider_valueChanged(int value);
	void on_AxisZLabelAxialRotationSpin_valueChanged(int value);
	void on_AxisZLabelInPlaneRotationSlider_valueChanged(int value);
	void on_AxisZLabelInPlaneRotationSpin_valueChanged(int value);
	void on_AxisZLabelDistanceSpin_valueChanged(double value);
	void on_AxisZTitleHOffsetSlider_valueChanged(int value);
	void on_AxisZTitleHOffsetLeftButton_clicked(bool checked);
	void on_AxisZTitleHOffsetCentreButton_clicked(bool checked);
	void on_AxisZTitleHOffsetRightButton_clicked(bool checked);
	void on_AxisZTitleAnchorCombo_currentIndexChanged(int index);
	void on_AxisZTitleAxialRotationSlider_valueChanged(int value);
	void on_AxisZTitleAxialRotationSpin_valueChanged(int value);
	void on_AxisZTitleInPlaneRotationSlider_valueChanged(int value);
	void on_AxisZTitleInPlaneRotationSpin_valueChanged(int value);
	void on_AxisZTitleDistanceSpin_valueChanged(double value);
	// -- -- Grid
	void on_AxisZGridLineMajorCheck_clicked(bool checked);
	void on_AxisZGridLineMinorCheck_clicked(bool checked);
	void on_AxisZGridLineFullCheck_clicked(bool checked);
	void on_AxisZGridLineMajorStyleButton_clicked(bool checked);
	void on_AxisZGridLineMinorStyleButton_clicked(bool checked);
	void on_AxisZGridLineApplyStyleButton_clicked(bool checked);


	/*
	 * Update Functions
	 */
	public:
	// Update controls and show window
	void updateAndShow();
	// Update controls
	void updateControls(bool force = false);
};

#endif
