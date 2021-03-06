/*
	*** UChroma - Info Bar 
	*** src/gui/uchroma_info.cpp
	Copyright T. Youngs 2013-2015

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
#include "templates/reflist.h"
#include "templates/variantpointer.h"

// Update coordinate info
void UChromaWindow::updateCoordinateInfo()
{
	if (interactionAxis_ == -1) ui.InfoCoordinateLabel->setText("{--,--,--}");
	else if (interactionAxis_ == 0) ui.InfoCoordinateLabel->setText("{"+QString::number(currentInteractionValue_)+",--,--}");
	else if (interactionAxis_ == 1) ui.InfoCoordinateLabel->setText("{--,"+QString::number(currentInteractionValue_)+",--}");
	else if (interactionAxis_ == 2) ui.InfoCoordinateLabel->setText("{--,--,"+QString::number(currentInteractionValue_)+"}");
}

// Update text of renderTimeLabel_ in statusbar
void UChromaWindow::updateRenderTimeLabel(QString text)
{
	statusBarRenderingTimeLabel_->setText(text);
}
