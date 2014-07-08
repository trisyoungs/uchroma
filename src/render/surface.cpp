/*
	*** Surface Generation
	*** src/render/surface.cpp
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

#include "render/surface.h"

// Construct normal / colour data for vertex specified
Vec3<double> Surface::constructVertexNormals(const Array<double>& abscissa, int index, DisplayDataSet* targetDataSet, DisplayDataSet* previousDataSet, DisplayDataSet* nextDataSet, int nPoints)
{
	Vec3<double> normals;

	// Grab references to first target array
	const Array<double>& yTarget = targetDataSet->y();

	// Check whether previous / next slice information is available
	if ((previousDataSet == NULL) && (nextDataSet == NULL))
	{
		// Neither previous nor next slices exist - the normal will depend only on the previous and/or next points

		// Check on index provided
		if (index == 0)
		{
			// First point in array, so no previous points available
		}
		else if (index == (nPoints-1))
		{
			// Last point in array, so no next point available
		}
		else
		{
			// Point somewhere in middle of array, so have previous and next points
		}
	}
	else if (previousDataSet == NULL)
	{
		// No previous slice available, so normals depend on this and next slice only

		// Check on index provided
		if (index == 0)
		{
			// First point in array, so no previous points available
		}
		else if (index == (nPoints-1))
		{
			// Last point in array, so no next point available
		}
		else
		{
			// Point somewhere in middle of array, so have previous and next points
		}
	}
	else if (nextDataSet == NULL)
	{
		// No next slice available, so normals depend on this and previous slice only

		// Check on index provided
		if (index == 0)
		{
			// First point in array, so no previous points available
		}
		else if (index == (nPoints-1))
		{
			// Last point in array, so no next point available
		}
		else
		{
			// Point somewhere in middle of array, so have previous and next points
		}
	}
	else
	{
		// Both previous and next slices available, so use all available data
	}

}

// Construct normal / colour data for slice specified
void Surface::constructSurfaceStrip(const Array<double>& abscissa, DisplayDataSet* targetSlice, double yAxisScale, bool yLogarithmic, Array< Vec3<double> >& normals, Array< Vec4<GLfloat> >& colours, const ColourScale& colourScale, DisplayDataSet* previousSlice, DisplayDataSet* nextSlice)
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
		colour = colourScale.colour((yLogarithmic ? pow(10.0, yTarget.value(n)): yTarget.value(n)) / yAxisScale);
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
