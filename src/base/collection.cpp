/*
	*** Data Collection
	*** src/base/collection.cpp
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

#include "base/collection.h"

// Constructor
Collection::Collection() : ListItem<Collection>()
{
	// Set variable defaults
	slices_.clear();
	dataFileDirectory_ = getenv("PWD");
	title_ = "Empty Collection";

	// Transform
	dataMin_.zero();
	dataMax_.set(10.0, 10.0, 10.0);
	transformMin_.zero();
	transformMax_.set(10.0, 10.0, 10.0);
	transformMinPositive_.set(0.1, 0.1, 0.1);
	transformMaxPositive_.set(10.0, 10.0, 10.0);
	transforms_[0].setEnabled(false);
	transforms_[1].setEnabled(false);
	transforms_[2].setEnabled(false);
	transforms_[0].setEquation("x");
	transforms_[1].setEquation("y");
	transforms_[2].setEquation("z");
	interpolate_.set(false, false, false);
	interpolateConstrained_.set(false, false, false);
	interpolationStep_.set(1.0, 1.0, 1.0);

	// Colours
	colourScale_.clear();
	colourSinglePoint_.set(0.0, QColor(255,255,255));
	colourRGBGradientAPoint_.set(0.0, QColor(255,255,255));
	colourRGBGradientBPoint_.set(1.0, QColor(0,0,255));
	colourHSVGradientAPoint_.set(0.0, QColor(255,0,0));
	colourHSVGradientBPoint_.set(1.0, QColor(100,40,255));
	customColourScale_.clear();
	colourSource_ = SingleColourSource;
	alphaControl_ = Collection::OwnAlpha;
	fixedAlpha_ = 128;
	regenerateColourScale_ = true;

	// Surface
	surfaceData_.clear();
	surfacePrimitive_.setColourData(true);
	regenerateDisplayData_ = true;
	regenerateSurfacePrimitive_ = true;
}

// Destructor
Collection::~Collection()
{
}

/*
 * Data
 */

// Set title
void Collection::setTitle(QString title)
{
	title_ = title;
}

// Return title
QString Collection::title()
{
	return title_;
}

// Add slice
Slice* Collection::addSlice()
{
	// Create new slice
	Slice* slice = slices_.add();

	regenerateDisplayData_ = true;
	return slice;
}

// Add slice at specified z
Slice* Collection::addSlice(double z)
{
	Slice* slice = slices_.add();
	setSliceZ(slice, z);
	return slice;
}

// Add slice, copying from supplied data
void Collection::addSlice(Slice* source)
{
	// Create new slice
	Slice* slice = slices_.add();
	(*slice) = (*source);

	regenerateDisplayData_ = true;
}

// Return number of slices
int Collection::nSlices()
{
	return slices_.nItems();
}

// Remove slice
void Collection::removeSlice(Slice* slice)
{
	slices_.remove(slice);
	regenerateDisplayData_ = true;
}

// Set z value of specified slice
void Collection::setSliceZ(Slice* target, double z)
{
	bool minBad = true, maxBad = true;
	int dummy = 0;

	// Check that this Slice is owned by the collection
	if (!slices_.contains(target))
	{
		msg.print("Internal Error : Tried to set the Z value of a slice using the wrong collection.\n");
		return;
	}
	target->data().setZ(z);

	// Adjust position of the slice in the list if necessary
	do
	{
		// Shift item if necessary
		if (target->prev && (target->prev->data().z() > target->data().z()))
		{
			// Shift specified target up the list
			slices_.shiftUp(target);
			regenerateDisplayData_ = true;
			minBad = (target->prev ? (target->prev->data().z() > target->data().z()) : false);
		}
		else minBad = false;
		if (target->next && (target->next->data().z() < target->data().z()))
		{
			// Move specified target down the list
			slices_.shiftDown(target);
			regenerateDisplayData_ = true;
			maxBad = (target->next ? (target->next->data().z() < target->data().z()) : false);
		}
		else maxBad = false;
		if (++dummy == 10) break;
		
	} while (minBad || maxBad);
}

// Return first slice in list
Slice* Collection::slices()
{
	return slices_.first();
}

// Return last slice in list
Slice* Collection::lastSlice()
{
	return slices_.last();
}

// Return nth slice in list
Slice* Collection::slice(int index)
{
	return slices_[index];
}

// Return number of slices with no data present
int Collection::nEmptySlices()
{
	int count = 0;
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next) if (slice->data().nPoints() < 2) ++count;
	return count;
}

