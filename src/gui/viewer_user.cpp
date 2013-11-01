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

// Calculate axis slice selection for given axis at current mouse position
void Viewer::calculateMouseAxisValues()
{
	if (sliceAxis_ != -1)
	{
// 		printf("Test: min=%f, max=%f\n", axisMin_[0], axisMax_[0]);
// 		rMouseLast_.print();
// 		axisCoordMin_[0].print();
		// Project axis coordinates to get a screen-based yardstick
		Vec4<double> axmin = modelToScreen(axisCoordMin_[sliceAxis_]);
		Vec4<double> axmax = modelToScreen(axisCoordMax_[sliceAxis_]);
// 		axmin.print();
// 		axmax.print();

		// Calculate vectors between axis minimum and mouse position (AM) and axis maximum (AB)
		Vec3<double> ab(axmax.x - axmin.x, axmax.y - axmin.y, 0.0);
		Vec3<double> am(rMouseLast_.x - axmin.x, (contextHeight_ - rMouseLast_.y) - axmin.y, 0.0);
		Vec3<double> amNorm = am, abNorm = ab;
		double ratio = am.magnitude() / ab.magnitude();
		abNorm.normalise();
		amNorm.normalise();
		double angle = acos(abNorm.dp(amNorm)) ;
// 		printf("Angle = %f, %f\n", angle, angle * DEGRAD);
		
		// Calculate slice axis value - no need to account for inverted axes here, since this is accounted for by axmin and axmax
		sliceAxisValue_ = abNorm.dp(amNorm)*ratio * (axisMax_[sliceAxis_] - axisMin_[sliceAxis_]) + axisMin_[sliceAxis_];
		
		// Clamp value to data range
		if (sliceAxisValue_ < axisMin_[sliceAxis_]) sliceAxisValue_ = axisMin_[sliceAxis_];
		else if (sliceAxisValue_ > axisMax_[sliceAxis_]) sliceAxisValue_ = axisMax_[sliceAxis_];
// 		printf("ACMAG = %f, X = %f\n", ratio, sliceAxisValue_[sliceAxis_]);

		// Account for log and inverted scales
		if (axisLogarithmic_[sliceAxis_]) emit(sliceAxisValueChanged(sliceAxis_, pow(10.0, sliceAxisValue_)));
		else emit(sliceAxisValueChanged(sliceAxis_, sliceAxisValue_));
	}
	else emit(sliceAxisValueChanged(-1, 0.0));
}

