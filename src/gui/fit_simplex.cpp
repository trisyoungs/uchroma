/*
	*** Fit Widget - Simplex Minimiser
	*** src/gui/fit_simplex.cpp
	Copyright T. Youngs 2012-2013

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

#include "gui/fit.h"
#include "gui/uchroma.h"
#include "templates/simplex.h"

// Simplex Minimise
bool FitDialog::simplexMinimise(Array<double>& alpha)
{
	// Check number of variables to fit
	if (alpha.nItems() < 2)
	{
		ui.OutputEdit->append("ERROR: Can't use Simplex minimiser for this few variables.");
		return false;
	}

	// Setup the simplex minimiser 
	ui.OutputEdit->append("Initialising Simplex minimiser");
        Simplex<FitDialog> simplex(this, &FitDialog::sosError);

	simplex.initialise(alpha, 0.0, 0.01);

	// Perform minimisation
	Array<double> best = simplex.minimise(100, 10, 0.001, 1.0);
}