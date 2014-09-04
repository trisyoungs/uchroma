/*
	*** Log Window
	*** src/log.h
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

#ifndef UCHROMA_LOG_H
#define UCHROMA_LOG_H

#include "gui/ui_log.h"

// Forward Declarations
class UChromaWindow;

class LogWindow : public QWidget
{
	// All Qt declarations must include this macro
	Q_OBJECT


	/*
	// Window Functions
	*/
	public:
	// Constructor / Destructor
	LogWindow(UChromaWindow& parent);
	~LogWindow();
	// Main form declaration
	Ui::LogWindow ui;
	// UChromaWindow reference
	UChromaWindow& uChroma_;


	/*
	// Widget Slots
	*/
	private slots:
	// Close button
	void on_CloseButton_clicked(bool checked);
};

#endif
