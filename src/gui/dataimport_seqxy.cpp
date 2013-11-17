/*
	*** Data Import Dialog - Sequential XY Import 
	*** src/gui/dataimport_seqxy.cpp
	Copyright T. Youngs 2013

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

#include "gui/dataimport.h"
#include "base/lineparser.h"

// Import sequential XY data
bool DataImportDialog::importSequentialXY()
{
	// Clear any old imported slices
	importedSlices_.clear();

	// Grab some values from the UI
	Vec3<int> columns(ui.SeqXYColumnXSpin->value()-1, ui.SeqXYColumnYSpin->value()-1,  ui.SeqXYColumnZSpin->value()-1);
	int maxColumn = columns.max();
	int nStartSkip = ui.SeqXYNSkip->value();
	Dnchar fileName = qPrintable(ui.DataFileEdit->text());
	
	// Open file and check that we're OK to proceed reading from it
	LineParser parser;

	if ((!parser.openInput(fileName)) || (!parser.isFileGoodForReading()))
	{
		msg.print("Couldn't open file '%s' for reading.\n", fileName.get());
		return false;
	}

	// Set up initial slice
	Slice* slice = importedSlices_.add();

	// Skip lines at start
	if (nStartSkip > 0) parser.skipLines(nStartSkip);

	int success, nCols = -1;
	Vec3<int> count(0, 0, 0);
	while (!parser.eofOrBlank())
	{
		if (nStartSkip == -1)
		{
			success = parser.getArgsDelim(LineParser::SkipBlanks);
			nStartSkip = 0;
		}
		else success = parser.getArgsDelim(LineParser::Defaults);
		if (success != 0)
		{
			parser.closeFiles();
			msg.print("Error reading from file '%s'.\n", fileName.get());
			return false;
		}

		// Is there anything on this line?
		if (parser.nArgs() == 0)
		{
			// Must be between slices in the file - check the current x count.
			// If it is non-zero, create a new Slice for the next round...
			if (count.x != 0)
			{
				slice = importedSlices_.add();
				++count.z;
			}
			count.x = 0;
			continue;
		}

		// Check requested columns against available columns
		if (maxColumn >= parser.nArgs()) msg.print("Not enough columns in file.\n");

		// Add datapoint
		slice->data().addPoint(columns.x == -1 ? count.x : parser.argd(columns.x), parser.argd(columns.y));

		// Set z value for slice
		slice->data().setZ(columns.z == -1 ? count.z : parser.argd(columns.z));

		// Increase x count
		++count.x;
	}

	return true;
}