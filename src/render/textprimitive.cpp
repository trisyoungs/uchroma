/*
	*** Text Primitive
	*** src/render/textprimitive.cpp
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

#include "render/textprimitive.h"
#include "math/constants.h"

/*
 * Text Primitive
 */

// Set data
void TextPrimitive::set(QString text, Vec3<double> origin, Vec3<double> centre, Matrix& transform)
{
	text_ = text;
	origin_ = origin;
	centre_.X(centre.x);
	centre_.Y(centre.y);
	centre_.Z(centre.z);
	localTransform_ = transform;
}

// Return local transform matrix
Matrix& TextPrimitive::localTransform()
{
	return localTransform_;
}

// Return text origin
Vec3<double> TextPrimitive::origin()
{
	return origin_;
}

// Return text centerpoint
FTPoint TextPrimitive::centre()
{
	return centre_;
}

// Return text to render
QString& TextPrimitive::text()
{
	return text_;
}

/*
 * Text Primitive Chunk
 */

// Constructor
TextPrimitiveChunk::TextPrimitiveChunk()
{
	// Public variables
	prev = NULL;
	next = NULL;

	// Private variable
	nTextPrimitives_ = 0;
}

// Forget all text primitives in list
void TextPrimitiveChunk::forgetAll()
{
	nTextPrimitives_ = 0;
}

// Return whether array is full
bool TextPrimitiveChunk::full()
{
	return (nTextPrimitives_ == TEXTCHUNKSIZE);
}

// Add primitive to chunk
void TextPrimitiveChunk::add(QString text, Vec3<double> origin, Vec3<double> centre, Matrix& transform)
{
	textPrimitives_[nTextPrimitives_].set(text, origin, centre, transform);
	++nTextPrimitives_;
}

// Render all primitives in chunk
void TextPrimitiveChunk::renderAll(Matrix viewMatrix, Vec3< double > globalCenter, FTFont* font, bool correctOrientation, double scaling)
{
	Matrix A, B;
	for (int n=0; n<nTextPrimitives_; ++n)
	{
		// Grab local transformation matrix and apply globalCenter translation
		A = textPrimitives_[n].localTransform();
		A.addTranslation(globalCenter + textPrimitives_[n].origin());

		// Calculate general matrix to shift to correct position and rotation
		B = viewMatrix * A;

		// Apply corrections
		if (correctOrientation)
		{
			if (A[5] < 0.0)
			{
				// Very lazy - could write this out by hand...
				A.columnMultiply(1, -1.0);
				B = viewMatrix * A;
			}
			if (A[0] < 0.0)
			{
				// Very lazy - could write this out by hand...
				A.columnMultiply(0, -1.0);
				B = viewMatrix * A;
			}
		}

		// We now scale the matrix in order to give consistent text size regardless of zoom
		// Also corrects aspect ratio of text when using 2D view with scaled view matrix
		double scale = B[14] / scaling;
		B.columnMultiply(2, scale);
		B.columnNormalise(0);
		B.columnNormalise(1);
		scale = B.columnMagnitude(2);
		B.columnMultiply(0, scale);
		B.columnMultiply(1, scale);
		glLoadMatrixd(B.matrix());
		
		font->Render(qPrintable(textPrimitives_[n].text()), -1, textPrimitives_[n].centre());
	}
}

/*
 * Text Primitive List
 */

// Constructor
TextPrimitiveList::TextPrimitiveList()
{
	currentChunk_ = NULL;
}

// Forget all text primitives, but keeping lists intact
void TextPrimitiveList::forgetAll()
{
	for (TextPrimitiveChunk *chunk = textPrimitives_.first(); chunk != NULL; chunk = chunk->next) chunk->forgetAll();
	currentChunk_ = textPrimitives_.first();
}

// Set data from literal coordinates and text
void TextPrimitiveList::add(QString text, Vec3<double> origin, Vec3<double> centre, Matrix& transform)
{
	if (currentChunk_ == NULL) currentChunk_ = textPrimitives_.add();
	else if (currentChunk_->full()) currentChunk_ = textPrimitives_.add();

	// Add primitive and set data
	currentChunk_->add(text, origin, centre, transform);
}

// Render all primitives in list
void TextPrimitiveList::renderAll(Matrix viewMatrix, Vec3<double> globalCenter, FTFont* font, bool correctOrientation, double scaling)
{
	for (TextPrimitiveChunk *chunk = textPrimitives_.first(); chunk != NULL; chunk = chunk->next) chunk->renderAll(viewMatrix, globalCenter, font, correctOrientation, scaling);
}