// Set root directory for datafiles
void Collection::setDataFileDirectory(QDir directory)
{
	dataFileDirectory_ = directory;
}

// Return root directory for datafiles
QDir Collection::dataFileDirectory()
{
	return dataFileDirectory_;
}

// Load data for specified slice index
bool Collection::loadSliceData(Slice* slice)
{
	if (!slice) return false;
	return slice->loadData(dataFileDirectory_);
}

// Reload data for all slices
int Collection::loadAllSlices()
{
	int nFailed = 0;
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next) if (!slice->loadData(dataFileDirectory_)) ++nFailed;
	return nFailed;
}

/*
 * Transforms
 */

// Return data minima
Vec3<double> Collection::dataMin()
{
	return dataMin_;
}

// Return data maxima
Vec3<double> Collection::dataMax()
{
	return dataMax_;
}

// Return transformed data minima
Vec3<double> Collection::transformMin()
{
	return transformMin_;
}

// Return transformed data maxima
Vec3<double> Collection::transformMax()
{
	return transformMax_;
}

// Return transformed positive data minima
Vec3<double> Collection::transformMinPositive()
{
	return transformMinPositive_;
}

// Return transformed positive data maxima
Vec3<double> Collection::transformMaxPositive()
{
	return transformMaxPositive_;
}

// Set transform equation for data
void Collection::setTransformEquation(int axis, QString transformEquation)
{
	transforms_[axis].setEquation(transformEquation);
	regenerateDisplayData_ = true;
}

// Return transform equation for data
QString Collection::transformEquation(int axis)
{
	return transforms_[axis].text();
}

// Return whether specified transform equation is valid
bool Collection::transformEquationValid(int axis)
{
	return transforms_[axis].valid();
}

// Set whether specified transform is enabled
void Collection::setTransformEnabled(int axis, bool enabled)
{
	transforms_[axis].setEnabled(enabled);
	regenerateDisplayData_ = true;
}

// Return whether specified transform is enabled
bool Collection::transformEnabled(int axis)
{
	return transforms_[axis].enabled();
}

// Recalculate data limits
void Collection::calculateDataLimits()
{
	dataMin_ = 0.0;
	dataMax_ = 0.0;
	if (slices_.nItems() > 0)
	{
		// Grab first slice and set initial values
		Slice* slice = slices_.first();
		dataMin_.set(slice->data().xMin(), slice->data().yMin(), slice->data().z());
		dataMax_.set(slice->data().xMax(), slice->data().yMax(), slice->data().z());
		double mmin, mmax;
		for (slice = slice->next; slice != NULL; slice = slice->next)
		{
// 			printf("Z = %f\n", slice->data().z());
			mmin = slice->data().xMin();
			mmax = slice->data().xMax();
			if (mmin < dataMin_.x) dataMin_.x = mmin;
			if (mmax > dataMax_.x) dataMax_.x = mmax;
			mmin = slice->data().yMin();
			mmax = slice->data().yMax();
			if (mmin < dataMin_.y) dataMin_.y = mmin;
			if (mmax > dataMax_.y) dataMax_.y = mmax;
			if (slice->data().z() < dataMin_.z) dataMin_.z = slice->data().z();
			else if (slice->data().z() > dataMax_.z) dataMax_.z = slice->data().z();
		}
	}
	updateDataTransforms();
}

