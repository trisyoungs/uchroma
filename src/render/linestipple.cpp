/*
	*** Line Stipple
	*** src/base/linestipple.cpp
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

#include "render/linestipple.h"
#include <string.h>

// Static list of LineStipples
LineStipple LineStipple::stipple[] = {
	{ 1,	0xffff,		"Solid" },
	{ 1,	0xf0f0,		"Dash" }
};

// Convert text string to StippleType
LineStipple::StippleType LineStipple::stippleType(const char* s)
{
	for (int n=0; n<LineStipple::nStippleTypes; ++n) if (strcmp(s,LineStipple::stipple[n].name) == 0) return (LineStipple::StippleType) n;
	return LineStipple::nStippleTypes;
}

// Convert InputBlock to text string
const char* LineStipple::stippleType(LineStipple::StippleType st)
{
	return LineStipple::stipple[st].name;
}

/*
 * GL
 */

// Apply stipple pattern
void LineStipple::apply()
{
	glLineStipple(stippleFactor, stipplePattern);
}
