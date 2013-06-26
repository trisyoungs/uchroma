/*
	*** Slice
	*** src/main/slice.h
	Copyright T. Youngs 2013

	This file is part of FQPlot.

	FQPlot is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	FQPlot is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FQPlot.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FQPLOT_SLICE_H
#define FQPLOT_SLICE_H

#include "base/data2d.h"
#include "base/dnchar.h"
#include "templates/list.h"
#include <QtCore/QDir>

// Forward Declarations
/* None */

// Slice
class Slice : public ListItem<Slice>
{
	public:
	// Constructor / Destructor
	Slice();
	~Slice();
	// Copy constructor
	Slice(const Slice& source);
	// Assignment operator
	void operator=(const Slice& source);


	/*
	// Data
	*/
	private:
	// Source file (not including path)
	QString fileName_;
	// Data
	Data2D data_;
	// Z coordinate
	double z_;

	public:
	// Set source filename
	void setFileName(QString fileName);
	// Return source filename
	QString fileName();
	// Load data from file
	bool loadData(QDir sourceDir);
	// Return data
	Data2D& data();
	// Set z-coordinate of slice
	void setZ(double z);
	// Return z-coordinate of slice
	double z();
};

#endif
