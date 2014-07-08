/*
	*** Surface Generation - Line ZY
	*** src/render/surface_linezy.cpp
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

// Construct line representation of data in ZY slices
void Surface::constructLineZY(PrimitiveList& primitives, double yAxisScale, bool yLogarithmic, const Array<double>& abscissa, List<DisplayDataSet>& displayData, ColourScale colourScale)
{
	// Forget all data in current primitives
	primitives.forgetAll();

	// Check for low number of datasets or low number of points in x
	if (displayData.nItems() < 2) return;
	if (abscissa.nItems() < 1) return;

	// Resize primitive list so it's large enough for our needs
	primitives.reinitialise(abscissa.nItems(), true, displayData.nItems(), (displayData.nItems()-1)*2, GL_LINES, true);

	// Check for low number of datasets
	if (displayData.nItems() == 0) return;

	// Temporary variables
	Vec4<GLfloat> colour(0,0,0,1);
	int n, nPoints;
	Vec3<double> nrm(0.0, 1.0, 0.0);

	Primitive* currentPrimitive = primitives[0];

	// Create lines for slices
	GLuint vertexA, vertexB;
	DisplayDataSet* dataSet;
	nPoints = abscissa.nItems();
	for (n=0; n<nPoints; ++n)
	{
		// Get initial vertex
		dataSet = displayData.first();
		if (dataSet->yType().value(n) != DisplayDataSet::NoPoint)
		{
			colourScale.colour((yLogarithmic ? pow(10.0, dataSet->y().value(n)) : dataSet->y().value(n)) / yAxisScale, colour);
			vertexA = currentPrimitive->defineVertex(abscissa.value(n), dataSet->y().value(n), dataSet->z(), nrm, colour);
		}
		else vertexA = -1;

		for (dataSet = dataSet->next; dataSet != NULL; dataSet = dataSet->next)
		{
			// Define vertex index for this point (if one exists)
			if (dataSet->yType().value(n) != DisplayDataSet::NoPoint)
			{
				colourScale.colour((yLogarithmic ? pow(10.0, dataSet->y().value(n)) : dataSet->y().value(n)) / yAxisScale, colour);
				vertexB = currentPrimitive->defineVertex(abscissa.value(n), dataSet->y().value(n), dataSet->z(), nrm, colour);
			}
			else vertexB = -1;

			// If both vertices are valid, plot a line
			if ((vertexA != -1) && (vertexB != -1)) currentPrimitive->defineIndices(vertexA, vertexB);

			vertexA = vertexB;
		}
		currentPrimitive = currentPrimitive->next;
	}
}
