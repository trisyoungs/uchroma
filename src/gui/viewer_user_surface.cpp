/*
	*** Scene Rendering Functions (User) - Surface Generation
	*** src/gui/viewer_user_surface.cpp
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

// Construct normal / colour data for slice specified
void Viewer::constructSurfaceStrip(Data2D* targetSlice, double yAxisScale, Array< Vec3<double> >& normals, Array< Vec4<GLfloat> >& colours, const ColourScale& colourScale, Data2D* previousSlice, Data2D* nextSlice)
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

// Construct full surface representation of data
void Viewer::constructFullSurface(Primitive& primitive, List<Data2D>& displayData, ColourScale colourScale)
{
	// Set primitive options
	primitive.setType(GL_TRIANGLES);

	GLfloat zA, zB;

	// Sanity check - are there enough slices to proceed?
	if (displayData.nItems() < 2) return;

	// Temporary variables
	Array< Vec3<double> > normA, normB;
	Array< Vec4<GLfloat> > colourA, colourB;
	int n;
	QColor colour;
	double yAxisScale = uChroma_->axisStretch(1);
	Vec3<double> nrm(0.0, 1.0, 0.0);

	// We need to skip over slices with zero points, so construct a reflist here of those which contain usable data
	RefList<Data2D,int> slices;
	for (Data2D* slice = displayData.first(); slice != NULL; slice = slice->next) if (slice->nPoints() > 1) slices.add(slice);

	// If there are not enough slices with a valid number of points
	if (slices.nItems() < 2) return;

	// Construct first slice data and set initial min/max values
	RefListItem<Data2D,int>* ri = slices.first();
	constructSurfaceStrip(ri->item, yAxisScale, normA, colourA, colourScale, NULL, ri->next ? ri->next->item : NULL);
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
		constructSurfaceStrip(sliceB, yAxisScale, normB, colourB, colourScale, sliceA, sliceC);

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
			primitive.defineVertex(xA[n], yA[n], zA, normA[n], colourA[n], true);
			primitive.defineVertex(xA[n+1], yA[n+1], zA, normA[n+1], colourA[n+1], true);
			primitive.defineVertex(xB[n+1], yB[n+1], zB, normB[n+1], colourB[n+1], true);
			primitive.defineVertex(xA[n], yA[n], zA, normA[n], colourA[n], true);
			primitive.defineVertex(xB[n], yB[n], zB, normB[n], colourB[n], true);
			primitive.defineVertex(xB[n+1], yB[n+1], zB, normB[n+1], colourB[n+1], true);
		}

		// Copy arrays ready for next pass
		normA = normB;
		colourA = colourB;
		ri = rj;
	}
}
