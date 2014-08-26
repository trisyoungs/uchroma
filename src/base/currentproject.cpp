/*
	*** CurrentProject Flag
	*** src/base/modified.cpp
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

#include "base/currentproject.h"
#include "version.h"
#include <QtGui/QMainWindow>

// Static variables
bool CurrentProject::modified_ = false;
QMainWindow* CurrentProject::mainWindow_ = NULL;
bool CurrentProject::lastTitleBarModificationStatus_ = false;
QString CurrentProject::inputFile_;

/*
 * Main Window Pointer
 */

// Set main window pointer
void CurrentProject::setMainWindow(QMainWindow* ptr)
{
	mainWindow_ = ptr;

	updateTitleBar();
}

// Update title bar of main window
void CurrentProject::updateTitleBar()
{
	if (!mainWindow_) return;

	if (modified_) mainWindow_->setWindowTitle("uChroma v" + QString(UCHROMAREVISION) + " - " + inputFile_ + " (modified) ");
	else mainWindow_->setWindowTitle("uChroma v" + QString(UCHROMAREVISION) + " - " + inputFile_);

	lastTitleBarModificationStatus_ = modified_;
}

/*
 * Input File
 */

// Set name of input file
void CurrentProject::setInputFile(QString fileName)
{
	inputFile_ = fileName;

	updateTitleBar();
}

// Return name of input file
QString CurrentProject::inputFile()
{
	return inputFile_;
}

/*
 * Modification Flag
 */

// Set as modified
void CurrentProject::setAsModified()
{
	modified_ = true;

	if (modified_ != lastTitleBarModificationStatus_) updateTitleBar();
}

// Set as not modified
void CurrentProject::setAsNotModified()
{
	modified_ = false;

	if (modified_ != lastTitleBarModificationStatus_) updateTitleBar();
}

// Return status of flag
bool CurrentProject::isModified()
{
	return modified_;
}
