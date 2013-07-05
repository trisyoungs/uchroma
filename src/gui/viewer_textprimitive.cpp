/*
	*** Text Primitive
	*** src/gui/viewer_textprimitive.cpp
	Copyright T. Youngs 2013

	This file is part of FQPlot.

	FQPlot is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	FQPlot is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FQPlot.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/viewer_textprimitive.h"
#include "gui/viewer.uih"

/*
// Text Primitive
*/

// Set data
void TextPrimitive::set(QString text, double scale, double fontBaseHeight, Vec3<double> origin, Vec3<double> direction, Vec3<double> up)
{
	text_ = text;

	// Construct basic transformation matrix
	direction.normalise();
	up.normalise();
	localTransform_.setIdentity();
	localTransform_.setTranslation(origin);
	localTransform_.setColumn(0, direction*scale, 0.0);
	localTransform_.setColumn(1, up*scale, 0.0);
	localTransform_.setColumn(2, (direction * up)*scale, 0.0);
	
	// Apply a shift along the up vector (Y) to center the middle of the text characters in line with the origin
	localTransform_.addTranslation(-up*fontBaseHeight*scale*scale);
}

// Return local transform matrix
Matrix& TextPrimitive::localTransform()
{
	return localTransform_;
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
void TextPrimitiveChunk::add(QString text, double scale, double fontBaseHeight, Vec3< double > origin, Vec3< double > direction, Vec3< double > up)
{
	textPrimitives_[nTextPrimitives_].set(text, fontBaseHeight, scale, origin, direction, up);
	++nTextPrimitives_;
}

// Render all primitives in chunk
void TextPrimitiveChunk::renderAll(Matrix viewMatrix, Vec3<double> globalCenter, FTFont* font)
{
	Matrix A;
	for (int n=0; n<nTextPrimitives_; ++n)
	{
		// Grab local transformation matrix and apply globalCenter translation
		A = textPrimitives_[n].localTransform();
		A.addTranslation(globalCenter);

		glLoadMatrixd((viewMatrix * A).matrix());
		font->Render(qPrintable(textPrimitives_[n].text()));
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
void TextPrimitiveList::add(QString text, double scale, double fontBaseHeight, Vec3<double> origin, Vec3<double> direction, Vec3<double> up)
{
	if (currentChunk_ == NULL) currentChunk_ = textPrimitives_.add();
	else if (currentChunk_->full()) currentChunk_ = textPrimitives_.add();
	// Add primitive and set data
	currentChunk_->add(text, scale, fontBaseHeight, origin, direction, up);
}

// Render all primitives in list
void TextPrimitiveList::renderAll(Matrix viewMatrix, Vec3< double > center, FTFont* font)
{
	for (TextPrimitiveChunk *chunk = textPrimitives_.first(); chunk != NULL; chunk = chunk->next) chunk->renderAll(viewMatrix, center, font);
}