// Create necessary primitives (must be called before any rendering is done)
void Viewer::createPrimitives()
{
	// Setup primitives
	surfacePrimitive_.setColourData(true);
	slicePrimitive_.setNoInstances();
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

// Regenerate slice primitive
void Viewer::regenerateSlicePrimitive()
{
	slicePrimitive_.forgetAll();
	if (sliceAxis_ == -1) return;
	
	const int nPoints = 100;
	double delta, value;
	Vec4<GLfloat> colour(0.0, 0.0, 0.0, 0.5);
	Vec3<double> normal(0.0, 0.0, 0.0);
	
	if (sliceAxis_ == 0)
	{
		value = axisCoordMin_[2].z;
		delta = (axisCoordMax_[2].z - axisCoordMin_[2].z) / nPoints;
		normal[sliceAxis_] = 1.0;
		for (int n=0; n<nPoints; ++n)
		{
			slicePrimitive_.defineVertex(0.0, axisCoordMin_[1].y, value, normal.x, normal.y, normal.z, true);
			slicePrimitive_.defineVertex(0.0, axisCoordMax_[1].y, value, normal.x, normal.y, normal.z, true);
			slicePrimitive_.defineVertex(0.0, axisCoordMax_[1].y, value+delta, normal.x, normal.y, normal.z, true);
			slicePrimitive_.defineVertex(0.0, axisCoordMax_[1].y, value+delta, normal.x, normal.y, normal.z, true);
			slicePrimitive_.defineVertex(0.0, axisCoordMin_[1].y, value+delta, normal.x, normal.y, normal.z, true);
			slicePrimitive_.defineVertex(0.0, axisCoordMin_[1].y, value, normal.x, normal.y, normal.z, true);
			value += delta;
		}
	}
	if (sliceAxis_ == 2)
	{
		value = axisCoordMin_[0].x;
		delta = (axisCoordMax_[0].x - axisCoordMin_[0].x) / nPoints;
		normal[sliceAxis_] = 1.0;
		for (int n=0; n<nPoints; ++n)
		{
			slicePrimitive_.defineVertex(axisCoordMin_[0].x, value, 0.0, normal.x, normal.y, normal.z, true);
			slicePrimitive_.defineVertex(axisCoordMax_[0].x, value, 0.0, normal.x, normal.y, normal.z, true);
			slicePrimitive_.defineVertex(axisCoordMax_[0].x, value+delta, 0.0, normal.x, normal.y, normal.z, true);
			slicePrimitive_.defineVertex(axisCoordMax_[0].x, value+delta, 0.0, normal.x, normal.y, normal.z, true);
			slicePrimitive_.defineVertex(axisCoordMin_[0].x, value+delta, 0.0, normal.x, normal.y, normal.z, true);
			slicePrimitive_.defineVertex(axisCoordMin_[0].x, value, 0.0, normal.x, normal.y, normal.z, true);
			value += delta;
		}
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

// Set source slice data
void Viewer::setSourceSliceList(List<Slice>* sliceData)
{
	sliceData_ = sliceData;
}

// Construct normal / colour data for slice specified
void Viewer::constructSliceData(Slice* targetSlice, double yAxisScale, Array< Vec3<double> >& normals, Array< Vec4<GLfloat> >& colours, ColourScale& colourScale, Slice* previousSlice, Slice* nextSlice)
{
	normals.clear();
	colours.clear();
	if ((previousSlice == NULL) && (nextSlice == NULL)) return;
	
	// Grab references to target arrays
	Array<double>& xTarget = targetSlice->data().arrayX();
	Array<double>& yTarget = targetSlice->data().arrayY();
	int nPoints = xTarget.nItems();
	if (nPoints < 2) return;

	// Get colour data
	int n;
	QColor colour;
	for (n=0; n<nPoints; ++n)
	{
		colour = colourScale.colour((axisLogarithmic_.y ? pow(10.0, yTarget[n]): yTarget[n]) / yAxisScale);
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
		Array<double>& yPrev = previousSlice->data().arrayY();
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
		Array<double>& yNext = nextSlice->data().arrayY();
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
	RefList<Slice,int> slices;
	for (Slice* slice = sliceData_->first(); slice != NULL; slice = slice->next) if (slice->data().nPoints() > 1) slices.add(slice);

	// Construct first slice data and set initial min/max values
	RefListItem<Slice,int>* ri = slices.first();
	constructSliceData(ri->item, yAxisScale_, normA, colourA, colourScale_, NULL, ri->next ? ri->next->item : NULL);
	if (slices.nItems() < 2) return;
	int nPoints = slices.first()->item->data().nPoints();
	
	// Create triangles
	for (RefListItem<Slice,int>* rj = ri->next; rj != NULL; rj = rj->next)
	{
		// Grab slice pointers
		Slice* sliceA = ri->item;
		Slice* sliceB = rj->item;
		Slice* sliceC = (rj->next == NULL ? NULL : rj->next->item);

		// Construct data for current slice
		constructSliceData(sliceB, yAxisScale_, normB, colourB, colourScale_, sliceA, sliceC);

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

// Create axis primitives
void Viewer::createAxis(int axis, Vec3< double > axisPosition, double axisMin, double axisMax, bool inverted, double stretch, double firstTick, double tickDelta, int nMinorTicks, Vec3<double> tickDirection, Vec3<double> labelOrientation, Vec4<double> titleOrientation, int titleAlignment, QString titleText)
{
	QString s;
	FTBBox boundingBox;
	Vec3<double> centre;
	double dpX, dpY, textWidth;

	// Store min/max values and coordinates
	axisLogarithmic_[axis] = false;
	axisInverted_[axis] = inverted;
	axisStretch_[axis] = stretch;
	axisMin_[axis] = axisMin;
	axisMax_[axis] = axisMax;
	axisCoordMin_[axis] = axisPosition;
	axisCoordMin_[axis].set(axis, (inverted ? axisMax : axisMin) * stretch);
	axisCoordMax_[axis] = axisPosition;
	axisCoordMax_[axis].set(axis, (inverted ? axisMin : axisMax) * stretch);
	
	// Draw a line from min to max limits, passing through the defined axisPosition
	axisPrimitives_[axis].plotLine(axisCoordMin_[axis], axisCoordMax_[axis]);

	// Check tickDelta
	if (((axisMax-axisMin) / tickDelta) > 1e6) return;

	// Normalise tickDirection and titleOffsetDir
	tickDirection.normalise();

	// Create label and title transformation matrices
	Matrix A, baseLabelTransform;
	Vec3<double> leftToRight(0.0, 0.0, 0.0), upVector(0.0, 0.0, 0.0);
	// Set left-to-right direction of text to be along axis direction
	leftToRight.set(axis, 1.0);
	// Our basic 'up' vector will be another orthogonal axis (but never z)
	if (axis == 0) upVector.set(0.0, 1.0, 0.0);
	else upVector.set(-1.0, 0.0, 0.0);

	baseLabelTransform.setColumn(0, leftToRight, 0.0);
	baseLabelTransform.setColumn(1, upVector, 0.0);
	baseLabelTransform.setColumn(2, leftToRight * upVector, 0.0);
	baseLabelTransform.setColumn(3, 0.0, 0.0, 0.0, 1.0);

// 	if (labelFaceViewer_)
// 	{
// 		labelTransform = ui.MainView->viewMatrix();
// 		labelTransform.removeTranslationAndScaling();
// 		labelTransform.invert();
// 
// 		titleTransform = ui.MainView->viewMatrix();
// 		titleTransform.removeTranslationAndScaling();
// 		titleTransform.invert();
// 	}
// 	else
	{
		// Label Transform
		// -- Apply rotation out of AB plane...
// 			labelTransform.applyTranslation(0.0, 1.0, 0.0);
// 		Matrix rotMat;
// 		rotMat.createRotationZ(axisLabelOrientation_[axis].y);
// 		rotMat.print();
// 		rotMat.createRotationAxis(0.0, 0.0, -1.0, axisLabelOrientation_[axis].y, false);
// 		rotMat.print();
// 		labelTransform *= rotMat;
// // 			labelTransform.applyRotationAxis(0.0, 0.0, -1.0, axisLabelOrientation_[axis].y, false);
// // 			labelTransform.applyRotationAxis(leftToRight.x, leftToRight.y, leftToRight.z, axisLabelOrientation_[axis].x, false);
// 		upVector = labelTransform.columnAsVec3(1);
		// ...and in current text planeY
	}
	// Grab label translation vector from transform matrix, and remove it
// 	Vec3<double> labelTranslate = labelTransform.columnAsVec3(3);
// 	labelTransform.removeTranslationAndScaling();

	// Plot tickmarks - maximum of 100 minor tickmarks between major lines
	int count = 0;
	double delta = tickDelta / (nMinorTicks+1);
	double value = firstTick;
	Vec3<double> u = axisCoordMin_[axis];
	u.set(axis, (inverted ? (axisMax - firstTick) + axisMin : firstTick) * stretch);
	while (value <= axisMax)
	{
		// Draw tick here, only if value >= limitMin_
		if (value >= axisMin)
		{
			if (count %(nMinorTicks+1) == 0)
			{
				axisPrimitives_[axis].plotLine(u, u+ tickDirection *0.1);
				
				// Determine resulting text primitive width (so we can do label rotations)
				s = QString::number(value);
				if (font_) boundingBox = font_->BBox(qPrintable(s));
				textWidth = fabs(boundingBox.Upper().X() - boundingBox.Lower().X());

				// Create transformation matrix for text label
				A.setIdentity();
				// 1) Translate to anchor point of text
// 				A.applyTranslation(textWidth*0.5, fontBaseHeight_*0.5, 0.0);
				// 2) Apply rotation in plane of text
				// 3) Translate to 'end of tick' position
				// 4) Perform axial rotation
				A.applyRotationAxis(leftToRight.x, leftToRight.y, leftToRight.z, labelOrientation.x, false);
				A.applyTranslation(upVector * labelOrientation.z);
				A.applyRotationAxis(0.0, 0.0, 1.0, labelOrientation.y, false);
				A.applyScaling(labelScale_, labelScale_, labelScale_);

				// Project the tick direction onto the X and Y axes of the basic text projection matrix
				// The matrix has already had the labelScale_ factor applied...
// 				dpX = tickDirection.dp(labelTransform.columnAsVec3(0));
// 				dpY = tickDirection.dp(labelTransform.columnAsVec3(1));
				dpX = 0.0;
				dpY = 0.0;

				// Construct final centre coordinate and rotation matrix
// 				centre.set(-textWidth*0.5*labelScale_ + dpX*textWidth*0.5, -fontBaseHeight_*0.5*labelScale_ + dpY*fontBaseHeight_*0.5, 0.0);
				centre.set(-textWidth*0.5*labelScale_, -fontBaseHeight_*0.5*labelScale_, 0.0);
				axisTextPrimitives_[axis].add(s, u, A);
				
				count = 0;
			}
			else axisPrimitives_[axis].plotLine(u, u+ tickDirection *0.05);
		}
		u.add(axis, inverted ? -delta*stretch : delta*stretch);
		value += delta;
		++count;
	}

// 	// Draw axis title
// 	Vec3<double> textSize;
// 	if (font_) boundingBox = font_->BBox(qPrintable(titleText));
// 	textSize.set(fabs(boundingBox.Upper().X() - boundingBox.Lower().X()), fontBaseHeight_);
// 	textSize = titleTransform * textSize;
// 
// // 	dpX = titleOffsetVector.dp(titleTransform.columnAsVec3(0));
// // 	dpY = titleOffsetVector.dp(titleTransform.columnAsVec3(1));
// 
// 	// Set general (horizontal) centre of text
// 	centre = axisCoordMin_[axis] + (axisCoordMax_[axis] - axisCoordMin_[axis]) * titleHOffset;
// 
// 	// Apply offset away from axis line - the translation vector of titleTransform is the direction we move in (we also remove it afterwards)
// 	centre += titleTransform.columnAsVec3(3)*titleDistance*titleScale_;
// 
// 	// Adjust anchor point to reflect text alignment chosen
// 	if (titleAlignment == UChromaWindow::LeftAlign) centre.add(0.0, -textSize.y*0.5*titleScale_, 0.0);
// 	else if (titleAlignment == UChromaWindow::RightAlign) centre.add(-textSize.x*titleScale_, -textSize.y*0.5*titleScale_, 0.0);
// 	else centre.add(-textSize.x*0.5*titleScale_, -textSize.y*0.5*titleScale_, 0.0);
// 
// 	titleTransform.removeTranslationAndScaling();
// 	axisTextPrimitives_[axis].add(titleText, centre, titleTransform);
}

// Create logarithmic axis primitives
void Viewer::createLogAxis(int axis, Vec3<double> axisPosition, double axisMin, double axisMax, bool inverted, double stretch, int nMinorTicks, Vec3<double> tickDirection, Matrix& labelTransform)
{
	// For the log axis, the associated surface data coordinate will already be in log form
	if ((axisMin < 0.0) || (axisMax < 0.0))
	{
		msg.print("Axis range is inappropriate for a log scale (%f < x < %f). Axis will not be drawn.\n", axisMin, axisMax);
		return;
	}

	// Enforce sensible minimum for log axes
	if (axisMin <= 0.0) axisMin = 1.0e-10;

	// Store min/max values and coordinates
	axisLogarithmic_[axis] = true;
	axisInverted_[axis] = inverted;
	axisStretch_[axis] = stretch;
	axisMin_[axis] = log10(axisMin);
	axisMax_[axis] = log10(axisMax);
	axisCoordMin_[axis] = axisPosition;
	axisCoordMin_[axis].set(axis, (inverted ? log10(axisMax/axisMin) : log10(axisMin)) * stretch);
	axisCoordMax_[axis] = axisPosition;
	axisCoordMax_[axis].set(axis, (inverted ? 0.0 : log10(axisMax)) * stretch);
	
	QString s;
	FTBBox boundingBox;
	Vec3<double> centre;
	double dpX, dpY, textWidth;

	// Draw a line from min to max range, passing through the defined axisPosition
	axisPrimitives_[axis].plotLine(axisCoordMin_[axis], axisCoordMax_[axis]);

	// Plot tickmarks
	if (nMinorTicks > 8) nMinorTicks = 8;
	// Start at floored (ceiling'd) integer of logAxisMin (logAxisMax), and go from there.
	int count = 0;
	double power = floor(axisMin_[axis]);
	double value = pow(10,power);
	Vec3<double> u = axisCoordMin_[axis];
	while (true)
	{
		// Check break condition
		if (value > axisMax) break;

		// If the current value is in range, plot a tick
		u[axis] = (inverted ? log10(axisMax/value) : log10(value)) * stretch;
		if (value >= axisMin)
		{
			// Tick mark
			axisPrimitives_[axis].plotLine(u, u+tickDirection*(count == 0 ? 0.1 : 0.05));

			// Tick label
			if (count == 0)
			{
				// Determine resulting text primitive width (so we can do label rotations)
				s = QString::number(value);
				if (font_) boundingBox = font_->BBox(qPrintable(s));
				textWidth = fabs(boundingBox.Upper().X() - boundingBox.Lower().X());

				// Project the tick direction onto the X and Y axes of the basic text projection matrix
				// The matrix has already had the labelScale_ factor applied...
				dpX = tickDirection.dp(labelTransform.columnAsVec3(0));
				dpY = tickDirection.dp(labelTransform.columnAsVec3(1));

				// Construct final centre coordinate and rotation matrix
				centre.set(-textWidth*0.5*labelScale_ + dpX*textWidth*0.5, -fontBaseHeight_*0.5*labelScale_ + dpY*fontBaseHeight_*0.5, 0.0);
				axisTextPrimitives_[axis].add(s, centre + (u + tickDirection*0.1), labelTransform);
			}
		}

		// Increase tick counter, value, and power if necessary
		++count;
		if (count > nMinorTicks)
		{
			count = 0;
			power = power + 1.0;
			value = pow(10,power);
		}
		else value += pow(10,power);
	}
}

// Create axis title
void Viewer::createAxisTitle(int axis, bool inverted, double stretch, bool logarithmic, QString title, Vec3<double> direction, Vec3<double> up, int zrotation)
{
	// All axis min/max and coordinate values have already been set by calls to Viewer::createAxis()...
	FTBBox boundingBox;

	// Plot label string
	if (font_) boundingBox = font_->BBox(qPrintable(title));
// 	axisTextPrimitives_[axis].add(title, titleScale_, fontBaseHeight_, fabs(boundingBox.Upper().X() - boundingBox.Lower().X()), (axisCoordMax_[axis]+axisCoordMin_[axis])*0.5, direction, up, zrotation);
}

// Set whether axis is visible
void Viewer::setAxisVisible(int axis, bool visible)
{
	axisVisible_[axis] = visible;
}

// Create bounding box
void Viewer::createBoundingBox(int type, double planeY)
{
	boundingBoxPrimitive_.forgetAll();
	
	if (type == 0) return;
	else if (type == 1)
	{
		// Plane in XZ, spanning data range
		boundingBoxPrimitive_.plotLine(Vec3<double>(axisCoordMin_[0].x, planeY, axisCoordMin_[2].z), Vec3<double>(axisCoordMin_[0].x, planeY, axisCoordMax_[2].z));
		boundingBoxPrimitive_.plotLine(Vec3<double>(axisCoordMin_[0].x, planeY, axisCoordMax_[2].z), Vec3<double>(axisCoordMax_[0].x, planeY, axisCoordMax_[2].z));
		boundingBoxPrimitive_.plotLine(Vec3<double>(axisCoordMax_[0].x, planeY, axisCoordMax_[2].z), Vec3<double>(axisCoordMax_[0].x, planeY, axisCoordMin_[2].z));
		boundingBoxPrimitive_.plotLine(Vec3<double>(axisCoordMax_[0].x, planeY, axisCoordMin_[2].z), Vec3<double>(axisCoordMin_[0].x, planeY, axisCoordMin_[2].z));
	}
}

// Set surface centre
void Viewer::setSurfaceCenter(Vec3<double> center)
{
	surfaceCenter_ = center;
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

// Set scale factor for axis labels
void Viewer::setLabelScale(double scale)
{
	labelScale_ = scale;
}

// Set scale factor for titles
void Viewer::setTitleScale(double scale)
{
	titleScale_ = scale;
}

// Set current slice axis
void Viewer::setSliceAxis(int axis)
{
	sliceAxis_ = axis;
	regenerateSlicePrimitive();
	calculateMouseAxisValues();
	repaint();
}
