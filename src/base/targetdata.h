/*
	*** Target Data
	*** src/base/targetdata.h
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

#ifndef UCHROMA_TARGETDATA_H
#define UCHROMA_TARGETDATA_H

#include "templates/reflist.h"
#include "render/primitivelist.h"

// Forward Declarations
class Collection;
class Axes;
class ViewPane;
class QGLContext;
class GLExtensions;

// Target Extra Data
class TargetExtraData : public ListItem<TargetExtraData>
{
	public:
	// Constructor / Destructor
// 	TargetExtraData();
// 	~TargetExtraData();
	// Data Type
	enum TargetDataType { SliceData, nTargetDataTypes };


	/*
	 * Data
	 */
	private:
	// Collection containing data
	Collection* collection_;
	// Related primitive for display
	PrimitiveList primitive_;
	// Collection data version at which primitive was last created
	int primitiveDataVersion_;
	// Axes version at which primitive was last created
	int primitiveAxesVersion_;

	public:
};

// TargetData
class TargetData : public ListItem<TargetData>
{
	public:
	// Constructor / Destructor
	TargetData();
	~TargetData();
	// Copy constructor
	TargetData(const TargetData& source);
	// Assignment operator
	void operator=(const TargetData& source);


	/*
	 * Parent
	 */
	private:
	// Parent ViewPane
	ViewPane* parent_;

	public:
	// Set parent ViewPane
	void setParent(ViewPane* parent);


	/*
	 * Target Collection / Primitive
	 */
	private:
	// Target collection
	Collection* collection_;
	// Primitive for display of target collection (if necessary)
	PrimitiveList primitive_;
	// Collection data version at which primitive was last created
	int primitiveDataUsedAt_;
	// Collection colour version at which primitive was last created
	int primitiveColourUsedAt_;
	// ViewPane axes version at which primitive was last created
	int primitiveAxesUsedAt_;
	// Collection style version at which primitive was last created
	int primitiveStyleUsedAt_;

	public:
	// Set target collection
	void setCollection(Collection* collection);
	// Return target collection
	Collection* collection();
	// Return primary primitive for display
	PrimitiveList& primitive();


	/*
	 * Additional Data
	 */
	private:
	// List of additional data
	List<TargetExtraData> data_;

	public:
	// Add new additional data
	Collection* addData(TargetExtraData::TargetDataType type);
	// Return specified collection data type (if it exists)
	Collection* data(TargetExtraData::TargetDataType type);


	/*
	 * GL
	 */
	public:
	// Update primitive for target collection
	PrimitiveList& updatePrimitive(const QGLContext* context, GLExtensions* extensions, const Axes& axes, bool forcePrimitiveUpdate = false, bool dontPopInstance = false);
	// Send primitive to GL
	void sendToGL();
};

#endif
