/*
	*** Operate - Background Subtraction Dialog
	*** src/gui/operate_bgsub.h
	Copyright T. Youngs 2012-2014.

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

#ifndef UCHROMA_OPERATE_BGSUB_H
#define UCHROMA_OPERATE_BGSUB_H

#include "gui/ui_operate_bgsub.h"
#include <QtGui/QDialog>
#include <QtCore/QObject>

// Forward Declarations
class Collection;

/*
 * Background Subtraction Dialog
 */
class OperateBGSubDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	OperateBGSubDialog(QWidget *parent, Collection* targetCollection);
	// Destructor
	~OperateBGSubDialog();
	// Main form declaration
	Ui::OperateBGSubDialog ui;


	/*
	 * Local Variables
	 */
	private:
	// Collection whose data should be modified
	Collection* targetCollection_;


	/*
	 * Slots
	 */
	public slots:
	// -- Equation
	void on_FromEquationRadio_toggled(bool checked);
	void on_FromEquationEdit_textChanged(QString text);
	// -- Source Files

	void on_CancelButton_clicked(bool checked);
	void on_ApplyButton_clicked(bool checked);


	/*
	 * Functions
	 */
	public:



	/*
	 * Update
	 */
	private:
	// Update controls
	void updateControls();
};

#endif
