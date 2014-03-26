/*
	*** Data Collection
	*** src/base/collection.h
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

#ifndef UCHROMA_COLLECTION_H
#define UCHROMA_COLLECTION_H

#include "base/slice.h"
#include "base/transformer.h"
#include "base/colourscale.h"
#include "gui/viewer_primitive.h"

// Forward Declarations
/* None */

class Collection : public ListItem<Collection>
{
	public:
	// Constructor
	Collection();
	// Destructor
	~Collection();


	/*
	 * Data
	 */
	private:
	// Title
	QString title_;
	// List of slices
	List<Slice> slices_;
	// Root directory for datafiles
	QDir dataFileDirectory_;

	public:
	// Set title
	void setTitle(QString title);
	// Return title
	QString title();
	// Add slice
	Slice* addSlice();
	// Add slice at specified z
	Slice* addSlice(double z);
	// Add slice, copying from supplied data
	void addSlice(Slice* source);
	// Return number of slices
	int nSlices();
	// Remove slice
	void removeSlice(Slice* slice);
	// Set z value of specified slice
	void setSliceZ(Slice* target, double z);
	// Return first slice in list
	Slice* slices();
	// Return last slice in list
	Slice* lastSlice();
	// Return nth slice in list
	Slice* slice(int index);
	// Return number of slices with no data present
	int nEmptySlices();
	// Set root directory for datafiles
	void setDataFileDirectory(QDir directory);
	// Return root directory for datafiles
	QDir dataFileDirectory();
	// Load data for specified slice
	bool loadSliceData(Slice* slice);
	// Reload data for all slices
	int loadAllSlices();


	/*
	 * Transform
	 */
	private:
	// Extreme values of raw data
	Vec3<double> dataMin_, dataMax_;
	// Extreme values of transformed data 
	Vec3<double> transformMin_, transformMax_;
	// Transform for data
	Transformer transforms_[3];
	// Interpolation flags
	Vec3<bool> interpolate_, interpolateConstrained_;
	// Interpolation step sizes
	Vec3<double> interpolationStep_;

	public:
	// Return data minima
	Vec3<double> dataMin();
	// Return data maxima
	Vec3<double> dataMax();
	// Return transformed data minima
	Vec3<double> transformMin();
	// Return transformed data maxima
	Vec3<double> transformMax();
	// Set transform equation for data
	void setTransformEquation(int axis, QString transformEquation);
	// Return transform equation for data
	QString transformEquation(int axis);
	// Return whether specified transform equation is valid
	bool transformEquationValid(int axis);
	// Set whether specified transform is enabled
	void setTransformEnabled(int axis, bool enabled);
	// Return whether specified transform is enabled
	bool transformEnabled(int axis);
	// Recalculate data limits
	void calculateDataLimits();
	// Update data transforms and calculate transform limits
	void updateDataTransforms();
	// Set whether interpolation is enabled
	void setInterpolate(int axis, bool enabled);
	// Return whether interpolation is enabled
	bool interpolate(int axis);
	// Set whether interpolation is constrained
	void setInterpolateConstrained(int axis, bool enabled);
	// Return whether interpolation is constrained
	bool interpolateConstrained(int axis);
	// Set interpolation step size
	void setInterpolationStep(int axis, double step);
	// Return interpolation step size
	double interpolationStep(int axis);


	/*
	 * Colours
	 */
	public:
	// Available colourscale sources
	enum ColourSource { SingleColourSource, RGBGradientSource, HSVGradientSource, CustomGradientSource, nColourSources };
	// Convert text string to ColourSource
	static ColourSource colourSource(const char* s);
	// Convert ColourSource to text string
	static const char* colourSource(ColourSource kwd);
	// Alpha control options
	enum AlphaControl { OwnAlpha, FixedAlpha, nAlphaControls };
	// Convert text string to AlphaControl
	static AlphaControl alphaControl(const char* s);
	// Convert AlphaControl to text string
	static const char* alphaControl(AlphaControl kwd);

