/*
	*** Scene Rendering Functions (User) - Line Surface Generation
	*** src/gui/viewer_user_line.cpp
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

// Construct line representation of data
void Viewer::constructLineSurface(PrimitiveList& primitives, const Array<double>& abscissa, List<DisplaySlice>& displayData, ColourScale colourScale)
{
	// Forget all data in current primitives
	primitives.forgetAll();

	// Resize primitive list so it's large enough for our needs
	primitives.reinitialise(displayData.nItems(), true, abscissa.nItems()*2, 0, GL_LINES, true);
// 	primitives.reinitialise(displayData.nItems(), true, abscissa.nItems(), (abscissa.nItems()-1)*2, GL_LINES, true);

	// Temporary variables
	GLfloat zA;
	Vec4<GLfloat> colourA(0,0,0,1);
	int n, nPoints;
	double yAxisScale = uChroma_->axisStretch(1);
	Vec3<double> nrm(0.0, 1.0, 0.0);

	Primitive* currentPrimitive = primitives[0];

	// Create lines for slices
	for (DisplaySlice* slice = displayData.first(); slice != NULL; slice = slice->next)
	{
		// Grab y and z values
		const Array<double>& yA = slice->y();
		const Array<bool>& yAExists = slice->yExists();
		zA = (GLfloat) slice->z();

		// Get nPoints, and initial coordinates
		nPoints = abscissa.nItems();
		for (n=1; n<nPoints; ++n)
		{
			// If no valid points exists, plot no vertex
			if (!yAExists.value(n-1)) continue;
			if (!yAExists.value(n)) continue;

			// Add vertices for these points
			colourScale.colour((uChroma_->axisLogarithmic(1) ? pow(10.0, yA.value(n-1)) : yA.value(n-1)) / yAxisScale, colourA);
			currentPrimitive->defineVertex(abscissa.value(n-1), yA.value(n-1), zA, nrm, colourA);
			colourScale.colour((uChroma_->axisLogarithmic(1) ? pow(10.0, yA.value(n)) : yA.value(n)) / yAxisScale, colourA);
			currentPrimitive->defineVertex(abscissa.value(n), yA.value(n), zA, nrm, colourA);
		}
		currentPrimitive = currentPrimitive->next;
	}
}
