/*
	*** Main Window - Surface 
	*** src/gui/uchroma_surface.cpp
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
	Matrix A, baseLabelTransform;

	// Determine surface center and Y clip limits
	for (int n=0; n<3; ++n)
	{
		if (axisLogarithmic_[n]) surfaceCentre_[n] = (axisInverted_[n] ? log10(limitMax_[n]/limitMin_[n]) : log10(limitMax_[n]*limitMin_[n])) * 0.5 * axisStretch_[n];
		else surfaceCentre_[n] = (limitMax_[n]+limitMin_[n]) * 0.5 * axisStretch_[n];
	}
	printf("SURFACECENTRE = "); surfaceCentre_.print();
	if (axisLogarithmic_.y) ui.MainView->setYClip(log10(limitMin_.y) * axisStretch_.y, log10(limitMax_.y) * axisStretch_.y);
	else ui.MainView->setYClip(limitMin_.y * axisStretch_.y, limitMax_.y * axisStretch_.y);
	
	// Set basic extreme coordinates for axes - actual limits on axes will be set in following loop
	for (int axis=0; axis < 3; ++axis)
	{
		for (int n=0; n<3; ++n)
		{
			if (axisLogarithmic_[n]) axisCoordMin_[axis].set(n, (axisInverted_[n] ? log10(limitMax_[n] - axisPosition_[axis][n]) : log10(axisPosition_[axis][n])) * axisStretch_[n]);
			else axisCoordMin_[axis].set(n, (axisInverted_[n] ? limitMax_[n] - axisPosition_[axis][n] : axisPosition_[axis][n]) * axisStretch_[n]);
		}
		printf("ORIGIN Min %i = ", axis); axisCoordMin_[axis].print();
		axisCoordMax_[axis] = axisCoordMin_[axis];
	}

	// Construct axes
	for (int axis = 0; axis < 3; ++axis)
	{
		// Clear old axis primitives
		ui.MainView->clearAxisPrimitives(axis);

		// Create label and title transformation matrices
		Matrix labelTransform, titleTransform;
		// -- Set left-to-right direction of text to be along axis direction
		Vec3<double> leftToRight(0.0,0.0,0.0);
		leftToRight.set(axis, 1.0);
		// -- Our basic 'up' vector will be another orthogonal axis (but never z)
		Vec3<double> upVector(0.0,0.0,0.0);
		if (axis == 0) upVector.set(0.0, 1.0, 0.0);
		else upVector.set(-1.0, 0.0, 0.0);
		labelTransform.setColumn(0, leftToRight, 0.0);
		labelTransform.setColumn(1, upVector, 0.0);
		labelTransform.setColumn(2, leftToRight * upVector, 0.0);
		labelTransform.setColumn(3, 0.0, 0.0, 0.0, 1.0);
		titleTransform = labelTransform;
	
		if (labelFaceViewer_)
		{
			labelTransform = ui.MainView->viewMatrix();
			labelTransform.removeTranslationAndScaling();
			labelTransform.invert();

			titleTransform = ui.MainView->viewMatrix();
			titleTransform.removeTranslationAndScaling();
			titleTransform.invert();
		}
		else
		{printf("Axis %i MINCOORD = ", axis); axisCoordMin_[axis].print();
			// Label Transform
			// -- Apply rotation out of AB plane...
// 			labelTransform.applyTranslation(0.0, 1.0, 0.0);
			Matrix rotMat;
			rotMat.createRotationZ(axisLabelOrientation_[axis].y);
// 			rotMat.print();
			rotMat.createRotationAxis(0.0, 0.0, -1.0, axisLabelOrientation_[axis].y, false);
// 			rotMat.print();
			labelTransform *= rotMat;
// 			labelTransform.applyRotationAxis(0.0, 0.0, -1.0, axisLabelOrientation_[axis].y, false);
// 			labelTransform.applyRotationAxis(leftToRight.x, leftToRight.y, leftToRight.z, axisLabelOrientation_[axis].x, false);
			upVector = labelTransform.columnAsVec3(1);
			// ...and in current text plane

			// Title transform
			// -- Apply rotation out of AB plane...
			titleTransform.applyRotationAxis(leftToRight.x, leftToRight.y, leftToRight.z, axisTitleOrientation_[axis].x, false);
			upVector = titleTransform.columnAsVec3(1);
			// ...and in current text plane
			titleTransform.applyRotationAxis(titleTransform[8], titleTransform[9], titleTransform[10], axisTitleOrientation_[axis].y, false);
			titleTransform.setTranslation(0.0, axisTitleOrientation_[axis].z, 0.0);
		}
		labelTransform.columnMultiply( Vec3<double>(labelScale_,labelScale_,labelScale_) );
		titleTransform.columnMultiply( Vec3<double>(titleScale_,titleScale_,titleScale_) );

		// Normalise tickDirection
		tickDir = axisTickDirection_[axis];
		tickDir.normalise();

		if (axisLogarithmic_[axis])
		{
			// Set axis min/max coordinates
			axisCoordMin_[axis].set(axis, (axisInverted_[axis] ? log10(limitMax_[axis] / limitMin_[axis]) : log10(limitMin_[axis])) * axisStretch_[axis]);
			axisCoordMax_[axis].set(axis, (axisInverted_[axis] ? log10(limitMax_[axis] / limitMax_[axis]) : log10(limitMax_[axis])) * axisStretch_[axis]);

			printf("Axis %i LOGMINCOORD = ", axis); axisCoordMin_[axis].print();
			printf("Axis %i LOGMAXCOORD = ", axis); axisCoordMax_[axis].print();
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
			printf("Power, value = %f, %f\n", power, value);
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

						// Create transformation matrix for text label
						A.setIdentity();
						// 1) Translate to anchor point of text
		// 				A.applyTranslation(textWidth*0.5, fontBaseHeight_*0.5, 0.0);
						// 2) Apply rotation in plane of text
						// 3) Translate to 'end of tick' position
						// 4) Perform axial rotation
						A.applyRotationAxis(leftToRight.x, leftToRight.y, leftToRight.z, axisLabelOrientation_[axis].x, false);
						A.applyTranslation(upVector * axisLabelOrientation_[axis].z);
						A.applyRotationAxis(0.0, 0.0, 1.0, axisLabelOrientation_[axis].y, false);
						A.applyScaling(labelScale_, labelScale_, labelScale_);

						ui.MainView->addAxisText(axis, s, u, A, labelScale_);
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

			// Create label and title transformation matrices
			Vec3<double> leftToRight(0.0, 0.0, 0.0), upVector(0.0, 0.0, 0.0);
			// Set left-to-right direction of text to be along axis direction
			leftToRight.set(axis, 1.0);
			// Our basic 'up' vector will be another orthogonal axis (but never z)
			if (axis == 0) upVector.set(0.0, 1.0, 0.0);
			else upVector.set(-1.0, 0.0, 0.0);

			baseLabelTransform.setColumn(0, leftToRight, 0.0);
			baseLabelTransform.setColumn(1, upVector, 0.0);
			baseLabelTransform.setColumn(2, leftToRight * upVector, 0.0);
			baseLabelTransform.setColumn(3, 0.0, 0.0, 0.0, 1.0);

			// Plot tickmarks - maximum of 100 minor tickmarks between major lines
			int count = 0;
			delta = axisTickDelta_[axis] / (axisMinorTicks_[axis]+1);
			value = axisFirstTick_[axis];
			Vec3<double> u = axisCoordMin_[axis];
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

						// Create transformation matrix for text label
						A.setIdentity();
						// 1) Translate to anchor point of text
		// 				A.applyTranslation(textWidth*0.5, fontBaseHeight_*0.5, 0.0);
						// 2) Apply rotation in plane of text
						// 3) Translate to 'end of tick' position
						// 4) Perform axial rotation
						A.applyRotationAxis(leftToRight.x, leftToRight.y, leftToRight.z, axisLabelOrientation_[axis].x, false);
						A.applyTranslation(upVector * axisLabelOrientation_[axis].z);
						A.applyRotationAxis(0.0, 0.0, 1.0, axisLabelOrientation_[axis].y, false);
						A.applyScaling(labelScale_, labelScale_, labelScale_);

						ui.MainView->addAxisText(axis, s, u, A, labelScale_);
						
						count = 0;
					}
					else ui.MainView->addAxisLine(axis, u, u + tickDir*0.05);
				}
				u.add(axis, delta * (axisInverted_[axis] ? -axisStretch_[axis] : axisStretch_[axis]));
				value += delta;
				++count;
			}
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
			double z = transformValue(slice->z(), 2);
			// -- Is the transformed Z value within range?
			if ((z < limitMin_.z) || (z > limitMax_.z))
			{
				slice = axisInverted_.z ? slice->prev : slice->next;
				continue;
			}
			if (axisInverted_.z) z = (limitMax_.z - z) + limitMin_.z;
			if (axisLogarithmic_.z) z = log10(z);

			// Add new item to surfaceData_ array
			Slice* surfaceSlice = surfaceData_.add();
			surfaceSlice->setZ(z * axisStretch_.z);

			// Copy / interpolate arrays
			Array<double> array[2];
			if (interpolate_.x)
			{
				slice->data().interpolate(interpolateConstrained_.x);
				double x = limitMin_.x;
				while (x <= limitMax_.x)
				{
					array[0].add(x);
					array[1].add(slice->data().interpolated(x));
					x += interpolationStep_.x;
				}
			}
			else
			{
				array[0] = slice->data().arrayX();
				array[1] = slice->data().arrayY();
			}

			// X / Y
			for (int n=0; n<2; ++n)
			{
				// Apply pre-transform shift
				array[n] += preTransformShift_[n];

				// Apply transform
				switch (transformType_[n])
				{
					case (UChromaWindow::MultiplyTransform):
						array[n] *= transformValue_[n];
						break;
					case (UChromaWindow::DivideTransform):
						array[n] /= transformValue_[n];
						break;
					case (UChromaWindow::LogBase10Transform):
						array[n].takeLog();
						break;
					case (UChromaWindow::NaturalLogTransform):
						array[n].takeLn();
						break;
				}
				// Apply post-transform shift
				array[n] += postTransformShift_[n];
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
				surfaceSlice->data().addPoint(x, y);
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
				surfaceSlice->data().addPoint(x, y);
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

// Set slice axis
void UChromaWindow::setSliceAxis(int axis)
{
	sliceAxis_ = axis;
}

// Return current axis target for slice selection
int UChromaWindow::sliceAxis()
{
	return sliceAxis_;
}

// Calculate axis slice value at supplied mouse position
bool UChromaWindow::updateSliceValue(int mouseX, int mouseY)
{
	if (sliceAxis_ != -1)
	{
// 		printf("Test: min=%f, max=%f\n", axisMin_[0], axisMax_[0]);
// 		rMouseLast_.print();
// 		axisCoordMin_[0].print();
		// Project axis coordinates to get a screen-based yardstick
		Vec4<double> axmin = ui.MainView->modelToScreen(axisCoordMin_[sliceAxis_]);
		Vec4<double> axmax = ui.MainView->modelToScreen(axisCoordMax_[sliceAxis_]);
// 		axmin.print();
// 		axmax.print();

		// Calculate vectors between axis minimum and mouse position (AM) and axis maximum (AB)
		Vec3<double> ab(axmax.x - axmin.x, axmax.y - axmin.y, 0.0);
		Vec3<double> am(mouseX - axmin.x, mouseY - axmin.y, 0.0);
		Vec3<double> amNorm = am, abNorm = ab;
		double ratio = am.magnitude() / ab.magnitude();
		abNorm.normalise();
		amNorm.normalise();
		double angle = acos(abNorm.dp(amNorm)) ;
// 		printf("Angle = %f, %f\n", angle, angle * DEGRAD);
		
		// Calculate slice axis value - no need to account for inverted axes here, since this is accounted for by axmin and axmax
		sliceValue_ = abNorm.dp(amNorm)*ratio * (limitMax_[sliceAxis_] - limitMin_[sliceAxis_]) + limitMin_[sliceAxis_];
		
		// Clamp value to data range
		if (sliceValue_ < limitMin_[sliceAxis_]) sliceValue_ = limitMin_[sliceAxis_];
		else if (sliceValue_ > limitMax_[sliceAxis_]) sliceValue_ = limitMax_[sliceAxis_];
// 		printf("ACMAG = %f, X = %f\n", ratio, sliceAxisValue_[sliceAxis_]);

		// Account for log and inverted scales
// 		if (uChroma_->axisLogarithmic(sliceAxis_)) emit(sliceAxisValueChanged(sliceAxis_, pow(10.0, sliceValue_)));
// 		else emit(sliceAxisValueChanged(sliceAxis_, sliceValue_));
		return true;
	}
// 	else emit(sliceAxisValueChanged(-1, 0.0));
	return false;
}

// Return current axis value for slice selection
double UChromaWindow::sliceValue()
{
	return sliceValue_;
}