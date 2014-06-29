/*
	*** Viewer Functions
	*** src/gui/viewer_funcs.cpp
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

#include "gui/viewer.uih"

// Set whether framebuffer should be used for offscreen rendering
void Viewer::setUseFrameBuffer(bool b)
{
	useFrameBuffer_ = b;
}

// Return whether framebuffer should be used for offscreen rendering
bool Viewer::useFrameBuffer()
{
	return useFrameBuffer_;
}

// Set whether to correct transparency artefacts
void Viewer::setCorrectTransparency(bool b)
{
	correctTransparency_ = b;
}

// Return whether to correct transparency artefacts
bool Viewer::correctTransparency()
{
	return correctTransparency_;
}

// Return whether the view has perspective
bool Viewer::hasPerspective()
{
	return hasPerspective_;
}

// Set whether this view has perspective
void Viewer::setHasPerspective(bool b)
{
	hasPerspective_ = b;
	projectionMatrix_ = calculateProjectionMatrix(hasPerspective_, perspectiveFieldOfView_, viewMatrix_[14]);
}

// Set line width to use
void Viewer::setLineWidth(double lineWidth)
{
	lineWidth_ = lineWidth;
}
