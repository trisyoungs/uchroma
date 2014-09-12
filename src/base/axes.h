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

#include "base/numberformat.h"
#include "render/primitive.h"
#include "render/textprimitive.h"
#include "render/linestyle.h"
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
	Vec3<double> min_, max_;
	// Limiting values for axis limits (accounting for data, log scales etc.)
	Vec3<double> limitMin_, limitMax_;
	// Axis extreme coordinates
	Vec3<double> coordMin_[3], coordMax_[3];
	// Central coordinate of current axes
	Vec3<double> coordCentre_;
	// Whether to invert axes
	Vec3<bool> inverted_;
	// Whether axes should be plotted as logarithms
	Vec3<bool> logarithmic_;
	// Axis visibility
	Vec3<bool> visible_;
	// Stretch factors to apply to axes
	Vec3<double> stretch_;
	// Whether axis position is fractional or absolute
	Vec3<bool> positionIsFractional_;
	// Axis position in real or surface-space coordinates
	Vec3<double> positionReal_[3];
	// Axis position in fractional axis coordinates
	Vec3<double> positionFractional_[3];

	private:
	// Recalculate centre coordinate of axes
	void calculateCoordCentre();
	// Clamp axis position and min/max to current limits
	void clamp(int axis);

	public:
	// Set minimum value for specified axis
	void setMin(int axis, double value);
	// Return minimum value for specified axis
	double min(int axis);
	// Set maximum value for specified axis
	void setMax(int axis, double value);
	// Return maximum value for specified axis
	double max(int axis);
	// Return axis range
	double range(int axis);
	// Set axis to extreme limit
	void setToLimit(int axis, bool minLim);
	// Set axis minimum limit for specified axis
	void setLimitMin(int axis, double limit);
	// Return axis minimum limit for specified axis
	double limitMin(int axis);
	// Set axis maximum limit for specified axis
	void setLimitMax(int axis, double limit);
	// Return axis maximum limit for specified axis
	double limitMax(int axis);
	// Return coordinate at centre of axes
	Vec3<double> coordCentre();
	// Return coordinate at minimum of specified axis
	Vec3<double> coordMin(int axis);
	// Return coordinate at maximum of specified axis
	Vec3<double> coordMax(int axis);
	// Set whether axis is inverted
	void setInverted(int axis, bool b);
	// Return whether axis is inverted
	bool inverted(int axis);
	// Set whether axis is logarithmic
	void setLogarithmic(int axis, bool b);
	// Return whether axis is logarithmic
	bool logarithmic(int axis);
	// Set whether axis is visible
	void setVisible(int axis, bool b);
	// Return whether specified axis is visible
	bool visible(int axis);
	// Set stretch factor for axis
	void setStretch(int axis, double value);
	// Return stretch factor for axis
	double stretch(int axis);
	// Set fractional position flag for axis
	void setPositionIsFractional(int axis, bool value);
	// Return fractional position flag for axis
	bool positionIsFractional(int axis);
	// Set axis position (in real surface-space coordinates)
	void setPositionReal(int axis, int dir, double value);
	// Set axis position to axis limit (in real surface-space coordinates)
	void setPositionRealToLimit(int axis, int dir, bool minLim);
	// Return axis position (in real surface-space coordinates)
	Vec3<double> positionReal(int axis);
	// Set axis position (in fractional axis coordinates)
	void setPositionFractional(int axis, int dir, double value);
	// Return axis position (in fractional axis coordinates)
	Vec3<double> positionFractional(int axis);


	/*
	 * Ticks
	 */
	private:
	// Axis tick direction
	Vec3<double> tickDirection_[3];
	// Axis tick size (relative to font size)
	Vec3<double> tickSize_;
	// Position of first tick delta on axes
	Vec3<double> tickFirst_;
	// Tick delta for axes
	Vec3<double> tickDelta_;
	// Whether to calculate ticks automatically
	Vec3<bool> autoTicks_;
	// Number of minor ticks in major tick intervals
	Vec3<int> minorTicks_;

	private:
	// Recalculate tick deltas for specified axis
	void calculateTickDeltas(int axis);

	public:
	// Set axis tick direction
	void setTickDirection(int axis, int dir, double value);
	// Return axis tick direction for specified axis
	Vec3<double> tickDirection(int axis);
	// Set axis tick size (relative to font size)
	void setTickSize(int axis, double size);
	// Return axis tick size (relative to font size)
	double tickSize(int axis);
	// Set position of first tick delta on specified axis
	void setFirstTick(int axis, double value);
	// Return position of first tick delta on specified axis
	double tickFirst(int axis);
	// Set tick delta for specified axis
	void setTickDelta(int axis, double value);
	// Return tick delta for specified axis
	double tickDelta(int axis);
	// Set whether to calculate ticks automatically for specified axis
	void setAutoTicks(int axis, bool b);
	// Return whether to calculate ticks automatically for specified axis
	bool autoTicks(int axis);
	// Set number of minor ticks in major tick intervals for specified axis
	void setMinorTicks(int axis, int value);
	// Return number of minor ticks in major tick intervals for specified axis
	int minorTicks(int axis);


	/*
	 * Labels
	 */
	private:
	// Number formats for labels
	NumberFormat numberFormat_[3];
	// Orientation of axis labels (axial rot, in-plane rot, distance)
	Vec3<double> labelOrientation_[3];
	// Axis label text anchor positions
	TextPrimitive::TextAnchor labelAnchor_[3];
	// Axis titles
	QString title_[3];
	// Orientation of axis titles (axial rot, in-plane rot, distance, h-offset)
	Vec4<double> titleOrientation_[3];
	// Axis title text anchor positions
	TextPrimitive::TextAnchor titleAnchor_[3];

	public:
	// Return number format for specified axis
	NumberFormat& numberFormat(int axis);
	// Set orientation of labels for specified axis
	void setLabelOrientation(int axis, int component, double value);
	// Return orientation of labels for specified axis
	Vec3<double> labelOrientation(int axis);
	// Set axis label text anchor position for specified axis
	void setLabelAnchor(int axis, TextPrimitive::TextAnchor anchor);
	// Return axis label text anchor position for specified axis
	TextPrimitive::TextAnchor labelAnchor(int axis);
	// Set title for specified axis
	void setTitle(int axis, QString title);
	// Return title for specified axis
	QString title(int axis);
	// Set orientation of titles for specified axis
	void setTitleOrientation(int axis, int component, double value);
	// Return orientation of titles for specified axis
	Vec4<double> titleOrientation(int axis);
	// Set axis title text anchor position for specified axis
	void setTitleAnchor(int axis, TextPrimitive::TextAnchor anchor);
	// Return axis title text anchor position for specified axis
	TextPrimitive::TextAnchor titleAnchor(int axis);


	/*
	 * GridLines
	 */
	private:
	// Whether gridlines cover entire volume or just at axis lines
	Vec3<bool> gridLinesFull_;
	// Whether gridLines at major tick intervals are active
	Vec3<bool> gridLinesMajor_;
	// Whether gridLines at minor tick intervals are active
	Vec3<bool> gridLinesMinor_;

	public:
	// Set whether gridlines cover entire volume or just at axis lines
	void setGridLinesFull(int axis, bool b);
	// Return whether gridlines cover entire volume or just at axis lines
	bool gridLinesFull(int axis);
	// Set whether gridLines at major tick intervals are active for specified axis
	void setGridLinesMajor(int axis, bool on);
	// Return whether gridLines at major tick intervals are active for specified axis
	bool gridLinesMajor(int axis);
	// Set whether gridLines at minor tick intervals are active for specified axis
	void setGridLinesMinor(int axis, bool on);
	// Return whether gridLines at minor tick intervals are active for specified axis
	bool gridLinesMinor(int axis);


	/*
	 * GL
	 */
	private:
	// Y axis clip plane position
	GLdouble clipPlaneYMin_, clipPlaneYMax_;
	// Display primitives
	Primitive axisPrimitives_[3];
	// Axis text primitives
	TextPrimitiveList labelPrimitives_[3];
	// Axis title primitives
	TextPrimitiveList titlePrimitives_[3];
	// GridLine primitives
	Primitive gridLineMajorPrimitives_[3], gridLineMinorPrimitives_[3];
	// GridLine styles
	LineStyle gridLineMajorStyle_[3], gridLineMinorStyle_[3];
	// Whether axis primitives are valid
	bool primitivesValid_;

	private:
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
	// Return axis label primitive list for axis specified
	TextPrimitiveList& labelPrimitive(int axis);
	// Return axis title primitive list for axis specified
	TextPrimitiveList& titlePrimitive(int axis);
	// Return minor gridline primitive for axis specified
	Primitive& gridLineMajorPrimitive(int axis);
	// Return major gridline primitive for axis specified
	Primitive& gridLineMinorPrimitive(int axis);
	// Return major GridLine style
	LineStyle& gridLineMajorStyle(int axis);
	// Return minor GridLine style
	LineStyle& gridLineMinorStyle(int axis);
};

#endif
