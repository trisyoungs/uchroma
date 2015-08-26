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
#include <QDir>

// Forward Declarations
class QTreeWidgetItem;
class Collection;

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
	static DataSource dataSource(QString s);
	// Convert DataSource to text string
	static const char* dataSource(DataSource kwd);


	/*
	 * Parent Collection
	 */
	private:
	// Parent collection
	Collection* parent_;

	private:
	// Notify parent that data has changed
	void notifyParent();

	public:
	// Set parent
	void setParent(Collection* parent);
	// Return parent
	Collection* parent();


	/*
	 * Data
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
	const Data2D& data() const;
	// Return X array from data
	const Array<double>& x() const;
	// Return Y array from data
	const Array<double>& y() const;
	// Return z value from data
	double z() const;
	// Transform original data with supplied transformers
	void transform(Transformer& xTransformer, Transformer& yTransformer, Transformer& zTransformer);
	// Return transformed data
	Data2D& transformedData();


	/*
	 * Data Operations
	 */
	public:
	// Reset (zero) data
	void resetData();
	// Initialise data to specified number of points
	void initialiseData(int nPoints);
	// Set data from supplied Data2D
	void setData(Data2D& source);
	// Add point to data
	void addPoint(double x, double y);
	// Set x value
	void setX(int index, double newX);
	// Set y value
	void setY(int index, double newY);
	// Set z data
	void setZ(double z);
	// Add to specified axis value`
	void addConstantValue(int axis, double value);
	// Calculate average y value over x range specified
	double averageY(double xMin, double xMax) const;
};

#endif
