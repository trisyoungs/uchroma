/*
	*** DataSet
	*** src/base/dataset.cpp
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

#include "base/dataset.h"
#include "collection.h"

// Data Sources
const char* DataSourceKeywords[] = { "File", "Internal" };

/*!
 * \brief Convert text string to SettingsKeyword
 */
DataSet::DataSource DataSet::dataSource(QString s)
{
	for (int n=0; n<nDataSources; ++n) if (s == DataSourceKeywords[n]) return (DataSet::DataSource) n;
	return nDataSources;
}

/*!
 * \brief Convert SettingsKeyword to text string
 */
const char* DataSet::dataSource(DataSet::DataSource kwd)
{
	return DataSourceKeywords[kwd];
}

// Constructor
DataSet::DataSet() : ListItem<DataSet>()
{
	sourceFileName_ = "";
	dataSource_ = DataSet::InternalSource;
	name_ = "New DataSet";
	parent_ = NULL;
}

// Destructor
DataSet::~DataSet()
{
}

// Copy constructor
DataSet::DataSet(const DataSet& source)
{
	(*this) = source;
}

// Assignment operator
void DataSet::operator=(const DataSet& source)
{
	sourceFileName_ = source.sourceFileName_;
	name_ = source.name_;
	data_ = source.data_;
	transformedData_ = source.transformedData_;
	dataSource_ = source.dataSource_;
}

/*
 * Parent Collection
 */

// Set parent
void DataSet::setParent(Collection* parent)
{
	parent_ = parent;
}

// Return parent
Collection* DataSet::parent()
{
	return parent_;
}

// Notify parent that data has changed
void DataSet::notifyParent()
{
	if (parent_) parent_->notifyDataChanged();
}

/* 
 * Data
 */

// Set source of data
void DataSet::setDataSource(DataSet::DataSource source)
{
	dataSource_ = source;
}

// Return source of data
DataSet::DataSource DataSet::dataSource()
{
	return dataSource_;
}

// Set source filename
void DataSet::setSourceFileName(QString fileName)
{
	sourceFileName_ = fileName;
}

// Return source filename
QString DataSet::sourceFileName()
{
	return sourceFileName_;
}

// Set associated data name
void DataSet::setName(QString name)
{
	name_ = name;
}

// Return name
QString DataSet::name()
{
	return name_;
}

// Load data from file
bool DataSet::loadData(QDir sourceDir)
{
	// Check that a fileName is specified - otherwise there is nothing to do
	if (dataSource_ != DataSet::FileSource)
	{
		msg.print("DataSet::loadData() - Datasource != FileSource\n");
		return false;
	}

	// Clear any existing data
	data_.arrayX().clear();
	data_.arrayY().clear();

	// Check file exists
	if (!QFile::exists(sourceDir.absoluteFilePath(sourceFileName_)))
	{
		msg.print("Error: File '%s' not found.\n", qPrintable(sourceFileName_));
		return false;
	}

	// Read in the data
	return data_.load(qPrintable(sourceDir.absoluteFilePath(sourceFileName_)));
}

// Return data
const Data2D& DataSet::data() const
{
	return data_;
}

// Return X array from data
const Array<double>& DataSet::x() const
{
	return data_.constArrayX();
}

// Return Y array from data
const Array<double>& DataSet::y() const
{
	return data_.constArrayY();
}

// Return z value from data
double DataSet::z() const
{
	return data_.z();
}

// Transform original data with supplied transformers
void DataSet::transform(Transformer& xTransformer, Transformer& yTransformer, Transformer& zTransformer)
{
	// X
	if (xTransformer.enabled()) transformedData_.arrayX() = xTransformer.transformArray(data_.arrayX(), data_.arrayY(), data_.z(), 0);
	else transformedData_.arrayX() = data_.arrayX();

	// Y
	if (yTransformer.enabled()) transformedData_.arrayY() = yTransformer.transformArray(data_.arrayX(), data_.arrayY(), data_.z(), 1);
	else transformedData_.arrayY() = data_.arrayY();

	// Z
	if (zTransformer.enabled()) transformedData_.setZ(zTransformer.transform(0.0, 0.0, data_.z()));
	else transformedData_.setZ(data_.z());
}

// Return transformed data
Data2D& DataSet::transformedData()
{
	return transformedData_;
}

/*
 * Data Operations
 */

// Reset (zero) data
void DataSet::resetData()
{
	data_.reset();

	notifyParent();
}

// Initialise data to specified number of points
void DataSet::initialiseData(int nPoints)
{
	data_.initialise(nPoints);

	notifyParent();
}

// Set data from supplied Data2D
void DataSet::setData(Data2D& source)
{
	data_ = source;

	notifyParent();
}

// Add point to data
void DataSet::addPoint(double x, double y)
{
	data_.addPoint(x, y);

	notifyParent();
}

// Set x value
void DataSet::setX(int index, double newX)
{
	data_.setX(index, newX);

	notifyParent();
}

// Set y value
void DataSet::setY(int index, double newY)
{
	data_.setY(index, newY);

	notifyParent();
}

// Set z data
void DataSet::setZ(double z)
{
	data_.setZ(z);

	notifyParent();
}

// Add to specified axis value
void DataSet::addConstantValue(int axis, double value)
{
	if (axis == 0) data_.arrayX() += value;
	else if (axis == 1) data_.arrayY() += value;
	else if (axis == 2) data_.addZ(value);

	notifyParent();
}

// Calculate average y value over x range specified
double DataSet::averageY(double xMin, double xMax) const
{
	double result = 0.0;
	int nAdded = 0;
	for (int n=0; n<data_.nPoints(); ++n)
	{
		if (data_.x(n) < xMin) continue;
		else if (data_.x(n) > xMax) break;
		++nAdded;
		result += data_.y(n);
	}
	return (nAdded == 0 ? 0.0 : result / nAdded);
}
