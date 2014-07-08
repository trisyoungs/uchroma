/*
	*** Axes Definitions 
	*** src/base/axes.cpp
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

#include "base/axes.h"
#include "base/viewpane.h"
#include "render/fontinstance.h"

// Constructor
Axes::Axes(ViewPane& parent) : parent_(parent)
{
	// Definition
	axisMin_.zero();
	axisMax_.set(10.0, 10.0, 10.0);
	axisInverted_.set(false, false, false);
	axisLogarithmic_.set(false, false, false);
	axisVisible_.set(true, true, true);
	axisPositionIsFractional_.set(true, true, true);
	axisPositionReal_[0].zero();
	axisPositionReal_[1].zero();
	axisPositionReal_[2].zero();
	axisPositionFractional_[0].zero();
	axisPositionFractional_[1].zero();
	axisPositionFractional_[2].zero();
	axisCoordMin_[0].zero();
	axisCoordMin_[1].zero();
	axisCoordMin_[2].zero();
	axisCoordMax_[0].zero();
	axisCoordMax_[1].zero();
	axisCoordMax_[2].zero();
	axisStretch_.set(1.0, 1.0, 1.0);

	// Ticks / Labels
	axisTickDirection_[0].set(0.0, -1.0, 0.0);
	axisTickDirection_[1].set(-1.0, 0.0, 0.0);
	axisTickDirection_[2].set(1.0, 0.0, 0.0);
	axisFirstTick_.zero();
	axisTickDelta_.set(1.0,1.0,1.0);
	axisAutoTicks_.set(true, true, true);
	axisMinorTicks_.set(1,1,1);
	axisLabelOrientation_[0].set(0.0, 0.0, 0.2);
	axisLabelOrientation_[1].set(0.0, 0.0, 0.2);
	axisLabelOrientation_[2].set(0.0, 180.0, 0.2);
	axisTitle_[0] = "X Axis";
	axisTitle_[1] = "Y Axis";
	axisTitle_[2] = "Z Axis";
	axisTitleOrientation_[0].set(0.0, 0.0, 0.2, 0.5);
	axisTitleOrientation_[1].set(0.0, 0.0, 0.2, 0.5);
	axisTitleOrientation_[2].set(0.0, 0.0, 0.2, 0.5);
	axisTitleAnchor_[0] = AnchorCentre;
	axisTitleAnchor_[1] = AnchorCentre;
	axisTitleAnchor_[2] = AnchorCentre;

	// GL
	for (int n=0; n<3; ++n)
	{
		axisPrimitives_[n].initialise(1020, 0, GL_LINES, false);
		axisPrimitives_[n].setNoInstances();
	}
	clipPlaneYMin_ = 0.0;
	clipPlaneYMax_ = 0.0;
	primitivesValid_ = false;
}

// Destructor
Axes::~Axes()
{
}

/*
 * Definition
 */

// Clamp axis position and min/max to current limits
void Axes::clampAxis(int axis)
{
	// Clamp current axis values if necessary
	if (axisMin_[axis] < axisLimitMin_[axis]) setAxisToLimit(axis, true);
	if (axisMax_[axis] > axisLimitMax_[axis]) setAxisToLimit(axis, false);

	// Clamp axis position point values if necessary
	for (int axis=0; axis < 3; ++axis)
	{
		if (axisPositionReal_[axis][(axis+1)%3] < axisLimitMin_[(axis+1)%3])
		{
			axisPositionReal_[axis].set((axis+1)%3, axisLimitMin_[(axis+1)%3]);
			primitivesValid_ = false;
		}
		if (axisPositionReal_[axis][(axis+2)%3] < axisLimitMin_[(axis+2)%3])
		{
			axisPositionReal_[axis].set((axis+2)%3, axisLimitMin_[(axis+2)%3]);
			primitivesValid_ = false;
		}
	}
}

// Set minimum value for specified axis
void Axes::setAxisMin(int axis, double value)
{
	axisMin_[axis] = value;

	primitivesValid_ = false;
}

// Return minimum value for specified axis
double Axes::axisMin(int axis)
{
	return axisMin_[axis];
}

// Set maximum value for specified axis
void Axes::setAxisMax(int axis, double value)
{
	axisMax_[axis] = value;

	primitivesValid_ = false;
}

// Return maximum value for specified axis
double Axes::axisMax(int axis)
{
	return axisMax_[axis];
}

