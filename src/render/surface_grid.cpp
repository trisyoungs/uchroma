/*
	*** Surface Generation - Grid
	*** src/render/surface_grid.cpp
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

// Construct grid surface representation of data
void Surface::constructGrid(PrimitiveList& primitives, double yAxisScale, bool yLogarithmic, const Array<double>& abscissa, List<DisplayDataSet>& displayData, ColourScale colourScale)
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
	Vec3<double> nrm(0.0, 1.0, 0.0);

	DisplayDataSet** slices = displayData.array(), slice;
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
		const Array<DisplayDataSet::DataPointType>& yType = slices[0]->yType();
		z = slices[0]->z();
		
		// Generate lines / vertices for first row
		for (n=0; n<nLimit; ++n)
		{
			i = offset+n;
			if (yType.value(i) != DisplayDataSet::NoPoint)
			{
				// A value exists here, so define a vertex
				colourScale.colour((yLogarithmic ? pow(10.0, y.value(i)) : y.value(i)) / yAxisScale, colour);
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
			const Array<DisplayDataSet::DataPointType>& yType = slices[slice]->yType();
			z = slices[slice]->z();

			// Generate vertices for this row
			for (n=0; n<nLimit; ++n)
			{
				i = offset+n;
				if (yType.value(offset+n) != DisplayDataSet::NoPoint)
				{
					// A value exists here, so define a vertex
					colourScale.colour((yLogarithmic ? pow(10.0, y.value(i)) : y.value(i)) / yAxisScale, colour);
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
