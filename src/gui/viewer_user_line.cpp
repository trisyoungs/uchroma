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
void Viewer::constructLineSurface(Primitive& primitive, List<Data2D>& displayData, ColourScale colourScale)
{
	// Set primitive options
	primitive.setType(GL_LINES);

	GLfloat zA;

	// Temporary variables
	Vec4<GLfloat> colourA(0,0,0,1);
	int n, nPoints;
	double yAxisScale = uChroma_->axisStretch(1);
	Vec3<double> nrm(0.0, 1.0, 0.0);

	// We need to skip over slices with zero points, so construct a reflist here of those which contain usable data
	RefList<Data2D,int> slices;
	for (Data2D* slice = displayData.first(); slice != NULL; slice = slice->next) if (slice->nPoints() > 1) slices.add(slice);

	// If there are not enough slices with a valid number of points, get out now
	if (slices.nItems() < 1) return;

	// Create lines for slices
	for (RefListItem<Data2D,int>* ri = slices.first(); ri != NULL; ri = ri->next)
	{
		// Grab slice pointers
		Data2D* data = ri->item;

		// Grab z values
		zA = (GLfloat) data->z();

		// Get nPoints, and initial coordinates
		nPoints = data->nPoints();
		Array<double>& xA = data->arrayX();
		Array<double>& yA = data->arrayY();
		for (n=1; n<nPoints-1; ++n)
		{
			// Add vertex for this point
			colourScale.colour((uChroma_->axisLogarithmic(1) ? pow(10.0, yA[n-1]) : yA[n-1]) / yAxisScale, colourA);
			primitive.defineVertex(xA[n-1], yA[n-1], zA, nrm, colourA, false);

			colourScale.colour((uChroma_->axisLogarithmic(1) ? pow(10.0, yA[n]) : yA[n]) / yAxisScale, colourA);
			primitive.defineVertex(xA[n], yA[n], zA, nrm, colourA, false);
		}
	}
}