// Return axis range
double Axes::axisRange(int axis)
{
	return axisMax_[axis] - axisMin_[axis];
}

// Set axis to extreme limit
void Axes::setAxisToLimit(int axis, bool minLim)
{
	if (minLim) axisMin_[axis] = axisLimitMin_[axis];
	else axisMax_[axis] = axisLimitMax_[axis];

	primitivesValid_ = false;
}

// Set axis minimum limit for specified axis
void Axes::setAxisLimitMin(int axis, double limit)
{
	axisLimitMin_[axis] = limit;
	
	clampAxis(axis);
}

// Return axis minimum limit for specified axis
double Axes::axisLimitMin(int axis)
{
	return axisLimitMin_[axis];
}

// Set axis maximum limit for specified axis
void Axes::setAxisLimitMax(int axis, double limit)
{
	axisLimitMax_[axis] = limit;
	
	clampAxis(axis);
}

// Return axis maximum limit for specified axis
double Axes::axisLimitMax(int axis)
{
	return axisLimitMax_[axis];
}

// Return coordinate at centre of current axes
Vec3<double> Axes::axesCoordCentre()
{
	return axesCoordCentre_;
}

// Return coordinate at minimum of specified axis
Vec3<double> Axes::axisCoordMin(int axis)
{
	return axisCoordMin_[axis];
}

// Return coordinate at maximum of specified axis
Vec3<double> Axes::axisCoordMax(int axis)
{
	return axisCoordMax_[axis];
}

// Set whether axis is inverted
void Axes::setAxisInverted(int axis, bool b)
{
	axisInverted_[axis] = b;

	primitivesValid_ = false;
}

// Return whether axis is inverted
bool Axes::axisInverted(int axis)
{
	return axisInverted_[axis];
}

// Set whether axis is logarithmic
void Axes::setAxisLogarithmic(int axis, bool b)
{
	axisLogarithmic_[axis] = b;

	// Update and clamp axis values according to data
	clampAxis(axis);

	primitivesValid_ = false;
}

// Return whether axis is logarithmic
bool Axes::axisLogarithmic(int axis)
{
	return axisLogarithmic_[axis];
}

// Set whether axis is visible
void Axes::setAxisVisible(int axis, bool b)
{
	axisVisible_[axis] = b;
}

// Return whether specified axis is visible
bool Axes::axisVisible(int axis)
{
	return axisVisible_[axis];
}

// Set stretch factor for axis
void Axes::setAxisStretch(int axis, double value)
{
	axisStretch_[axis] = value;

	primitivesValid_ = false;
}

// Return stretch factor for axis
double Axes::axisStretch(int axis)
{
	return axisStretch_[axis];
}

// Set fractional position flag for axis
void Axes::setAxisPositionIsFractional(int axis, bool b)
{
	axisPositionIsFractional_[axis] = b;

	primitivesValid_ = false;
}

// Return fractional position flag for axis
bool Axes::axisPositionIsFractional(int axis)
{
	return axisPositionIsFractional_[axis];
}

// Set axis position (in real surface-space coordinates)
void Axes::setAxisPositionReal(int axis, int dir, double value)
{
	// Clamp range to limits
	if (value > axisLimitMax_[dir]) value = axisLimitMax_[dir];
	else if (value < axisLimitMin_[dir]) value = axisLimitMin_[dir];
	axisPositionReal_[axis].set(dir, value);

	primitivesValid_ = false;
}

// Set axis position to axis limit (in real surface-space coordinates)
void Axes::setAxisPositionRealToLimit(int axis, int dir, bool minLim)
{
	axisPositionReal_[axis].set(dir, minLim ? axisLimitMin_[dir] : axisLimitMax_[dir]);

	primitivesValid_ = false;
}

// Return axis position (in real surface-space coordinates)
Vec3<double> Axes::axisPositionReal(int axis)
{
	return axisPositionReal_[axis];
}

// Set axis position (in fractional axis coordinates)
void Axes::setAxisPositionFractional(int axis, int dir, double value)
{
	// Clamp range to limits
	if (value > 1.0) value = 1.0;
	else if (value < 0.0) value = 0.0;
	axisPositionFractional_[axis].set(dir, value);

	primitivesValid_ = false;
}

// Return axis position (in fractional axis coordinates)
Vec3<double> Axes::axisPositionFractional(int axis)
{
	return axisPositionFractional_[axis];
}

