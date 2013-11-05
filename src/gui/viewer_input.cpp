/*
	*** Viewer Input Functions
	*** src/gui/viewer_input.cpp
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

#include "gui/viewer.uih"
#include "gui/uchroma.h"
#include <QtGui/QMouseEvent>

/*
// Mouse Input
*/

// Qt Signal (mouse press event)
void Viewer::mousePressEvent(QMouseEvent *event)
{
	// Handle button presses (button down) from the mouse
	msg.enter("Viewer::mousePressEvent");
	buttonState_ = event->buttons();
	Qt::KeyboardModifiers km = event->modifiers();

	// Store event information
	rMouseDown_.set(event->x(), event->y(), 0.0);

	if (buttonState_&Qt::LeftButton)
	{
		if (uChroma_->sliceAxis() != -1)
		{
// 			calculateMouseAxisValues();
// 			if (uChroma_->axisLogarithmic(sliceAxis_)) emit(sliceAxisClicked(sliceAxis_, pow(10.0, sliceAxisValue_))); // TODO
// 			else emit(sliceAxisClicked(sliceAxis_, sliceAxisValue_));
		}
		
	}

	// Do something with the button press event (e.g. context menu function)?
	if (buttonState_&Qt::RightButton)
	{

	}

	msg.exit("Viewer::mousePressEvent");
}

// Qt Signal (mouse release event)
void Viewer::mouseReleaseEvent(QMouseEvent *event)
{
	// Handle button releases (button up) from the mouse
	msg.enter("Viewer::mouseReleaseEvent");
	buttonState_ = event->buttons();
	Qt::KeyboardModifiers km = event->modifiers();
	


	postRedisplay();
	
	msg.exit("Viewer::mouseReleaseEvent");
}

// Qt Signal (mouse move event)
void Viewer::mouseMoveEvent(QMouseEvent *event)
{
	Vec3<double> delta;
	Matrix A;
	
	// Get event information and position delta
	Qt::KeyboardModifiers km = event->modifiers();
	delta.set(event->x(), event->y(), 0.0);
	delta = delta - rMouseLast_;
	bool refresh = false;

	if (buttonState_&Qt::RightButton)
	{
		// View manipulation
		if (km&Qt::ShiftModifier)
		{
		}
		else if (km&Qt::ControlModifier)
		{
		}
		else 
		{
			// Rotate the view ('XY plane')
			A.createRotationXY(delta.y/2.0, delta.x/2.0);
			A.copyTranslationAndScaling(viewMatrix_);
			// Reset translation and scaling on original matrix, and multiply
			viewMatrix_.removeTranslationAndScaling();
			viewMatrix_ = A * viewMatrix_;
			refresh = true;
		}
	}
	else if (buttonState_&Qt::MidButton)
	{
		viewMatrix_.adjustColumn(3, delta.x/15.0, -delta.y/15.0, 0.0, 0.0);
		refresh = true;
	}
	
	// Recalculate slice value
	if (uChroma_->updateSliceValue(event->x(), contextHeight_-event->y())) refresh = true;

	rMouseLast_.set(event->x(), event->y(), 0.0);
	setFocus();
	
	if (refresh) postRedisplay();
}

// Qt Signal (mouse wheel event)
void Viewer::wheelEvent(QWheelEvent *event)
{
	msg.enter("Viewer::wheelEvent");
	bool scrollup = event->delta() > 0;
	
	// Perform camera zoom
	double dz = -viewMatrix_[14] * 0.15;
	if (scrollup) dz = -dz;
	viewMatrix_.adjustColumn(3, 0.0, 0.0, dz, 0.0);
	// Never let camera z go below -1.0...
	if (viewMatrix_[14] > -1.0) viewMatrix_[14] = -1.0;
	postRedisplay();

	msg.exit("Viewer::wheelEvent");
}

// Return mouse coordinates at last mousedown event
Vec3<double> Viewer::rMouseDown()
{
	return rMouseDown_;
}

// Return mouse coordinates at last mousemove event
Vec3<double> Viewer::rMouseLast()
{
	return rMouseLast_;
}

/*
// Key Input
*/

// Qt Slot (key press event)
void Viewer::keyPressEvent(QKeyEvent *event)
{
	// Check datamodel...
	bool refresh = false, ignore = true;
	Qt::KeyboardModifiers km = event->modifiers();
	Matrix A;
	int n;
	
	switch (event->key())
	{
		case (Qt::Key_Left):
			A.createRotationXY(0.0, km.testFlag(Qt::ShiftModifier) ? -1.0 : -10.0);
			A.copyTranslationAndScaling(viewMatrix_);
			viewMatrix_.removeTranslationAndScaling();
			viewMatrix_ = A * viewMatrix_;
			refresh = true;
			ignore = false;
			break;
		case (Qt::Key_Right):
			A.createRotationXY(0.0, km.testFlag(Qt::ShiftModifier) ? 1.0 : 10.0);
			A.copyTranslationAndScaling(viewMatrix_);
			viewMatrix_.removeTranslationAndScaling();
			viewMatrix_ = A * viewMatrix_;
			refresh = true;
			ignore = false;
			break;
		case (Qt::Key_Up):
			A.createRotationXY(km.testFlag(Qt::ShiftModifier) ? -1.0 : -10.0, 0.0);
			A.copyTranslationAndScaling(viewMatrix_);
			viewMatrix_.removeTranslationAndScaling();
			viewMatrix_ = A * viewMatrix_;
			refresh = true;
			ignore = false;
			break;
		case (Qt::Key_Down):
			A.createRotationXY(km.testFlag(Qt::ShiftModifier) ? 1.0 : 10.0, 0.0);
			A.copyTranslationAndScaling(viewMatrix_);
			viewMatrix_.removeTranslationAndScaling();
			viewMatrix_ = A * viewMatrix_;
			refresh = true;
			ignore = false;
			break;
		case (Qt::Key_X):
		case (Qt::Key_Y):
		case (Qt::Key_Z):
			uChroma_->setSliceAxis(uChroma_->sliceAxis() == (event->key() - Qt::Key_X) ? -1 : (event->key() - Qt::Key_X));
			setSlicePrimitive(uChroma_->sliceAxis());
			uChroma_->updateSliceValue(rMouseLast_.x, contextHeight_ - rMouseLast_.y);
			refresh = true;
			ignore = false;
			break;
		// Cycle render styles
		case (Qt::Key_F8):
			ignore = false;
			break;
		// Cycle colouring styles
		case (Qt::Key_F9):
			ignore = false;
			break;
		default:
			break;
	}
	
	// Update display if necessary
	if (refresh) postRedisplay();
	if (ignore) event->ignore();
	else event->accept();
}

// Qt Slot (key release event)
void Viewer::keyReleaseEvent(QKeyEvent *event)
{
	// Check datamodel...
	bool refresh = false, ignore = true;
	Qt::KeyboardModifiers km = event->modifiers();
	
	switch (event->key())
	{
		// Cycle render styles
		case (Qt::Key_F8):
			ignore = false;
			break;
		// Cycle colouring styles
		case (Qt::Key_F9):
			ignore = false;
			break;
		default:
			break;
	}
	
	// Update display if necessary
	if (refresh) postRedisplay();
	if (ignore) event->ignore();
	else event->accept();
}
