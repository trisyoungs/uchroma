/*
	*** UChroma - Interaction
	*** src/gui/uchroma_interaction.cpp
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

#include "gui/uchroma.h"
#include "gui/operate_bgsub.h"

/*
 * Interaction Modes
 */

// Static list of interaction modes
InteractionMode InteractionMode::interactionModes[] = {
	{ "Fit (select X)",			"<b>Left-click-drag</b> Select X region for fitting" },
	{ "Fit (select Z)",			"<b>Left-click-drag</b> Select Z region for fitting" },
	{ "BG Subtraction (select Y)",	  	"<b>Left-click</b> select Y value for subtraction"  },
	{ "BG Subtraction (select X range)", 	"<b>Left-click-drag</b> select X range for determination of average value"  },	
	{ "BG Subtraction (select Z range)", 	"<b>Left-click-drag</b> select Z range for determination of average value"  },	
	{ "View",				"<b>Right</b> Rotate view, <b>Middle</b> Translate view (3D) or shift axis limits (2D), <b>Wheel</b> Zoom view" },
	{ "Zoom",				"<b>Left-click-drag</b> Zoom to selected region, <b>Ctrl+Left</b> Extract slice" }
};

/*
 * Private Functions
 */

// Calculate interaction axis coordinate from supplied screen coordinates
double UChromaWindow::screenToAxis(int axis, int mouseX, int mouseY)
{
	// Check for a valid axis
	if (axis == -1) return 0.0;

	// Check for valid interaction pane
	ViewPane* viewPane = UChromaSession::currentViewPane();
	if (viewPane == NULL) return 0.0;

// 	printf("Test: min=%f, max=%f\n", min_[0], max_[0]);
// 	rMouseLast_.print();
// 	axisCoordMin_[0].print();
	// Project axis coordinates to get a screen-based yardstick
	Vec3<double> axmin = viewPane->modelToScreen(viewPane->axes().coordMin(axis));
	Vec3<double> axmax = viewPane->modelToScreen(viewPane->axes().coordMax(axis));
// 	axmin.print();
// 	axmax.print();

	// Calculate vectors between axis minimum and mouse position (AM) and axis maximum (AB)
	Vec3<double> ab(axmax.x - axmin.x, axmax.y - axmin.y, 0.0);
	Vec3<double> am(mouseX - axmin.x, mouseY - axmin.y, 0.0);
	Vec3<double> amNorm = am, abNorm = ab;
	double ratio = am.magnitude() / ab.magnitude();
	abNorm.normalise();
	amNorm.normalise();
// 	double angle = acos(abNorm.dp(amNorm));
//	printf("Angle = %f, %f\n", angle, angle * DEGRAD);

	// Calculate slice axis value - no need to account for inverted axes here, since this is accounted for in the vectors axmin and axmax
	Axes& axes = viewPane->axes();
	double axisValue;
	if (axes.logarithmic(axis)) axisValue = pow(10, abNorm.dp(amNorm)*ratio * (log10(axes.max(axis)) - log10(axes.min(axis))) + log10(axes.min(axis)));
	else axisValue = abNorm.dp(amNorm)*ratio * (axes.max(axis) - axes.min(axis)) + axes.min(axis);
//	printf("slicevalue = %f (%f)\n", axisValue, abNorm.dp(amNorm)*ratio);

	// Clamp value to data range
	if (axisValue < axes.min(axis)) axisValue = axes.min(axis);
	else if (axisValue > axes.max(axis)) axisValue = axes.max(axis);
// 	printf("ACMAG = %f, X = %f\n", ratio, axisValue);

	return axisValue;
}

/*
 * Public Functions
 */

// Set interaction mode and axis
void UChromaWindow::setInteractionMode(InteractionMode::Mode mode, int axis)
{
	// Cancel any previous interaction mode...
	cancelInteraction();

	interactionMode_ = mode;
	interactionAxis_ = axis;

	// Generate interaction primitives for all panes
	UChromaSession::viewLayout().updateInteractionPrimitives(axis);

	// Update statusbar help text
	statusBarInfoLabel_->setText(InteractionMode::interactionModes[mode].modeText);
}

// Return interaction mode
InteractionMode::Mode UChromaWindow::interactionMode()
{
	return interactionMode_;
}

