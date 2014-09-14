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
 * Text Fragment
 */

// Constructor
TextFragment::TextFragment() : ListItem<TextFragment>()
{
	scale_ = 1.0;
}

// Destructor
TextFragment::~TextFragment()
{

}

// Set fragment data
void TextFragment::set(QString& text, double scale, Vec3<double> translation)
{
	text_ = text;
	scale_ = scale;
	translation_ = translation;
}

// Return text of fragment
QString TextFragment::text()
{
	return text_;
}

// Return local scale for fragment
double TextFragment::scale()
{
	return scale_;
}

// Return local translation for fragment
Vec3<double> TextFragment::translation()
{
	return translation_;
}

/*
 * Text Primitive
 */

// Constructor
TextPrimitive::TextPrimitive() : ListItem<TextPrimitive>()
{
}

// Destructor
TextPrimitive::~TextPrimitive()
{
}

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
	TextFragment* fragment;
	Vec3<double> ll, ur, translation;
	double basicHeight, scale, y;
	QString fragmentText;
	int supSub = 0;

	// Get the height of a basic bounding box so we have a reference
	FontInstance::boundingBox("A", ll, ur);
	basicHeight = ur.y - ll.y;

	int n=0;
	while (n<text.length())
	{
		// If the current character is a backslash, check the next character to see what action we need to take...
		if (text[n] == QChar('\\'))
		{
			// Increment n and do a quick check on the next character (in case it's another backslash)
			++n;
			if (text[n] == QChar('\\'))
			{
				fragmentText += text[n];
				++n;
				continue;
			}
			else if (!fragmentText.isEmpty())
			{
				// Create a fragment for any previously-stored characters

				// -- Generate vertical text position
				translation.y = 0.0;
				y = basicHeight;
				if (supSub > 0) for (int i=0; i<supSub; ++i) { translation.y += y - (y/3.0); y *= 0.583; }
				scale = pow(0.583,abs(supSub));
				fragment = fragments_.add();
				fragment->set(fragmentText, scale, translation);

				// -- Increase x offset
				FontInstance::boundingBox(fragmentText, ll, ur);
				translation.x += (ur.x - ll.x) * scale;

				// -- Empty string
				fragmentText = QString();
			}

			// Check the character again, this time for our own escape sequences
			if (text[n] == QChar('S')) ++supSub;
		}
		else fragmentText += text[n];

		++n;
	}

	// Any text remaining?
	if (!fragmentText.isEmpty())
	{
		// Generate vertical text position
		translation.y = 0.0;
		y = basicHeight;
		if (supSub > 0) for (int i=0; i<supSub; ++i) { translation.y += y - (y/3.0); y *= 0.583; }
		fragment = fragments_.add();
		fragment->set(fragmentText, pow(0.583,abs(supSub)), translation);
	}

// 	XXX Title="\it{Q}, A\sup{-1}"
// 	XXX Shear matrix for fake italics
// 	XXX Temporarily increase glLineWidth for bold chars.

	anchorPoint_ = anchorPoint;
	anchorPosition_ = anchorPosition;
	adjustmentVector_ = adjustmentVector;
	localRotation_ = rotation;
	textSize_ = textSize;
}

// Return transformation matrix to use when rendering the text
Matrix TextPrimitive::transformationMatrix(double baseFontSize, TextFragment* fragment)
{
	Matrix textMatrix, A;
	Vec3<double> lowerLeft, upperRight, anchorPos, anchorPosRotated, textCentre;

	// Calculate scaling factor for font
	double scale = FontInstance::fontBaseHeight() * textSize_ / baseFontSize;
	
	// Calculate bounding box and anchor position on it
	boundingBox(lowerLeft, upperRight);
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
	// -- Translate to centre of text bounding box (not rotated) accounting for fragment translation if one was specified
	if (fragment) textCentre -= fragment->translation();
	textMatrix.createTranslation(-textCentre);
	// -- Apply scaled local rotation matrix
	A = localRotation_;
	A.applyScaling(scale, scale, scale);
	textMatrix *= A;
	// -- Apply translation to text anchor point
	textMatrix.applyTranslation(-anchorPosRotated+anchorPoint_+adjustmentVector_*scale);
	// -- Apply local scaling to text (if fragment was provided)
	if (fragment) textMatrix.applyScaling(fragment->scale());

	return textMatrix;
}