// Update data transforms and calculate transform limits
void Collection::updateDataTransforms()
{
	// Loop over slices_ list, updating transforms as we go
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next) slice->transform(transforms_[0], transforms_[1], transforms_[2]);

	transformMin_ = 0.0;
	transformMax_ = 0.0;
	transformMinPositive_ = 0.1;
	transformMaxPositive_ = -1.0;
	if (slices_.nItems() == 0) return;
	
	// Grab first slice and set initial values
	Slice* slice = slices_.first();
	transformMin_.set(slice->transformedData().xMin(), slice->transformedData().yMin(), slice->transformedData().z());
	transformMax_.set(slice->transformedData().xMax(), slice->transformedData().yMax(), slice->transformedData().z());
	double mmin, mmax;
	for (slice = slice->next; slice != NULL; slice = slice->next)
	{
		mmin = slice->transformedData().xMin();
		mmax = slice->transformedData().xMax();
		if (mmin < transformMin_.x) transformMin_.x = mmin;
		if (mmax > transformMax_.x) transformMax_.x = mmax;
		mmin = slice->transformedData().yMin();
		mmax = slice->transformedData().yMax();
		if (mmin < transformMin_.y) transformMin_.y = mmin;
		if (mmax > transformMax_.y) transformMax_.y = mmax;
		if (slice->transformedData().z() < transformMin_.z) transformMin_.z = slice->transformedData().z();
		else if (slice->transformedData().z() > transformMax_.z) transformMax_.z = slice->transformedData().z();
	}

	// Now determine minimum positive limits
	for (slice = slices_.first(); slice != NULL; slice = slice->next)
	{
		// Loop over XY points in data, searching for first positive, non-zero value
		Data2D& data = slice->transformedData();
		for (int n=0; n<data.nPoints(); ++n)
		{
			// X
			if (data.x(n) > 0.0)
			{
				if (data.x(n) < transformMinPositive_.x) transformMinPositive_.x = data.x(n);
				if (data.x(n) > transformMaxPositive_.x) transformMaxPositive_.x = data.x(n);
			}
			// Y
			if (data.y(n) > 0.0)
			{
				if (data.y(n) < transformMinPositive_.y) transformMinPositive_.y = data.y(n);
				if (data.y(n) > transformMaxPositive_.y) transformMaxPositive_.y = data.y(n);
			}
		}
		
		// Z
		if (data.z() > 0.0)
		{
			if (data.z() < transformMinPositive_.z) transformMinPositive_.z = data.z();
			if (data.z() > transformMaxPositive_.z) transformMaxPositive_.z = data.z();
		}
	}

	// Check maximum positive values (since all datapoints might have been negative
	if (transformMaxPositive_.x < 0.0) transformMaxPositive_.x = 1.0;
	if (transformMaxPositive_.y < 0.0) transformMaxPositive_.y = 1.0;
	if (transformMaxPositive_.z < 0.0) transformMaxPositive_.z = 1.0;
}

// Set whether interpolation is enabled
void Collection::setInterpolate(int axis, bool enabled)
{
	interpolate_[axis] = enabled;
}

// Return whether interpolation is enabled
bool Collection::interpolate(int axis)
{
	return interpolate_[axis];
}

// Set whether interpolation is constrained
void Collection::setInterpolateConstrained(int axis, bool enabled)
{
	interpolateConstrained_[axis] = enabled;
}

// Return whether interpolation is constrained
bool Collection::interpolateConstrained(int axis)
{
	return interpolateConstrained_[axis];
}

// Set interpolation step size
void Collection::setInterpolationStep(int axis, double step)
{
	interpolationStep_[axis] = step;
}

// Return interpolation step size
double Collection::interpolationStep(int axis)
{
	return interpolationStep_[axis];
}

/*
 * Colours
 */

// Update colour scale
void Collection::updateColourScale()
{
	colourScale_.clear();
	if (colourSource_ == Collection::SingleColourSource) colourScale_.addPoint(0.0, colourSinglePoint_.colour());
	else if (colourSource_ == Collection::RGBGradientSource)
	{
		colourScale_.setUseHSV(false);
		colourScale_.addPoint(colourRGBGradientAPoint_.value(), colourRGBGradientAPoint_.colour());
		colourScale_.addPoint(colourRGBGradientBPoint_.value(), colourRGBGradientBPoint_.colour());
	}
	else if (colourSource_ == Collection::HSVGradientSource)
	{
		colourScale_.setUseHSV(true);
		colourScale_.addPoint(colourHSVGradientAPoint_.value(), colourHSVGradientAPoint_.colour());
		colourScale_.addPoint(colourHSVGradientBPoint_.value(), colourHSVGradientBPoint_.colour());
	}
	else if (colourSource_ == Collection::CustomGradientSource) colourScale_ = customColourScale_;

	// Set alpha value for all points if alphaControl_ == FixedAlpha
	if (alphaControl_ == Collection::FixedAlpha) colourScale_.setAllAlpha(fixedAlpha_);

	regenerateColourScale_ = false;

	// Surface will now need to be regenerated...
	regenerateSurfacePrimitive_ = true;
}

// Set colourscale source to use
void Collection::setColourSource(ColourSource source)
{
	colourSource_ = source;
	regenerateColourScale_ = true;
}

// Return colourscale source to use
Collection::ColourSource Collection::colourSource()
{
	return colourSource_;
}

