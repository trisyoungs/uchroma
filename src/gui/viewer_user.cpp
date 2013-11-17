/*
	*** Scene Rendering Functions (User)
	*** src/gui/viewer_user.cpp
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

#include "gui/viewer.uih"
#include "gui/uchroma.h"

// Create necessary primitives (must be called before any rendering is done)
void Viewer::createPrimitives()
{
	// Setup primitives
	surfacePrimitive_.setColourData(true);
	slicePrimitive_.setNoInstances();
	slicePrimitiveBox_.setNoInstances();
	slicePrimitiveBox_.setType(GL_LINES);
	boundingBoxPrimitive_.setNoInstances();
	boundingBoxPrimitive_.setType(GL_LINES);
	for (int n=0; n<3; ++n)
	{
		axisPrimitives_[n].setType(GL_LINES);
		axisPrimitives_[n].setNoInstances();
	}

	// Every primitive using instances must be added to the primitiveList_
	primitiveList_.add(&surfacePrimitive_);
}

// Setup basic GL properties (called each time before renderScene())
void Viewer::setupGL()
{
	msg.enter("Viewer::setupGL");

	// Define colours etc.
	GLfloat backgroundColour[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat spotlightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat spotlightDiffuse[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat spotlightSpecular[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat spotlightPosition[4] = { 100.0f, 100.0f, 0.0f, 0.0f };
	GLfloat specularColour[4] = { 0.9f, 0.9f, 0.9f, 1.0f };

	// Clear (background) colour
	glClearColor(backgroundColour[0], backgroundColour[1], backgroundColour[2], backgroundColour[3]);

	// Perspective hint
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);

	// Smooth shading
	glShadeModel(GL_SMOOTH);

	// Auto-normalise surface normals
	glEnable(GL_NORMALIZE);	

	// Set up the light model
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, spotlightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, spotlightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spotlightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, spotlightPosition);
	glEnable(GL_LIGHT0);

	// Set specular reflection colour
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColour);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 127);

	// Configure antialiasing
	glDisable(GL_MULTISAMPLE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	// Configure fog effects
//	glFogi(GL_FOG_MODE, GL_LINEAR);
//	prefs.copyColour(Prefs::BackgroundColour, col);
//	glFogfv(GL_FOG_COLOR, col);
//	glFogf(GL_FOG_DENSITY, 0.35f);
//	glHint(GL_FOG_HINT, GL_NICEST);
//	glFogi(GL_FOG_START, prefs.depthNear());
//	glFogi(GL_FOG_END, prefs.depthFar());
//	glEnable(GL_FOG);
	glDisable(GL_FOG);

	// Configure face culling
// 	glCullFace(GL_BACK);
// 	glEnable(GL_CULL_FACE);

	msg.exit("Viewer::setupGL");
}

// Set source slice data
void Viewer::setSourceSliceList(List<Data2D>* sliceData)
{
	sliceData_ = sliceData;
}

// Construct normal / colour data for slice specified
void Viewer::constructSliceData(Data2D* targetSlice, double yAxisScale, Array< Vec3<double> >& normals, Array< Vec4<GLfloat> >& colours, ColourScale& colourScale, Data2D* previousSlice, Data2D* nextSlice)
{
	normals.clear();
	colours.clear();
	if ((previousSlice == NULL) && (nextSlice == NULL)) return;
	
	// Grab references to target arrays
	Array<double>& xTarget = targetSlice->arrayX();
	Array<double>& yTarget = targetSlice->arrayY();
	int nPoints = xTarget.nItems();
	if (nPoints < 2) return;

	// Get colour data
	int n;
	QColor colour;
	for (n=0; n<nPoints; ++n)
	{
		colour = colourScale.colour((uChroma_->axisLogarithmic(1) ? pow(10.0, yTarget[n]): yTarget[n]) / yAxisScale);
		colours.add(Vec4<GLfloat>(colour.redF(), colour.greenF(), colour.blueF(), colour.alphaF()));
	}

	// Calculate normals
	Vec3<double> v1, v2, v3;
	double dz;
	if (previousSlice && nextSlice)
	{
		// Grab other array references
		Array<double>& xPrev = previousSlice->arrayX();
		Array<double>& xNext = nextSlice->arrayX();
		Array<double>& yPrev = previousSlice->arrayY();
		Array<double>& yNext = nextSlice->arrayY();
		dz = previousSlice->z() - nextSlice->z();

		// -- First point
		v1.set(xTarget[1] - xTarget[0], yTarget[1] - yTarget[0], 0);
		v2.set(0.0, yNext[0] - yPrev[0], dz);
		normals.add(v1 * v2);
// 		v3 = v1 * v2;
// 		v3.normalise();
// 		normals.add(v3);
// 		printf("Norm %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", 0, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		// -- Points 1 to N-2
		for (n=1; n<nPoints-1; ++n)
		{
			v1.set(xTarget[n+1] - xTarget[n-1], yTarget[n+1] - yTarget[n-1], 0.0);
			v2.set(0.0, yNext[n] - yPrev[n], dz);
			normals.add(v1 * v2);
// 			v3 = v1 * v2;
// 			v3.normalise();
// 			normals.add(v3);
// 			printf("Norm %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", n, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		}
		// -- Last point
		v1.set(xTarget[nPoints-1] - xTarget[nPoints-2], yTarget[nPoints-1] - yTarget[nPoints-2], 0.0);
		v2.set(0.0, yPrev[nPoints-1] - yNext[nPoints-1], dz);
		normals.add(v1 * v2);
// 		v3 = v1 * v2;
// 		v3.normalise();
// 		normals.add(v3);
// 		printf("Norm %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", nPoints-1, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
	}
	else if (previousSlice)
	{
		// Grab other array reference
		Array<double>& yPrev = previousSlice->arrayY();
		dz = previousSlice->z() - targetSlice->z();

		// -- First point
		v1.set(xTarget[1] - xTarget[0], yTarget[1] - yTarget[0], 0.0);
		v2.set(0.0, yTarget[0] - yPrev[0], dz);
		normals.add(v1 * v2);
// 		v3 = v1 * v2;
// 		v3.normalise();
// 		normals.add(v3);
// 		printf("Last %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", 0, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		// -- Points 1 to N-2
		for (n=1; n<nPoints-1; ++n)
		{
			v1.set(xTarget[n+1] - xTarget[n-1], yTarget[n+1] - yTarget[n-1], 0.0);
			v2.set(0.0, yTarget[n] - yPrev[n], dz);
			normals.add(v1 * v2);
// 			v3 = v1 * v2;
// 			v3.normalise();
// 			normals.add(v3);
// 			printf("Last %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", n, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		}
		// -- Last point
		v1.set(xTarget[nPoints-1] - xTarget[nPoints-2], yTarget[nPoints-1] - yTarget[nPoints-2], 0.0);
		v2.set(0.0, yTarget[nPoints-1] - yPrev[nPoints-1], dz);
		normals.add(v1 * v2);
// 		v3 = v1 * v2;
// 		v3.normalise();
// 		normals.add(v3);
// 		printf("Last %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", nPoints-1, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
	}
	else
	{
		// Grab other array reference
		Array<double>& yNext = nextSlice->arrayY();
		dz = targetSlice->z() - nextSlice->z();

		// -- First point
		v1.set(xTarget[1] - xTarget[0], yTarget[1] - yTarget[0], 0.0);
		v2.set(0.0, yNext[0] - yTarget[0], dz);
		normals.add(v1 * v2);
// 		v3 = v1 * v2;
// 		v3.normalise();
// 		normals.add(v3);
// 		printf("Frst %i = (%f %f %f) * (%f %f %f) = (%f %f %f)\n", 0, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		// -- Points 1 to N-2
		for (n=1; n<nPoints-1; ++n)
		{
			v1.set(xTarget[n+1] - xTarget[n-1], yTarget[n+1] - yTarget[n-1], 0.0);
			v2.set(0.0, yNext[n] - yTarget[n], dz);
			normals.add(v1 * v2);
// 			v3 = v1 * v2;
// 			v3.normalise();
// 			normals.add(v3);
// 			printf("Frst %i = (%f %f %f) * (%f %f %f) = (%f %f %f)\n", n, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		}
		// -- Last point
		v1.set(xTarget[nPoints-1] - xTarget[nPoints-2], yTarget[nPoints-1] - yTarget[nPoints-2], 0.0);
		v2.set(0.0, yNext[nPoints-1] - yTarget[nPoints-1], dz);
		normals.add(v1 * v2);
// 		v3 = v1 * v2;
// 		v3.normalise();
// 		normals.add(v3);
// 		printf("Frst %i = (%f %f %f) * (%f %f %f) = (%f %f %f)\n", nPoints-1, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
	}
	
	// Normalise normals
	for (n=0; n<normals.nItems(); ++n) normals[n].normalise();
}

// Create surface primitive
void Viewer::createSurface(ColourScale colourScale, double yAxisScale)
{
	// Check for valid slice list
	if (sliceData_ == NULL) return;

	// Take copy of colourscale and yAxisScale values
	colourScale_ = colourScale;
	yAxisScale_ = yAxisScale;

	GLfloat zA, zB;

	// Pop old primitive instance - we need to recreate the display list / vertex array
	surfacePrimitive_.popInstance(context());
	surfacePrimitive_.forgetAll();

	// Sanity check - are there enough slices to proceed?
	if (sliceData_->nItems() < 2) return;

	// Temporary variables
	Array< Vec3<double> > normA, normB;
	Array< Vec4<GLfloat> > colourA, colourB;
	int n;
	QColor colour;
	Vec3<double> nrm(0.0,1.0,0.0);

	// We need to skip over slices with zero points, so construct a reflist here of those which contain usable data
	RefList<Data2D,int> slices;
	for (Data2D* slice = sliceData_->first(); slice != NULL; slice = slice->next) if (slice->nPoints() > 1) slices.add(slice);

	// Construct first slice data and set initial min/max values
	RefListItem<Data2D,int>* ri = slices.first();
	constructSliceData(ri->item, yAxisScale_, normA, colourA, colourScale_, NULL, ri->next ? ri->next->item : NULL);
	if (slices.nItems() < 2) return;
	int nPoints = slices.first()->item->nPoints();
	
	// Create triangles
	for (RefListItem<Data2D,int>* rj = ri->next; rj != NULL; rj = rj->next)
	{
		// Grab slice pointers
		Data2D* sliceA = ri->item;
		Data2D* sliceB = rj->item;
		Data2D* sliceC = (rj->next == NULL ? NULL : rj->next->item);

		// Construct data for current slice
		constructSliceData(sliceB, yAxisScale_, normB, colourB, colourScale_, sliceA, sliceC);

		// Grab z values
		zA = (GLfloat) sliceA->z();
		zB = (GLfloat) sliceB->z();
	
		// Get nPoints, and initial coordinates
		Array<double>& xA = sliceA->arrayX();
		Array<double>& yA = sliceA->arrayY();
		Array<double>& xB = sliceB->arrayX();
		Array<double>& yB = sliceB->arrayY();
		for (int n=0; n<nPoints-1; ++n)
		{
			// Add triangles for this quadrant
			surfacePrimitive_.defineVertex(xA[n], yA[n], zA, normA[n], colourA[n], true);
			surfacePrimitive_.defineVertex(xA[n+1], yA[n+1], zA, normA[n+1], colourA[n+1], true);
			surfacePrimitive_.defineVertex(xB[n+1], yB[n+1], zB, normB[n+1], colourB[n+1], true);
			surfacePrimitive_.defineVertex(xA[n], yA[n], zA, normA[n], colourA[n], true);
			surfacePrimitive_.defineVertex(xB[n], yB[n], zB, normB[n], colourB[n], true);
			surfacePrimitive_.defineVertex(xB[n+1], yB[n+1], zB, normB[n+1], colourB[n+1], true);
		}

		// Copy arrays ready for next pass
		normA = normB;
		colourA = colourB;
		ri = rj;
	}

	// Push a new instance to create the new display list / vertex array
	surfacePrimitive_.pushInstance(context());

	msg.print("Surface contains %i vertices.\n", surfacePrimitive_.nDefinedVertices());
}

// Create surface using previous colourscale and y axis scaling factor
void Viewer::createSurface()
{
	createSurface(colourScale_, yAxisScale_);
}

// Clear specified axix primitives
void Viewer::clearAxisPrimitives(int axis)
{
	axisPrimitives_[axis].forgetAll();
	axisTextPrimitives_[axis].forgetAll();
}

// Add line to axis primitive
void Viewer::addAxisLine(int axis, Vec3<double> v1, Vec3<double> v2)
{
	axisPrimitives_[axis].plotLine(v1, v2);
}

// Add entry to axis text primitive
void Viewer::addAxisText(int axis, QString text, Vec3<double> origin, Matrix transform, TextPrimitive::HorizontalAnchor anchor)
{
	FTBBox boundingBox;
	if (font_) boundingBox = font_->BBox(qPrintable(text));
	double textWidth = fabs(boundingBox.Upper().X() - boundingBox.Lower().X());

	// Construct final centre coordinate based on alignment requested
	Vec3<double> textCentre(0.0, -fontBaseHeight_*0.5, 0.0);
	if (anchor == TextPrimitive::AnchorLeft) textCentre.x = 0.0;
	else if (anchor == TextPrimitive::AnchorRight) textCentre.x = -textWidth;
	else textCentre.x = -textWidth*0.5;

	axisTextPrimitives_[axis].add(text, origin, textCentre, transform);
}

// Create bounding box
void Viewer::createBoundingBox(int type, double planeY)
{
	boundingBoxPrimitive_.forgetAll();
	
	if (type == 0) return;
	else if (type == 1)
	{
		// Plane in XZ, spanning data range   // TODO
// 		boundingBoxPrimitive_.plotLine(Vec3<double>(axisCoordMin_[0].x, planeY, axisCoordMin_[2].z), Vec3<double>(axisCoordMin_[0].x, planeY, axisCoordMax_[2].z));
// 		boundingBoxPrimitive_.plotLine(Vec3<double>(axisCoordMin_[0].x, planeY, axisCoordMax_[2].z), Vec3<double>(axisCoordMax_[0].x, planeY, axisCoordMax_[2].z));
// 		boundingBoxPrimitive_.plotLine(Vec3<double>(axisCoordMax_[0].x, planeY, axisCoordMax_[2].z), Vec3<double>(axisCoordMax_[0].x, planeY, axisCoordMin_[2].z));
// 		boundingBoxPrimitive_.plotLine(Vec3<double>(axisCoordMax_[0].x, planeY, axisCoordMin_[2].z), Vec3<double>(axisCoordMin_[0].x, planeY, axisCoordMin_[2].z));
	}
}

// Set Y clip plane limits
void Viewer::setYClip(double yMin, double yMax)
{
	clipPlaneYMin_ = yMin - clipPlaneDelta_;
	clipPlaneYMax_ = yMax + clipPlaneDelta_;
}

// Return number of triangles in surface
int Viewer::surfaceNTriangles()
{
	return surfacePrimitive_.nDefinedVertices() / 3;
}

// Setup slice primitive
void Viewer::setSlicePrimitive(int axis)
{
	slicePrimitive_.forgetAll();
	slicePrimitiveBox_.forgetAll();
	if (axis == -1) return;

	// Grab axes, and knock out values in the supplied vectors which correspond to the activated axis
	Vec3<double> axisMinA = uChroma_->axisCoordMin((axis+1)%3), axisMaxA = uChroma_->axisCoordMax((axis+1)%3);
	Vec3<double> axisMinB = uChroma_->axisCoordMin((axis+2)%3), axisMaxB = uChroma_->axisCoordMax((axis+2)%3);
	axisMinA[axis] = 0.0;
	axisMaxA[axis] = 0.0;
	axisMinB[axis] = 0.0;
	axisMaxB[axis] = 0.0;

	// Work out deltas for each direction
	const int nPoints = 50;
	Vec3<double> deltaA, deltaB, pos;
	deltaA = (axisMaxA - axisMinA) / nPoints;
	deltaB = (axisMaxB - axisMinB) / nPoints;

	// Create 'bounding box' for slice primitive
	slicePrimitiveBox_.plotLine(axisMinA, axisMaxA);
	slicePrimitiveBox_.plotLine(axisMaxA, axisMaxA + axisMaxB - axisMinB);
	slicePrimitiveBox_.plotLine(axisMaxA + axisMaxB - axisMinB, axisMinA + axisMaxB - axisMinB);
	slicePrimitiveBox_.plotLine(axisMinA + axisMaxB - axisMinB, axisMinA);

	// Set normal
	Vec3<double> normal(0.0, 0.0, 0.0);
	normal[axis] = 1.0;

	// Construct plane
	for (int n=0; n<nPoints; ++n)
	{
		pos = axisMinA + deltaA*n;
		for (int m=0; m<nPoints; ++m)
		{
			slicePrimitive_.defineVertex(pos, normal, true);
			slicePrimitive_.defineVertex(pos + deltaA, normal, true);
			slicePrimitive_.defineVertex(pos + deltaA + deltaB, normal, true);
			slicePrimitive_.defineVertex(pos + deltaA + deltaB, normal, true);
			slicePrimitive_.defineVertex(pos + deltaB, normal, true);
			slicePrimitive_.defineVertex(pos, normal, true);
			pos += deltaB;
		}
	}
}
