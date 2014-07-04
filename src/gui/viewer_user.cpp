/*
	*** Scene Rendering Functions (User)
	*** src/gui/viewer_user.cpp
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

#include "gui/viewer.uih"
#include "gui/uchroma.h"

// Create necessary primitives (must be called before any rendering is done)
void Viewer::createPrimitives()
{
	// Setup primitives
	interactionPrimitive_.setNoInstances();
	interactionBoxPrimitive_.setNoInstances();
	boundingBoxPrimitive_.setNoInstances();
	for (int n=0; n<3; ++n)
	{
		axisPrimitives_[n].initialise(1020, 0, GL_LINES, false);
		axisPrimitives_[n].setNoInstances();
	}
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

// Create/update surface primitive
bool Viewer::updateSurfacePrimitive(Collection* collection, bool forceUpdate)
{
	// Check for valid collection
	if (!collection) return false;

	// Update additional data first
	int nUpdated = 0;
	// -- Fit collections
	for (Collection* fit = collection->fitData(); fit != NULL; fit = fit->next) if (updateSurfacePrimitive(fit, forceUpdate)) ++nUpdated;
	// -- Extracted data
	for (Collection* extract = collection->fitData(); extract != NULL; extract = extract->next) if (updateSurfacePrimitive(extract, forceUpdate)) ++nUpdated;

	// Check whether the primitive for this collection needs updating
	if (collection->displayPrimitivesValid() && collection->colourScaleValid() && (!forceUpdate)) return (nUpdated != 0);

	// Pop old primitive instances and adjust primitive settings
	collection->displayPrimitives().popInstance(context());
	collection->displayPrimitives().forgetAll();
	
	// Recreate primitive depending on current style
	switch (collection->displayStyle())
	{
		case (Collection::LineXYStyle):
			constructLineXYSurface(collection->displayPrimitives(), collection->displayAbscissa(), collection->displayData(), collection->colourScale());
			break;
		case (Collection::LineZYStyle):
			constructLineZYSurface(collection->displayPrimitives(), collection->displayAbscissa(), collection->displayData(), collection->colourScale());
			break;
		case (Collection::GridStyle):
			constructGridSurface(collection->displayPrimitives(), collection->displayAbscissa(), collection->displayData(), collection->colourScale());
			break;
		case (Collection::SurfaceStyle):
			constructFullSurface(collection->displayPrimitives(), collection->displayAbscissa(), collection->displayData(), collection->colourScale());
			break;
	}

	// Push a new instance to create the new display list / vertex array
	collection->displayPrimitives().pushInstance(context(), extensionsStack_.last());
	collection->setDisplayPrimitiveValid();

	return true;
}

// Add supplied surface primitive to list
void Viewer::addCollectionPrimitive(Primitive* primitive)
{
	primitiveList_.add(primitive);
}

// Remove surface primitive from primitive list
void Viewer::removeCollectionPrimitive(Primitive* primitive)
{
	primitiveList_.remove(primitive);
}

// Clear specified axis primitives
void Viewer::clearAxisPrimitives(int axis)
{
	axisPrimitives_[axis].forgetAll();
	axisTextPrimitives_[axis].forgetAll();
}

// Add line to axis primitive
void Viewer::addAxisLine(int axis, Vec3<double> v1, Vec3<double> v2)
{
	axisPrimitives_[axis].defineVertex(v1.x, v1.y, v1.z, 1.0, 0.0, 0.0);
	axisPrimitives_[axis].defineVertex(v2.x, v2.y, v2.z, 1.0, 0.0, 0.0);
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

// Setup interaction primitive
void Viewer::setInteractionPrimitive(int axis)
{
	const int nPoints = 16;
	
	interactionPrimitive_.initialise(nPoints*nPoints*4, nPoints*nPoints*6, GL_TRIANGLES, false);
	interactionPrimitive_.forgetAll();
	interactionBoxPrimitive_.initialise(4, 8, GL_LINES, false);
	interactionBoxPrimitive_.forgetAll();
	if (axis == -1) return;

	// Grab axes, and knock out values in the supplied vectors which correspond to the activated axis
	Vec3<double> axisMinA, axisMinB, axisMaxA, axisMaxB;
	axisMinA[(axis+1)%3] = uChroma_->axisCoordMin((axis+1)%3)[(axis+1)%3];
	axisMaxA[(axis+1)%3] = uChroma_->axisCoordMax((axis+1)%3)[(axis+1)%3];
	axisMinB[(axis+2)%3] = uChroma_->axisCoordMin((axis+2)%3)[(axis+2)%3];
	axisMaxB[(axis+2)%3] = uChroma_->axisCoordMax((axis+2)%3)[(axis+2)%3];
	axisMinA[axis] = 0.0;
	axisMaxA[axis] = 0.0;
	axisMinB[axis] = 0.0;
	axisMaxB[axis] = 0.0;

	// Create 'bounding box' for slice primitive
	Vec3<double> normal(0.0, 0.0, 1.0);
	
	interactionBoxPrimitive_.defineVertex(axisMinA + axisMinB, normal);
	interactionBoxPrimitive_.defineVertex(axisMinA + axisMaxB, normal);
	interactionBoxPrimitive_.defineVertex(axisMaxA + axisMaxB, normal);
	interactionBoxPrimitive_.defineVertex(axisMaxA + axisMinB, normal);
	interactionBoxPrimitive_.defineIndices(0,1);
	interactionBoxPrimitive_.defineIndices(1,2);
	interactionBoxPrimitive_.defineIndices(2,3);
	interactionBoxPrimitive_.defineIndices(3,0);

	// Work out deltas for each direction
	Vec3<double> deltaA, deltaB, pos;
	deltaA = (axisMaxA - axisMinA) / nPoints;
	deltaB = (axisMaxB - axisMinB) / nPoints;

	// Set normal
	normal.zero();
	normal[axis] = 1.0;

	// Construct plane
	GLuint a, b, c, d;
	for (int n=0; n<nPoints; ++n)
	{
		pos = axisMinA + axisMinB + deltaA*n;
		for (int m=0; m<nPoints; ++m)
		{
			a = interactionPrimitive_.defineVertex(pos, normal);
			b = interactionPrimitive_.defineVertex(pos + deltaA, normal);
			c = interactionPrimitive_.defineVertex(pos + deltaA + deltaB, normal);
			d = interactionPrimitive_.defineVertex(pos + deltaB, normal);
			interactionPrimitive_.defineIndices(a, b, c);
			interactionPrimitive_.defineIndices(c, d, a);
			pos += deltaB;
		}
	}
}
