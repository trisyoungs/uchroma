/*
	*** Object List Template Class
	*** src/templates/objectlist.h
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

#ifndef UCHROMA_OBJECTLIST_H
#define UCHROMA_OBJECTLIST_H

#include "templates/reflist.h"
#include <stdio.h>

/*!
 * \brief Linked List Reference Class
 * \details Linked list of references for/to user-defined classes. Subclassing ListItem or RefListItem is not necessary in order to place objects in a RefList.
*/
template <class T> class ObjectList
{
	public:
	// Constructor
	ObjectList<T>(T* parent)
	{
		// If the passed pointer is NULL, do not add anything to the list (we were probably called from a copy constructor)
		if (parent != NULL)
		{
			// Store the parent object pointer, and add it to the master list
			objectPointer_ = parent;
			objects_.add(objectPointer_);
		}
	}
	// Destructor
	~ObjectList<T>()
	{
		// Remove our pointer from the master list
// 		printf("Removing object %p from the list..\n", objectPointer_);
		objects_.remove(objectPointer_);
// 		printf("Now %i items remain.\n", objects_.nItems());
	}


	/*
	 * Object Pointer
	 */
	private:
	// Pointer to object that this ObjectList was created with
	T* objectPointer_;


	/*
	 * Object List
	 */
	private:
	// Master list of available objects
	static RefList<T,bool> objects_;

	public:
	// Return whether specified object still exists
	static bool objectValid(T* object)
	{
		if ((object == NULL) || (!objects_.contains(object))) return false;
		return true;
	}
	// Return whether specified object still exists, reporting errors if it does not
	static bool objectValid(T* object, const char* objectDescription)
	{
		if (object == NULL)
		{
			printf("Invalid Object: Specified %s is NULL.\n", objectDescription);
			return false;
		}
		else if (!objects_.contains(object))
		{
			printf("Invalid Object: Specified %s no longer exists (original pointer was %p).\n", objectDescription, object);
			return false;
		}
		return true;
	}
};

#endif