// Set axis tick direction
void Axes::setAxisTickDirection(int axis, int dir, double value)
{
	axisTickDirection_[axis].set(dir, value);

	primitivesValid_ = false;
}

// Return axis tick direction
Vec3<double> Axes::axisTickDirection(int axis)
{
	return axisTickDirection_[axis];
}

// Set position of first tick delta on axes
void Axes::setAxisFirstTick(int axis, double value)
{
	axisFirstTick_[axis] = value;

	primitivesValid_ = false;

}

// Return position of first tick delta on axes
double Axes::axisFirstTick(int axis)
{
	return axisFirstTick_[axis];
}

// Set tick delta for axes
void Axes::setAxisTickDelta(int axis, double value)
{
	axisTickDelta_[axis] = value;

	primitivesValid_ = false;
}

// Return tick delta for axes
double Axes::axisTickDelta(int axis)
{
	return axisTickDelta_[axis];
}

// Set whether to calculate ticks automatically
void Axes::setAxisAutoTicks(int axis, bool b)
{
	axisAutoTicks_[axis] = b;

	primitivesValid_ = false;
}

// Return whether to calculate ticks automatically
bool Axes::axisAutoTicks(int axis)
{
	return axisAutoTicks_[axis];
}

// Set number of minor ticks in major tick intervals
void Axes::setAxisMinorTicks(int axis, int value)
{
	axisMinorTicks_[axis] = value;

	primitivesValid_ = false;
}

// Return number of minor ticks in major tick intervals
int Axes::axisMinorTicks(int axis)
{
	return axisMinorTicks_[axis];
}

// Recalculate tick deltas for specified axis
void Axes::calculateTickDeltas(int axis)
{
	const int nBaseValues = 5, maxIterations = 10, maxTicks = 10;
	int power = 1, baseValues[nBaseValues] = { 1, 2, 3, 4, 5 }, baseValueIndex = 0, nTicks, iteration, minTicks = maxTicks/2;
	double clampedStartValue;

	baseValueIndex = 0;
	power = int(log10((axisMax_[axis]-axisMin_[axis]) / maxTicks) - 1);
	iteration = 0;

	if ((axisMax_[axis] - axisMin_[axis]) > 1.0e-10)
	{
		do
		{
			// Calculate current tickDelta
			axisTickDelta_[axis] = baseValues[baseValueIndex]*pow(10.0,power);

			// Get first tickmark value
			axisFirstTick_[axis] = int(axisMin_[axis] / axisTickDelta_[axis]) * axisTickDelta_[axis];
			clampedStartValue = (axisFirstTick_[axis] < axisMin_[axis] ? axisFirstTick_[axis] + axisTickDelta_[axis] : axisFirstTick_[axis]);

			// How many ticks now fit between the firstTick and max value?
			// Add 1 to get total ticks for this delta (i.e. including firstTick)
			nTicks = int((axisMax_[axis]-axisMin_[axis]) / axisTickDelta_[axis]);
			++nTicks;

			// Check n...
			if (nTicks > maxTicks)
			{
				++baseValueIndex;
				if (baseValueIndex == nBaseValues) ++power;
				baseValueIndex = baseValueIndex%nBaseValues;
			}
			else if (nTicks < minTicks)
			{
				--baseValueIndex;
				if (baseValueIndex == -1)
				{
					--power;
					baseValueIndex += nBaseValues;
				}
			}

			++iteration;
			if (iteration == maxIterations) break;

		} while ((nTicks > maxTicks) || (nTicks < minTicks));
	}
	else
	{
		axisFirstTick_[axis] = axisMin_[axis];
		axisTickDelta_[axis] = 1.0;
	}
}

// Set orientation of labels for specified axis
void Axes::setAxisLabelOrientation(int axis, int component, double value)
{
	axisLabelOrientation_[axis].set(component, value);

	primitivesValid_ = false;
}

// Return orientation of labels for specified axis
Vec3<double> Axes::axisLabelOrientation(int axis)
{
	return axisLabelOrientation_[axis];
}

// Set title for specified axis
void Axes::setAxisTitle(int axis, QString title)
{
	axisTitle_[axis] = title;

	primitivesValid_ = false;
}

// Return title for specified axis
QString Axes::axisTitle(int axis)
{
	return axisTitle_[axis];
}

