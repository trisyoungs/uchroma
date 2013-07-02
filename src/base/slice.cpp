/*
	*** Slice
	*** src/main/slice.cpp
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

#include "base/slice.h"
#include <QtGui/QMessageBox>

// Constructor
Slice::Slice() : ListItem<Slice>()
{
	fileName_ = "";
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
	fileName_ = source.fileName_;
	data_ = source.data_;
	z_ = source.z_;
}

/*
// Data
*/

// Set source filename
void Slice::setFileName(QString fileName)
{
	fileName_ = fileName;

	// Determine instrument and run number strings
	QRegExp re("([A-Z]+)([0-9]+)");
	if (re.indexIn(fileName_) > -1)
	{
		instrument_ = re.cap(1);
		runNumberString_ = re.cap(2);
	}
}

// Return source filename
QString Slice::fileName()
{
	return fileName_;
}

// Return determined base file name with no extension
QString Slice::baseFileName()
{
	return instrument_+runNumberString_;
}

// Load data from file
bool Slice::loadData(QDir sourceDir)
{
	// Clear any existing data
	data_.clear();

	// Check file exists
	if (!QFile::exists(sourceDir.absoluteFilePath(fileName_)))
	{
		QMessageBox::warning(NULL, "File Not Found", QString("The file ") + fileName_ + " could not be found.");
		return false;
	}

	// Open the file, and read in the data
	return data_.load(qPrintable(sourceDir.absoluteFilePath(fileName_)));
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

