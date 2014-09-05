/*
	*** Line Style Dialog
	*** src/gui/linestyledialog.h
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

#ifndef UCHROMA_LINESTYLEDIALOG_H
#define UCHROMA_LINESTYLEDIALOG_H

#include "gui/ui_linestyledialog.h"
#include "render/linestyle.h"
#include <QtGui/QDialog>

// Forward Declarations
/* none */

class LineStyleDialog : public QDialog
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	LineStyleDialog(QWidget *parent);
	~LineStyleDialog();
	// Main form declaration
	Ui::LineStyleDialog ui;


	/*
	 * Window Functions
	 */
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;
	// Target reference pointer
	LineStyle* lineStyleTarget_;
	// Backup copy of LineStyle (in case the dialog is cancelled)
	LineStyle lineStyleBackup_;

	protected:
	// Window close event
	void closeEvent(QCloseEvent* event);

	public slots:
	void reject();
	void on_OKButton_clicked(bool checked);
	void on_CancelButton_clicked(bool checked);

	public:
	// Call dialog to edit specified reference
	bool call(LineStyle* target);


	/*
	 * Slots
	 */
	private slots:
	void on_LineColourButton_clicked(bool checked);
	void on_LineWidthSpin_valueChanged(double value);
	void on_LineStippleCombo_currentIndexChanged(int currentIndex);
};

#endif
