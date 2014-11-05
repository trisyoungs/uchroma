/*
	*** Session Statics
	*** src/base/session.cpp
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

#include "base/session.h"
#include "version.h"
#include <QtGui/QMainWindow>

// Static variables
bool UChromaSession::modified_ = false;
QMainWindow* UChromaSession::mainWindow_ = NULL;
bool UChromaSession::lastTitleBarModificationStatus_ = false;
QString UChromaSession::inputFile_;

/*
 * Main Window Pointer
 */

// Set main window pointer
void UChromaSession::setMainWindow(QMainWindow* ptr)
{
	mainWindow_ = ptr;

	updateTitleBar();
}

// Update title bar of main window
void UChromaSession::updateTitleBar()
{
	if (!mainWindow_) return;

	// Do nothing if the modification status is the same as it was last time we were called
	if (modified_ == lastTitleBarModificationStatus_) return;

	if (modified_) mainWindow_->setWindowTitle("uChroma v" + QString(UCHROMAREVISION) + " - " + inputFile_ + " (modified) ");
	else mainWindow_->setWindowTitle("uChroma v" + QString(UCHROMAREVISION) + " - " + inputFile_);

	lastTitleBarModificationStatus_ = modified_;
}

/*
 * Input File
 */

// Set name of input file
void UChromaSession::setInputFile(QString fileName)
{
	inputFile_ = fileName;

	updateTitleBar();
}

// Return name of input file
QString UChromaSession::inputFile()
{
	return inputFile_;
}

/*
 * Modification Flag
 */

// Set as modified
void UChromaSession::setAsModified()
{
	modified_ = true;

	updateTitleBar();
}

// Set as not modified
void UChromaSession::setAsNotModified()
{
	modified_ = false;

	updateTitleBar();
}

// Return status of flag
bool UChromaSession::isModified()
{
	return modified_;
}
