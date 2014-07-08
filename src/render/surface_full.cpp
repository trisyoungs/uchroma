/*
	*** Sufrace Generation - Full
	*** src/render/surface_full.cpp
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

// Construct full surface representation of data
void Surface::constructFull(PrimitiveList& primitives, double yAxisScale, bool yLogarithmic, const Array<double>& abscissa, List<DisplayDataSet>& displayData, ColourScale colourScale)
{
	GLfloat zA, zB;

	// Forget all data in current primitives
	primitives.forgetAll();

	// Check for low number of datasets or low number of points in x
	if (displayData.nItems() == 0) return;
	if (displayData.nItems() == 1)
	{
		// Special case, if there is exactly one dataset, draw a standard XY line surface instead
		constructLineXY(primitives, yAxisScale, yLogarithmic, abscissa, displayData, colourScale);
		return;
	}
	if (abscissa.nItems() < 2) return;

	// Resize primitive list so it's large enough for our needs
	int maxVertices = (abscissa.nItems()+1)*2;
	int maxIndices = (abscissa.nItems()-1)*6;
	primitives.reinitialise(displayData.nItems()-1, false, maxVertices, maxIndices, GL_TRIANGLES, true);

	// Temporary variables
	Array< Vec3<double> > normA, normB;
	Array< Vec4<GLfloat> > colourA, colourB;
	int n;
	QColor colour;
	Vec3<double> nrm(0.0, 1.0, 0.0);

	// Construct first slice data and set initial min/max values
	DisplayDataSet* dataSetA = displayData.first();
	constructSurfaceStrip(abscissa, dataSetA, yAxisScale, yLogarithmic, normA, colourA, colourScale, NULL, dataSetA->next);
	int nPoints = abscissa.nItems();

	// Create triangles
	int balls = 1;
	int nBit, nPlusOneBit, totalBit;
	GLuint vertexAn = -1, vertexBn = -1, vertexAnPlusOne = -1, vertexBnPlusOne = -1;
	Primitive* currentPrimitive = primitives[0];
	for (DisplayDataSet* dataSetB = dataSetA->next; dataSetB != NULL; dataSetB = dataSetB->next)
	{
		// Grab slice pointers
		DisplayDataSet* dataSetC = (dataSetB->next == NULL ? NULL : dataSetB->next);

		// Construct data for current slice
		constructSurfaceStrip(abscissa, dataSetB, yAxisScale, yLogarithmic, normB, colourB, colourScale, dataSetA, dataSetC);

		// Grab z values
		zA = (GLfloat) dataSetA->z();
		zB = (GLfloat) dataSetB->z();
	
		// Get nPoints and array references
		const Array<double>& yA = dataSetA->y();
		const Array<double>& yB = dataSetB->y();
		const Array<DisplayDataSet::DataPointType>& yTypeA = dataSetA->yType();
		const Array<DisplayDataSet::DataPointType>& yTypeB = dataSetB->yType();

		// Use a simple bit to quickly determine which triangles to draw, given possible lack of datapoints in slices
		//
		//			n	n+1	n	n+1
		//	Slice A		4-------1	4-------1	0 = Both	5 = None
		//			| ....TR|	|TL.... |	1 = BL		6 = None
		//			|   ....|	|....   |	2 = TL		7 = None
		//			|BL   ..|	|..   BR|	3 = None	8 = TR
		//	Slice B		8-------2	8-------2	4 = BR		9+= None

		// Set initial bit, and generate initial vertices
		nBit = 0;
		if (yTypeA.value(0) == DisplayDataSet::NoPoint)
		{
			nBit += 4;
			vertexAn = -1;
		}
		else vertexAn = currentPrimitive->defineVertex(abscissa.value(0), yA.value(0), zA, normA[0], colourA[0]);
		if (yTypeB.value(0) == DisplayDataSet::NoPoint)
		{
			nBit += 8;
			vertexBn = -1;
		}
		else vertexBn = currentPrimitive->defineVertex(abscissa.value(0), yB.value(0), zB, normB[0], colourB[0]);

		for (int n=0; n<nPoints-1; ++n)
		{
			// Construct bit for n+1
			nPlusOneBit = 0;
			if (yTypeA.value(n+1) == DisplayDataSet::NoPoint) nPlusOneBit += 1;
			if (yTypeB.value(n+1) == DisplayDataSet::NoPoint) nPlusOneBit += 2;
			totalBit = nBit + nPlusOneBit;

			// Reset indices for current (n+1) column
			vertexAnPlusOne = -1;
			vertexBnPlusOne = -1;

			// Add triangles for this quadrant
			if (totalBit == 0)
			{
				// Draw both
				if (vertexAn == -1) vertexAn = currentPrimitive->defineVertex(abscissa.value(n), yA.value(n), zA, normA[n], colourA[n]);
				if (vertexBn == -1) vertexBn = currentPrimitive->defineVertex(abscissa.value(n), yB.value(n), zB, normB[n], colourB[n]);
				vertexAnPlusOne = currentPrimitive->defineVertex(abscissa.value(n+1), yA.value(n+1), zA, normA[n+1], colourA[n+1]);
				vertexBnPlusOne = currentPrimitive->defineVertex(abscissa.value(n+1), yB.value(n+1), zB, normB[n+1], colourB[n+1]);
				currentPrimitive->defineIndices(vertexAn, vertexAnPlusOne, vertexBnPlusOne);
				currentPrimitive->defineIndices(vertexAn, vertexBn, vertexBnPlusOne);
			}
			else if (totalBit == 1)
			{
				// Bottom left corner only
				if (vertexAn == -1) vertexAn = currentPrimitive->defineVertex(abscissa.value(n), yA.value(n), zA, normA[n], colourA[n]);
				if (vertexBn == -1) vertexBn = currentPrimitive->defineVertex(abscissa.value(n), yB.value(n), zB, normB[n], colourB[n]);
				vertexBnPlusOne = currentPrimitive->defineVertex(abscissa.value(n+1), yB.value(n+1), zB, normB[n+1], colourB[n+1]);
				currentPrimitive->defineIndices(vertexAn, vertexBnPlusOne, vertexBn);
			}
			else if (totalBit == 2)
			{
				// Top left corner only
				if (vertexAn == -1) vertexAn = currentPrimitive->defineVertex(abscissa.value(n), yA.value(n), zA, normA[n], colourA[n]);
				if (vertexBn == -1) vertexBn = currentPrimitive->defineVertex(abscissa.value(n), yB.value(n), zB, normB[n], colourB[n]);
				vertexAnPlusOne = currentPrimitive->defineVertex(abscissa.value(n+1), yA.value(n+1), zA, normA[n+1], colourA[n]);
				currentPrimitive->defineIndices(vertexAn, vertexAnPlusOne, vertexBn);
			}
			else if (totalBit == 4)
			{
				// Bottom right corner only
				if (vertexBn == -1) vertexBn = currentPrimitive->defineVertex(abscissa.value(n), yB.value(n), zB, normB[n], colourB[n]);
				vertexAnPlusOne = currentPrimitive->defineVertex(abscissa.value(n+1), yA.value(n+1), zA, normA[n+1], colourA[n+1]);
				vertexBnPlusOne = currentPrimitive->defineVertex(abscissa.value(n+1), yB.value(n+1), zB, normB[n+1], colourB[n+1]);
				currentPrimitive->defineIndices(vertexAnPlusOne, vertexBnPlusOne, vertexBn);
			}
			else if (totalBit == 8)
			{
				// Top right corner only
				if (vertexAn == -1) vertexAn = currentPrimitive->defineVertex(abscissa.value(n), yA.value(n), zA, normA[n], colourA[n]);
				vertexAnPlusOne = currentPrimitive->defineVertex(abscissa.value(n+1), yA.value(n+1), zA, normA[n+1], colourA[n+1]);
				vertexBnPlusOne = currentPrimitive->defineVertex(abscissa.value(n+1), yB.value(n+1), zB, normB[n+1], colourB[n+1]);
				currentPrimitive->defineIndices(vertexAn, vertexAnPlusOne, vertexBnPlusOne);
			}

			// Store new nBit for next index
			nBit = nPlusOneBit*4;
			vertexAn = vertexAnPlusOne;
			vertexBn = vertexBnPlusOne;
		}

		// Copy arrays ready for next pass
		normA = normB;
		colourA = colourB;
		dataSetA = dataSetB;

		// Increment primitive pointer
		currentPrimitive = currentPrimitive->next;
	}
}
