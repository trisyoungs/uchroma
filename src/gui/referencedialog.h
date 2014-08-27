/*
	*** Reference Setup Dialog
	*** src/gui/referencedialog.h
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

#ifndef UCHROMA_REFERENCEDIALOG_H
#define UCHROMA_REFERENCEDIALOG_H

#include "gui/ui_referencedialog.h"
#include <QtGui/QDialog>

// Forward Declarations
class UChromaWindow;

class ReferenceSetupDialog : public QDialog
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	ReferenceSetupDialog(UChromaWindow& parent);
	~ReferenceSetupDialog();
	// Main form declaration
	Ui::ReferenceSetupDialog ui;
	// UChromaWindow reference
	UChromaWindow& uChroma_;

	private:

	public:


	/*
	 * Slots
	 */
	private slots:
	void on_SelectFileNameButton_clicked(bool checked);
	void on_ImageWidthSpin_valueChanged(int value);
	void on_MaintainAspectRatioCheck_toggled(bool checked);
	void on_SaveImageButton_clicked(bool checked);
	void on_CancelButton_clicked(bool checked);
};

#endif
