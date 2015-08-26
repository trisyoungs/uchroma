/*
	*** Viewer Input Functions
	*** src/gui/viewer_input.cpp
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

#include "gui/viewer.hui"
#include "gui/uchroma.h"
#include <QMouseEvent>

/*
 * Mouse Input
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

	// The clicked pane will now become the current pane
	if (UChromaSession::setCurrentViewPane(event->x(), height()-event->y())) postRedisplay();

	// Do something with the button press event (e.g. context menu function, or interaction start)
	if (buttonState_&Qt::LeftButton) uChromaWindow_->startInteraction(event->x(), contextHeight_-event->y(), km);
	else if (buttonState_&Qt::RightButton)
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
// 	Qt::KeyboardModifiers km = event->modifiers();

	// Notify uChroma that the mouse button has been released
	if (buttonState_&Qt::LeftButton) uChromaWindow_->endInteraction(event->x(), contextHeight_-event->y());

	uChromaWindow_->updateGUI();
	
	msg.exit("Viewer::mouseReleaseEvent");
}

// Qt Signal (mouse move event)
void Viewer::mouseMoveEvent(QMouseEvent *event)
{
	Vec3<double> delta;

	// Get event information and position delta
	Qt::KeyboardModifiers km = event->modifiers();
	delta.set(event->x(), event->y(), 0.0);
	delta = delta - rMouseLast_;
	bool refresh = false;

	// Is the current view pane valid?
	ViewPane* targetPane = UChromaSession::currentViewPane();
	if (ViewPane::objectValid(targetPane, "ViewPane in Viewer::MouseMoveEvent"))
	{
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
				targetPane->rotateView(delta.y/2.0, delta.x/2.0);
				refresh = true;
			}
		}
		else if (buttonState_&Qt::MidButton)
		{
			// If this is a flat view, shift the axis limits rather than the view
			if (targetPane->isFlatView()) targetPane->shiftFlatAxisLimits(delta.x, -delta.y);
			else targetPane->translateView(delta.x/15.0, -delta.y/15.0, 0.0);
			refresh = true;
		}
	}
	
	// Update interaction position
	if (uChromaWindow_->interactionAxis() != -1)
	{
		uChromaWindow_->updateInteractionPosition(event->x(), contextHeight_-event->y());
		refresh = true;
	}

	rMouseLast_.set(event->x(), event->y(), 0.0);

	setFocus();

	if (refresh) uChromaWindow_->updateGUI();
}

// Qt Signal (mouse wheel event)
void Viewer::wheelEvent(QWheelEvent *event)
{
	msg.enter("Viewer::wheelEvent");
	bool scrollup = event->delta() > 0;

	// The pane underneath the mouse will now become the current pane
	if (UChromaSession::setCurrentViewPane(event->x(), height()-event->y())) postRedisplay();

	// Perform camera zoom
	if (UChromaSession::currentViewPane())
	{
		double zrange = UChromaSession::currentViewPane()->axes().stretch(2) * UChromaSession::currentViewPane()->axes().realRange(2);
		if (zrange < 1.0) zrange = 1.0;
		UChromaSession::currentViewPane()->translateView(0.0, 0.0, 0.5*zrange*(scrollup ? -1.0 : 1.0));
	}

	postRedisplay();

	msg.exit("Viewer::wheelEvent");
}

// Qt Virtual (mouse double click event)
void Viewer::mouseDoubleClickEvent(QMouseEvent* event)
{
	uChromaWindow_->doubleClickInteraction(event->x(), height()-event->y());
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
 * Key Input
 */

// Qt Slot (key press event)
void Viewer::keyPressEvent(QKeyEvent *event)
{
	// Check datamodel...
	bool refresh = false, ignore = true;
	Qt::KeyboardModifiers km = event->modifiers();
	
	switch (event->key())
	{
		case (Qt::Key_Left):
			if (UChromaSession::currentViewPane()) UChromaSession::currentViewPane()->rotateView(0.0, km.testFlag(Qt::ShiftModifier) ? -1.0 : -10.0);
			refresh = true;
			ignore = false;
			break;
		case (Qt::Key_Right):
			if (UChromaSession::currentViewPane()) UChromaSession::currentViewPane()->rotateView(0.0, km.testFlag(Qt::ShiftModifier) ? 1.0 : 10.0);
			refresh = true;
			ignore = false;
			break;
		case (Qt::Key_Up):
			if (UChromaSession::currentViewPane()) UChromaSession::currentViewPane()->rotateView(km.testFlag(Qt::ShiftModifier) ? -1.0 : -10.0, 0.0);
			refresh = true;
			ignore = false;
			break;
		case (Qt::Key_Down):
			if (UChromaSession::currentViewPane()) UChromaSession::currentViewPane()->rotateView(km.testFlag(Qt::ShiftModifier) ? 1.0 : 10.0, 0.0);
			refresh = true;
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
// 	Qt::KeyboardModifiers km = event->modifiers();
	
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
