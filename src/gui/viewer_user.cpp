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
void Viewer::updateSurfacePrimitive(Collection* collection, bool forceUpdate)
{
	// Check for valid collection, and whether primitive needs updating
	if (!collection) return;
	if (collection->displayPrimitivesValid() && collection->colourScaleValid() && (!forceUpdate)) return;

	// Pop old primitive instances and adjust primitive settings
	collection->displayPrimitives().popInstance(context());
	collection->displayPrimitives().forgetAll();
	
	// Recreate primitive depending on current style
	switch (collection->displayStyle())
	{
		case (Collection::LineStyle):
			constructLineSurface(collection->displayPrimitives(), collection->displayAbscissa(), collection->displayData(), collection->colourScale());
			break;
		case (Collection::GridStyle):
			break;
		case (Collection::SurfaceStyle):
			constructFullSurface(collection->displayPrimitives(), collection->displayAbscissa(), collection->displayData(), collection->colourScale());
			break;
	}

	// Push a new instance to create the new display list / vertex array
	collection->displayPrimitives().pushInstance(context());
	collection->setDisplayPrimitiveValid();
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
