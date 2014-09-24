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
#include <QtGui/QMessageBox>

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
	if (dataSource_ != DataSet::FileSource) return false;

	// Clear any existing data
	data_.arrayX().clear();
	data_.arrayY().clear();

	// Check file exists
	if (!QFile::exists(sourceDir.absoluteFilePath(sourceFileName_)))
	{
// 		QMessageBox::warning(NULL, "File Not Found", QString("The file ") + sourceFileName_ + " could not be found.");
		return false;
	}

	// Read in the data
	return data_.load(qPrintable(sourceDir.absoluteFilePath(sourceFileName_)));
}

// Return data
Data2D& DataSet::data()
{
	return data_;
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