// Return current axis target for interaction
int UChromaWindow::interactionAxis()
{
	return interactionAxis_;
}

// Return whether the user is currently interacting with the display
bool UChromaWindow::interacting()
{
	return interacting_;
}

// Cancel current interaction
void UChromaWindow::cancelInteraction()
{
	// Cancel interaction type
	switch (interactionMode_)
	{
		case (InteractionMode::FitSetupSelectXInteraction):
		case (InteractionMode::FitSetupSelectZInteraction):
			editFitSetupDialog_.updateAndExec();
			break;
		case (InteractionMode::OperateBGSubSelectYValueInteraction):
		case (InteractionMode::OperateBGSubSelectXRangeInteraction):
		case (InteractionMode::OperateBGSubSelectZRangeInteraction):
			OperateBGSubDialog(*this).updateAndExec();
			break;
		case (InteractionMode::ViewInteraction):
		case (InteractionMode::ZoomInteraction):
			break;
		default:
			printf("Internal Error: Don't know how to complete interaction mode %i\n", interactionMode_);
			break;
	}
}

// Start interaction at the specified screen coordinates
void UChromaWindow::startInteraction(int mouseX, int mouseY, Qt::KeyboardModifiers modifiers)
{
	// Check that an interaction mode is currently set
	if (interactionMode_ == InteractionMode::ViewInteraction) return;

	// Check interaction pane
	if (!ViewPane::objectValid(UChromaSession::currentViewPane()))
	{
		msg.print("Internal Error: Invalid interaction pane - refusing to start interaction.\n");
		return;
	}

	// Calculate axis value at start of interaction
	clickedInteractionValue_ = screenToAxis(interactionAxis_, mouseX, mouseY);

	// Store keyboard modifiers
	clickedInteractionModifiers_ = modifiers;

	interacting_ = true;
}

// Update current interaction position
void UChromaWindow::updateInteractionPosition(int mouseX, int mouseY)
{
	if (interactionAxis_ != -1)
	{
		// Calculate axis value
		currentInteractionValue_ = screenToAxis(interactionAxis_, mouseX, mouseY);

		// Extract slice from collections in current pane (unless this is a SliceMonitor)
		if (UChromaSession::currentViewPane()->role() != ViewPane::SliceMonitorRole) UChromaSession::currentViewPane()->collectionsUpdateCurrentSlices(interactionAxis_, currentInteractionValue_);
	}
	else
	{
		clickedInteractionValue_ = 0.0;
		currentInteractionValue_ = 0.0;
	}

	// Update the displayed coordinate box informstion
	updateCoordinateInfo();
}

