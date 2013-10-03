/*
	*** NXS Import
	*** src/base/nxs.h
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

#ifndef UCHROMA_NXS_H
#define UCHROMA_NXS_H

#include "base/data2d.h"
#include <hdf5.h>
#include <QtCore/QString>

// Forward Declarations
/* none */

// Nexus File Helper
class NexusHelper
{
	public:
	// Constructor
	NexusHelper();
	// Destructor
	~NexusHelper();

	static hid_t timeDataSet_, valueDataSet_;

	private:
	// Retrieve string from specified Nexut dataset
	static bool nexusExtractString(hid_t rootLocation, const char* name, QString& dest);
	// Retrieve time/value data from specified Nexus datasets
// 	static herr_t nexusExtractTimeValueData(RunData* runData, hid_t time, hid_t value);

	public:
	// Iterator callback for HDF5 (group access)
	static herr_t nexusGroupIterator(hid_t loc_id, const char *name, const H5L_info_t *info, void *operator_data);
	// Iterator for block data
	static herr_t nexusBlockIterator(hid_t loc_id, const char *name, const H5L_info_t *info, void *operator_data);
	// Parse Nexus file
// 	static bool parseNexusFile(RunData* runData, QString fileName);
	// Load slice data from NXS file
	static bool loadSlice(QString fileName, QString root, QString xAxisValues, QString yAxisValues, QString zAxisValues, QString dataSpaceName, int zSlice, List< Data2D >& slices);
};

#endif
