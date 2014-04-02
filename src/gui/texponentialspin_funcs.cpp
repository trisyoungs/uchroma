/*
	*** TExponentialSpin Functions
	*** src/gui/texponentialspin_funcs.cpp
	Copyright T. Youngs 2014

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

#include "gui/texponentialspin.uih"
#include <QLineEdit>

// Constructor
TExponentialSpin::TExponentialSpin(QWidget *parent) : QAbstractSpinBox(parent)
{
	// Set validator
	QRegExp regExp("[-+]?[0-9]*[.]?[0-9]+([eE][-+]?[0-9]+)?");
	validator_.setRegExp(regExp);
	lineEdit()->setValidator(&validator_);

	// Connect signals to slots
	connect(this, SIGNAL(editingFinished()), this, SLOT(updateValue()));
}

/*
 * Data
 */

// Clamp current value to allowable range
bool TExponentialSpin::clamp()
{
	if (value_.value() < valueMin_)
	{
		value_ = valueMin_;
		emit(valueChanged(value_.value()));
		return true;
	}
	else if (value_.value() > valueMax_)
	{
		value_ = valueMax_;
		emit(valueChanged(value_.value()));
		return true;
	}
	return false;
}

// Create text from current value, and display in lineEdit
void TExponentialSpin::updateText(int precision)
{
	lineEdit()->setText(value_.text(precision));
}

// Return double value
double TExponentialSpin::value()
{
	return value_.value();
}

// Set value
void TExponentialSpin::setValue(double value)
{
	// Store number, and then clamp it to range
	value_ = value;
	clamp();

	// Update text
	updateText();
}

// Set allowable range of value
void TExponentialSpin::setRange(double minvalue, double maxvalue)
{
	valueMin_ = minvalue;
	valueMax_ = maxvalue;

	// Clamp current value if necessary
	if (clamp()) updateText();
}

// Set single step value
void TExponentialSpin::setSingleStep(double step)
{
	valueStep_ = step;
}

/*
 * Slots (Private)
 */

// Update value from current text
void TExponentialSpin::updateValue()
{
	value_.set(qPrintable(text()));
	emit(valueChanged(value_.value()));
}

/*
 * Virtual Overloads
 */

// Step value by specified amount
void TExponentialSpin::stepBy(int nSteps)
{
	value_ = value_.value() + valueStep_*nSteps;

	// Check new value and update text
	clamp();
	updateText();
	emit(valueChanged(value_.value()));
}

// Return which steps should be enabled
QAbstractSpinBox::StepEnabled TExponentialSpin::stepEnabled() const
{
	bool up = value_.value() < valueMax_;
	bool down = value_.value() > valueMin_;

	if (up && down) return (QAbstractSpinBox::StepUpEnabled | QAbstractSpinBox::StepDownEnabled);
	else if (up) return QAbstractSpinBox::StepUpEnabled;
	else if (down) return QAbstractSpinBox::StepDownEnabled;
	else return QAbstractSpinBox::StepNone;
}
