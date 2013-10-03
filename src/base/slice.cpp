/*
	*** Slice
	*** src/base/slice.cpp
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

#include "base/slice.h"
#include "base/nxs.h"
#include <QtGui/QMessageBox>

// Constructor
Slice::Slice() : ListItem<Slice>()
{
	sourceFileName_ = "";
	fileAssociated_ = false;
	title_ = "";
	z_ = 0.0;
}

// Destructor
Slice::~Slice()
{
}

// Copy constructor
Slice::Slice(const Slice& source)
{
	(*this) = source;
}

// Assignment operator
void Slice::operator=(const Slice& source)
{
	sourceFileName_ = source.sourceFileName_;
	title_ = source.title_;
	data_ = source.data_;
	z_ = source.z_;
	fileAssociated_ = source.fileAssociated_;
}

/*
// Data
*/

// Set source filename
void Slice::setSourceFileName(QString fileName)
{
	sourceFileName_ = fileName;
	fileAssociated_ = true;
}

// Return source filename
QString Slice::sourceFileName()
{
	return sourceFileName_;
}

// Set source title
void Slice::setTitle(QString title)
{
	title_ = title;
}

// Return source title
QString Slice::title()
{
	return title_;
}

// Load data from file
bool Slice::loadData(QDir sourceDir)
{
	// Check that a fileName is specified - otherwise there is nothing to do
	if (!fileAssociated_) return false;

	// Clear any existing data
	data_.clear();

	// Check file exists
	if (!QFile::exists(sourceDir.absoluteFilePath(sourceFileName_)))
	{
// 		QMessageBox::warning(NULL, "File Not Found", QString("The file ") + sourceFileName_ + " could not be found.");
		return false;
	}

	// Read in the data
	return data_.load(qPrintable(sourceDir.absoluteFilePath(sourceFileName_)));
}

// Return data
Data2D& Slice::data()
{
	return data_;
}

// Set z-coordinate of slice
void Slice::setZ(double z)
{
	z_ = z;
}

// Return z-coordinate of slice
double Slice::z()
{
	return z_;
}

