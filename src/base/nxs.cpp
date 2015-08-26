/*
	*** NXS Import
	*** src/base/nxs.cpp
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

#include "base/nxs.h"
#include "base/messenger.h"
#include <hdf5.h>

// Iterator callback for HDF5 (group access)
herr_t NexusHelper::nexusGroupIterator(hid_t loc_id, const char* name, const H5L_info_t *info, void *operator_data)
{
	// Get type of object - if it is not a Group then I don't care
	H5O_info_t infobuf;
	herr_t status = H5Oget_info_by_name(loc_id, name, &infobuf, H5P_DEFAULT);
	if (infobuf.type == H5O_TYPE_GROUP)
	{
		// It's a group, which means we want to extract some useful data from it.
		// Each group potentially has within it several control variables etc., and a 'value_log' group containing the time/value data
// 		RunData* rd = (RunData*) operator_data;
// 		timeDataSet_ = -1;
// 		valueDataSet_ = -1;
		hid_t block = H5Gopen2(loc_id, name, H5P_DEFAULT);
		H5Literate(block, H5_INDEX_NAME, H5_ITER_NATIVE, NULL, &NexusHelper::nexusBlockIterator, operator_data);
	}

	return 0;
}

// Iterator for block data
herr_t NexusHelper::nexusBlockIterator(hid_t loc_id, const char* name, const H5L_info_t *info, void *operator_data)
{
	H5O_info_t infobuf;
	herr_t status = H5Oget_info_by_name(loc_id, name, &infobuf, H5P_DEFAULT);
	if (infobuf.type == H5O_TYPE_GROUP)
	{
		if (strcmp("value_log",name) == 0)
		{
			// Found the value log!
			hid_t valueLog = H5Gopen2(loc_id, "value_log", H5P_DEFAULT);
			if (valueLog < 0) msg.print("Error opening value_log for block.\n");
			else
			{
				// Get time/value identifiers for this group
				hid_t time = H5Dopen2(valueLog, "time", H5P_DEFAULT);
				if (time < 0)
				{
// 					msg.print("Warning - value_log for NEXUS group '%s' did not contain a 'time' dataset.\n", qPrintable(currentBlock_));
					return -1;
				}
				hid_t value = H5Dopen2(valueLog, "value", H5P_DEFAULT);
				if (value < 0)
				{
// 					msg.print("Warning - value_log for NEXUS group '%s' did not contain a 'value' dataset.\n", qPrintable(currentBlock_));
					return -1;
				}

				// Get data
// 				nexusExtractTimeValueData((RunData*) operator_data, time, value);
				
				// Cleanup
				H5Dclose(time);
				H5Dclose(value);
			}
		}
	}
	else if (infobuf.type == H5O_TYPE_DATASET)
	{
		// Some blocks (especially those in the 'runlog' group) don't have time/value datasets in a value_log subgroup.
		// Check here to see if we get both for a given blockName
// 		if (strcmp("time", name) == 0) timeDataSet_ = loc_id;
// 		else if (strcmp("value", name) == 0) valueDataSet_= loc_id;

// // 		// Do we now have both time and value datasets?
// // 		if ((timeDataSet_ > 0) && (valueDataSet_ > 0))
// // 		{
// // 			hid_t time = H5Dopen2(timeDataSet_, "time", H5P_DEFAULT);
// // 			hid_t value= H5Dopen2(valueDataSet_, "value", H5P_DEFAULT);
// // // 			nexusExtractTimeValueData((RunData*) operator_data, time, value);
// // 			timeDataSet_ = -1;
// // 			valueDataSet_ = -1;
// // 			
// // 			// Cleanup
// // 			H5Dclose(time);
// // 			H5Dclose(value);
// // 		}
	}
	return 0;
}

// Retrieve string from specified Nexut dataset
bool NexusHelper::nexusExtractString(hid_t rootLocation, const char* name, QString& dest)
{
	// Try to check existence of dataset in a 'nice' way first....
	H5O_info_t objectInfo;
	if (H5Oget_info_by_name(rootLocation, name, &objectInfo, H5P_DEFAULT) < 0) return false;

	hid_t dataSet = H5Dopen1(rootLocation, name);
	if (dataSet < 0)
	{
		msg.print("Warning - Failed to open '%s' dataset in nexus file.\n", name);
		return false;
	}

	// Get the dataspaces for each dataset
	hid_t space = H5Dget_space(dataSet);

	// Get data properties
	int nDims = H5Sget_simple_extent_ndims(space);
	hsize_t* nValues = new hsize_t[nDims];
	H5Sget_simple_extent_dims(space, nValues, NULL);

	// Make some checks...
	if (nDims != 1)
	{
		msg.print("Warning - Tried to extract a single value from a multi-arrayed dataset '%s'.\n", name);
		delete[] nValues;
		H5Dclose(dataSet);
		return false;
	}
	if (nValues[0] > 1)
	{
		msg.print("Warning - Tried to extract a single value from a multi-valued dataset '%s'.\n", name);
		delete[] nValues;
		H5Dclose(dataSet);
		return false;
	}

	// Check type of value data
	hid_t valueType = H5Dget_type(dataSet);
	int intBuffer;
	double doubleBuffer;
	char charBuffer[256];
	herr_t status;
	hid_t memType, charSpace;
	switch (H5Tget_class(valueType))
	{
		case (H5T_INTEGER):
			status = H5Dread(dataSet, H5T_NATIVE_INT_g, H5S_ALL, H5S_ALL, H5P_DEFAULT, &intBuffer);
			dest = QString::number(intBuffer);
			break;
		case (H5T_FLOAT):
			status = H5Dread(dataSet, H5T_NATIVE_DOUBLE_g, H5S_ALL, H5S_ALL, H5P_DEFAULT, &doubleBuffer);
			dest = QString::number(doubleBuffer);
			break;
		case (H5T_STRING):
			memType = H5Tcopy(H5T_C_S1);
			status = H5Tset_size(memType, 256);
			status = H5Dread(dataSet, memType, H5S_ALL, H5S_ALL, H5P_DEFAULT, &charBuffer);
			dest = charBuffer;
			break;
	}

	// Cleanup
	delete[] nValues;
	H5Sclose(space);
	
	return true;
}

// Retrieve time/value data from specified NEXUS group
// herr_t NexusHelper::nexusExtractTimeValueData(RunData* runData, hid_t time, hid_t value)
// {
// 	// Get the dataspaces for each dataset
// 	hid_t timeSpace = H5Dget_space(time);
// 	hid_t valueSpace = H5Dget_space(value);
// 
// 	// Get data properties
// 	int timeNDims = H5Sget_simple_extent_ndims(timeSpace);
// 	int valueNDims = H5Sget_simple_extent_ndims(valueSpace);
// 	hsize_t* nTime = new hsize_t[timeNDims];
// 	hsize_t* nValue = new hsize_t[valueNDims];
// 	H5Sget_simple_extent_dims(timeSpace, nTime, NULL);
// 	H5Sget_simple_extent_dims(valueSpace, nValue, NULL);
// 
// 	// Get type (and size) of value data
// 	hid_t valueType = H5Dget_type(value);
// 	int valueSize = H5Tget_size(valueType);
// 
// 	// Make some checks...
// 	if (timeNDims != 1)
// 	{
// 		msg.print("Error - time array in value_log for '%s' does not consist of exactly one dimension.\n", qPrintable(currentBlock_));
// 		H5Sclose(timeSpace);
// 		H5Sclose(valueSpace);
// 		return -1;
// 	}
// 	if (valueNDims != 1)
// 	{
// 		msg.print("Warning - value array in the value_log for '%s' does not consist of exactly one dimension.\n", qPrintable(currentBlock_));
// 		H5Sclose(timeSpace);
// 		H5Sclose(valueSpace);
// 		return -1;
// 	}
// 
// 	if (nTime[0] != nValue[0])
// 	{
// 		msg.print("Warning - Array sizes of time/value domains do not match (%i / %i).\n", nTime[0], nValue[0]);
// 		H5Sclose(timeSpace);
// 		H5Sclose(valueSpace);
// 		return -1;
// 	}
// 
// 	// Create temporary arrays to store time data, and retrieve it
// 	double* timeData = new double[(long int) nTime[0]];
// 	herr_t status = H5Dread(time, H5T_NATIVE_DOUBLE_g, H5S_ALL, H5S_ALL, H5P_DEFAULT, timeData);
// 
// 	// Read in data
// 	int* intBuffer;
// 	double* doubleBuffer;
// 	char** charBuffer;
// 	hid_t memType, charSpace;
// 	Data2D* data = runData->addBlockData(currentBlock_, currentGroup_, startTime_, endTime_);
// 	switch (H5Tget_class(valueType))
// 	{
// 		case (H5T_INTEGER):
// 			intBuffer = new int[(long int) nValue[0]];
// 			status = H5Dread(value, H5T_NATIVE_INT_g, H5S_ALL, H5S_ALL, H5P_DEFAULT, intBuffer);
// 			for (int n=0; n<nValue[0]; ++n) data->addRelativePoint(startTime_.addSecs(timeData[n]), (double) intBuffer[n]);
// 			delete[] intBuffer;
// 			break;
// 		case (H5T_FLOAT):
// 			doubleBuffer = new double[(long int) nValue[0]];
// 			status = H5Dread(value, H5T_NATIVE_DOUBLE_g, H5S_ALL, H5S_ALL, H5P_DEFAULT, doubleBuffer);
// 			for (int n=0; n<nValue[0]; ++n) data->addRelativePoint(startTime_.addSecs(timeData[n]), doubleBuffer[n]);
// 			delete[] doubleBuffer;
// 			break;
// 		case (H5T_STRING):
// 			// Make room for null terminators of strings
// 			++valueSize;
// 			charBuffer = new char*[(long int) nValue[0]];
// // 			for (int n=0; n<(long int) nValue[0]; ++n) charBuffer[n] = new char[valueSize];
// 			charBuffer[0] = new char[(long int) nValue[0] * valueSize];
// 			for (int n=1; n<(long int) nValue[0]; ++n) charBuffer[n] = charBuffer[n-1] + valueSize;
// // 			charSpace = H5Dget_space(value);
// 			memType = H5Tcopy(H5T_C_S1);
// 			status = H5Tset_size(memType, valueSize);
// 			status = H5Dread(value, memType, H5S_ALL, H5S_ALL, H5P_DEFAULT, charBuffer[0]);
// 			for (int n=0; n<nValue[0]; ++n) data->addRelativePoint(startTime_.addSecs(timeData[n]), runData->enumeratedBlockValue(currentBlock_, QString(charBuffer[n])));
// // 			for (int n=0; n<(long int) nValue[0]; ++n) delete charBuffer[n];
// 			delete charBuffer[0];
// 			delete[] charBuffer;
// 			break;
// 	}
// 	
// 	// Cleanup
// 	delete[] nTime, nValue;
// 	H5Sclose(timeSpace);
// 	H5Sclose(valueSpace);
// 
// 	return 0;
// }

// Parse log information from Nexus file
// bool NexusHelper::parseNexusFile(RunData* runData, QString fileName)
// {
// 	// Open HDF5 file
// 	hid_t file = H5Fopen(qPrintable(fileName), H5F_ACC_RDONLY, H5P_DEFAULT);
// 
// 	// Get start/end times
// 	QString tempString;
// 	if (nexusExtractString(file, "/raw_data_1/start_time", tempString)) startTime_ = QDateTime::fromString(tempString, "yyyy-MM-ddTHH:mm:ss");
// 	else
// 	{
// 		msg.print("Warning - Failed to get start time from NEXUS file. Using value from RunData instead.\n");
// 		startTime_ = runData->startDateTime();
// 	}
// 	if (nexusExtractString(file, "/raw_data_1/end_time", tempString)) endTime_ = QDateTime::fromString(tempString, "yyyy-MM-ddTHH:mm:ss");
// 	else
// 	{
// 		msg.print("Warning - Failed to get end time from NEXUS file. Using value from RunData instead.\n");
// 		endTime_ = runData->endDateTime();
// 	}
// 	
// 	// Specify path to sample environment group and open it
// 	hid_t selog = H5Gopen1(file, "/raw_data_1/selog");
// 	if (selog > 0)
// 	{
// 		// Iterate over items in SE log using H5Literate - HORRIBLE WAY TO DO THINGS!
// 		H5Literate(selog, H5_INDEX_NAME, H5_ITER_NATIVE, NULL, &NexusHelper::nexusGroupIterator, runData);
// 		H5Gclose(selog);
// 	}
// 
// 	// Specify path to run data group and open it
// 	hid_t runlog = H5Gopen1(file, "/raw_data_1/runlog");
// 	if (runlog > 0)
// 	{
// 		// Iterate over items in run log using H5Literate - DITTO!
// 		H5Literate(runlog, H5_INDEX_NAME, H5_ITER_NATIVE, NULL, &NexusHelper::nexusGroupIterator, runData);
// 		H5Gclose(runlog);
// 	}
// 
// 	H5Fclose(file);
// 
// 	return true;
// }

bool NexusHelper::loadSlice(QString fileName, QString root, QString xAxisValues, QString yAxisValues, QString zAxisValues, QString dataSpaceName, int zSlice, List<Data2D>& slices)
{
	// Open HDF5 file
	hid_t file = H5Fopen(qPrintable(fileName), H5F_ACC_RDONLY, H5P_DEFAULT);

	// Check that root group exists
	hid_t rootGroup = H5Gopen1(file, qPrintable(root));
	if (rootGroup <= 0)
	{
		msg.print("Couldn't open root group '%s' in Nexus file '%s'.\n", qPrintable(root), qPrintable(fileName));
		H5Fclose(file);
		return false;
	}

	// Get dataspaces for each specified dataset
	Vec3<bool> hasAxis;
	Vec3<hid_t> axisDataSet, axisDataSpace, axisDataType;
	Vec3<int> axisNDims;
	hsize_t axisDim[3];
	QString axisDataSetNames[3] = { xAxisValues, yAxisValues, zAxisValues };
	for (int axis=0; axis<3; ++axis)
	{
		hasAxis[axis] = false;
		if (axisDataSetNames[axis].isEmpty()) continue;

// 		printf("Axis %i = %s\n", axis, qPrintable(axisDataSetNames[axis]));
		// Open DataSet
		axisDataSet[axis] = H5Dopen2(rootGroup, qPrintable(axisDataSetNames[axis]), H5P_DEFAULT);
		if (axisDataSet[axis] <= 0)
		{
			msg.print("Failed to open dataset for axis %i (name '%s') in Nexus file '%s', root group '%s'.\n", axis, qPrintable(axisDataSetNames[axis]), qPrintable(fileName), qPrintable(root));
			H5Fclose(file);
			return false;
		}
		// Open DataSpace
		axisDataSpace[axis] = H5Dget_space(axisDataSet[axis]);
		axisNDims[axis] = H5Sget_simple_extent_ndims(axisDataSpace[axis]);

		// Check dataspace
		if (axisNDims[axis] != 1)
		{
			msg.print("Can't read Nexus data - DataSpace for axis %i is multi-dimensional (%i).\n", axis, axisNDims[axis]);
			H5Fclose(file);
			return false;
		}

		// Get size of array
// 		axisDims[axis] = new hsize_t[axisNDims[axis]];
		H5Sget_simple_extent_dims(axisDataSpace[axis], &axisDim[axis], NULL);
		axisDataType[axis] = H5Dget_type(axisDataSet[axis]);
		hasAxis[axis] = true;
	}

	// Is a dataspace provided?
	bool hasDataSpace = false;
	hid_t dataSpaceSet, dataSpace, dataSpaceType;
	int dataSpaceNDims, nDataSpaceElements;
	hsize_t* dataSpaceDims;
	if (!dataSpaceName.isEmpty())
	{
		// Open DataSet
		dataSpaceSet = H5Dopen2(rootGroup, qPrintable(dataSpaceName), H5P_DEFAULT);
		if (dataSpaceSet <= 0)
		{
			msg.print("Failed to open dataset for dataSpace specified (name '%s') in Nexus file '%s', root group '%s'.\n", qPrintable(dataSpaceName), qPrintable(fileName), qPrintable(root));
			H5Fclose(file);
			return false;
		}
		// Open DataSpace
		dataSpace = H5Dget_space(dataSpaceSet);
		dataSpaceNDims = H5Sget_simple_extent_ndims(dataSpace);

		// Check dataspace
		if (dataSpaceNDims == 1)
		{
			msg.print("Can't read Nexus data - DataSpace for specified dataSpace is one-dimensional.\n");
			H5Fclose(file);
			return false;
		}

		// Get size of array
		dataSpaceDims = new hsize_t[dataSpaceNDims];
		H5Sget_simple_extent_dims(dataSpace, dataSpaceDims, NULL);
		dataSpaceType = H5Dget_type(dataSpaceSet);
		hasDataSpace = true;
		nDataSpaceElements = dataSpaceDims[0];
		for (int n=1; n<dataSpaceNDims; ++n) nDataSpaceElements *= dataSpaceDims[n];
		printf("NElements in dataSpace = %i\n", nDataSpaceElements);
	}

	// See what we can do....
	int* intBuffer;
	double* doubleBuffer;
	hid_t status;
	if (hasAxis[0] && hasAxis[1] && (!hasAxis[2]))
	{
		if (hasDataSpace)
		{
			hsize_t dims[2] = { 1, 10 };
			hsize_t offset_in[2] = { 1, 0 }, offset_out[2] = { 0, 0 };
			hid_t sliceSpace = H5Screate_simple(2, dataSpaceDims, NULL);
			status = H5Sselect_hyperslab(sliceSpace, H5S_SELECT_SET, offset_out, NULL, dims, NULL);
			switch (H5Tget_class(dataSpaceType))
			{
// 				case (H5T_INTEGER):
// 					intBuffer = new int[(long int) 10];
// 					status = H5Dread(value, H5T_NATIVE_INT_g, H5S_ALL, H5S_ALL, H5P_DEFAULT, intBuffer);
// 					for (int n=0; n<nValue[0]; ++n) data->addRelativePoint(startTime_.addSecs(timeData[n]), (double) intBuffer[n]);
// 					delete[] intBuffer;
// 					break;
				case (H5T_FLOAT):
					doubleBuffer = new double[(long int) nDataSpaceElements];
					status = H5Sselect_hyperslab(dataSpace, H5S_SELECT_SET, offset_out, NULL, dims, NULL);
					status = H5Dread(dataSpaceSet, H5T_NATIVE_DOUBLE_g, dataSpace, sliceSpace, H5P_DEFAULT, doubleBuffer);
					for (int n=0; n<10; ++n) printf("Read value: %f\n", doubleBuffer[n]);
					delete[] doubleBuffer;
					break;
			}
		}
		else
		{
			// Looks like simple XY data, with x values in one dataset and y values in another...

			// Check that the number of datapoints in each dataspace is the same...
			if (axisDim[0] != axisDim[1])
			{
				msg.print("Sizes of specified X and Y value DataSpaces do not match (%i, %i).\n", axisDim[0], axisDim[1]);
				H5Fclose(file);
				return false;
			}

			// TODO
		}
	}
	else if (yAxisValues.isEmpty())
	{
		// Check that the number of datapoints in each dataspace is the same...
		if (axisDim[0] != axisDim[1])
		{
			msg.print("Sizes of specified X and Y value DataSpaces do not match (%i, %i).\n", axisDim[0], axisDim[1]);
			H5Fclose(file);
			return false;
		}

		// TODO
	}

	// Done
	H5Fclose(file);
	
	return true;
}