	private:
	// Current colourscale source to use
	ColourSource colourSource_;
	// Points for SingleColour, RGBGradient and HSVGradient sources
	ColourScalePoint colourSinglePoint_;
	ColourScalePoint colourRGBGradientAPoint_, colourRGBGradientBPoint_;
	ColourScalePoint colourHSVGradientAPoint_, colourHSVGradientBPoint_;
	// Actual ColourScale used by surface
	ColourScale colourScale_;
	// Custom ColourScale source
	ColourScale customColourScale_;
	// Current alpha control
	AlphaControl alphaControl_;
	// Fixed alpha value (for FixedAlpha option)
	int fixedAlpha_;
	// Flag indicating that colourscale needs to be regenerated
	bool regenerateColourScale_;

	public:
	// Update colour scale
	void updateColourScale();
	// Set colourscale source to use
	void setColourSource(ColourSource source);
	// Return colourscale source to use
	ColourSource colourSource();
	// Set colourscale point colour
	void setColourScalePoint(ColourSource source, QColor colour, double value = 0.0, int index = -1);
	// Return colourscale point specified
	const ColourScalePoint* colourScalePoint(ColourSource source, int index = -1);
	// Return colour of point specified
	QColor colourScalePointColour(ColourSource source, int index = -1);
	// Return value of point specified
	double colourScalePointValue(ColourSource source, int index = -1);
	// Add empty point to end of custom colourscale
	void addCustomColourScalePoint();
	// Add point to custom colourscale
	void addCustomColourScalePoint(double value, QColor colour);
	// Return number of custom colourscale points
	int nCustomColourScalePoints();
	// Return first custom colourscale point in list
	ColourScalePoint* customColourScalePoints();
	// Return custom colourscale point with index specified
	ColourScalePoint* customColourScalePoint(int id);
	// Remove specified colourscale point
	void removeCustomColourScalePoint(ColourScalePoint* point);
	// Set alpha control
	void setAlphaControl(AlphaControl alpha);
	// Return current alpha control
	AlphaControl alphaControl();
	// Set fixed alpha value
	void setFixedAlpha(double alpha);
	// Return fixed alpha value
	double fixedAlpha();
	// Return current colourscale
	const ColourScale& colourScale();
	// Return whether colourscale needs to be updated
	bool regenerateColourScale();


	/*
	 * Surface
	 */
	public:
	// Surface types enum
	enum SurfaceStyle { LineSurface, FullSurface, GridSurface, nSurfaceStyles };
	// Convert text string to SurfaceStyle
	static SurfaceStyle surfaceStyle(const char* s);
	// Convert SurfaceStyle to text string
	static const char* surfaceStyle(SurfaceStyle kwd);

	private:
	// Transformed data to display
	List<Data2D> surfaceData_;
	// Display style of data
	SurfaceStyle surfaceStyle_;
	// Flag indicating that display data needs to be regenerated
	bool regenerateDisplayData_;
	// GL primitive containing surface
	Primitive surfacePrimitive_;
	// Flag indicating that display data needs to be regenerated
	bool regenerateSurfacePrimitive_;

	public:
	// Return transformed data to display
	List<Data2D>& surfaceData();
	// Set display style of data
	void setSurfaceStyle(SurfaceStyle style);
	// R%eturn display style of data
	SurfaceStyle surfaceStyle();
	// Manually set the flag to force regeneration of surface data
	void setRegenerateDisplayData();
	// Flag that the primitive has been updated
	void setSurfacePrimitiveUpdated();
	// Return whether primitive needs to be updated
	bool regenerateSurfacePrimitive();
	// Return surface primitive
	Primitive& surfacePrimitive();
	// Update display data and surface if necessary
	void updateSurfaceData(Vec3<double> axisMin, Vec3<double> axisMax, Vec3<bool> axisInverted, Vec3<bool> axisLogarithmic, Vec3<double> axisStretch);
};

#endif