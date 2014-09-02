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
#include "fontinstance.h"
#include "math/constants.h"
#include <base/viewpane.h>

/*
 * Text Primitive
 */

// Text Anchor Keywords
const char* TextAnchorKeywords[] = { "TopLeft", "TopMiddle", "TopRight", "MiddleLeft", "Central", "MiddleRight", "BottomLeft", "BottomMiddle", "BottomRight" };

// Convert text string to TextAnchor
TextPrimitive::TextAnchor TextPrimitive::textAnchor(const char* s)
{
	for (int n=0; n<TextPrimitive::nTextAnchors; ++n) if (strcmp(s,TextAnchorKeywords[n]) == 0) return (TextPrimitive::TextAnchor) n;
	return TextPrimitive::nTextAnchors;
}

// Convert TextAnchor to text string
const char* TextPrimitive::textAnchor(TextPrimitive::TextAnchor anchor)
{
	return TextAnchorKeywords[anchor];
}

// Set data
void TextPrimitive::set(QString text, Vec3<double> anchorPoint, TextPrimitive::TextAnchor anchorPosition, Vec3<double> adjustmentVector, Matrix& rotation, double textSize)
{
	text_ = text;
	anchorPoint_ = anchorPoint;
	anchorPosition_ = anchorPosition;
	adjustmentVector_ = adjustmentVector;
	localRotation_ = rotation;
	textSize_ = textSize;
}

