/*
	*** Save Image Dialog
	*** src/gui/saveimage.h
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

#ifndef UCHROMA_SAVEIMAGEDIALOG_H
#define UCHROMA_SAVEIMAGEDIALOG_H

#include "gui/ui_saveimage.h"
#include "session/session.h"
#include <QtGui/QDialog>

// Forward Declarations
/* None */

class SaveImageDialog : public QDialog
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	SaveImageDialog(UChromaWindow& parent);
	~SaveImageDialog();
	// Main form declaration
	Ui::SaveImageDialog ui;
	// UChromaWindow reference
	UChromaWindow& uChroma_;

	private:
	// Current directory for image filename
	QDir currentDirectory_;
	// Current aspect ratio
	double aspectRatio_;

	public:
	// Call dialog to get/update image save information in UChromaSession
	bool getImageDetails(double currentAspect);


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
