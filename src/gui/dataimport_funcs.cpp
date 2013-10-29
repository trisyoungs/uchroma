/*
	*** Data Import Dialog - Functions 
	*** src/gui/dataimport_funcs.cpp
	Copyright T. Youngs 2013

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

#include "gui/dataimport.h"
#include <QtGui/QFileDialog>

// Constructor
DataImportDialog::DataImportDialog(QWidget *parent) : QDialog(parent)
{
	// Call the main creation function
	ui.setupUi(this);

	// Set widget values
	currentDirectory_ = QDir::homePath();
	ui.DataFileEdit->setText(currentDirectory_.path());

	// Set variable defaults
	refreshing_ = false;
}

// Destructor
DataImportDialog::~DataImportDialog()
{
}

// Window close event
void DataImportDialog::closeEvent(QCloseEvent *event)
{
	reject();
}

/*
 * Data Import
 */

// Run the import, showing the dialog
bool DataImportDialog::import()
{
	importedSlices_.clear();
	int result = exec();
}


// Return first imported slice
Slice* DataImportDialog::importedSlices()
{
	return importedSlices_.first();
}

/*
 * Private Slots
 */

void DataImportDialog::on_DataFileSelectButton_clicked(bool checked)
{
	QString newFile = QFileDialog::getOpenFileName(this, "Choose import file name", currentDirectory_.absolutePath(), "All files (*.*)");
	if (!newFile.isEmpty())
	{
		ui.DataFileEdit->setText(newFile);
		currentDirectory_ = newFile;
	}
}

void DataImportDialog::on_ImportButton_clicked(bool checked)
{
	// Based on the current data type selected, call the correct import function
	bool result;
	if (ui.DataTypeList->currentRow() == 0) result = importSequentialXY();
	
	if (result) accept();
	else reject();
}

void DataImportDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}