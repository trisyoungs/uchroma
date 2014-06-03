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
void Viewer::constructSurfaceStrip(const Array<double>& abscissa, DisplaySlice* targetSlice, double yAxisScale, Array< Vec3<double> >& normals, Array< Vec4<GLfloat> >& colours, const ColourScale& colourScale, DisplaySlice* previousSlice, DisplaySlice* nextSlice)
{
	normals.clear();
	colours.clear();
	if ((previousSlice == NULL) && (nextSlice == NULL)) return;
	
	// Grab references to target arrays
	const Array<double>& yTarget = targetSlice->y();
	int nPoints = abscissa.nItems();
	if (nPoints < 2) return;

	// Get colour data
	int n;
	QColor colour;
	for (n=0; n<nPoints; ++n)
	{
		colour = colourScale.colour((uChroma_->axisLogarithmic(1) ? pow(10.0, yTarget.value(n)): yTarget.value(n)) / yAxisScale);
		colours.add(Vec4<GLfloat>(colour.redF(), colour.greenF(), colour.blueF(), colour.alphaF()));
	}

	// Calculate normals
	Vec3<double> v1, v2, v3;
	double dz;
	if (previousSlice && nextSlice)
	{
		// Grab other array references
		const Array<double>& yPrev = previousSlice->y();
		const Array<double>& yNext = nextSlice->y();
		dz = previousSlice->z() - nextSlice->z();

		// -- First point
		v1.set(abscissa.value(1) - abscissa.value(0), yTarget.value(1) - yTarget.value(0), 0);
		v2.set(0.0, yNext.value(0) - yPrev.value(0), dz);
		normals.add(v1 * v2);
// 		v3 = v1 * v2;
// 		v3.normalise();
// 		normals.add(v3);
// 		printf("Norm %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", 0, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		// -- Points 1 to N-2
		for (n=1; n<nPoints-1; ++n)
		{
			v1.set(abscissa.value(n+1) - abscissa.value(n-1), yTarget.value(n+1) - yTarget.value(n-1), 0.0);
			v2.set(0.0, yNext.value(n) - yPrev.value(n), dz);
			normals.add(v1 * v2);
// 			v3 = v1 * v2;
// 			v3.normalise();
// 			normals.add(v3);
// 			printf("Norm %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", n, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		}
		// -- Last point
		v1.set(abscissa.value(nPoints-1) - abscissa.value(nPoints-2), yTarget.value(nPoints-1) - yTarget.value(nPoints-2), 0.0);
		v2.set(0.0, yPrev.value(nPoints-1) - yNext.value(nPoints-1), dz);
		normals.add(v1 * v2);
// 		v3 = v1 * v2;
// 		v3.normalise();
// 		normals.add(v3);
// 		printf("Norm %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", nPoints-1, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
	}
	else if (previousSlice)
	{
		// Grab other array reference
		const Array<double>& yPrev = previousSlice->y();
		dz = previousSlice->z() - targetSlice->z();

		// -- First point
		v1.set(abscissa.value(1) - abscissa.value(0), yTarget.value(1) - yTarget.value(0), 0.0);
		v2.set(0.0, yTarget.value(0) - yPrev.value(0), dz);
		normals.add(v1 * v2);
// 		v3 = v1 * v2;
// 		v3.normalise();
// 		normals.add(v3);
// 		printf("Last %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", 0, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		// -- Points 1 to N-2
		for (n=1; n<nPoints-1; ++n)
		{
			v1.set(abscissa.value(n+1) - abscissa.value(n-1), yTarget.value(n+1) - yTarget.value(n-1), 0.0);
			v2.set(0.0, yTarget.value(n) - yPrev.value(n), dz);
			normals.add(v1 * v2);
// 			v3 = v1 * v2;
// 			v3.normalise();
// 			normals.add(v3);
// 			printf("Last %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", n, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		}
		// -- Last point
		v1.set(abscissa.value(nPoints-1) - abscissa.value(nPoints-2), yTarget.value(nPoints-1) - yTarget.value(nPoints-2), 0.0);
		v2.set(0.0, yTarget.value(nPoints-1) - yPrev.value(nPoints-1), dz);
		normals.add(v1 * v2);
// 		v3 = v1 * v2;
// 		v3.normalise();
// 		normals.add(v3);
// 		printf("Last %i = (%f %f %f) * (%f %f %f) = (%f %f %f\n", nPoints-1, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
	}
	else
	{
		// Grab other array reference
		const Array<double>& yNext = nextSlice->y();
		dz = targetSlice->z() - nextSlice->z();

		// -- First point
		v1.set(abscissa.value(1) - abscissa.value(0), yTarget.value(1) - yTarget.value(0), 0.0);
		v2.set(0.0, yNext.value(0) - yTarget.value(0), dz);
		normals.add(v1 * v2);
// 		v3 = v1 * v2;
// 		v3.normalise();
// 		normals.add(v3);
// 		printf("Frst %i = (%f %f %f) * (%f %f %f) = (%f %f %f)\n", 0, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		// -- Points 1 to N-2
		for (n=1; n<nPoints-1; ++n)
		{
			v1.set(abscissa.value(n+1) - abscissa.value(n-1), yTarget.value(n+1) - yTarget.value(n-1), 0.0);
			v2.set(0.0, yNext.value(n) - yTarget.value(n), dz);
			normals.add(v1 * v2);
// 			v3 = v1 * v2;
// 			v3.normalise();
// 			normals.add(v3);
// 			printf("Frst %i = (%f %f %f) * (%f %f %f) = (%f %f %f)\n", n, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
		}
		// -- Last point
		v1.set(abscissa.value(nPoints-1) - abscissa.value(nPoints-2), yTarget.value(nPoints-1) - yTarget.value(nPoints-2), 0.0);
		v2.set(0.0, yNext.value(nPoints-1) - yTarget.value(nPoints-1), dz);
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
void Viewer::constructFullSurface(Primitive& primitive, const Array<double>& abscissa, List<DisplaySlice>& displayData, ColourScale colourScale)
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

	// Return now if there are not enough slices to work with
	if (displayData.nItems() < 2) return;

	// Construct first slice data and set initial min/max values
	DisplaySlice* sliceA = displayData.first();
	constructSurfaceStrip(abscissa, sliceA, yAxisScale, normA, colourA, colourScale, NULL, sliceA->next);
	int nPoints = abscissa.nItems();
	
	// Create triangles
	for (DisplaySlice* sliceB = sliceA->next; sliceB != NULL; sliceB = sliceB->next)
	{
		// Grab slice pointers
		DisplaySlice* sliceC = (sliceB->next == NULL ? NULL : sliceB->next);

		// Construct data for current slice
		constructSurfaceStrip(abscissa, sliceB, yAxisScale, normB, colourB, colourScale, sliceA, sliceC);

		// Grab z values
		zA = (GLfloat) sliceA->z();
		zB = (GLfloat) sliceB->z();
	
		// Get nPoints, and initial coordinates
		const Array<double>& yA = sliceA->y();
		const Array<double>& yB = sliceB->y();
		for (int n=0; n<nPoints-1; ++n)
		{
			// Add triangles for this quadrant
			primitive.defineVertex(abscissa.value(n), yA.value(n), zA, normA[n], colourA[n], true);
			primitive.defineVertex(abscissa.value(n+1), yA.value(n+1), zA, normA[n+1], colourA[n+1], true);
			primitive.defineVertex(abscissa.value(n+1), yB.value(n+1), zB, normB[n+1], colourB[n+1], true);
			primitive.defineVertex(abscissa.value(n), yA.value(n), zA, normA[n], colourA[n], true);
			primitive.defineVertex(abscissa.value(n), yB.value(n), zB, normB[n], colourB[n], true);
			primitive.defineVertex(abscissa.value(n+1), yB.value(n+1), zB, normB[n+1], colourB[n+1], true);
		}

		// Copy arrays ready for next pass
		normA = normB;
		colourA = colourB;
		sliceA = sliceB;
	}
}
