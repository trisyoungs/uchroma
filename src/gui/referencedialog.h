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
#include "base/referencevariable.h"
#include <QtGui/QDialog>

// Forward Declarations
class FitKernel;

class ReferenceSetupDialog : public QDialog
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	ReferenceSetupDialog(QWidget *parent);
	~ReferenceSetupDialog();
	// Main form declaration
	Ui::ReferenceSetupDialog ui;


	/*
	 * Window Functions
	 */
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;
	// Parent FitKernel
	FitKernel* referenceParent_;
	// Target reference pointer
	ReferenceVariable* referenceTarget_;
	// Backup copy of reference (in case the dialog is cancelled)
	ReferenceVariable referenceBackup_;

	protected:
	// Window close event
	void closeEvent(QCloseEvent* event);

	public slots:
	void reject();
	void on_OKButton_clicked(bool checked);
	void on_CancelButton_clicked(bool checked);

	public:
	// Call dialog to edit specified reference
	bool call(ReferenceVariable* target, FitKernel* parentKernel);


	/*
	 * Slots
	 */
	private slots:
	void on_NameEdit_textChanged(QString text);
	void on_XNormalRadio_clicked(bool checked);
	void on_XFixedRadio_clicked(bool checked);
	void on_XRelativeRadio_clicked(bool checked);
	void on_XFixedSpin_valueChanged(int value);
	void on_XRelativeSpin_valueChanged(int value);
	void on_ZNormalRadio_clicked(bool checked);
	void on_ZFixedRadio_clicked(bool checked);
	void on_ZRelativeRadio_clicked(bool checked);
	void on_ZRelativeSpin_valueChanged(int value);	
	void on_ZDataSetCombo_currentIndexChanged(int index);


	private:
	// Update labels
	void updateLabels();
};

#endif
