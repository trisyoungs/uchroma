/*
	*** Index Chunk Storage
	*** src/gui/viewer_indexchunk.h
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

#ifndef UCHROMA_INDEXCHUNK_H
#define UCHROMA_INDEXCHUNK_H

#ifdef _WIN32
#include <windows.h>
#endif
#ifdef _MAC
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include "math/constants.h"

#define INDEXCHUNKSIZE 1024

// Chunk of indices
class IndexChunk
{
	public:
	// Constructor / Destructor
	IndexChunk();
	~IndexChunk();
	// List pointers
	IndexChunk *prev, *next;
	
	private:
	// Index data array
	GLuint *indexData_;
	// Number of defined indices in current chunk
	int nDefinedIndices_;
	// Number of primitive types (nDefinedIndices/indicesPerType) currently defined
	int nDefinedTypes_;
	// Maximum number of allowable indices
	int maxIndices_;
	// Primitive type (GL)
	GLenum type_;
	// Number of vertices per primitive type
	int indicesPerType_;
	
	public:
	// Initialise structure
	void initialise(GLenum type);
	// Forget all index data currently stored in array (but retain array)
	void forgetAll();
	// Define triplet of indices
	void defineTriple(GLuint a, GLuint b, GLuint c);
	// Return whether current array is full
	bool full();
	// Return number of defined primitive (GL) types
	int nDefinedTypes();
	// Return index array
	GLuint* indexData();
	// Return number of defined indices in chunk
	int nDefinedIndices();
	// Send to OpenGL (i.e. render)
	void sendToGL();
};

#endif
