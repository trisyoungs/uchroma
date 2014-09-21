/*
	*** Surface Generation - Line XY
	*** src/render/surface_linexy.cpp
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
#include "base/axes.h"

// Construct line representation of data in XY slices
void Surface::constructLineXY(PrimitiveList& primitiveList, const Axes& axes, const Array<double>& displayAbscissa, List<DisplayDataSet>& displayData, ColourScale colourScale)
{
	// Forget all data in current primitives
	primitiveList.forgetAll();

	printf("It's LineXY time...\n");
	// Get extents of displayData to use based on current axes limits
	Vec3<int> minIndex, maxIndex;
	if (!calculateExtents(axes, displayAbscissa, displayData, minIndex, maxIndex)) return;
	int nZ = (maxIndex.z - minIndex.z) + 1;

	// Copy and transform abscissa values (still in data space) into axes coordinates
	Array<double> x(displayAbscissa, minIndex.x, maxIndex.x);
	axes.transformX(x);
	int nX = x.nItems();
	if (nX < 2) return;
	
	// Resize primitive list so it's large enough for our needs
	primitiveList.reinitialise(nZ, true, nX, (nX-1)*2, GL_LINES, true);

	// Get some values from axes so we can calculate colours properly
	bool yLogarithmic = axes.logarithmic(1);
	double yStretch = axes.stretch(1);

	// Temporary variables
	GLfloat z;
	Vec4<GLfloat> colour(0,0,0,1);
	int n, nPoints;
	Vec3<double> nrm(0.0, 1.0, 0.0);
	Array<double> y;
	Array<DisplayDataSet::DataPointType> yType;

	DisplayDataSet** slices = displayData.array();
	Primitive* currentPrimitive = primitiveList[0];

	// Create lines for slices
	GLuint vertexA, vertexB;
	for (int slice = minIndex.z; slice <= maxIndex.z; ++slice)
	{
		// Grab y and z values
		y.copy(slices[slice]->y(), minIndex.x, maxIndex.x);
		axes.transformY(y);
		yType.copy(slices[slice]->yType(), minIndex.x, maxIndex.x);
		z = axes.transformZ(slices[slice]->z());

		// Reset vertexA to -1 so we don't draw a line at n=0
		vertexA = -1;

		// Loop over x values
		for (n=0; n<nX; ++n)
		{
			// Define vertex index for this point (if one exists)
			if (yType.value(n) != DisplayDataSet::NoPoint)
			{
				colourScale.colour((yLogarithmic ? pow(10.0, y.value(n)) : y.value(n)) / yStretch, colour);
				vertexB = currentPrimitive->defineVertex(x.value(n), y.value(n), z, nrm, colour);
			}
			else vertexB = -1;

			// If both vertices are valid, plot a line
			if ((vertexA != -1) && (vertexB != -1)) currentPrimitive->defineIndices(vertexA, vertexB);

			vertexA = vertexB;
		}

		// Move to next primitive
		currentPrimitive = currentPrimitive->next;
	}
}
