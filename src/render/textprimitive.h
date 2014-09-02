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
#include "templates/vector3.h"
#include "templates/list.h"
#include <FTGL/ftgl.h>
#include <QtCore/QString>

#define TEXTCHUNKSIZE 100

// Forward Declarations
class ViewPane;

// Text Primitive
class TextPrimitive
{
	public:
	// Text Anchors
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
	// Text to render
	QString text_;

	public:
	// Set data
	void set(QString text, Vec3<double> anchorPoint, TextAnchor anchorPosition, Vec3<double> adjustmentVector, Matrix& rotation, double textSize);
	// Return text to render
	QString& text();
	// Return transformation matrix to use when rendering the text
	Matrix transformationMatrix(double baseFontSize);
};

// Text Primitive Chunk
class TextPrimitiveChunk
{
	public:
	// Constructor
	TextPrimitiveChunk();
	// List pointers
	TextPrimitiveChunk *prev, *next;

	private:
	// Array of TextPrimitive
	TextPrimitive textPrimitives_[TEXTCHUNKSIZE];
	// Number of text primitives currently in the array
	int nTextPrimitives_;

	public:
	// Forget all text primitives in list
	void forgetAll();
	// Return whether array is full
	bool full();
	// Add primitive to list
	void add(QString text, Vec3<double> anchorPoint, TextPrimitive::TextAnchor anchorPosition, Vec3<double> adjustmentVector, Matrix& rotation, double textSize);
	// Return number of primitives in the list
	int nPrimitives();
	// Calculate global bounding box for all text primitives in the chunk
	void boundingBox(ViewPane& pane, Vec4<double>& boundingBox, Matrix viewMatrix, bool correctOrientation, double baseFontSize);
	// Render all primitives in chunk
	void renderAll(Matrix viewMatrix, bool correctOrientation, double baseFontSize);
};

// Text Primitive List
class TextPrimitiveList
{
	public:
	// Constructor
	TextPrimitiveList();

	private:
	// List of text primitive chunks
	List<TextPrimitiveChunk> textPrimitives_;
	// Current TextPrimitiveChunk
	TextPrimitiveChunk *currentChunk_;

	public:
	// Forget all text primitives, but keeping lists intact
	void forgetAll();
	// Add primitive to list
	void add(QString text, Vec3<double> anchorPoint, TextPrimitive::TextAnchor anchorPosition, Vec3<double> adjustmentVector, Matrix& rotation, double textSize);
	// Return number of primitives in the list
	int nPrimitives();
	// Calculate global bounding box for all text primitives in the list
	Vec4<double> boundingBox(ViewPane& pane, Matrix viewMatrix, bool correctOrientation, double baseFontSize);
	// Render all primitives in list
	void renderAll(Matrix viewMatrix, bool correctOrientation, double baseFontSize);
};

#endif
