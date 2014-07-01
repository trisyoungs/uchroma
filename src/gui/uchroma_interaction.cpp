/*
	*** Selection
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
 * Private Functions
 */

// Calculate interaction axis coordinate from supplied screen coordinates
double UChromaWindow::screenToAxis(int axis, int mouseX, int mouseY)
{
	if (axis == -1) return 0.0;

// 	printf("Test: min=%f, max=%f\n", axisMin_[0], axisMax_[0]);
// 	rMouseLast_.print();
// 	axisCoordMin_[0].print();
	// Project axis coordinates to get a screen-based yardstick
	Vec4<double> axmin = ui.MainView->modelToScreen(axisCoordMin_[axis]);
	Vec4<double> axmax = ui.MainView->modelToScreen(axisCoordMax_[axis]);
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
	double axisValue;
	if (axisLogarithmic_[axis]) axisValue = pow(10, abNorm.dp(amNorm)*ratio * (log10(axisMax_[axis]) - log10(axisMin_[axis])) + log10(axisMin_[axis]));
	else axisValue = abNorm.dp(amNorm)*ratio * (axisMax_[axis] - axisMin_[axis]) + axisMin_[axis];
//	printf("slicevalue = %f (%f)\n", axisValue, abNorm.dp(amNorm)*ratio);

	// Clamp value to data range
	if (axisValue < axisMin_[axis]) axisValue = axisMin_[axis];
	else if (axisValue > axisMax_[axis]) axisValue = axisMax_[axis];
// 	printf("ACMAG = %f, X = %f\n", ratio, axisValue);

	return axisValue;
}

// Return axis bin value of closest point to supplied value
int UChromaWindow::closestBin(int axis, double value)
{
	if (!currentCollection_) return -1;
	if (currentCollection_->nDataSets() == 0) return -1;

	if (axis == 0)
	{
		// Check X array of first slice
		Array<double>& x = currentCollection_->dataSets()->transformedData().arrayX();
		int midIndex, loIndex = 0, hiIndex = x.nItems() - 1;
		if (value < x.value(0)) return 0;
		if (value > x.value(hiIndex)) return hiIndex;
		// Binary... chop!
		while ((hiIndex - loIndex) > 1)
		{
			midIndex = (hiIndex + loIndex) / 2;
			if (x.value(midIndex) <= value) loIndex = midIndex;
			else hiIndex = midIndex;
		}
		if (fabs(x.value(loIndex) - value) < fabs(x.value(hiIndex) - value)) return loIndex;
		else return hiIndex;
	}
	else if (axis == 1)
	{
		// ???
	}
	else if (axis == 2)
	{
		// Check z-values
		int closest = 0, n = 0;
		double delta, closestDelta = fabs(currentCollection_->dataSets()->transformedData().z() - value);
		for (DataSet* dataSet = currentCollection_->dataSets()->next; dataSet != NULL; dataSet = dataSet->next)
		{
			delta = fabs(dataSet->transformedData().z() - value);
			++n;
			if (delta < closestDelta)
			{
				closest = n;
				closestDelta = delta;
			}
		}
		return closest;
	}
	else return -1;
}

/*
 * Public Functions
 */

// Set interaction mode and axis
void UChromaWindow::setInteractionMode(UChromaWindow::InteractionMode mode, int axis)
{
	interactionMode_ = mode;
	interactionAxis_ = axis;
	ui.MainView->setInteractionPrimitive(axis);
}

// Return interaction mode
UChromaWindow::InteractionMode UChromaWindow::interactionMode()
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
	if (interactionMode_ == UChromaWindow::NoInteraction) return;

	// Calculate axis value at start of interaction
	clickedInteractionValue_ = screenToAxis(interactionAxis_, mouseX, mouseY);

	interacting_ = true;
}

// Update current interaction position
void UChromaWindow::updateInteractionPosition(int mouseX, int mouseY)
{
	if (interactionAxis_ != -1)
	{
		// Calculate axis value
		currentInteractionValue_ = screenToAxis(interactionAxis_, mouseX, mouseY);

		// Extract slice from data?
		if (sliceMonitorWindow_.isVisible()) extractSlice(interactionAxis_, closestBin(interactionAxis_, currentInteractionValue_));
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

	// Finalise interaction type
	switch (interactionMode_)
	{
		case (UChromaWindow::FitDialogSelectXInteraction):
			fitDialog_.ui.SourceXMinSpin->setValue(min(clickedInteractionValue_, currentInteractionValue_));
			fitDialog_.ui.SourceXMaxSpin->setValue(max(clickedInteractionValue_, currentInteractionValue_));
			fitDialog_.show();
			break;
		case (UChromaWindow::ZoomInteraction):
			printf("Zooming %i axis to %f/%f\n", interactionAxis_, clickedInteractionValue_, currentInteractionValue_);
			setAxisMin(interactionAxis_, min(clickedInteractionValue_, currentInteractionValue_));
			setAxisMax(interactionAxis_, max(clickedInteractionValue_, currentInteractionValue_));
			updateAxesPrimitives();
			axesWindow_.updateControls();
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
	if (interactionAxis_ == -1) return 0.0;

	if (axisLogarithmic_[interactionAxis_]) return (axisInverted_[interactionAxis_] ? log10(axisMax_[interactionAxis_]/clickedInteractionValue_) : log10(clickedInteractionValue_));
	else return (axisInverted_[interactionAxis_] ? axisMax_[interactionAxis_] - clickedInteractionValue_ : clickedInteractionValue_);
}

// Return current interaction coordinate on axis
double UChromaWindow::currentInteractionCoordinate()
{
	if (interactionAxis_ == -1) return 0.0;

	if (axisLogarithmic_[interactionAxis_]) return (axisInverted_[interactionAxis_] ? log10(axisMax_[interactionAxis_]/currentInteractionValue_) : log10(currentInteractionValue_));
	else return (axisInverted_[interactionAxis_] ? axisMax_[interactionAxis_] - currentInteractionValue_ : currentInteractionValue_);
}