// End interaction at the specified screen coordinates
void UChromaWindow::endInteraction(int mouseX, int mouseY)
{
	// If we are not interacting, exit now
	if (!interacting_) return;

	// Check interaction pane
	if (!ViewPane::objectValid(UChromaSession::currentViewPane()))
	{
		msg.print("Internal Error: Invalid interaction pane - refusing to end interaction.\n");
		return;
	}

	// Copy current interaction mode, and reset back to ViewInteraction
	InteractionMode::Mode currentMode = interactionMode_;
	int currentAxis = interactionAxis_;
	interactionMode_ = InteractionMode::ViewInteraction;
	interactionAxis_ = -1;

	// Finalise interaction type
	switch (currentMode)
	{
		case (InteractionMode::FitSetupSelectXInteraction):
			editFitSetupDialog_.ui.XAbsoluteMinSpin->setValue(std::min(clickedInteractionValue_, currentInteractionValue_));
			editFitSetupDialog_.ui.XAbsoluteMaxSpin->setValue(std::max(clickedInteractionValue_, currentInteractionValue_));
			editFitSetupDialog_.updateAndExec();
			break;
		case (InteractionMode::FitSetupSelectZInteraction):
			editFitSetupDialog_.ui.ZAbsoluteMinSpin->setValue(std::min(clickedInteractionValue_, currentInteractionValue_));
			editFitSetupDialog_.ui.ZAbsoluteMaxSpin->setValue(std::max(clickedInteractionValue_, currentInteractionValue_));
			editFitSetupDialog_.updateAndExec();
			break;
		case (InteractionMode::OperateBGSubSelectYValueInteraction):
			OperateBGSubDialog::setConstantValue(currentInteractionValue_);
			OperateBGSubDialog(*this).updateAndExec();
			break;
		case (InteractionMode::OperateBGSubSelectXRangeInteraction):
			OperateBGSubDialog::setXRange(std::min(clickedInteractionValue_, currentInteractionValue_), std::max(clickedInteractionValue_, currentInteractionValue_));
			OperateBGSubDialog(*this).updateAndExec();
			break;
		case (InteractionMode::OperateBGSubSelectZRangeInteraction):
			OperateBGSubDialog::setZRange(std::min(clickedInteractionValue_, currentInteractionValue_), std::max(clickedInteractionValue_, currentInteractionValue_));
			OperateBGSubDialog(*this).updateAndExec();
			break;
		case (InteractionMode::ZoomInteraction):
			// None : Zoom to defined region
			// Ctrl : Extract slice
			if (clickedInteractionModifiers_.testFlag(Qt::ControlModifier))
			{
				UChromaSession::currentCollection()->extractCurrentSlice(currentAxis, currentInteractionValue_);
				refreshCollections();
			}
			else
			{
				double newMin = std::min(clickedInteractionValue_, currentInteractionValue_);
				double newMax = std::max(clickedInteractionValue_, currentInteractionValue_);
				if ((newMax-newMin) > 1.0e-10)
				{
					UChromaSession::currentViewPane()->axes().setMin(currentAxis, newMin);
					UChromaSession::currentViewPane()->axes().setMax(currentAxis, newMax);
					axesWindow_.updateControls();
				}
			}
			updateDisplay();
			break;
		default:
			printf("Internal Error: Don't know how to complete interaction mode %i\n", currentMode);
			break;
	}

	interacting_ = false;
}

// Return clicked interaction value on axis
double UChromaWindow::clickedInteractionValue()
{
	return clickedInteractionValue_;
}

// Return current interaction value on axis
double UChromaWindow::currentInteractionValue()
{
	return currentInteractionValue_;
}

// Return clicked interaction coordinate on axis
double UChromaWindow::clickedInteractionCoordinate()
{
	// Check for valid interaction axis
	if (interactionAxis_ == -1) return 0.0;

	// Check for valid view pane
	if (UChromaSession::currentViewPane() == NULL) return 0.0;

	Axes& axes = UChromaSession::currentViewPane()->axes();
	if (axes.logarithmic(interactionAxis_)) return (axes.inverted(interactionAxis_) ? log10(axes.max(interactionAxis_)/clickedInteractionValue_) : log10(clickedInteractionValue_));
	else return (axes.inverted(interactionAxis_) ? axes.max(interactionAxis_) - clickedInteractionValue_ : clickedInteractionValue_);
}

// Return current interaction coordinate on axis
double UChromaWindow::currentInteractionCoordinate()
{
	// Check for valid interaction axis
	if (interactionAxis_ == -1) return 0.0;

	// Check for valid view pane
	if (UChromaSession::currentViewPane() == NULL) return 0.0;

	Axes& axes = UChromaSession::currentViewPane()->axes();
	if (axes.logarithmic(interactionAxis_)) return (axes.inverted(interactionAxis_) ? log10(axes.max(interactionAxis_)/currentInteractionValue_) : log10(currentInteractionValue_));
	else return (axes.inverted(interactionAxis_) ? axes.max(interactionAxis_) - currentInteractionValue_ : currentInteractionValue_);
}

// Perform relevant double-click action, occurring at specified coordinate
void UChromaWindow::doubleClickInteraction(int mouseX, int mouseY)
{
	// Determine pane that the event occured in
	ViewPane* pane = UChromaSession::viewLayout().paneAt(mouseX, mouseY);
	if (pane == NULL) return;

	ui.MainView->setQueryCoordinates(mouseX, mouseY);
	ui.MainView->repaint();

// 	printf("Object at clicked position = %i, '%s'\n", ui.MainView->objectAtQueryCoordinates(), qPrintable(ui.MainView->infoAtQueryCoordinates()));

	// Now find out what, if anything, was under the mouse...
// 	int axis = pane->axisTitleAt(mouseX, mouseY);
// 	if (axis != -1) printf("Here's axis %i\n", axis);
}