// Set colourscale point colour
void Collection::setColourScalePoint(ColourSource source, QColor colour, double value, int index)
{
	switch (source)
	{
		case (Collection::SingleColourSource):
			colourSinglePoint_.setColour(colour);
			break;
		case (Collection::RGBGradientSource):
			if (index == 0) colourRGBGradientAPoint_.set(value, colour);
			else colourRGBGradientBPoint_.set(value, colour);
			break;
		case (Collection::HSVGradientSource):
			if (index == 0) colourHSVGradientAPoint_.set(value, colour);
			else colourHSVGradientBPoint_.set(value, colour);
			break;
		case (Collection::CustomGradientSource):
			customColourScale_.setPoint(index, value, colour);
			break;
	}

	// Update colourscale?
	if (source == colourSource_) regenerateColourScale_ = true;
}

// Return colourscale point specified
const ColourScalePoint* Collection::colourScalePoint(ColourSource source, int index)
{
	switch (source)
	{
		case (Collection::SingleColourSource):
			return &colourSinglePoint_;
			break;
		case (Collection::RGBGradientSource):
			return (index == 0 ? &colourRGBGradientAPoint_ : &colourRGBGradientBPoint_);
			break;
		case (Collection::HSVGradientSource):
			return (index == 0 ? &colourHSVGradientAPoint_ : &colourHSVGradientBPoint_);
			break;
		case (Collection::CustomGradientSource):
			return customColourScale_.point(index);
			break;
	}
}

// Return colour of point specified
QColor Collection::colourScalePointColour(ColourSource source, int index)
{
	const ColourScalePoint* point = colourScalePoint(source, index);
	if (point) return point->colour();
	else return Qt::black;
}

// Return value of point specified
double Collection::colourScalePointValue(ColourSource source, int index)
{
	const ColourScalePoint* point = colourScalePoint(source, index);
	if (point) return point->value();
	else return 0.0;
}

// Add point to custom colour scale
void Collection::addCustomColourScalePoint()
{
	customColourScale_.addPoint(customColourScale_.lastPoint() ? customColourScale_.lastPoint()->value() + 1.0 : 0.0, Qt::white);

	// Update colourscale?
	if (colourSource_ == Collection::CustomGradientSource) regenerateColourScale_ = true;
}

// Add point to custom colourscale
void Collection::addCustomColourScalePoint(double value, QColor colour)
{
	customColourScale_.addPoint(value, colour);
}

// Return number of custom colourscale points
int Collection::nCustomColourScalePoints()
{
	return customColourScale_.nPoints();
}

// Return first custom colourscale point in list
ColourScalePoint* Collection::customColourScalePoints()
{
	return customColourScale_.firstPoint();
}

// Return custom colourscale point with index specified
ColourScalePoint* Collection::customColourScalePoint(int id)
{
	return customColourScale_.point(id);
}

// Remove specified colourscale point
void Collection::removeCustomColourScalePoint(ColourScalePoint* point)
{
	customColourScale_.removePoint(point);

	// Update colourscale?
	if (colourSource_ == Collection::CustomGradientSource) regenerateColourScale_ = true;
}

// Set alpha control
void Collection::setAlphaControl(Collection::AlphaControl alpha)
{

	alphaControl_ = alpha;
	regenerateColourScale_ = true;
}

// Return current alpha control
Collection::AlphaControl Collection::alphaControl()
{
	return alphaControl_;
}

// Set fixed alpha value
void Collection::setFixedAlpha(double alpha)
{
	fixedAlpha_ = alpha;
	regenerateColourScale_ = true;
}

// Return fixed alpha value
double Collection::fixedAlpha()
{
	return fixedAlpha_;
}

// Return current colourscale
const ColourScale& Collection::colourScale()
{
	// Does the colourscale need updating first?
	if (regenerateColourScale_) updateColourScale();
	
	return colourScale_;
}

// Return whether colourscale needs to be updated
bool Collection::regenerateColourScale()
{
	return regenerateColourScale_;
}

/*
 * Surface
 */

// Suface Style Keywords
const char* SurfaceStyleKeywords[] = { "Line", "Full", "Grid" };

// Convert text string to SurfaceStyle
Collection::SurfaceStyle Collection::surfaceStyle(const char* s)
{
	for (int n=0; n<nSurfaceStyles; ++n) if (strcmp(s,SurfaceStyleKeywords[n]) == 0) return (Collection::SurfaceStyle) n;
	return nSurfaceStyles;
}

