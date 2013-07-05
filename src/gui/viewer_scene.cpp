/*
	*** Scene Rendering Functions (User)
	*** src/gui/viewer_scene.cpp
	Copyright T. Youngs 2013

	This file is part of FQPlot.

	FQPlot is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	FQPlot is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FQPlot.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/viewer.uih"
#include "gui/colourscale.uih"
#include "gui/fqplot.h"

// Create necessary primitives (must be called before any rendering is done)
void Viewer::createPrimitives()
{
	// Create test primitives
	spherePrimitive_.plotSphere(0.5, 8, 10);
	cubePrimitive_.plotCube(0.5, 4, 0.0, 0.0, 0.0);

	// Setup surface primitive
	surface_.setColourData(true);
	for (int n=0; n<3; ++n)
	{
		axisPrimitives_[n].setType(GL_LINES);
		axisPrimitives_[n].setNoInstances();
	}

	// Every created primitive must be added to the primitiveList_
	primitiveList_.add(&spherePrimitive_);
	primitiveList_.add(&cubePrimitive_);
	primitiveList_.add(&surface_);
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
	GLfloat spotlightPosition[4] = { 10.0f, 10.0f, 100.0f, 0.0f };
	GLfloat specularColour[4] = { 0.9f, 0.9f, 0.9f, 1.0f };

	// Clear (background) colour
	glClearColor(backgroundColour[0], backgroundColour[1], backgroundColour[2], backgroundColour[3]);
	//glClearDepth(1.0);

	// Perspective hint
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_FASTEST);

	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);

	// Smooth shading
	glShadeModel(GL_SMOOTH);

	// Auto-normalise surface normals
	glEnable(GL_NORMALIZE);

	// Set alpha-blending function
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);

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
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
// 	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
// 	glEnable(GL_LINE_SMOOTH);
// 	glEnable(GL_POLYGON_SMOOTH);

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

// Draw the scene to display (i.e. store primitives ready for sending to GL)
void Viewer::drawScene()
{
	// Set colour mode
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	// Prepare for model rendering
// 	glViewport(vp[0], vp[1], vp[2], vp[3]);
	glMatrixMode(GL_PROJECTION);
	setProjectionMatrix();
	glLoadMatrixd(projectionMatrix_.matrix());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLfloat colourRed[4] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat colourGreen[4] = { 0.0, 1.0, 0.0, 1.0 };
	GLfloat colourBlue[4] = { 0.0, 0.0, 1.0, 1.0 };
	GLfloat colourWhite[4] = { 1.0, 1.0, 1.0, 0.8 };
	GLfloat colourBlack[4] = { 0.0, 0.0, 0.0, 1.0 };
	
	// Set up initial matrix
	Matrix A;
	A.setIdentity();
	if (invertZ_) A[10] = -1.0;

	if (surface_.nDefinedVertices() == 0)
	{
		renderPrimitive(&spherePrimitive_, colourWhite, A);
		A.setTranslation(1.0,0.0,0.0);
		renderPrimitive(&cubePrimitive_, colourRed, A);
		A.setTranslation(-1.0,0.0,0.0);
		renderPrimitive(&cubePrimitive_, colourRed, A);
		A.setTranslation(0.0,1.0,0.0);
		renderPrimitive(&cubePrimitive_, colourGreen, A);
		A.setTranslation(0.0,-1.0,0.0);
		renderPrimitive(&cubePrimitive_, colourGreen, A);
		A.setTranslation(0.0,0.0,1.0);
		renderPrimitive(&cubePrimitive_, colourBlue, A);
		A.setTranslation(0.0,0.0,-1.0);
		renderPrimitive(&cubePrimitive_, colourBlue, A);
		return;
	}

	// Shift to center coordinates
	A.setTranslation(-surfaceCenter_.x, -surfaceCenter_.y, -surfaceCenter_.z);

	// Render axes
	for (int axis=0; axis<3; ++axis) renderPrimitive(&axisPrimitives_[axis], colourBlack, A);

	// Render surface
	renderPrimitive(&surface_, colourRed, A);
}

// Construct normal / colour data for slice specified
void Viewer::constructSliceData(Slice* targetSlice, Array< Vec3<double> >& normals, Array< Vec4<GLfloat> >& colours, ColourScale* colourScale, Slice* previousSlice, Slice* nextSlice)
{
	normals.clear();
	colours.clear();
	if ((previousSlice == NULL) && (nextSlice == NULL)) return;

	// Get colour data
	Array<double>& xTarget = targetSlice->data().arrayX();
	Array<double>& yTarget = targetSlice->data().arrayY();
	int n, nPoints = xTarget.nItems();
	QColor colour;
	for (n=0; n<nPoints; ++n)
	{
		colour = colourScale->colour(yTarget[n]);
		colours.add(Vec4<GLfloat>(colour.redF(), colour.greenF(), colour.blueF(), colour.alphaF()));
	}

	// Calculate normals
	Vec3<double> v1, v2, v3;
	double dz;
	if (previousSlice && nextSlice)
	{
		// Grab other array references
		Array<double>& xPrev = previousSlice->data().arrayX();
		Array<double>& xNext = nextSlice->data().arrayX();
		Array<double>& yPrev = previousSlice->data().arrayY();
		Array<double>& yNext = nextSlice->data().arrayY();
		dz = previousSlice->z() - nextSlice->z();

		// -- First point
		v1.set(xTarget[1] - xTarget[0], yTarget[1] - yTarget[0], 0);
		v2.set(0.0, yNext[n] - yPrev[n], dz);
		v3 = v1 * v2;
		v3.normalise();
		normals.add(v3);
// 		printf("Norm %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", 0, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		// -- Points 1 to N-2
		for (n=1; n<nPoints-1; ++n)
		{
			v1.set(xTarget[n+1] - xTarget[n-1], yTarget[n+1] - yTarget[n-1], 0.0);
			v2.set(0.0, yNext[n] - yPrev[n], dz);
			v3 = v1 * v2;
			v3.normalise();
			normals.add(v3);
// 			printf("Norm %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", n, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		}
		// -- Last point
		v1.set(xTarget[nPoints-1] - xTarget[nPoints-2], yTarget[nPoints-1] - yTarget[nPoints-2], 0.0);
		v2.set(0.0, yPrev[nPoints-1] - yNext[nPoints-1], dz);
		v3 = v1 * v2;
		v3.normalise();
		normals.add(v3);
// 		printf("Norm %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", nPoints-1, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
	}
	else if (previousSlice)
	{
		// Grab other array reference
		Array<double>& yPrev = previousSlice->data().arrayY();
		dz = previousSlice->z() - targetSlice->z();

		// -- First point
		v1.set(xTarget[1] - xTarget[0], yTarget[1] - yTarget[0], 0.0);
		v2.set(0.0, yTarget[n] - yPrev[n], dz);
		v3 = v1 * v2;
		v3.normalise();
		normals.add(v3);
// 		printf("Last %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", 0, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		// -- Points 1 to N-2
		for (n=1; n<nPoints-1; ++n)
		{
			v1.set(xTarget[n+1] - xTarget[n-1], yTarget[n+1] - yTarget[n-1], 0.0);
			v2.set(0.0, yTarget[n] - yPrev[n], dz);
			v3 = v1 * v2;
			v3.normalise();
			normals.add(v3);
// 			printf("Last %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", n, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		}
		// -- Last point
		v1.set(xTarget[nPoints-1] - xTarget[nPoints-2], yTarget[nPoints-1] - yTarget[nPoints-2], 0.0);
		v2.set(0.0, yTarget[nPoints-1] - yPrev[nPoints-1], dz);
		v3 = v1 * v2;
		v3.normalise();
		normals.add(v3);
// 		printf("Last %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", nPoints-1, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
	}
	else
	{
		// Grab other array reference
		Array<double>& yNext = nextSlice->data().arrayY();
		dz = targetSlice->z() - nextSlice->z();

		// -- First point
		v1.set(xTarget[1] - xTarget[0], yTarget[1] - yTarget[0], 0.0);
		v2.set(0.0, yNext[n] - yTarget[n], dz);
		v3 = v1 * v2;
		v3.normalise();
		normals.add(v3);
// 		printf("Frst %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", 0, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		// -- Points 1 to N-2
		for (n=1; n<nPoints-1; ++n)
		{
			v1.set(xTarget[n+1] - xTarget[n-1], yTarget[n+1] - yTarget[n-1], 0.0);
			v2.set(0.0, yNext[n] - yTarget[n], dz);
			v3 = v1 * v2;
			v3.normalise();
			normals.add(v3);
// 			printf("Frst %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", n, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		}
		// -- Last point
		v1.set(xTarget[nPoints-1] - xTarget[nPoints-2], yTarget[nPoints-1] - yTarget[nPoints-2], 0.0);
		v2.set(0.0, yNext[n] - yTarget[n], dz);
		v3 = v1 * v2;
		v3.normalise();
		normals.add(v3);
// 		printf("Frst %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", nPoints-1, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
	}
	
	// Normalise normals
	for (n=0; n<normals.nItems(); ++n) normals[n].normalise();
}

// Create surface primitive
void Viewer::createSurface(const List<Slice>& slices, ColourScale* colourScale)
{
	GLfloat zA, zB;
	
	surface_.popInstance(context());
	surface_.forgetAll();

	// Sanity check - are there enough slices to proceed?
	if (slices.nItems() < 2) return;

	// Temporary variables
	Array< Vec3<double> > normA, normB;
	Array< Vec4<GLfloat> > colourA, colourB;
	int n, nPoints = slices.first()->data().nPoints();
	QColor colour;
	Vec3<double> nrm(0.0,1.0,0.0);

	// Construct first slice data and set initial min/max values
	Slice* sliceA = slices.first();
	constructSliceData(sliceA, normA, colourA, colourScale, NULL, sliceA->next);
	
	// Create triangles
	for (Slice* sliceB = sliceA->next; sliceB != NULL; sliceB = sliceB->next)
	{
		// Construct data for current slice
		constructSliceData(sliceB, normB, colourB, colourScale, sliceA, sliceB->next);

		// Grab z values
		zA = (GLfloat) sliceA->z();
		zB = (GLfloat) sliceB->z();

		// Get nPoints, and initial coordinates
		Array<double>& xA = sliceA->data().arrayX();
		Array<double>& yA = sliceA->data().arrayY();
		Array<double>& xB = sliceB->data().arrayX();
		Array<double>& yB = sliceB->data().arrayY();
		for (int n=0; n<nPoints-1; ++n)
		{
			// Add triangles for this quadrant
			surface_.defineVertex(xA[n], yA[n], zA, normA[n], colourA[n], true);
			surface_.defineVertex(xA[n+1], yA[n+1], zA, normA[n+1], colourA[n+1], true);
			surface_.defineVertex(xB[n+1], yB[n+1], zB, normB[n+1], colourB[n+1], true);
			surface_.defineVertex(xA[n], yA[n], zA, normA[n], colourA[n], true);
			surface_.defineVertex(xB[n], yB[n], zB, normB[n], colourB[n], true);
			surface_.defineVertex(xB[n+1], yB[n+1], zB, normB[n+1], colourB[n+1], true);
		}

		// Copy arrays ready for next pass
		normA = normB;
		colourA = colourB;
		sliceA = sliceB;
	}

	surface_.pushInstance(context());

// 	msg.print("Surface contains %i vertices.\n", surface_.nDefinedVertices());
}

// Create axis primitives
void Viewer::createAxis(int axis, Vec3<double> axisPosition, double axisMin, double axisMax, double firstTick, double tickDelta, int nMinorTicks, Vec3<double> direction)
{
	// Clear old primitive data
	axisPrimitives_[axis].forgetAll();
	axisTextPrimitives_[axis].forgetAll();

	Vec3<double> u, v;
	double range = axisMax - axisMin;

	// Draw a line from min to max range, passing through the defined axisPosition
	u = axisPosition;
	u.set(axis, axisMin);
	v = axisPosition;
	v.set(axis, axisMax);
	axisPrimitives_[axis].plotLine(u, v);

	// Plot tickmarks
	int count = 0;
	double delta = tickDelta / (nMinorTicks+1);
	u.set(axis, firstTick);
	while (u[axis] <= axisMax)
	{
		if (count%(nMinorTicks+1) == 0) axisPrimitives_[axis].plotLine(u, u+direction*0.1);
		else axisPrimitives_[axis].plotLine(u, u+direction*0.05);
		u.add(axis, delta);
		count = (count+1)%(nMinorTicks+1);
	}
	
	// Plot axis labels
	count = 0;
	u.set(axis, firstTick);
	while (u[axis] <= axisMax)
	{
		axisTextPrimitives_[axis].add(QString::number(u[axis]), axisLabelScale_, fontBaseHeight_, u, direction, Vec3<double>(0.0,1.0,0.0));
		u.add(axis, tickDelta);
		++count;
		if (count == 50)
		{
			printf("Maximum ticks exceeded.\n");
			break;
		}
	}
// 	textPrimitives_.add(QString::number(axisMax[axis]), v, Vec3<double>(1.0,0.0,0.0), Vec3<double>(0.0,1.0,0.0));
}

// Set surface centre
void Viewer::setSurfaceCenter(Vec3<double> center)
{
	surfaceCenter_ = center;
}

// Return number of triangles in surface
int Viewer::surfaceNTriangles()
{
	return surface_.nDefinedVertices() / 3;
}

// Set whether Z-axis is inverted
void Viewer::setInvertZ(bool b)
{
	invertZ_ = b;
}

// Set scale factor for axis labels
void Viewer::setAxisLabelScale(double scale)
{
	axisLabelScale_ = scale;
}
