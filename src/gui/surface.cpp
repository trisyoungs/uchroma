/*
	*** Surface Generation
	*** src/gui/surface.cpp
	Copyright T. Youngs 2013

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

// Update surface data after data change
void UChromaWindow::updateSurface(bool dataHasChanged)
{
	QString s;
	FTBBox boundingBox;
	Vec3<double> centre;
	double dpX, dpY, textWidth, delta, value;
	int n;
	Vec3<double> u, tickDir;
	Matrix labelTransform, titleTransform;

	// Determine surface center and Y clip limits
	for (int n=0; n<3; ++n)
	{
		if (axisLogarithmic_[n]) surfaceCentre_[n] = (axisInverted_[n] ? log10(limitMax_[n]/limitMin_[n]) : log10(limitMax_[n]*limitMin_[n])) * 0.5 * axisStretch_[n];
		else surfaceCentre_[n] = (limitMax_[n]+limitMin_[n]) * 0.5 * axisStretch_[n];
	}
	if (axisLogarithmic_.y) ui.MainView->setYClip(log10(limitMin_.y) * axisStretch_.y, log10(limitMax_.y) * axisStretch_.y);
	else ui.MainView->setYClip(limitMin_.y * axisStretch_.y, limitMax_.y * axisStretch_.y);
	
	// Set basic extreme coordinates for axes - actual limits on axes will be set in following loop
	for (int axis=0; axis < 3; ++axis)
	{
		for (int n=0; n<3; ++n)
		{
			if (axisLogarithmic_[n]) axisCoordMin_[axis].set(n, (axisInverted_[n] ? log10(limitMax_[n]/axisPosition_[axis][n]) : log10(axisPosition_[axis][n])) * axisStretch_[n]);
			else axisCoordMin_[axis].set(n, (axisInverted_[n] ? limitMax_[n] - axisPosition_[axis][n] : axisPosition_[axis][n]) * axisStretch_[n]);
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
			axisCoordMin_[axis].set(axis, (axisInverted_[axis] ? log10(limitMax_[axis] / limitMin_[axis]) : log10(limitMin_[axis])) * axisStretch_[axis]);
			axisCoordMax_[axis].set(axis, (axisInverted_[axis] ? log10(limitMax_[axis] / limitMax_[axis]) : log10(limitMax_[axis])) * axisStretch_[axis]);

			// For the log axis, the associated surface data coordinate will already be in log form
			if (limitMax_[axis] < 0.0)
			{
				msg.print("Axis range is inappropriate for a log scale (%f < x < %f). Axis will not be drawn.\n", limitMin_[axis], limitMax_[axis]);
				return;
			}

			// Draw a line from min to max range, passing through the defined axisPosition
			ui.MainView->addAxisLine(axis, axisCoordMin_[axis], axisCoordMax_[axis]);

			// Grab logged min/max values for convenience, enforcing sensible minimum
			double axisMin = log10(limitMin_[axis] <= 0.0 ? 1.0e-10 : limitMin_[axis]);
			double axisMax = log10(limitMax_[axis]);

			// Plot tickmarks - Start at floored (ceiling'd) integer of logAxisMin (logAxisMax), and go from there.
			int nMinorTicks = axisMinorTicks_[axis] > 8 ? 8 : axisMinorTicks_[axis];
			int count = 0;
			double power = floor(axisMin);
			double value = pow(10,power);
			Vec3<double> u = axisCoordMin_[axis];
			while (true)
			{
				// Check break condition
				if (value > limitMax_[axis]) break;

				// If the current value is in range, plot a tick
				u[axis] = (axisInverted_[axis] ? log10(limitMax_[axis]/value): log10(value)) * axisStretch_[axis];
				if (value >= axisMin)
				{
					// Tick mark
					ui.MainView->addAxisLine(axis, u, u+tickDir*(count == 0 ? 0.1 : 0.05));

					// Tick label
					if (count == 0)
					{
						// Determine resulting text primitive width (so we can do label rotations)
						s = QString::number(value);

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
			value = limitMin_[axis] + (limitMax_[axis] - limitMin_[axis]) * axisTitleOrientation_[axis].w;
			u.set(axis, (axisInverted_[axis] ? log10(limitMax_[axis]/value) : log10(value)) * axisStretch_[axis]);
			u += tickDir * axisTitleOrientation_[axis].z;
			ui.MainView->addAxisText(axis, axisTitle_[axis], u, titleTransform, axisTitleAnchor_[axis]);
		}
		else
		{
			// Set axis min/max coordinates
			axisCoordMin_[axis].set(axis, (axisInverted_[axis] ? limitMax_[axis] : limitMin_[axis]) * axisStretch_[axis]);
			axisCoordMax_[axis].set(axis, (axisInverted_[axis] ? limitMin_[axis] : limitMax_[axis]) * axisStretch_[axis]);
		
			// Calculate autoticks if necessary
			if (axisAutoTicks_[axis]) calculateTickDeltas(axis);

			// Draw a line from min to max limits, passing through the defined axisPosition
			ui.MainView->addAxisLine(axis, axisCoordMin_[axis], axisCoordMax_[axis]);

			// Check tickDelta
			if (((limitMax_[axis]-limitMin_[axis]) / axisTickDelta_[axis]) > 1e6) return;

			// Plot tickmarks - maximum of 100 minor tickmarks between major lines
			int count = 0;
			delta = axisTickDelta_[axis] / (axisMinorTicks_[axis]+1);
			value = axisFirstTick_[axis];
			u = axisCoordMin_[axis];
			u.set(axis, (axisInverted_[axis] ? (limitMax_[axis] - axisFirstTick_[axis]) + limitMin_[axis]: axisFirstTick_[axis]) * axisStretch_[axis]);
			while (value <= limitMax_[axis])
			{
				// Draw tick here, only if value >= limitMin_
				if (value >= limitMin_[axis])
				{
					if (count %(axisMinorTicks_[axis]+1) == 0)
					{
						ui.MainView->addAxisLine(axis, u, u + tickDir*0.1);
						
						// Get formatted label text
						s = QString::number(value);

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
			value = limitMin_[axis] + (limitMax_[axis] - limitMin_[axis]) * axisTitleOrientation_[axis].w;
			u.set(axis, (axisInverted_[axis] ? (limitMax_[axis] - value) + limitMin_[axis]: value) * axisStretch_[axis]);
			u += tickDir * axisTitleOrientation_[axis].z;
			ui.MainView->addAxisText(axis, axisTitle_[axis], u, titleTransform, axisTitleAnchor_[axis]);
		}
	}

	// Reconstruct surface
	if (dataHasChanged)
	{
		// Clear existing display slices
		surfaceData_.clear();
		double x, y;

		// Loop over slices, apply any transforms (X or Y) and check limits
		Slice* slice = axisInverted_.z ? slices_.last() : slices_.first();
		while (slice)
		{
			// Z
			double z = slice->transformedData().z();
			// -- Is the transformed Z value within range?
			if ((z < limitMin_.z) || (z > limitMax_.z))
			{
				slice = axisInverted_.z ? slice->prev : slice->next;
				continue;
			}
			if (axisInverted_.z && axisLogarithmic_.z) z = log10(limitMax_[n]/z);
			else if (axisInverted_.z) z = (limitMax_.z - z) + limitMin_.z;
			else if (axisLogarithmic_.z) z = log10(z);

			// Add new item to surfaceData_ array
			Data2D* surfaceSlice = surfaceData_.add();
			surfaceSlice->setZ(z * axisStretch_.z);

			// Copy / interpolate arrays
			Array<double> array[2];
			if (interpolate_.x)
			{
				slice->transformedData().interpolate(interpolateConstrained_.x);
				double x = limitMin_.x;
				while (x <= limitMax_.x)
				{
					array[0].add(x);
					array[1].add(slice->transformedData().interpolated(x));
					x += interpolationStep_.x;
				}
			}
			else
			{
				array[0] = slice->transformedData().arrayX();
				array[1] = slice->transformedData().arrayY();
			}

			// Add data to surfaceSlice, obeying defined x-limits
			if (axisInverted_.x) for (int n=array[0].nItems()-1; n >= 0; --n)
			{
				x = array[0].value(n);
				if ((x < limitMin_.x) || (x > limitMax_.x)) continue;
				if (axisLogarithmic_.x) x = log10(limitMax_.x / x);
				else x = (limitMax_.x - x) + limitMin_.x;
				x *= axisStretch_.x;
				y = array[1].value(n);
				if (axisLogarithmic_.y) y = (axisInverted_.y ? log10(limitMax_.y / y) : log10(y));
				else if (axisInverted_.y) y = (limitMax_.y - y) + limitMin_.y;
				y *= axisStretch_.y;
				surfaceSlice->addPoint(x, y);
			}
			else for (int n=0; n<array[0].nItems(); ++n)
			{
				x = array[0].value(n);
				if ((x < limitMin_.x) || (x > limitMax_.x)) continue;
				if (axisLogarithmic_.x) x = log10(x);
				x *= axisStretch_.x;
				y = array[1].value(n);
				if (axisLogarithmic_.y)
				{
					if (y < 0.0) y = 1.0e-10;
					else y = (axisInverted_.y ? log10(limitMax_.y / y) : log10(y));
				}
				else if (axisInverted_.y) y = (limitMax_.y - y) + limitMin_.y;
				y *= axisStretch_.y;
				surfaceSlice->addPoint(x, y);
			}

			// Move to next Z slice
			slice = axisInverted_.z ? slice->prev : slice->next;
		}
	}

	// Create temporary colourScale_
	ColourScale scale = colourScale_;
	if (alphaControl_ == UChromaWindow::FixedAlpha) scale.setAllAlpha(fixedAlpha_);

	// Update surface GL object
	ui.MainView->createSurface(scale, axisStretch_.y);

	// Setup Bounding Box
	ui.MainView->createBoundingBox(boundingBox_, axisLogarithmic_.y ? log10(boundingBoxPlaneY_) : boundingBoxPlaneY_);

	ui.MainView->update();

	ui.NTrianglesLabel->setText("NTriangles: " + QString::number(ui.MainView->surfaceNTriangles()));
}

// Return central coordinate of surface
Vec3<double> UChromaWindow::surfaceCentre()
{
	return surfaceCentre_;
}