// Convert SurfaceStyle to text string
const char* Collection::surfaceStyle(Collection::SurfaceStyle kwd)
{
	return SurfaceStyleKeywords[kwd];
}

// Return transformed data to display
List<Data2D>& Collection::surfaceData()
{
	return surfaceData_;
}

// Set display style of data
void Collection::setSurfaceStyle(SurfaceStyle style)
{
	surfaceStyle_ = style;

	// Adjust primitive settings here 
	if (surfaceStyle_ == Collection::FullSurface) surfacePrimitive_.setType(GL_TRIANGLES);
	else surfacePrimitive_.setType(GL_LINES);
}

// Return display style of data
Collection::SurfaceStyle Collection::surfaceStyle()
{
	return surfaceStyle_;
}

// Manually set the flag to force regeneration of surface data
void Collection::setRegenerateDisplayData()
{
	regenerateDisplayData_ = true;
}

// Flag that the primitive has been updated
void Collection::setSurfacePrimitiveUpdated()
{
	regenerateSurfacePrimitive_ = false;
}

// Return whether primitive needs to be updated
bool Collection::regenerateSurfacePrimitive()
{
	return regenerateSurfacePrimitive_;
}

// Return surface primitive
Primitive& Collection::surfacePrimitive()
{
	return surfacePrimitive_;
}

// Update surface data if necessary
void Collection::updateSurfaceData(Vec3<double> axisMin, Vec3<double> axisMax, Vec3<bool> axisInverted, Vec3<bool> axisLogarithmic, Vec3<double> axisStretch)
{
	// Is surface reconstruction necessary?
	if (!regenerateDisplayData_) return;

	// Clear existing display slices
	surfaceData_.clear();
	double x, y;

	// Loop over slices, apply any transforms (X or Y) and check limits
	Slice* slice = axisInverted.z ? slices_.last() : slices_.first();
	while (slice)
	{
		// Z
		double z = slice->transformedData().z();
		// -- Is the transformed Z value within range?
		if ((z < axisMin.z) || (z > axisMax.z))
		{
			slice = axisInverted.z ? slice->prev : slice->next;
			continue;
		}
		if (axisInverted.z && axisLogarithmic.z) z = log10(axisMax.z/z);   // XXX TODO Changed from 'log10(axisMax[n]/z)' which was a bug???
		else if (axisInverted.z) z = (axisMax.z - z) + axisMin.z;
		else if (axisLogarithmic.z) z = log10(z);

		// Add new item to surfaceData_ array
		Data2D* surfaceSlice = surfaceData_.add();
		surfaceSlice->setZ(z * axisStretch.z);

		// Copy / interpolate arrays
		Array<double> array[2];
		if (interpolate_.x)
		{
			slice->transformedData().interpolate(interpolateConstrained_.x);
			double x = axisMin.x;
			while (x <= axisMax.x)
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
		if (axisInverted.x) for (int n=array[0].nItems()-1; n >= 0; --n)
		{
			x = array[0].value(n);
			if ((x < axisMin.x) || (x > axisMax.x)) continue;
			if (axisLogarithmic.x) x = log10(axisMax.x / x);
			else x = (axisMax.x - x) + axisMin.x;
			x *= axisStretch.x;
			y = array[1].value(n);
			if (axisLogarithmic.y) y = (axisInverted.y ? log10(axisMax.y / y) : log10(y));
			else if (axisInverted.y) y = (axisMax.y - y) + axisMin.y;
			y *= axisStretch.y;
			surfaceSlice->addPoint(x, y);
		}
		else for (int n=0; n<array[0].nItems(); ++n)
		{
			x = array[0].value(n);
			if ((x < axisMin.x) || (x > axisMax.x)) continue;
			if (axisLogarithmic.x) x = log10(x);
			x *= axisStretch.x;
			y = array[1].value(n);
			if (axisLogarithmic.y)
			{
				if (y < 0.0) y = 1.0e-10;
				else y = (axisInverted.y ? log10(axisMax.y / y) : log10(y));
			}
			else if (axisInverted.y) y = (axisMax.y - y) + axisMin.y;
			y *= axisStretch.y;
			surfaceSlice->addPoint(x, y);
		}

		// Move to next Z slice
		slice = axisInverted.z ? slice->prev : slice->next;
	}

	regenerateSurfacePrimitive_ = true;
	regenerateDisplayData_ = false;
}