/*
	*** Text Primitive
	*** src/gui/viewer_textprimitive.cpp
	Copyright T. Youngs 2013

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

#include "gui/viewer_textprimitive.h"
#include "gui/viewer.uih"
#include "math/constants.h"

/*
// Text Primitive
*/

// Set data
void TextPrimitive::set(QString text, Vec3<double> origin, Vec3<double> centre, Matrix& transform)
{
	text_ = text;
	origin_ = origin;
	centre_ = centre;
	localTransform_ = transform;
}

// Return local transform matrix
Matrix& TextPrimitive::localTransform()
{
	return localTransform_;
}

// Return text centerpoint
Vec3< double > TextPrimitive::center()
{
	return center_;
}

// Return text to render
QString& TextPrimitive::text()
{
	return text_;
}

/*
// Text Primitive Chunk
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
void TextPrimitiveChunk::add(QString text, Vec3<double> origin, Matrix& transform)
{
	textPrimitives_[nTextPrimitives_].set(text, origin, transform);
	++nTextPrimitives_;
}

// Render all primitives in chunk
void TextPrimitiveChunk::renderAll(Matrix viewMatrix, bool correctView, Vec3<double> globalCenter, FTFont* font)
{
	Matrix A, B;
	for (int n=0; n<nTextPrimitives_; ++n)
	{
		// Grab local transformation matrix and apply globalCenter translation
		A = textPrimitives_[n].localTransform();
		A.addTranslation(globalCenter + textPrimitives_[n].origin());

		B = viewMatrix * A;
		if (correctView)
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
		
		glLoadMatrixd(B.matrix());
		font->Render(qPrintable(textPrimitives_[n].text()), -1, textPrimitives_[n].centre());
	}
}

/*
// Text Primitive List
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
void TextPrimitiveList::add(QString text, Vec3< double > origin, Vec3< double > centre, Matrix& transform)
{
	if (currentChunk_ == NULL) currentChunk_ = textPrimitives_.add();
	else if (currentChunk_->full()) currentChunk_ = textPrimitives_.add();

	// Add primitive and set data
	currentChunk_->add(text, origin, transform);
}

// Render all primitives in list
void TextPrimitiveList::renderAll(Matrix viewMatrix, bool correctView, Vec3< double > center, FTFont* font)
{
	for (TextPrimitiveChunk *chunk = textPrimitives_.first(); chunk != NULL; chunk = chunk->next) chunk->renderAll(viewMatrix, center, font);
}

