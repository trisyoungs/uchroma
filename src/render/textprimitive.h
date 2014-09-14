/*
	*** Text Primitive
	*** src/render/textprimitive.h
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

#ifndef UCHROMA_TEXTPRIMITIVE_H
#define UCHROMA_TEXTPRIMITIVE_H

#include "math/matrix.h"
#include "math/cuboid.h"
#include "templates/vector3.h"
#include "templates/list.h"
#include <QtCore/QString>

// Forward Declarations
class ViewPane;

// Text Fragment
class TextFragment : public ListItem<TextFragment>
{
	public:
	// Constructor / Destructor
	TextFragment();
	~TextFragment();


	/*
	 * Fragment Definition
	 */
	private:
	// Fragment text
	QString text_;
	// Local scale for fragment
	double scale_;
	// Local translation for fragment
	Vec3<double> translation_;

	public:
	// Set fragment data
	void set(QString& text, double scale = 1.0, Vec3<double> translation = Vec3<double>());
	// Return fragment text
	QString text();
	// Return local scale for fragment
	double scale();
	// Return local translation for fragment
	Vec3<double> translation();
};

// Text Primitive
class TextPrimitive : public ListItem<TextPrimitive>
{
	public:
	// Constructor / Destructor
	TextPrimitive();
	~TextPrimitive();
	// Text Anchors enum
	enum TextAnchor { TopLeftAnchor, TopMiddleAnchor, TopRightAnchor, MiddleLeftAnchor, CentralAnchor, MiddleRightAnchor, BottomLeftAnchor, BottomMiddleAnchor, BottomRightAnchor, nTextAnchors };
	// Convert text string to TextAnchor
	static TextAnchor textAnchor(const char* s);
	// Convert TextAnchor to text string
	static const char* textAnchor(TextAnchor anchor);

	private:
	// Coordinates of anchorpoint of text
	Vec3<double> anchorPoint_;
	// Location of anchorpoint on text bounding box
	TextAnchor anchorPosition_;
	// Vector by which to adjust position of text
	Vec3<double> adjustmentVector_;
	// Local transform matrix for the text
	Matrix localRotation_;
	// Text size
	double textSize_;
	// Text fragments to render
	List<TextFragment> fragments_;

	public:
	// Set data
	void set(QString text, Vec3<double> anchorPoint, TextAnchor anchorPosition, Vec3<double> adjustmentVector, Matrix& rotation, double textSize);
	// Return transformation matrix to use when rendering (including fragment scale/translation if one is specified)
	Matrix transformationMatrix(double baseFontSize, TextFragment* fragment = NULL);
	// Calculate bounding box of primitive
	void boundingBox(Vec3<double>& lowerLeft, Vec3<double>& upperRight);
	// Render primitive
	void render(Matrix viewMatrix, bool correctOrientation, double baseFontSize);
};


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
	void add(QString text, Vec3<double> anchorPoint, TextPrimitive::TextAnchor anchorPosition, Vec3<double> adjustmentVector, Matrix& rotation, double textSize);
	// Update global bounding cuboid for all text primitives in the list
	Cuboid boundingCuboid(ViewPane& pane, bool correctOrientation, double baseFontSize, Cuboid startingCuboid = Cuboid());
	// Render all primitives in list
	void renderAll(Matrix viewMatrix, bool correctOrientation, double baseFontSize);
};

#endif