// Calculate bounding box of primitive
void TextPrimitive::boundingBox(Vec3<double>& lowerLeft, Vec3<double>& upperRight)
{
	// Set initial lowerLeft and upperRight from the first primitive in the list
	if (fragments_.first()) FontInstance::boundingBox(fragments_.first()->text(), lowerLeft, upperRight);
	else
	{
		// No fragments in list!
		lowerLeft.zero();
		upperRight.zero();
		return;
	}
	
	// Loop over remaining fragments, keeping track of the total width of the primitive and the max/min y values
	Vec3<double> ll, ur;
	double width = upperRight.x - lowerLeft.x;
	for (TextFragment* fragment = fragments_.first()->next; fragment != NULL; fragment = fragment->next)
	{
		// Get bounding box for this fragment
		FontInstance::boundingBox(fragment->text(), ll, ur);

		// Scale the box by the current scaling factor...
		ur.x = ll.x + (ur.x - ll.x)*fragment->scale();
		ur.y = ll.y + (ur.y - ll.y)*fragment->scale();

		// Translate the box by the defined amount
		ll += fragment->translation();
		ur += fragment->translation();

		// Update lowerLeft and upperRight values
		if (ll.y < lowerLeft.y) lowerLeft.y = ll.y;
		if (ur.y > upperRight.y) upperRight.y = ur.y;
		if (ur.x > upperRight.x) upperRight.x = ur.x;
	}
}

// Render primitive
void TextPrimitive::render(Matrix viewMatrix, bool correctOrientation, double baseFontSize)
{
	Matrix textMatrix;

	// Loop over fragments
	for (TextFragment* fragment = fragments_.first(); fragment != NULL; fragment = fragment->next)
	{
		textMatrix = transformationMatrix(baseFontSize, fragment) * viewMatrix;
		glLoadMatrixd(textMatrix.matrix());

		// Draw bounding boxes around each fragment
		if (false)
		{
			glDisable(GL_LINE_STIPPLE);
			glLineWidth(1.0);
			Vec3<double> ll, ur;
			FontInstance::boundingBox(fragment->text(), ll, ur);
			glBegin(GL_LINE_LOOP);
			glVertex3d(ll.x, ll.y, 0.0);
			glVertex3d(ur.x, ll.y, 0.0);
			glVertex3d(ur.x, ur.y, 0.0);
			glVertex3d(ll.x, ur.y, 0.0);
			glEnd();
		}

		FontInstance::font()->Render(qPrintable(fragment->text()));
	}
}

/*
 * Text Primitive List
 */

// Constructor
TextPrimitiveList::TextPrimitiveList()
{
}

// Clear list
void TextPrimitiveList::clear()
{
	textPrimitives_.clear();
}

// Set data from literal coordinates and text
void TextPrimitiveList::add(QString text, Vec3<double> anchorPoint, TextPrimitive::TextAnchor anchorPosition, Vec3<double> adjustmentVector, Matrix& rotation, double textSize)
{
	TextPrimitive* primitive = textPrimitives_.add();
	primitive->set(text, anchorPoint, anchorPosition, adjustmentVector, rotation, textSize);
}

// Update global bounding cuboid for all text primitives in the list
Cuboid TextPrimitiveList::boundingCuboid(ViewPane& pane, bool correctOrientation, double baseFontSize, Cuboid startingCuboid)
{
	Cuboid result = startingCuboid;
	Matrix textMatrix;
	Vec3<double> corners[4], local;
	for (TextPrimitive* primitive = textPrimitives_.first(); primitive != NULL; primitive = primitive->next)
	{
		// Get transformation matrix and bounding box for text
		textMatrix = primitive->transformationMatrix(baseFontSize);
		primitive->boundingBox(corners[0], corners[1]);
		corners[2].set(corners[0].x, corners[1].y, 0.0);
		corners[3].set(corners[1].x, corners[0].y, 0.0);

		// Transform the four corners of the bounding box with the text primitive's transformation matrix
		// and determine the extreme x, y, and z coordinates of the primitives in the local frame
		for (int m=0; m <4; ++m)
		{
// 			screen = pane.modelToScreen(textMatrix*corners[m], viewMatrix);
			local = textMatrix*corners[m];
			result.updateExtremes(local);
		}

	}

	return result;
}

// Render all primitives in list
void TextPrimitiveList::renderAll(Matrix viewMatrix, bool correctOrientation, double baseFontSize)
{
	for (TextPrimitive* primitive = textPrimitives_.first(); primitive != NULL; primitive = primitive->next) primitive->render(viewMatrix, correctOrientation, baseFontSize);
}

