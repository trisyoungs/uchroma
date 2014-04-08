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

// Cost function for simplex minimise
double FitDialog::simplexCost(Array<double>& alpha)
{
	// Poke current values back into the equation variables
	int n = 0;
	for (RefListItem<EquationVariable,bool>* ri = fitVariables_.first(); ri != NULL; ri = ri->next) ri->item->variable()->set(alpha[n++]);
	
	double yCalc, sos = 0.0;
	if (currentFitSlice_ == NULL)
	{
		// Get current SOS error over all points in all slices
		// TODO
		return 0.0;
	}
	else
	{
		// Get SOS error over points in current slice
		Data2D& data = currentFitSlice_->data();
		zVariable_->set(data.z());
		for (n = 0; n<data.nPoints(); ++n)
		{
			xVariable_->set(data.x(n));
			yVariable_->set(data.y(n));
			yCalc = equation_.execute();
			sos += (data.y(n) - yCalc) * (data.y(n) - yCalc);
		}
	}
	return sos;
}

// Simplex Minimise
void FitDialog::simplexMinimise()
{
	// Setup the simplex minimiser 
        Simplex<FitDialog> simplex(this, &FitDialog::simplexCost);
        Array<double> alpha;
	for (RefListItem<EquationVariable,bool>* ri = fitVariables_.first(); ri != NULL; ri = ri->next)
	{
		// Grab variable pointer from FitVariable
		EquationVariable* var = ri->item;
		alpha.add(var->value());
	}
	simplex.initialise(alpha, 0.0, 0.01);

	// Perform minimisation
	Array<double> best = simplex.minimise(100, 10, 0.001, 1.0);
	for (int n=0; n< best.nItems(); ++n) printf("%i  %f\n", n, best[n]);
	simplexCost(best);
}
