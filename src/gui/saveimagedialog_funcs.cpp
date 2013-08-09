/*
	*** Save Image Dialog
	*** src/gui/saveimagedialog.cpp
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

#include "gui/saveimagedialog.h"
#include <QtGui/QFileDialog>

// Constructor
SaveImageDialog::SaveImageDialog(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);
	
	// Populate format combo
	for (int n=0; n<Viewer::nImageFormats; ++n) ui.ImageFormatCombo->addItem( QString(Viewer::imageFormatExtension((Viewer::ImageFormat) n)).toUpper());
}

// Destructor
SaveImageDialog::~SaveImageDialog()
{
}

// Call dialog to get image save information
bool SaveImageDialog::getImageDetails(QString currentFilename, int width, int height, Viewer::ImageFormat format, bool maintainAspect, double currentAspect)
{
	ui.FileNameEdit->setText(currentFilename);
	currentDirectory_.setPath(currentFilename);
	aspectRatio_ = currentAspect;
	ui.MaintainAspectRatioCheck->setChecked(maintainAspect);
	ui.ImageWidthSpin->setValue(width);
	ui.ImageHeightSpin->setValue(maintainAspect ? width/aspectRatio_ : height);
	ui.ImageFormatCombo->setCurrentIndex(format);

	int result = exec();
	return (result == 1);
}

// Return selected filename
QString SaveImageDialog::imageFileName()
{
	return ui.FileNameEdit->text();
}

// Return specified width
int SaveImageDialog::imageWidth()
{
	return ui.ImageWidthSpin->value();
}

// Return specified height
int SaveImageDialog::imageHeight()
{
	return ui.ImageHeightSpin->value();
}

// Return selected image format
Viewer::ImageFormat SaveImageDialog::imageFormat()
{
	return Viewer::imageFormat(qPrintable(ui.ImageFormatCombo->currentText().toLower()));
}

// Return whether aspect ratio is maintained
bool SaveImageDialog::imageAspectRatioMaintained()
{
	return ui.MaintainAspectRatioCheck->isChecked();
}

/*
 * Slots
 */

void SaveImageDialog::on_SelectFileNameButton_clicked(bool checked)
{
	QString newFile = QFileDialog::getSaveFileName(this, "Choose image save file name", currentDirectory_.absolutePath(), QString(Viewer::imageFormatFilter((Viewer::ImageFormat) ui.ImageFormatCombo->currentIndex())) + ";;All files (*.*)");
	if (!newFile.isEmpty()) ui.FileNameEdit->setText(newFile);
}

void SaveImageDialog::on_ImageWidthSpin_valueChanged(int value)
{
	if (ui.MaintainAspectRatioCheck->isChecked()) ui.ImageHeightSpin->setValue(value / aspectRatio_);
}

void SaveImageDialog::on_MaintainAspectRatioCheck_toggled(bool checked)
{
	ui.ImageHeightSpin->setDisabled(checked);
	if (checked) ui.ImageHeightSpin->setValue(ui.ImageWidthSpin->value() / aspectRatio_);
}

void SaveImageDialog::on_SaveImageButton_clicked(bool checked)
{
	accept();
}

void SaveImageDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}
