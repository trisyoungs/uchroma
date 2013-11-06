/*
	*** Text Primitive
	*** src/gui/viewer_textprimitive.h
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

#ifndef UCHROMA_TEXTPRIMITIVE_H
#define UCHROMA_TEXTPRIMITIVE_H

#include <QtGui/QtGui>
#include "math/matrix.h"
#include "templates/vector3.h"
#include "templates/list.h"
#include <FTGL/ftgl.h>

#define TEXTCHUNKSIZE 100

// Forward Declarations
class Viewer;

// Text Primitive
class TextPrimitive
{
	public:
	// Horizontal anchor enum
	enum HorizontalAnchor { AnchorLeft=0, AnchorCentre=1, AnchorRight=2 };

	private:
	// Local transform matrix for the text
	Matrix localTransform_;
	// Origin of text
	Vec3<double> origin_;
	// Centre (anchor) of text
	FTPoint centre_;
	// Text to render
	QString text_;

	public:
	// Set data
	void set(QString text, Vec3<double> origin, Vec3<double> centre, Matrix& transform);
	// Return local transform 
	Matrix& localTransform();
	// Return text origin
	Vec3<double> origin();
	// Return text centre
	FTPoint centre();
	// Return text to render
	QString& text();
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
	void add(QString text, Vec3<double> origin, Vec3<double> centre, Matrix& transform);
	// Render all primitives in chunk
	void renderAll(Matrix viewMatrix, bool correctView, Vec3<double> globalCenter, FTFont* font);
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
	void add(QString text, Vec3<double> origin, Vec3<double> centre, Matrix& transform);
	// Render all primitives in list
	void renderAll(Matrix viewMatrix, bool correctView, Vec3<double> globalCenter, FTFont* font);
};

#endif
