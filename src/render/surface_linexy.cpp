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

// Construct line representation of data in XY slices
void Surface::constructLineXY(PrimitiveList& primitives, double yAxisScale, bool yLogarithmic, const Array<double>& abscissa, List<DisplayDataSet>& displayData, ColourScale colourScale)
{
	// Forget all data in current primitives
	primitives.forgetAll();

	// Check for low number of datasets or low number of points in x
	if (displayData.nItems() == 0) return;
	if (abscissa.nItems() < 2) return;

	// Resize primitive list so it's large enough for our needs
	primitives.reinitialise(displayData.nItems(), true, abscissa.nItems(), (abscissa.nItems()-1)*2, GL_LINES, true);

	// Temporary variables
	GLfloat z;
	Vec4<GLfloat> colour(0,0,0,1);
	int n, nPoints;
	Vec3<double> nrm(0.0, 1.0, 0.0);

	Primitive* currentPrimitive = primitives[0];

	// Create lines for slices
	GLuint vertexA, vertexB;
	for (DisplayDataSet* dataSet = displayData.first(); dataSet != NULL; dataSet = dataSet->next)
	{
		// Grab y and z values
		const Array<double>& y = dataSet->y();
		const Array<DisplayDataSet::DataPointType>& yType = dataSet->yType();
		z = (GLfloat) dataSet->z();

		// Get nPoints, and initial vertex
		nPoints = abscissa.nItems();
		if (yType.value(0) != DisplayDataSet::NoPoint)
		{
			colourScale.colour((yLogarithmic ? pow(10.0, y.value(0)) : y.value(0)) / yAxisScale, colour);
			vertexA = currentPrimitive->defineVertex(abscissa.value(0), y.value(0), z, nrm, colour);
		}
		else vertexA = -1;

		for (n=1; n<nPoints; ++n)
		{
			// Define vertex index for this point (if one exists)
			if (yType.value(n) != DisplayDataSet::NoPoint)
			{
				colourScale.colour((yLogarithmic ? pow(10.0, y.value(n)) : y.value(n)) / yAxisScale, colour);
				vertexB = currentPrimitive->defineVertex(abscissa.value(n), y.value(n), z, nrm, colour);
			}
			else vertexB = -1;

			// If both vertices are valid, plot a line
			if ((vertexA != -1) && (vertexB != -1)) currentPrimitive->defineIndices(vertexA, vertexB);

			vertexA = vertexB;
		}
		currentPrimitive = currentPrimitive->next;
	}
}
