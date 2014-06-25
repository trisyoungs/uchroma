/*
	*** Scene Rendering Functions (User) - Grid Surface Generation
	*** src/gui/viewer_user_grid.cpp
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

// Construct normal / colour data for vertex specified
Vec3<double> Viewer::constructVertexNormals(const Array<double>& abscissa, int index, DisplaySlice* targetSlice, DisplaySlice* previousSlice, DisplaySlice* nextSlice, int nPoints)
{
	Vec3<double> normals;

	// Grab references to first target array
	const Array<double>& yTarget = targetSlice->y();

	// Check whether previous / next slice information is available
	if ((previousSlice == NULL) && (nextSlice == NULL))
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
	else if (previousSlice == NULL)
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
	else if (nextSlice == NULL)
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

// Construct grid surface representation of data
void Viewer::constructGridSurface(PrimitiveList& primitives, const Array<double>& abscissa, List<DisplaySlice>& displayData, ColourScale colourScale)
{
	// Forget all data in current primitives
	primitives.forgetAll();

	// Return if no data
	if (displayData.nItems() < 1) return;

	// Decide how large to make VertexChunks in Primitives
	// We will consider multiple slices at once in order to take most advantage of the post-transform cache
	// forming (abscissa.nItems()/(cacheSize-1)+1) Primitives (we divide by (cacheSize-1) because we will force an overlap
	// of one gridpoint between adjacent strips).
	const int cacheSize = 12;
	int nPrimitives = abscissa.nItems()/(cacheSize-1) + 1;
	int maxVertices = cacheSize*displayData.nItems();
	int maxIndices = displayData.nItems()*(cacheSize-1) + (displayData.nItems()-1)*cacheSize;

	// Reinitialise primitive list
	primitives.reinitialise(nPrimitives, true, maxVertices, maxIndices*10, GL_LINES, true);

	// Temporary variables
	int n, nPoints, offset = 0, i, nLimit, nMax;
	Vec4<GLfloat> colour;
	GLfloat zA, zB;
	double yAxisScale = uChroma_->axisStretch(1);
	Vec3<double> nrm(0.0, 1.0, 0.0);

	DisplaySlice** slices = displayData.array(), slice;
	Primitive* currentPrimitive = primitives[0];
	GLuint verticesA[cacheSize], verticesB[cacheSize];
	double z;

	// Loop over abscissa indices
	while (offset <= abscissa.nItems())
	{
		// Set nLimit to ensure we don't go beyond the end of the data arrays
		nLimit = std::min(cacheSize, abscissa.nItems()-offset);

		// Grab arrays from first slice
		const Array<double>& y = slices[0]->y();
		const Array<DisplaySlice::DataPointType>& yType = slices[0]->yType();
		z = slices[0]->z();
		
		// Generate lines / vertices for first row
		for (n=0; n<nLimit; ++n)
		{
			i = offset+n;
			if (yType.value(i) != DisplaySlice::NoPoint)
			{
				// A value exists here, so define a vertex
				colourScale.colour((uChroma_->axisLogarithmic(1) ? pow(10.0, y.value(i)) : y.value(i)) / yAxisScale, colour);
				verticesA[n] = currentPrimitive->defineVertex(abscissa.value(i), y.value(i), z, nrm, colour);

				// If the previous vertex also exists, draw a line here
				if ((n != 0) && (verticesA[n-1] != -1)) currentPrimitive->defineIndices(verticesA[n-1], verticesA[n]);
			}
			else verticesA[n] = -1;
		}

		// Loop over remaining displayData
		for (int slice = 1; slice < displayData.nItems(); ++slice)
		{
			// Grab arrays
			const Array<double>& y = slices[slice]->y();
			const Array<DisplaySlice::DataPointType>& yType = slices[slice]->yType();
			z = slices[slice]->z();

			// Generate vertices for this row
			for (n=0; n<nLimit; ++n)
			{
				i = offset+n;
				if (yType.value(offset+n) != DisplaySlice::NoPoint)
				{
					// A value exists here, so define a vertex
					colourScale.colour((uChroma_->axisLogarithmic(1) ? pow(10.0, y.value(i)) : y.value(i)) / yAxisScale, colour);
					verticesB[n] = currentPrimitive->defineVertex(abscissa.value(i), y.value(i), z, nrm, colour);

					// If the previous vertex on this row also exists, draw a line here
					if ((n != 0) && (verticesB[n-1] != -1)) currentPrimitive->defineIndices(verticesB[n-1], verticesB[n]);
				}
				else verticesB[n] = -1;
			}

			// Draw lines across slices
			nMax= (displayData.nItems()-slice) > 1 ? nLimit-1 : nLimit;
			for (n=0; n<nMax; ++n)
			{
				if ((verticesA[n] != -1) && (verticesB[n] != -1)) currentPrimitive->defineIndices(verticesA[n], verticesB[n]);
			}

			// Store current vertices for next pass
			for (n=0; n<cacheSize; ++n) verticesA[n] = verticesB[n];
		}

		// Move to next primitive and increase offset
		currentPrimitive = currentPrimitive->next;
		offset += cacheSize-1;
	}
}