// Set orientation of titles for specified axis
void Axes::setAxisTitleOrientation(int axis, int component, double value)
{
	axisTitleOrientation_[axis].set(component, value);

	primitivesValid_ = false;
}

// Return orientation of titles for specified axis
Vec4<double> Axes::axisTitleOrientation(int axis)
{
	return axisTitleOrientation_[axis];
}

// Set axis title text anchor position for specified axis
void Axes::setAxisTitleAnchor(int axis, Axes::AxisAnchor anchor)
{
	axisTitleAnchor_[axis] = anchor;

	primitivesValid_ = false;
}

// Return axis title text anchor position for specified axis
Axes::AxisAnchor Axes::axisTitleAnchor(int axis)
{
	return axisTitleAnchor_[axis];
}

/*
 * GL
 */

// Add line to axis primitive
void Axes::addPrimitiveLine(int axis, Vec3<double> v1, Vec3<double> v2)
{
	axisPrimitives_[axis].defineVertex(v1.x, v1.y, v1.z, 1.0, 0.0, 0.0);
	axisPrimitives_[axis].defineVertex(v2.x, v2.y, v2.z, 1.0, 0.0, 0.0);
}

// Add entry to axis text primitive
void Axes::addPrimitiveText(int axis, QString text, Vec3<double> origin, Matrix transform, Axes::AxisAnchor anchor)
{
	FTBBox boundingBox = FontInstance::boundingBox(text);
	double textWidth = fabs(boundingBox.Upper().X() - boundingBox.Lower().X());

	// Construct final centre coordinate based on alignment requested
	Vec3<double> textCentre(0.0, -FontInstance::fontBaseHeight()*0.5, 0.0);
	if (anchor == Axes::AnchorLeft) textCentre.x = 0.0;
	else if (anchor == Axes::AnchorRight) textCentre.x = -textWidth;
	else textCentre.x = -textWidth*0.5;

	axisTextPrimitives_[axis].add(text, origin, textCentre, transform);
}

