/*
	*** Log Window Functinos
	*** src/log_funcs.cpp
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

#include "gui/log.h"
#include "gui/uchroma.h"

// Constructor
LogWindow::LogWindow(UChromaWindow& parent) : QWidget(&parent), uChroma_(parent)
{
	// Call the main creation function
	ui.setupUi(this);

	QWidget::setWindowFlags(Qt::Tool);
}

// Destructor
LogWindow::~LogWindow()
{
}

// Window close event
void LogWindow::closeEvent(QCloseEvent* event)
{
	emit(windowClosed(false));
}

/*
 * Widget Slots
 */

// Close button pressed
void LogWindow::on_CloseButton_clicked(bool checked)
{
	hide();
	emit(windowClosed(false));
}

/*
 * Update Functions
 */

// Update controls and show window
void LogWindow::updateAndShow()
{
	show();
	move(uChroma_.centrePos() - QPoint(width()/2, height()/2));
}
