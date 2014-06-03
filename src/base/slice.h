/*
	*** Slice
	*** src/base/slice.h
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

#ifndef UCHROMA_SLICE_H
#define UCHROMA_SLICE_H

#include "base/data2d.h"
#include "base/dnchar.h"
#include "base/transformer.h"
#include "templates/list.h"
#include <QtCore/QDir>

// Forward Declarations
class ExtractedSliceGroup;
class QTreeWidgetItem;

// Slice
class Slice : public ListItem<Slice>
{
	public:
	// Constructor / Destructor
	Slice();
	~Slice();
	// Copy constructor
	Slice(const Slice& source);
	// Assignment operator
	void operator=(const Slice& source);
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
// 	// Whether a file has been associated to this slice
// 	bool fileAssociated_;
	// Title
	QString title_;
	// Data
	Data2D data_;
	// Transformed data
	Data2D transformedData_;

	public:
	// Set source of data
	void setDataSource(Slice::DataSource source);
	// Return source of data
	Slice::DataSource dataSource();
	// Set source filename
	void setSourceFileName(QString fileName);
	// Return source filename
	QString sourceFileName();
	// Set title
	void setTitle(QString title);
	// Return title
	QString title();
	// Load data from file
	bool loadData(QDir sourceDir);
	// Return data
	Data2D& data();
	// Transform original data with supplied transformers
	void transform(Transformer& xTransformer, Transformer& yTransformer, Transformer& zTransformer);
	// Return transformed data
	Data2D& transformedData();
};

// Extracted Slice
class ExtractedSlice : public ListItem<ExtractedSlice>
{
	public:
	// Constructor / Destructor
	ExtractedSlice();
	~ExtractedSlice();
	// Copy constructor
	ExtractedSlice(const ExtractedSlice& source);
	// Assignment operator
	void operator=(const ExtractedSlice& source);


	/*
	// Data
	*/
	private:
	// Slice title
	QString title_;
	// Original slice data, acquired from raw loaded data
	Data2D data_;
	// Transformed data
	Data2D transformedData_;
	// Axis along which slice was taken
	int axis_;
	// Axis value at which slice was taken
	double axisValue_;
	// Parent slice group
	ExtractedSliceGroup* group_;
	// Tree node associated to the slice
	QTreeWidgetItem* treeItem_;

	public:
	// Set title
	void setTitle(QString title);
	// Return title
	QString title();
	// Return original data
	Data2D& data();
	// Transform original data
	void transformData(int xTransform, int yTransform);
	// Return transformed data
	Data2D& transformedData();
	// Return axis along which slice was generated
	int axis();
	// Return axis value at which slice was generated
	double axisValue();
	// Set group
	void setGroup(ExtractedSliceGroup* group);
	// Return group
	ExtractedSliceGroup* group();
	// Set tree node associated to the slice
	void setTreeItem(QTreeWidgetItem* item);
	// Return tree node associated to the slice
	QTreeWidgetItem* treeItem();
};

// Extracted Slice Group
class ExtractedSliceGroup : public ListItem<ExtractedSliceGroup>
{
	public:
	// Constructor / Destructor
	ExtractedSliceGroup();
	~ExtractedSliceGroup();


	/*
	 * Pen Styles
	 */
	public:
	// Available line styles
	enum GroupLineStyle { SolidStyle, DotStyle, DashedStyle, nGroupLineStyles };

	private:
	// Array of line (dash) definitions
	static QVector<qreal> lineStyles_[nGroupLineStyles];


	/*
	// Data
	*/
	private:
	// Group name
	QString name_;
	// List of extracted slices contained within group
	List<ExtractedSlice> extractedSlices_;
	// X axis transformation to apply to generate transformed data
	int xAxisTransform_;
	// Y axis transformation to apply to generate transformed data
	int yAxisTransform_;
	// Line style
	ExtractedSliceGroup::GroupLineStyle lineStyle_;
	// Whether group is visible
	bool visible_;
	// Tree node associated to the group
	QTreeWidgetItem* treeItem_;

	public:
	// Set name
	void setName(QString name);
	// Return name
	QString name();
	// Add extracted slice to group
	ExtractedSlice* addSlice(const ExtractedSlice& slice);
	// Return list of extracted slices in group
	ExtractedSlice* extractedSlices();
	// Set associated line style
	void setLineStyle(ExtractedSliceGroup::GroupLineStyle style);
	// Return associated line dash pattern
	const QVector<qreal>& dashes();
	// Set visibility of group
	void setVisible(bool visible);
	// Return visibility of group
	bool visible();
	// Set tree node associated to the group
	void setTreeItem(QTreeWidgetItem* item);
	// Return tree node associated to the group
	QTreeWidgetItem* treeItem();
};

// Display Slice
class DisplaySlice : public ListItem<DisplaySlice>
{
	public:
	// Constructor / Destructor
	DisplaySlice();
	~DisplaySlice();


	/*
	 * Data
	 */
	private:
	// Y values of slice data
	Array<double> y_;
	// Flags indicating that y values exist at each point
	Array<bool> yExists_;
	// Z value of slice
	double z_;

	public:
	// Add y value and associated flag
	void add(double y);
	// Add dummy y value and associated flag
	void addDummy();
	// Return y array
	const Array<double>& y() const;
	// Return flags array
	const Array<bool>& yExists() const;
	// Set specific y value
	void setY(int index, double yNew);
	// Set z value of slice
	void setZ(double z);
	// Return z value of slice
	double z() const;
};

#endif
