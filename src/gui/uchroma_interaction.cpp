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

/*
 * Interaction Modes
 */

// Static list of interaction modes
InteractionMode InteractionMode::interactionModes[] = {
	{ "Fit (select X)",	"<b>Left, click-drag</b> Select X region for fitting" },
	{ "Fit (select Z)",	"<b>Left, click-drag</b> Select Z region for fitting" },
	{ "View",		"<b>Right</b> Rotate view, <b>Middle</b> Translate view, <b>Wheel</b> Zoom view" },
	{ "Zoom",		"<b>Left, click-drag</b> Zoom to selected region" }
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
	if (currentViewPane_ == NULL) return 0.0;

// 	printf("Test: min=%f, max=%f\n", min_[0], max_[0]);
// 	rMouseLast_.print();
// 	axisCoordMin_[0].print();
	// Project axis coordinates to get a screen-based yardstick
	Vec4<double> axmin = currentViewPane_->modelToScreen(currentViewPane_->axes().coordMin(axis));
	Vec4<double> axmax = currentViewPane_->modelToScreen(currentViewPane_->axes().coordMax(axis));
// 	axmin.print();
// 	axmax.print();

	// Calculate vectors between axis minimum and mouse position (AM) and axis maximum (AB)
	Vec3<double> ab(axmax.x - axmin.x, axmax.y - axmin.y, 0.0);
	Vec3<double> am(mouseX - axmin.x, mouseY - axmin.y, 0.0);
	Vec3<double> amNorm = am, abNorm = ab;
	double ratio = am.magnitude() / ab.magnitude();
	abNorm.normalise();
	amNorm.normalise();
	double angle = acos(abNorm.dp(amNorm)) ;
//	printf("Angle = %f, %f\n", angle, angle * DEGRAD);

	// Calculate slice axis value - no need to account for inverted axes here, since this is accounted for in the vectors axmin and axmax
	Axes& axes = currentViewPane_->axes();
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
	interactionMode_ = mode;
	interactionAxis_ = axis;

	// Generate interaction primitives for all panes
	viewLayout_.updateInteractionPrimitives(axis);

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

// Start interaction at the specified screen coordinates
void UChromaWindow::startInteraction(int mouseX, int mouseY, Qt::KeyboardModifiers modifiers)
{
	// Check that an interaction mode is currently set
	if (interactionMode_ == InteractionMode::ViewInteraction) return;

	// Check interaction pane
	if (currentViewPane_ == NULL)
	{
		msg.print("NULL interaction pane - refusing to start interaction.\n");
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

		// Extract slice from collections in current pane
		currentViewPane_->collectionsUpdateCurrentSlices(interactionAxis_, currentInteractionValue_);
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
	if (currentViewPane_ == NULL)
	{
		msg.print("NULL interaction pane - refusing to end interaction.\n");
		return;
	}

	// Finalise interaction type
	switch (interactionMode_)
	{
		case (InteractionMode::FitSetupSelectXInteraction):
			fitSetupDialog_.ui.XAbsoluteMinSpin->setValue(min(clickedInteractionValue_, currentInteractionValue_));
			fitSetupDialog_.ui.XAbsoluteMaxSpin->setValue(max(clickedInteractionValue_, currentInteractionValue_));
			fitSetupDialog_.show();
			break;
		case (InteractionMode::FitSetupSelectZInteraction):
			fitSetupDialog_.ui.ZAbsoluteMinSpin->setValue(min(clickedInteractionValue_, currentInteractionValue_));
			fitSetupDialog_.ui.ZAbsoluteMaxSpin->setValue(max(clickedInteractionValue_, currentInteractionValue_));
			fitSetupDialog_.show();
			break;
		case (InteractionMode::ZoomInteraction):
			// None : Zoom to defined region
			// Ctrl : Extract slice
			if (clickedInteractionModifiers_.testFlag(Qt::ControlModifier))
			{
				currentCollection_->extractCurrentSlice(interactionAxis_, currentInteractionValue_);
				refreshCollections();
			}
			else
			{
				double newMin = min(clickedInteractionValue_, currentInteractionValue_);
				double newMax = max(clickedInteractionValue_, currentInteractionValue_);
				if ((newMax-newMin) > 1.0e-10)
				{
					currentViewPane_->axes().setMin(interactionAxis_, newMin);
					currentViewPane_->axes().setMax(interactionAxis_, newMax);
					axesWindow_.updateControls();
				}
			}
			updateDisplay();
			break;
		default:
			printf("Internal Error: Don't know how to complete interaction mode %i\n", interactionMode_);
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
	if (currentViewPane_ == NULL) return 0.0;

	Axes& axes = currentViewPane_->axes();
	if (axes.logarithmic(interactionAxis_)) return (axes.inverted(interactionAxis_) ? log10(axes.max(interactionAxis_)/clickedInteractionValue_) : log10(clickedInteractionValue_));
	else return (axes.inverted(interactionAxis_) ? axes.max(interactionAxis_) - clickedInteractionValue_ : clickedInteractionValue_);
}

// Return current interaction coordinate on axis
double UChromaWindow::currentInteractionCoordinate()
{
	// Check for valid interaction axis
	if (interactionAxis_ == -1) return 0.0;

	// Check for valid view pane
	if (currentViewPane_ == NULL) return 0.0;

	Axes& axes = currentViewPane_->axes();
	if (axes.logarithmic(interactionAxis_)) return (axes.inverted(interactionAxis_) ? log10(axes.max(interactionAxis_)/currentInteractionValue_) : log10(currentInteractionValue_));
	else return (axes.inverted(interactionAxis_) ? axes.max(interactionAxis_) - currentInteractionValue_ : currentInteractionValue_);
}
