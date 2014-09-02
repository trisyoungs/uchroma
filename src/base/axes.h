/*
	*** Axes Description
	*** src/base/axes.h
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

#ifndef UCHROMA_AXES_H
#define UCHROMA_AXES_H

#include "render/primitive.h"
#include "render/textprimitive.h"
#include "templates/vector3.h"
#include "templates/vector4.h"
#include <QtCore/QString>

// Forward Declarations
class ViewPane;

// Axes
class Axes
{
	public:
	// Constructor / Destructor
	Axes(ViewPane& parent);
	~Axes();
	// Copy constructor
	Axes(const Axes& source);
	// Assignment operator
	void operator=(const Axes& source);


	/*
	 * Parent
	 */
	private:
	// Parent ViewPane in which these axes are displayed
	ViewPane& parent_;


	/*
	 * Definition
	 */
	private:
	// Data limits for surface generation
	Vec3<double> axisMin_, axisMax_;
	// Limiting values for axis limits (accounting for data, log scales etc.)
	Vec3<double> axisLimitMin_, axisLimitMax_;
	// Axis extreme coordinates
	Vec3<double> axisCoordMin_[3], axisCoordMax_[3];
	// Central coordinate of current axes
	Vec3<double> axesCoordCentre_;
	// Whether to invert axes
	Vec3<bool> axisInverted_;
	// Whether axes should be plotted as logarithms
	Vec3<bool> axisLogarithmic_;
	// Axis visibility
	Vec3<bool> axisVisible_;
	// Stretch factors to apply to axes
	Vec3<double> axisStretch_;
	// Whether axis position is fractional or absolute
	Vec3<bool> axisPositionIsFractional_;
	// Axis position in real or surface-space coordinates
	Vec3<double> axisPositionReal_[3];
	// Axis position in fractional axis coordinates
	Vec3<double> axisPositionFractional_[3];

	private:
	// Recalculate centre coordinate of axes
	void calculateAxesCoordCentre();
	// Clamp axis position and min/max to current limits
	void clampAxis(int axis);

	public:
	// Set minimum value for specified axis
	void setAxisMin(int axis, double value);
	// Return minimum value for specified axis
	double axisMin(int axis);
	// Set maximum value for specified axis
	void setAxisMax(int axis, double value);
	// Return maximum value for specified axis
	double axisMax(int axis);
	// Return axis range
	double axisRange(int axis);
	// Set axis to extreme limit
	void setAxisToLimit(int axis, bool minLim);
	// Set axis minimum limit for specified axis
	void setAxisLimitMin(int axis, double limit);
	// Return axis minimum limit for specified axis
	double axisLimitMin(int axis);
	// Set axis maximum limit for specified axis
	void setAxisLimitMax(int axis, double limit);
	// Return axis maximum limit for specified axis
	double axisLimitMax(int axis);
	// Return coordinate at centre of axes
	Vec3<double> axesCoordCentre();
	// Return coordinate at minimum of specified axis
	Vec3<double> axisCoordMin(int axis);
	// Return coordinate at maximum of specified axis
	Vec3<double> axisCoordMax(int axis);
	// Set whether axis is inverted
	void setAxisInverted(int axis, bool b);
	// Return whether axis is inverted
	bool axisInverted(int axis);
	// Set whether axis is logarithmic
	void setAxisLogarithmic(int axis, bool b);
	// Return whether axis is logarithmic
	bool axisLogarithmic(int axis);
	// Set whether axis is visible
	void setAxisVisible(int axis, bool b);
	// Return whether specified axis is visible
	bool axisVisible(int axis);
	// Set stretch factor for axis
	void setAxisStretch(int axis, double value);
	// Return stretch factor for axis
	double axisStretch(int axis);
	// Set fractional position flag for axis
	void setAxisPositionIsFractional(int axis, bool value);
	// Return fractional position flag for axis
	bool axisPositionIsFractional(int axis);
	// Set axis position (in real surface-space coordinates)
	void setAxisPositionReal(int axis, int dir, double value);
	// Set axis position to axis limit (in real surface-space coordinates)
	void setAxisPositionRealToLimit(int axis, int dir, bool minLim);
	// Return axis position (in real surface-space coordinates)
	Vec3<double> axisPositionReal(int axis);
	// Set axis position (in fractional axis coordinates)
	void setAxisPositionFractional(int axis, int dir, double value);
	// Return axis position (in fractional axis coordinates)
	Vec3<double> axisPositionFractional(int axis);


	/*
	 * Ticks / Labels
	 */
	private:
	// Axis tick direction
	Vec3<double> axisTickDirection_[3];
	// Position of first tick delta on axes
	Vec3<double> axisFirstTick_;
	// Tick delta for axes
	Vec3<double> axisTickDelta_;
	// Whether to calculate ticks automatically
	Vec3<bool> axisAutoTicks_;
	// Number of minor ticks in major tick intervals
	Vec3<int> axisMinorTicks_;
	// Orientation of axis labels (axial rot, in-plane rot, distance)
	Vec3<double> axisLabelOrientation_[3];
	// Axis laberl text anchor positions
	TextPrimitive::TextAnchor axisLabelAnchor_[3];
	// Axis titles
	QString axisTitle_[3];
	// Orientation of axis titles (axial rot, in-plane rot, distance, h-offset)
	Vec4<double> axisTitleOrientation_[3];
	// Axis title text anchor positions
	TextPrimitive::TextAnchor axisTitleAnchor_[3];

	public:
	// Set axis tick direction
	void setAxisTickDirection(int axis, int dir, double value);
	// Return axis tick direction for specified axis
	Vec3<double> axisTickDirection(int axis);
	// Set position of first tick delta on specified axis
	void setAxisFirstTick(int axis, double value);
	// Return position of first tick delta on specified axis
	double axisFirstTick(int axis);
	// Set tick delta for specified axis
	void setAxisTickDelta(int axis, double value);
	// Return tick delta for specified axis
	double axisTickDelta(int axis);
	// Set whether to calculate ticks automatically for specified axis
	void setAxisAutoTicks(int axis, bool b);
	// Return whether to calculate ticks automatically for specified axis
	bool axisAutoTicks(int axis);
	// Set number of minor ticks in major tick intervals for specified axis
	void setAxisMinorTicks(int axis, int value);
	// Return number of minor ticks in major tick intervals for specified axis
	int axisMinorTicks(int axis);
	// Recalculate tick deltas for specified axis
	void calculateTickDeltas(int axis);
	// Set orientation of labels for specified axis
	void setAxisLabelOrientation(int axis, int component, double value);
	// Return orientation of labels for specified axis
	Vec3<double> axisLabelOrientation(int axis);
	// Set axis label text anchor position for specified axis
	void setAxisLabelAnchor(int axis, TextPrimitive::TextAnchor anchor);
	// Return axis label text anchor position for specified axis
	TextPrimitive::TextAnchor axisLabelAnchor(int axis);
	// Set title for specified axis
	void setAxisTitle(int axis, QString title);
	// Return title for specified axis
	QString axisTitle(int axis);
	// Set orientation of titles for specified axis
	void setAxisTitleOrientation(int axis, int component, double value);
	// Return orientation of titles for specified axis
	Vec4<double> axisTitleOrientation(int axis);
	// Set axis title text anchor position for specified axis
	void setAxisTitleAnchor(int axis, TextPrimitive::TextAnchor anchor);
	// Return axis title text anchor position for specified axis
	TextPrimitive::TextAnchor axisTitleAnchor(int axis);


	/*
	 * GL
	 */
	private:
	// Y axis clip plane position
	GLdouble clipPlaneYMin_, clipPlaneYMax_;
	// Display primitives
	Primitive axisPrimitives_[3];
	// Axis text primitives
	TextPrimitiveList axisTextPrimitives_[3];
	// Whether axis primitives are valid
	bool primitivesValid_;

	private:
	// Add line to axis primitive
	void addPrimitiveLine(int axis, Vec3<double> v1, Vec3<double> v2);
	// Update primitives for axis
	void updateAxisPrimitives();

	public:
	// Return clip plane lower Y value
	GLdouble clipPlaneYMin();
	// Return clip plane upper Y value
	GLdouble clipPlaneYMax();
	// Flag primitives as invalid
	void setPrimitivesInvalid();
	// Return axis primitive for axis specified
	Primitive& axisPrimitive(int axis);
	// Return axis text primitive list for axis specified
	TextPrimitiveList& axisTextPrimitive(int axis);
};

#endif
