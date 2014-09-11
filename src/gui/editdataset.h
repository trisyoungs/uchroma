/*
	*** Edit DataSet Dialog
	*** src/gui/editdataset.h
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

#ifndef UCHROMA_EDITDATASETDIALOG_H
#define UCHROMA_EDITDATASETDIALOG_H

#include "gui/ui_editdataset.h"
#include "base/dataset.h"
#include <QtGui/QDialog>

// Forward Declarations
/* none */

class EditDataSetDialog : public QDialog
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	EditDataSetDialog(QWidget *parent);
	~EditDataSetDialog();
	// Main form declaration
	Ui::EditDataSetDialog ui;


	/*
	 * Window Functions
	 */
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;
	// Data we are editing
	DataSet dataSet_;

	protected:
	// Window close event
	void closeEvent(QCloseEvent* event);

	public slots:
	void reject();
	void on_OKButton_clicked(bool checked);
	void on_CancelButton_clicked(bool checked);

	public:
	// Call dialog to edit specified reference
	bool call(DataSet* target);
	// Get current data stored in dialog
	DataSet& dataSet();


	/*
	 * Slots
	 */
	private slots:
	void on_DataTable_cellChanged(int row, int column);
};

#endif
