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

// Create necessary primitives (must be called before any rendering is done)
void Viewer::createPrimitives()
{
	// Create test primitives
	spherePrimitive_.plotSphere(0.5, 8, 10);
	cubePrimitive_.plotCube(0.5, 4, 0.0, 0.0, 0.0);

	// Setup surface primitive
	surface_.setColourData(true);
	
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
	GLfloat spotlightPosition[4] = { 10.0f, 10.0f, -100.0f, 0.0f };
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
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_POLYGON_SMOOTH);
	glDisable(GL_MULTISAMPLE);

	// Configure antialiasing
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	glEnable(GL_LINE_SMOOTH);

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

	Matrix A;
	GLfloat colourRed[4] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat colourGreen[4] = { 0.0, 1.0, 0.0, 1.0 };
	GLfloat colourBlue[4] = { 0.0, 0.0, 1.0, 1.0 };
	GLfloat colourWhite[4] = { 1.0, 1.0, 1.0, 0.8 };

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
	}

	A.setTranslation(-surfaceCenter_.x, -surfaceCenter_.y, surfaceCenter_.z);
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
		dz = ((previousSlice->z() - targetSlice->z()) + (nextSlice->z() - targetSlice->z()))*0.5;

		// -- First point
		v1.set(xTarget[1] - xTarget[0], yTarget[1] - yTarget[0], 0);
		v2.set(0.0, ((yPrev[n] - yTarget[n]) + (yNext[n] - yTarget[n]))*0.5, dz);
		v3 = v1 * v2;
		v3.normalise();
		normals.add(v3);
		// -- Points 1 to N-2
		for (n=1; n<nPoints-1; ++n)
		{
// 			v1.set(((xTarget[n-1] - xTarget[n]) + (xTarget[n+1] - xTarget[n]))*0.5, ((yTarget[n-1] - yTarget[n]) + (yTarget[n+1] - yTarget[n]))*0.5, 0);
// 			v2.set(0.0, ((yPrev[n] - yTarget[n]) + (yNext[n] - yTarget[n]))*0.5, dz);
			v1.set(xTarget[n+1] - xTarget[n], yNext[n] - yTarget[n], 0.0);
			v2.set(xNext[n] - xTarget[n], yTarget[n+1] - yTarget[n], dz);
			v3 = v1 * v2;
			v3.normalise();
			normals.add(v3);
// 			if (normals.nItems() < 100) printf("v1=%f,%f,%f  v2=%f,%f,%f  v3=%f,%f,%f\n", v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		}
		// -- Last point
		v1.set(xTarget[nPoints-2] - xTarget[nPoints-1], yTarget[nPoints-2] - yTarget[nPoints-1], 0.0);
		v2.set(0.0, ((yPrev[nPoints-1] - yTarget[nPoints-1]) + (yNext[nPoints-1] - yTarget[nPoints-1]))*0.5, dz);
		v3 = v1 * v2;
		v3.normalise();
		normals.add(v3);
	}
	else if (previousSlice)
	{
		// Grab other array reference
		Array<double>& yPrev = previousSlice->data().arrayY();
		dz = (previousSlice->z() - targetSlice->z());

		// -- First point
		v1.set(xTarget[1] - xTarget[0], yTarget[1] - yTarget[0], 0.0);
		v2.set(0.0, yPrev[n] - yTarget[n], dz);
		normals.add(v1 * v2);
		// -- Points 1 to N-2
		for (n=1; n<nPoints-1; ++n)
		{
			v1.set(((xTarget[n-1] - xTarget[n]) + (xTarget[n+1] - xTarget[n]))*0.5, ((yTarget[n-1] - yTarget[n]) + (yTarget[n+1] - yTarget[n]))*0.5, 0.0);
			v2.set(0.0, yPrev[n] - yTarget[n], dz);
			normals.add(v1 * v2);
		}
		// -- Last point
		v1.set(xTarget[nPoints-2] - xTarget[nPoints-1], yTarget[nPoints-2] - yTarget[nPoints-1], 0.0);
		v2.set(0.0, yPrev[nPoints-1] - yTarget[nPoints-1], dz);
		normals.add(v1 * v2);
	}
	else
	{
		// Grab other array reference
		Array<double>& yNext = nextSlice->data().arrayY();
		dz = nextSlice->z() - targetSlice->z();

		// -- First point
		v1.set(xTarget[1] - xTarget[0], yTarget[1] - yTarget[0], 0.0);
		v2.set(0.0, yNext[n] - yTarget[n], dz);
		normals.add(v1 * v2);
		// -- Points 1 to N-2
		for (n=1; n<nPoints-1; ++n)
		{
			v1.set(((xTarget[n-1] - xTarget[n]) + (xTarget[n+1] - xTarget[n]))*0.5, ((yTarget[n-1] - yTarget[n]) + (yTarget[n+1] - yTarget[n]))*0.5, 0.0);
			v2.set(0.0, yNext[n] - yTarget[n], dz);
			normals.add(v1 * v2);
		}
		// -- Last point
		v1.set(xTarget[nPoints-2] - xTarget[nPoints-1], yTarget[nPoints-2] - yTarget[nPoints-1], 0.0);
		v2.set(0.0, yNext[n] - yTarget[n], dz);
		normals.add(v1 * v2);
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
	Vec3<double> nrm(0.0,1.0,0.0), dataMin, dataMax;
	double mmin, mmax;

	// Construct first slice data and set initial min/max values
	Slice* sliceA = slices.first();
	constructSliceData(sliceA, normA, colourA, colourScale, NULL, sliceA->next);
	dataMin.set(sliceA->data().xMin(), sliceA->data().yMin(), sliceA->z());
	dataMax.set(sliceA->data().xMax(), sliceA->data().yMax(), sliceA->z());
	
	// Create triangles
	for (Slice* sliceB = sliceA->next; sliceB != NULL; sliceB = sliceB->next)
	{
		// Construct data for current slice
		constructSliceData(sliceB, normB, colourB, colourScale, sliceA, sliceB->next);

		// Grab z values
		zA = (GLfloat) -sliceA->z();
		zB = (GLfloat) -sliceB->z();

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
// 			nrm = Vec3<double>(xA[n+1]-xA[n],yA[n+1]-yA[n],0.0) * Vec3<double>(xA[n+1]-xB[n+1],yA[n+1]-yB[n+1], zA-zB);
// 			nrm.normalise();
// 			surface_.defineVertex(xA[n], yA[n], zA, nrm, colourA[n], true);
// 			surface_.defineVertex(xA[n+1], yA[n+1], zA, nrm, colourA[n+1], true);
// 			surface_.defineVertex(xB[n+1], yB[n+1], zB, nrm, colourB[n+1], true);
// 			nrm = -Vec3<double>(xB[n]-xA[n],yB[n]-yA[n],zB-zA) * Vec3<double>(xB[n]-xB[n+1],yB[n]-yB[n+1], 0.0);
// 			nrm.normalise();
// 			surface_.defineVertex(xA[n], yA[n], zA, nrm, colourA[n], true);
// 			surface_.defineVertex(xB[n], yB[n], zB, nrm, colourB[n], true);
// 			surface_.defineVertex(xB[n+1], yB[n+1], zB, nrm, colourB[n+1], true);
		}

		// Adjust min/max range
		mmin = sliceB->data().xMin();
		mmax = sliceB->data().xMax();
		if (mmin < dataMin.x) dataMin.x = mmin;
		if (mmax > dataMax.x) dataMax.x = mmax;
		mmin = sliceB->data().yMin();
		mmax = sliceB->data().yMax();
		if (mmin < dataMin.y) dataMin.y = mmin;
		if (mmax > dataMax.y) dataMax.y = mmax;
		if (sliceB->z() < dataMin.z) dataMin.z = sliceB->z();
		if (sliceB->z() > dataMax.z) dataMax.z = sliceB->z();

		// Copy arrays ready for next pass
		normA = normB;
		colourA = colourB;
		sliceA = sliceB;
	}

	surface_.pushInstance(context());

	msg.print("Surface contains %i vertices.\n", surface_.nDefinedVertices());
	
	// Determine center coordinates of data (on which to base a view translation for the data)
	surfaceCenter_ = (dataMax - dataMin) * 0.5;
	surfaceCenter_.print();
}

// Return number of triangles in surface
int Viewer::surfaceNTriangles()
{
	return surface_.nDefinedVertices() / 3;
}