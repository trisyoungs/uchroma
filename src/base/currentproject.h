/*
	*** Current Project Statics
	*** src/base/currentproject.h
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

#ifndef UCHROMA_CURRENTPROJECT_H
#define UCHROMA_CURRENTPROJECT_H

#include <QtCore/QString>

// Forward Declarations
class QMainWindow;

// Modified Flag
class CurrentProject
{
	/*
	 * Main Window Link
	 */
	private:
	// Pointer to main window
	static QMainWindow* mainWindow_;

	private:
	// Update title bar of main window
	static void updateTitleBar();
	// Last modification status that the title bar was updated at
	static bool lastTitleBarModificationStatus_;

	public:
	// Set main window pointer
	static void setMainWindow(QMainWindow* ptr);


	/*
	 * Input File
	 */
	private:
	// Current input filename
	static QString inputFile_;

	public:
	// Set name of input file
	static void setInputFile(QString fileName);
	// Return name of input file
	static QString inputFile();


	/*
	 * Modification Flag
	 */
	private:
	// Modified flag
	static bool modified_;

	public:
	// Set status to true
	static void setAsModified();
	// Set status to false
	static void setAsNotModified();
	// Return modified status
	static bool isModified();
};

#endif
