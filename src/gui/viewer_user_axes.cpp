/*
	*** Axes Functions
	*** src/gui/axes.cpp
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

// Update axes primitives
 void UChromaWindow::updateAxesPrimitives()
{
	// Check whether we need to regenerate the axes primitives / data
	if (!regenerateAxes_) return;

	QString s;
	FTBBox boundingBox;
	Vec3<double> centre;
	double dpX, dpY, textWidth, delta, value;
	int n;
	Vec3<double> u, tickDir;
	Matrix labelTransform, titleTransform;

	// Determine central coordinate within current axes and the Y clip limits
	for (int n=0; n<3; ++n)
	{
		if (axisLogarithmic_[n]) axesCentre_[n] = (axisInverted_[n] ? log10(axisMax_[n]/axisMin_[n]) : log10(axisMax_[n]*axisMin_[n])) * 0.5 * axisStretch_[n];
		else axesCentre_[n] = (axisMax_[n]+axisMin_[n]) * 0.5 * axisStretch_[n];
	}
	if (axisLogarithmic_.y) ui.MainView->setYClip(log10(axisMin_.y) * axisStretch_.y, log10(axisMax_.y) * axisStretch_.y);
	else ui.MainView->setYClip(axisMin_.y * axisStretch_.y, axisMax_.y * axisStretch_.y);
	
	// Set basic extreme coordinates for axes - actual limits on axes will be set in following loop
	for (int axis=0; axis < 3; ++axis)
	{
		for (int n=0; n<3; ++n)
		{
			if (axisLogarithmic_[n]) axisCoordMin_[axis].set(n, (axisInverted_[n] ? log10(axisMax_[n]/axisPosition_[axis][n]) : log10(axisPosition_[axis][n])) * axisStretch_[n]);
			else axisCoordMin_[axis].set(n, (axisInverted_[n] ? axisMax_[n] - axisPosition_[axis][n] : axisPosition_[axis][n]) * axisStretch_[n]);
		}
		axisCoordMax_[axis] = axisCoordMin_[axis];
	}

	// Construct axes
	for (int axis = 0; axis < 3; ++axis)
	{
		// Clear old axis primitives
		ui.MainView->clearAxisPrimitives(axis);

		// Normalise tickDirection
		tickDir = axisTickDirection_[axis];
		tickDir.normalise();

		// Create tick label transformation matrix
		labelTransform.setIdentity();
		// 1) Apply axial rotation along X axis (left-to-right direction)
		labelTransform.applyRotationX(axisLabelOrientation_[axis].x);
		// 2) Translate to 'end of tick' position
		labelTransform.applyTranslation(tickDir * axisLabelOrientation_[axis].z);
		// 3) Perform in-plane rotation
		labelTransform.applyRotationZ(axisLabelOrientation_[axis].y);
		labelTransform.applyScaling(labelScale_, labelScale_, labelScale_);

		// Create axis title transformation matrix
		titleTransform.setIdentity();
		// 1) Apply axial rotation along X axis (left-to-right direction)
		titleTransform.applyRotationX(axisTitleOrientation_[axis].x);
		// 2) Translate to 'end of tick' position
		titleTransform.applyTranslation(tickDir * axisTitleOrientation_[axis].z);
		// 3) Perform in-plane rotation
		titleTransform.applyRotationZ(axisTitleOrientation_[axis].y);
		titleTransform.applyScaling(titleScale_, titleScale_, titleScale_);

		if (axisLogarithmic_[axis])
		{
			// Set axis min/max coordinates
			axisCoordMin_[axis].set(axis, (axisInverted_[axis] ? log10(axisMax_[axis] / axisMin_[axis]) : log10(axisMin_[axis])) * axisStretch_[axis]);
			axisCoordMax_[axis].set(axis, (axisInverted_[axis] ? log10(axisMax_[axis] / axisMax_[axis]) : log10(axisMax_[axis])) * axisStretch_[axis]);

			// For the log axis, the associated surface data coordinate will already be in log form
			if (axisMax_[axis] < 0.0)
			{
				msg.print("Axis range is inappropriate for a log scale (%f < x < %f). Axis will not be drawn.\n", axisMin_[axis], axisMax_[axis]);
				return;
			}

			// Draw a line from min to max range, passing through the defined axisPosition
			ui.MainView->addAxisLine(axis, axisCoordMin_[axis], axisCoordMax_[axis]);

			// Grab logged min/max values for convenience, enforcing sensible minimum
			double axisMin = log10(axisMin_[axis] <= 0.0 ? 1.0e-10 : axisMin_[axis]);
			double axisMax = log10(axisMax_[axis]);

			// Plot tickmarks - Start at floored (ceiling'd) integer of logAxisMin (logAxisMax), and go from there.
			int nMinorTicks = axisMinorTicks_[axis] > 8 ? 8 : axisMinorTicks_[axis];
			int count = 0;
			double power = floor(axisMin);
			double value = pow(10,power);
			Vec3<double> u = axisCoordMin_[axis];
			while (true)
			{
				// Check break condition
				if (value > axisMax_[axis]) break;

				// If the current value is in range, plot a tick
				u[axis] = (axisInverted_[axis] ? log10(axisMax_[axis]/value): log10(value)) * axisStretch_[axis];
				if (log10(value) >= axisMin)
				{
					// Tick mark
					ui.MainView->addAxisLine(axis, u, u+tickDir*(count == 0 ? 0.1 : 0.05));

					// Tick label
					if (count == 0)
					{
						// Create text, accounting for zero-roundoff, and add a text primitive
						if (fabs(value) < pow(10,power-5)) s = "0";
						else s = QString::number(value);

						ui.MainView->addAxisText(axis, s, u, labelTransform);
					}
				}

				// Increase tick counter, value, and power if necessary
				++count;
				if (count > nMinorTicks)
				{
					count = 0;
					power = power + 1.0;
					value = pow(10,power);
				}
				else value += pow(10,power);
			}

			// Add axis title
			u = axisCoordMin_[axis];
			value = axisMin_[axis] + (axisMax_[axis] - axisMin_[axis]) * axisTitleOrientation_[axis].w;
			u.set(axis, (axisInverted_[axis] ? log10(axisMax_[axis]/value) : log10(value)) * axisStretch_[axis]);
			u += tickDir * axisTitleOrientation_[axis].z;
			ui.MainView->addAxisText(axis, axisTitle_[axis], u, titleTransform, axisTitleAnchor_[axis]);
		}
		else
		{
			// Set axis min/max coordinates
			axisCoordMin_[axis].set(axis, (axisInverted_[axis] ? axisMax_[axis] : axisMin_[axis]) * axisStretch_[axis]);
			axisCoordMax_[axis].set(axis, (axisInverted_[axis] ? axisMin_[axis] : axisMax_[axis]) * axisStretch_[axis]);
		
			// Calculate autoticks if necessary
			if (axisAutoTicks_[axis]) calculateTickDeltas(axis);

			// Draw a line from min to max limits, passing through the defined axisPosition
			ui.MainView->addAxisLine(axis, axisCoordMin_[axis], axisCoordMax_[axis]);

			// Check tickDelta
			if (((axisMax_[axis]-axisMin_[axis]) / axisTickDelta_[axis]) > 1e6) return;

			// Plot tickmarks - maximum of 100 minor tickmarks between major lines
			int count = 0;
			delta = axisTickDelta_[axis] / (axisMinorTicks_[axis]+1);
			value = axisFirstTick_[axis];
			u = axisCoordMin_[axis];
			u.set(axis, (axisInverted_[axis] ? (axisMax_[axis] - axisFirstTick_[axis]) + axisMin_[axis]: axisFirstTick_[axis]) * axisStretch_[axis]);
			while (value <= axisMax_[axis])
			{
				// Draw tick here, only if value >= axisMin_
				if (value >= axisMin_[axis])
				{
					if (count %(axisMinorTicks_[axis]+1) == 0)
					{
						ui.MainView->addAxisLine(axis, u, u + tickDir*0.1);
						
						// Get formatted label text, acounting for roundoff error
						if (fabs(value) < axisTickDelta_[axis]*1.0e-10) s = "0";
						else s = QString::number(value);

						ui.MainView->addAxisText(axis, s, u, labelTransform);
						
						count = 0;
					}
					else ui.MainView->addAxisLine(axis, u, u + tickDir*0.05);
				}
				u.add(axis, delta * (axisInverted_[axis] ? -axisStretch_[axis] : axisStretch_[axis]));
				value += delta;
				++count;
			}

			// Add axis title
			u = axisCoordMin_[axis];
			value = axisMin_[axis] + (axisMax_[axis] - axisMin_[axis]) * axisTitleOrientation_[axis].w;
			u.set(axis, (axisInverted_[axis] ? (axisMax_[axis] - value) + axisMin_[axis]: value) * axisStretch_[axis]);
			u += tickDir * axisTitleOrientation_[axis].z;
			ui.MainView->addAxisText(axis, axisTitle_[axis], u, titleTransform, axisTitleAnchor_[axis]);
		}
	}

	// Setup Bounding Box
	ui.MainView->createBoundingBox(boundingBox_, axisLogarithmic_.y ? log10(boundingBoxPlaneY_) : boundingBoxPlaneY_);

	regenerateAxes_ = false;
}
