/*
	*** Save Image Dialog
	*** src/gui/saveimage_funcs.cpp
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

#include "gui/saveimage.h"
#include "gui/uchroma.h"
#include <QtGui/QFileDialog>

// Constructor
SaveImageDialog::SaveImageDialog(UChromaWindow& parent) : QDialog(&parent), uChroma_(parent)
{
	ui.setupUi(this);
	
	// Populate format combo
	for (int n=0; n<UChromaSession::nImageFormats; ++n) ui.ImageFormatCombo->addItem( QString(UChromaSession::imageFormatExtension((UChromaSession::ImageFormat) n)).toUpper());
}

// Destructor
SaveImageDialog::~SaveImageDialog()
{
}

// Call dialog to get image save information
bool SaveImageDialog::getImageDetails(double currentAspect)
{
	ui.FileNameEdit->setText(UChromaSession::imageExportFileName());
	currentDirectory_.setPath(UChromaSession::imageExportFileName());
	aspectRatio_ = currentAspect;
	ui.MaintainAspectRatioCheck->setChecked(UChromaSession::imageExportMaintainAspect());
	ui.ImageWidthSpin->setValue(UChromaSession::imageExportWidth());
	ui.ImageHeightSpin->setValue(UChromaSession::imageExportMaintainAspect() ? UChromaSession::imageExportWidth()/aspectRatio_ : UChromaSession::imageExportHeight());
	ui.ImageFormatCombo->setCurrentIndex(UChromaSession::imageExportFormat());

	int result = exec();
	return (result == 1);
}

/*
 * Slots
 */

void SaveImageDialog::on_SelectFileNameButton_clicked(bool checked)
{
	QString newFile = QFileDialog::getSaveFileName(this, "Choose image save file name", currentDirectory_.absolutePath(), QString(UChromaSession::imageFormatFilter((UChromaSession::ImageFormat) ui.ImageFormatCombo->currentIndex())) + ";;All files (*.*)");
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
	// Set updated values...
	UChromaSession::setImageExportFileName(ui.FileNameEdit->text());
	UChromaSession::setImageExportWidth(ui.ImageWidthSpin->value());
	UChromaSession::setImageExportHeight(ui.ImageHeightSpin->value());
	UChromaSession::setImageExportMaintainAspect(ui.MaintainAspectRatioCheck->isChecked());
	UChromaSession::setImageExportFormat((UChromaSession::ImageFormat) ui.ImageFormatCombo->currentIndex());

	accept();
}

void SaveImageDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}
