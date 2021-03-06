/*
	*** Text Primitive List
	*** src/render/textprimitivelist.h
	Copyright T. Youngs 2013-2015

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

#ifndef UCHROMA_TEXTPRIMITIVELIST_H
#define UCHROMA_TEXTPRIMITIVELIST_H

#include "render/textprimitive.h"
#include "math/matrix.h"
#include "math/cuboid.h"
#include "templates/vector3.h"
#include "templates/list.h"
#include <QString>

// Forward Declarations
class ViewPane;

// Text Primitive List
class TextPrimitiveList
{
	public:
	// Constructor
	TextPrimitiveList();

	private:
	// List of text primitive chunks
	List<TextPrimitive> textPrimitives_;

	public:
	// Clear list
	void clear();
	// Add primitive to list
	void add(QString text, Vec3<double> anchorPoint, TextPrimitive::TextAnchor anchorPosition, Vec3<double> adjustmentVector, Matrix& rotation, double textSize, bool flat);
	// Update global bounding cuboid for all text primitives in the list
	Cuboid boundingCuboid(const Matrix& viewMatrixInverse, double baseFontSize, Cuboid startingCuboid = Cuboid());
	// Render all primitives in list
	void renderAll(const Matrix& viewMatrix, const Matrix& viewMatrixInverse, double baseFontSize);
};

#endif
