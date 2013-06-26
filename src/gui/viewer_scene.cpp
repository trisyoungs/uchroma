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

// Create necessary primitives (must be called before any rendering is done)
void Viewer::createPrimitives()
{
	// Create test primitive
	spherePrimitive_.plotSphere(0.5, 8, 10);
	cubePrimitive_.plotCube(0.5, 4, 0.0, 0.0, 0.0);
	
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
	GLfloat spotlightPosition[4] = { 100.0f, 100.0f, 100.0f, 0.0f };
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

	// Auto-calculate surface normals
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

	renderPrimitive(&surface_, colourRed, A);
}

// Create surface primitive
void Viewer::createSurface(const List<Slice>& slices)
{
	GLfloat x1, x2, y1, y2, x1last, x2last, y1last, y2last, z1, z2, norm[3] = { 1.0, 0.0, 0.0 };
	
	surface_.popInstance(context());
	surface_.forgetAll();

	// TEST - Add all data.
	for (Slice* slice = slices.first(); slice != slices.last(); slice = slice->next)
	{
		// Grab pointer to next slice
		Slice* nextSlice = slice->next;
		z1 = (GLfloat) slice->z();
		z2 = (GLfloat) nextSlice->z();

		// Get nPoints, and initial coordinates
		int nPoints = (slice->data().nPoints() < nextSlice->data().nPoints() ? slice->data().nPoints() : nextSlice->data().nPoints());
		x1last = (GLfloat) slice->data().x(0);
		x2last = (GLfloat) nextSlice->data().x(0);
		y1last = (GLfloat) slice->data().y(0);
		y2last = (GLfloat) nextSlice->data().y(0);
		for (int n=1; n<nPoints; ++n)
		{
			// Get current coordinates
			x1 = (GLfloat) slice->data().x(n);
			x2 = (GLfloat) nextSlice->data().x(n);
			y1 = (GLfloat) slice->data().y(n);
			y2 = (GLfloat) nextSlice->data().y(n);

			// Add triangles for this quadrant
			surface_.defineVertex(x1last, y1last, z1, norm[0], norm[1], norm[2], true);
			surface_.defineVertex(x1, y1, z1, norm[0], norm[1], norm[2], true);
			surface_.defineVertex(x2, y2, z2, norm[0], norm[1], norm[2], true);
			surface_.defineVertex(x1last, y1last, z1, norm[0], norm[1], norm[2], true);
			surface_.defineVertex(x2last, y2last, z2, norm[0], norm[1], norm[2], true);
			surface_.defineVertex(x2, y2, z2, norm[0], norm[1], norm[2], true);
			
			// Copy current points to last points
			x1last = x1;
			x2last = x2;
			y1last = y1;
			y2last = y2;
		}
	}

	surface_.pushInstance(context());

	msg.print("Surface contains %i vertices.\n", surface_.nDefinedVertices());
}
