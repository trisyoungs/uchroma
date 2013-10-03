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
void TextPrimitive::set(QString text, double scale, double fontBaseHeight, double relativeWidth, Vec3<double> origin, Vec3<double> direction, Vec3<double> up, int zrotation)
{
	text_ = text;

	// Construct basic transformation matrix
	direction.normalise();
	up.normalise();
	localTransform_.setIdentity();
	localTransform_.setTranslation(origin + direction*(fabs(cos(zrotation/DEGRAD)*relativeWidth) + fabs(sin(zrotation/DEGRAD)*fontBaseHeight))*scale*0.5);
	localTransform_.setColumn(0, direction*scale, 0.0);
	localTransform_.setColumn(1, up*scale, 0.0);
	localTransform_.setColumn(2, (direction * up)*scale, 0.0);
	Matrix rotation;
	rotation.createRotationAxis(localTransform_[8], localTransform_[9], localTransform_[10], zrotation*1.0, true);
	localTransform_ *= rotation;

	// Calculate the text centerPoint_ so the rotation applied above operates on the centre of the text string
	// Don't apply the 'scale' multiplier again, since it already exists in the transformation matrix
	// Work in local coordinates, rather than the transform axes calculated above
	centerPoint_.X(-relativeWidth*0.5);
	centerPoint_.Y(-fontBaseHeight*0.5);
	centerPoint_.Z(0.0);
}

// Return local transform matrix
Matrix& TextPrimitive::localTransform()
{
	return localTransform_;
}

// Return text centerpoint
FTPoint TextPrimitive::centerPoint()
{
	return centerPoint_;
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
void TextPrimitiveChunk::add(QString text, double scale, double fontBaseHeight, double relativeWidth, Vec3<double> origin, Vec3<double> direction, Vec3<double> up, int zrotation)
{
	textPrimitives_[nTextPrimitives_].set(text, scale, fontBaseHeight, relativeWidth, origin, direction, up, zrotation);
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
		font->Render(qPrintable(textPrimitives_[n].text()), -1, textPrimitives_[n].centerPoint());
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
void TextPrimitiveList::add(QString text, double scale, double fontBaseHeight, double relativeWidth, Vec3<double> origin, Vec3<double> direction, Vec3<double> up, int zrotation)
{
	if (currentChunk_ == NULL) currentChunk_ = textPrimitives_.add();
	else if (currentChunk_->full()) currentChunk_ = textPrimitives_.add();
	// Add primitive and set data
	currentChunk_->add(text, scale, fontBaseHeight, relativeWidth, origin, direction, up, zrotation);
}

// Render all primitives in list
void TextPrimitiveList::renderAll(Matrix viewMatrix, Vec3< double > center, FTFont* font)
{
	for (TextPrimitiveChunk *chunk = textPrimitives_.first(); chunk != NULL; chunk = chunk->next) chunk->renderAll(viewMatrix, center, font);
}

