/*
	*** DataSet
	*** src/base/dataset.h
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

#ifndef UCHROMA_DATASET_H
#define UCHROMA_DATASET_H

#include "base/data2d.h"
#include "base/transformer.h"
#include "templates/list.h"
#include <QtCore/QDir>

// Forward Declarations
class QTreeWidgetItem;

// DataSet
class DataSet: public ListItem<DataSet>
{
	public:
	// Constructor / Destructor
	DataSet();
	~DataSet();
	// Copy constructor
	DataSet(const DataSet& source);
	// Assignment operator
	void operator=(const DataSet& source);
	// Data Sources
	enum DataSource { FileSource, InternalSource, nDataSources };
	// Convert text string to DataSource
	static DataSource dataSource(const char* s);
	// Convert DataSource to text string
	static const char* dataSource(DataSource kwd);


	/*
	// Data
	*/
	private:
	// Source of data
	DataSource dataSource_;
	// Source file, if one is specified (not including path)
	QString sourceFileName_;
	// Name
	QString name_;
	// Data
	Data2D data_;
	// Transformed data
	Data2D transformedData_;

	public:
	// Set source of data
	void setDataSource(DataSet::DataSource source);
	// Return source of data
	DataSet::DataSource dataSource();
	// Set source filename
	void setSourceFileName(QString fileName);
	// Return source filename
	QString sourceFileName();
	// Set name
	void setName(QString name);
	// Return name
	QString name();
	// Load data from file
	bool loadData(QDir sourceDir);
	// Return data
	Data2D& data();
	// Transform original data with supplied transformers
	void transform(Transformer& xTransformer, Transformer& yTransformer, Transformer& zTransformer);
	// Return transformed data
	Data2D& transformedData();
};

#endif
