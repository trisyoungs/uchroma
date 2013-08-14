/*
	*** Save Image Dialog
	*** src/gui/saveimage.h
	Copyright T. Youngs 2013

	This file is part of FQPlot.

	FQPlot is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	FQPlot is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FQPlot.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FQPLOT_SAVEIMAGEDIALOG_H
#define FQPLOT_SAVEIMAGEDIALOG_H

#include "gui/ui_saveimage.h"
#include "gui/viewer.uih"
#include <QtGui/QDialog>

// Forward Declarations
/* None */

class SaveImageDialog : public QDialog
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	SaveImageDialog(QWidget* parent);
	~SaveImageDialog();
	// Main form declaration
	Ui::SaveImageDialog ui;

	private:
	// Current directory for image filename
	QDir currentDirectory_;
	// Current aspect ratio
	double aspectRatio_;

	public:
	// Call dialog to get image save information
	bool getImageDetails(QString currentFilename, int width, int height, Viewer::ImageFormat format, bool maintainAspect, double currentAspect);
	// Return selected filename
	QString imageFileName();
	// Return specified width
	int imageWidth();
	// Return specified height
	int imageHeight();
	// Return selected image format
	Viewer::ImageFormat imageFormat();
	// Return whether aspect ratio is maintained
	bool imageAspectRatioMaintained();


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
