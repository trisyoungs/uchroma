/*
	*** Variant Pointer
	*** src/templates/variantpointer.h
	Copyright T. Youngs 2013-2015

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

#include <QVariant>

#ifndef UCHROMA_VARIANTPOINTER_H
#define UCHROMA_VARIANTPOINTER_H

// Simple class to convert between QVariant pointer (void*) and a custom class pointer
template <class A> class VariantPointer
{
	private:
	// Pointer to target class
	A* pointer_;

	public:
	// Constructor (from class pointer)
	VariantPointer(A* ptr)
	{
		pointer_ = ptr;
	}
	// Constructor (from QVariant)
	VariantPointer(QVariant variant)
	{
		pointer_ = (A*) variant.value<void*>();
	}

	// Convert from class pointer to QVariant
	operator QVariant()
	{
		return QVariant::fromValue((void*) pointer_);
	}

	// Convert from QVariant to class pointer
	operator A*()
	{
		return pointer_;
	}
};

#endif
