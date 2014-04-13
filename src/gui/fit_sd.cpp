/*
	*** Fit Widget - Steepest Descent Minimiser
	*** src/gui/fit_sd.cpp
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

// Steepest Descent Minimise
bool FitDialog::sdMinimise(Array<double>& alpha)
{
	// Create initial gradient
	Array<double> gradient(alpha.nItems()), tempAlpha;
	for (int n=0; n<alpha.nItems(); ++n)
	{
		tempAlpha = alpha;
		tempAlpha[n] = 1.01 * alpha[n];
		gradient[n] = rmsError(tempAlpha);
		tempAlpha[n] = 0.99 * alpha[n];
		gradient[n] -= rmsError(tempAlpha);
	}
	gradient /= 0.02;

	// Set initial stepsize
	double lambda = 1.0;

	// Get initial cost
	double oldRMSE = rmsError(alpha);
	ui.OutputEdit->append("Initial RMSE = " + QString::number(oldRMSE, 'e'));

	// Do some iterations
	int n;
	double currentRMSE;
	for (int step=0; step<100; ++step)
	{
		// Test stepsize - if it does not decrease the RMSE, decrease it until it does
		do
		{
			// Update alpha and get RMSE
			for (n=0; n<alpha.nItems(); ++n) tempAlpha[n] = alpha[n] - gradient[n]*lambda;
			currentRMSE = rmsError(tempAlpha);

			// Current RMSE higher than old RMSE?
			if (currentRMSE > oldRMSE) lambda *= 0.5;
		} while (currentRMSE > oldRMSE);
		alpha = tempAlpha;
		
		// Get new cost
		printf("Cost = %f (a=%f)\n", currentRMSE, alpha[0]);

		// Generate new gradient
		for (n=0; n<alpha.nItems(); ++n)
		{
			tempAlpha = alpha;
			tempAlpha[n] = 1.01 * alpha[n];
			gradient[n] = rmsError(tempAlpha);
			tempAlpha[n] = 0.99 * alpha[n];
			gradient[n] -= rmsError(tempAlpha);
		}
		gradient /= 0.02;
		printf("Lamba = %f\n", lambda);
		
		oldRMSE = currentRMSE;
	}

	return true;
}
