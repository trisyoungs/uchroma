/*
	*** Index Chunk
	*** src/gui/viewer_indexchunk.cpp
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

#include "gui/viewer_indexchunk.h"
#include <stdio.h>
#include <math.h>

// Constructor
IndexChunk::IndexChunk()
{
	// Public variables
	next = NULL;
	prev = NULL;
	
	// Private variables
	indexData_ = NULL;
	nDefinedIndices_ = 0;
	indicesPerType_ = 1;
	maxIndices_ = -1;
}

// Destructor
IndexChunk::~IndexChunk()
{
	if (indexData_ != NULL) delete[] indexData_;
}

// Initialise structure
void IndexChunk::initialise(GLenum type)
{
	if (type_ == GL_TRIANGLES) indicesPerType_ = 3;
	else if ((type_ == GL_LINES) || (type_ == GL_LINE_LOOP) || (type_ == GL_LINE_STRIP)) indicesPerType_ = 2;
	else if (type_ == GL_POINTS) indicesPerType_ = 1;
	else printf("Warning - Invalid GLenum type given to IndexChunk::initialise (%i)\n", type_);
	maxIndices_ = INDEXCHUNKSIZE*indicesPerType_;
	nDefinedIndices_ = 0;
	indexData_ = new GLuint[maxIndices_];
}

// Define next triplet of vertices
void IndexChunk::defineTriple(GLuint a, GLuint b, GLuint c)
{
	if (nDefinedIndices_ == maxIndices_) printf("Internal Error: Vertex limit for IndexChunk reached.\n");
	else if (indicesPerType_ != 3) printf("Internal Error: Tried to add a triplet of indices for a type which doesn't work in multiples of three.\n");
	// Store indices
	indexData_[nDefinedIndices_++] = a;
	indexData_[nDefinedIndices_++] = b;
	indexData_[nDefinedIndices_++] = c;
	++nDefinedTypes_;
}

// Return whether current array is full
bool IndexChunk::full()
{
	return (nDefinedIndices_ == maxIndices_);
}

// Forget all vertex data currently stored in array (but retain array)
void IndexChunk::forgetAll()
{
	nDefinedIndices_ = 0;
}

// Return index array
GLuint *IndexChunk::indexData()
{
	return indexData_;
}

// Return number of defined indices in chunk
int IndexChunk::nDefinedIndices()
{
	return nDefinedIndices_;
}