// Update axes primitives
void Axes::updateAxisPrimitives()
{
	// Check whether we need to regenerate the axes primitives / data
	if (primitivesValid_) return;

	QString s;
	FTBBox boundingBox;
	Vec3<double> centre;
	double dpX, dpY, textWidth, delta, value, clipPlaneDelta = 0.0001;
	int n;
	Vec3<double> u, tickDir;
	Matrix labelTransform, titleTransform;

	// Determine central coordinate within current axes and the Y clip limits
	for (int n=0; n<3; ++n)
	{
		if (axisLogarithmic_[n]) axesCoordCentre_[n] = (axisInverted_[n] ? log10(axisMax_[n]/axisMin_[n]) : log10(axisMax_[n]*axisMin_[n])) * 0.5 * axisStretch_[n];
		else axesCoordCentre_[n] = (axisMax_[n]+axisMin_[n]) * 0.5 * axisStretch_[n];
	}

	// Set Y clip
	if (axisLogarithmic_.y)
	{
		clipPlaneYMin_ = (log10(axisMin_.y) * axisStretch_.y) - clipPlaneDelta;
		clipPlaneYMax_ = (log10(axisMax_.y) * axisStretch_.y) + clipPlaneDelta;
	}
	else
	{
		clipPlaneYMin_ = (axisMin_.y * axisStretch_.y) - clipPlaneDelta;
		clipPlaneYMax_ = (axisMax_.y * axisStretch_.y) + clipPlaneDelta;
	}
	
	// Set basic extreme coordinates for axes - actual limits on axes will be set in following loop
	for (int axis=0; axis < 3; ++axis)
	{
		double axisPosition;
		for (int n=0; n<3; ++n)
		{
			// Get axis position
			axisPosition = (axisPositionIsFractional_[axis] ? axisPositionFractional_[axis][n]*(axisMax_[n]-axisMin_[n])+axisMin_[n] : axisPositionReal_[axis][n]);
			if (axisLogarithmic_[n]) axisCoordMin_[axis].set(n, (axisInverted_[n] ? log10(axisMax_[n]/axisPosition) : log10(axisPosition)) * axisStretch_[n]);
			else axisCoordMin_[axis].set(n, (axisInverted_[n] ? axisMax_[n] - axisPosition : axisPosition) * axisStretch_[n]);
		}
		axisCoordMax_[axis] = axisCoordMin_[axis];
	}

	// Construct axes
	for (int axis = 0; axis < 3; ++axis)
	{
		// Clear old axis primitives
		axisPrimitives_[axis].forgetAll();
		axisTextPrimitives_[axis].forgetAll();

		// Normalise tickDirection
		tickDir = axisTickDirection_[axis];
		tickDir.normalise();
		tickDir *= parent_.labelScale();

		// Create tick label transformation matrix
		labelTransform.setIdentity();
		// 1) Apply axial rotation along X axis (left-to-right direction)
		labelTransform.applyRotationX(axisLabelOrientation_[axis].x);
		// 2) Translate to 'end of tick' position
		labelTransform.applyTranslation(tickDir * axisLabelOrientation_[axis].z);
		// 3) Perform in-plane rotation
		labelTransform.applyRotationZ(axisLabelOrientation_[axis].y);
		labelTransform.applyScaling(parent_.labelScale(), parent_.labelScale(), parent_.labelScale());

		// Create axis title transformation matrix
		titleTransform.setIdentity();
		// 1) Apply axial rotation along X axis (left-to-right direction)
		titleTransform.applyRotationX(axisTitleOrientation_[axis].x);
		// 2) Translate to 'end of tick' position
		titleTransform.applyTranslation(tickDir * axisTitleOrientation_[axis].z);
		// 3) Perform in-plane rotation
		titleTransform.applyRotationZ(axisTitleOrientation_[axis].y);
		titleTransform.applyScaling(parent_.titleScale(), parent_.titleScale(), parent_.titleScale());

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
			addPrimitiveLine(axis, axisCoordMin_[axis], axisCoordMax_[axis]);

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
					addPrimitiveLine(axis, u, u+tickDir*(count == 0 ? 1.0 : 0.5));

					// Tick label
					if (count == 0)
					{
						// Create text, accounting for zero-roundoff, and add a text primitive
						if (fabs(value) < pow(10,power-5)) s = "0";
						else s = QString::number(value);

						addPrimitiveText(axis, s, u, labelTransform);
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
			addPrimitiveText(axis, axisTitle_[axis], u, titleTransform, axisTitleAnchor_[axis]);
		}
		else
		{
			// Set axis min/max coordinates
			axisCoordMin_[axis].set(axis, (axisInverted_[axis] ? axisMax_[axis] : axisMin_[axis]) * axisStretch_[axis]);
			axisCoordMax_[axis].set(axis, (axisInverted_[axis] ? axisMin_[axis] : axisMax_[axis]) * axisStretch_[axis]);
		
			// Calculate autoticks if necessary
			if (axisAutoTicks_[axis]) calculateTickDeltas(axis);

			// Draw a line from min to max limits, passing through the defined axisPosition
			addPrimitiveLine(axis, axisCoordMin_[axis], axisCoordMax_[axis]);

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
						addPrimitiveLine(axis, u, u + tickDir*0.1);
						
						// Get formatted label text, acounting for roundoff error
						if (fabs(value) < axisTickDelta_[axis]*1.0e-10) s = "0";
						else s = QString::number(value);

						addPrimitiveText(axis, s, u, labelTransform);
						
						count = 0;
					}
					else addPrimitiveLine(axis, u, u + tickDir*0.05);
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
			addPrimitiveText(axis, axisTitle_[axis], u, titleTransform, axisTitleAnchor_[axis]);
		}
	}

	primitivesValid_ = true;

	// The axes have changed, so all associated collection data also neds to be regenerated
	parent_.flagCollectionDataInvalid();
}

// Return clip plane lower Y value
GLdouble Axes::clipPlaneYMin()
{
	return clipPlaneYMin_;
}

// Return clip plane upper Y value
GLdouble Axes::clipPlaneYMax()
{
	return clipPlaneYMax_;
}

// Flag primitives as invalid
void Axes::setPrimitivesInvalid()
{
	primitivesValid_ = false;
}

// Return axis primitive for axis specified
Primitive& Axes::axisPrimitive(int axis)
{
	// Make sure primitives are up to date first...
	updateAxisPrimitives();

	return axisPrimitives_[axis];
}

// Return axis text primitive list for axis specified
TextPrimitiveList& Axes::axisTextPrimitive(int axis)
{
	// Make sure primitives are up to date first...
	updateAxisPrimitives();

	return axisTextPrimitives_[axis];
}