// Return transformation matrix to use when rendering the text
Matrix TextPrimitive::transformationMatrix(double baseFontSize)
{
	Matrix textMatrix, A;
	Vec3<double> lowerLeft, upperRight, anchorPos, anchorPosRotated, textCentre;

	// Calculate scaling factor for font
	double scale = FontInstance::fontBaseHeight() * textSize_ / baseFontSize;

	// Calculate bounding box and anchor position on it
	FontInstance::boundingBox(text_, lowerLeft, upperRight);
	switch (anchorPosition_)
	{
		case (TextPrimitive::TopLeftAnchor):
			anchorPos.set(lowerLeft.x, upperRight.y, 0.0);
			break;
		case (TextPrimitive::TopMiddleAnchor):
			anchorPos.set((lowerLeft.x+upperRight.x)*0.5, upperRight.y, 0.0);
			break;
		case (TextPrimitive::TopRightAnchor):
			anchorPos = upperRight;
			break;
		case (TextPrimitive::MiddleLeftAnchor):
			anchorPos.set(lowerLeft.x, (lowerLeft.y+upperRight.y)*0.5, 0.0);
			break;
		case (TextPrimitive::CentralAnchor):
			anchorPos.set((lowerLeft.x+upperRight.x)*0.5, (lowerLeft.y+upperRight.y)*0.5, 0.0);
			break;
		case (TextPrimitive::MiddleRightAnchor):
			anchorPos.set(upperRight.x, (lowerLeft.y+upperRight.y)*0.5, 0.0);
			break;
		case (TextPrimitive::BottomLeftAnchor):
			anchorPos = lowerLeft;
			break;
		case (TextPrimitive::BottomMiddleAnchor):
			anchorPos.set((lowerLeft.x+upperRight.x)*0.5, lowerLeft.y, 0.0);
			break;
		case (TextPrimitive::BottomRightAnchor):
			anchorPos.set(upperRight.x, lowerLeft.y, 0.0);
			break;
	}

	// Rotate anchor position with local rotation matrix
	textCentre = (lowerLeft + upperRight) * 0.5;
	anchorPosRotated = localRotation_ * (anchorPos - textCentre) * scale;

	// Construct matrix
	// -- Translate to centre of text bounding box (not rotated)
	textMatrix.createTranslation(-textCentre);
	// -- Apply scaled local rotation matrix
	A = localRotation_;
	A.applyScaling(scale, scale, scale);
	textMatrix *= A;
	// -- Apply transform to text anchor point
	textMatrix.applyTranslation(-anchorPosRotated+anchorPoint_+adjustmentVector_*scale);

	return textMatrix;
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
void TextPrimitiveChunk::add(QString text, Vec3<double> anchorPoint, TextPrimitive::TextAnchor anchorPosition, Vec3<double> adjustmentVector, Matrix& rotation, double textSize)
{
	textPrimitives_[nTextPrimitives_].set(text, anchorPoint, anchorPosition, adjustmentVector, rotation, textSize);
	++nTextPrimitives_;
}

// Return number of primitives in the list
int TextPrimitiveChunk::nPrimitives()
{
	return nTextPrimitives_;
}

// Calculate global bounding box for all text primitives in the chunk
void TextPrimitiveChunk::boundingBox(ViewPane& pane, Vec4<double>& boundingBox, Matrix viewMatrix, bool correctOrientation, double baseFontSize)
{
	Vec4<double> screen;

	Matrix textMatrix;
	Vec3<double> corners[4], textCentre;
	double scale;

	for (int n=0; n<nTextPrimitives_; ++n)
	{
		// Get transformation matrix and bounding box for text
		textMatrix = textPrimitives_[n].transformationMatrix(baseFontSize);
		FontInstance::boundingBox(textPrimitives_[n].text(), corners[0], corners[1]);
		corners[2].set(corners[0].x, corners[1].y, 0.0);
		corners[3].set(corners[1].x, corners[0].y, 0.0);

		// Transform the four corners of the bounding box, and get their extreme coordinates
		for (int m=0; m <4; ++m)
		{
			screen = pane.modelToScreen(textMatrix*corners[m], viewMatrix);
			if (screen.x < boundingBox.x) boundingBox.x = screen.x;
			if (screen.x > boundingBox.z) boundingBox.z = screen.x;
			if (screen.y < boundingBox.y) boundingBox.y = screen.y;
			if (screen.y > boundingBox.w) boundingBox.w = screen.y;
		}
	}
}

// Render all primitives in chunk
void TextPrimitiveChunk::renderAll(Matrix viewMatrix, bool correctOrientation, double baseFontSize)
{
	Matrix textMatrix;
	for (int n=0; n<nTextPrimitives_; ++n)
	{
		// Get transformation matrix for text, and multiply by general view matrix
		textMatrix = textPrimitives_[n].transformationMatrix(baseFontSize) * viewMatrix;
		glLoadMatrixd(textMatrix.matrix());
		FontInstance::font()->Render(qPrintable(textPrimitives_[n].text()));
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
void TextPrimitiveList::add(QString text, Vec3<double> anchorPoint, TextPrimitive::TextAnchor anchorPosition, Vec3<double> adjustmentVector, Matrix& rotation, double textSize)
{
	if (currentChunk_ == NULL) currentChunk_ = textPrimitives_.add();
	else if (currentChunk_->full()) currentChunk_ = textPrimitives_.add();

	// Add primitive and set data
	currentChunk_->add(text, anchorPoint, anchorPosition, adjustmentVector, rotation, textSize);
}

// Return number of primitives in the list
int TextPrimitiveList::nPrimitives()
{
	int n = 0;
	for (TextPrimitiveChunk *chunk = textPrimitives_.first(); chunk != NULL; chunk = chunk->next) n += chunk->nPrimitives();
	return n;
}

// Calculate global bounding box for all text primitives in the list
Vec4<double> TextPrimitiveList::boundingBox(ViewPane& pane, Matrix viewMatrix, bool correctOrientation, double baseFontSize)
{
	// Create initial, out-of-range result (xmin,ymin,xmax,ymax)
	Vec4<double> result(1.0e9, 1.0e9, -1.0e9, -1.0e9);

	for (TextPrimitiveChunk *chunk = textPrimitives_.first(); chunk != NULL; chunk = chunk->next) chunk->boundingBox(pane, result, viewMatrix, correctOrientation, baseFontSize);

	return result;
}

// Render all primitives in list
void TextPrimitiveList::renderAll(Matrix viewMatrix, bool correctOrientation, double baseFontSize)
{
	for (TextPrimitiveChunk *chunk = textPrimitives_.first(); chunk != NULL; chunk = chunk->next) chunk->renderAll(viewMatrix, correctOrientation, baseFontSize);
}

