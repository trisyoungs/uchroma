/*
	*** Data Import Dialot
	*** src/gui/dataimport.h
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

#ifndef UCHROMA_DATAIMPORT_H
#define UCHROMA_DATAIMPORT_H

#include "gui/ui_dataimport.h"
#include "base/dataset.h"

// Forward Declarations
/* None */

class DataImportDialog : public QDialog
{
	// All Qt declarations must include this macro
	Q_OBJECT


	/*
	// Window Functions
	*/
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;

	public:
	// Constructor / Destructor
	DataImportDialog(QWidget* parent = 0);
	~DataImportDialog();
	// Main form declaration
	Ui::DataImportDialog ui;
	// Finalise widgets (things that we couldn't do in Qt Designer)
	void finaliseUi();

	protected:
	// Window close event
	void closeEvent(QCloseEvent *event);


	/*
	 * Import Functions
	 */
	private:
	// Current directory for file selector
	QDir currentDirectory_;
	// List of imported datasets
	List<DataSet> importedDataSets_;

	private:
	// Import sequential XY data
	bool importSequentialXY();

	public:
	// Run the import, showing the dialog
	bool import();
	// Return first imported slice
	DataSet* importedSlices();


	/*
	 * Private Slots
	 */
	private slots:
	// -- Global Widgets
	void on_DataFileSelectButton_clicked(bool checked);
	void on_ImportButton_clicked(bool checked);
	void on_CancelButton_clicked(bool checked);
	// -- Sequential XY Data
	
};

#endif
