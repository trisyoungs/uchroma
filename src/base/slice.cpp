/*
	*** Slice
	*** src/base/slice.cpp
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

#include "base/slice.h"
#include <QtGui/QMessageBox>

/*
 * Slice
 */

// Constructor
Slice::Slice() : ListItem<Slice>()
{
	sourceFileName_ = "";
	fileAssociated_ = false;
	dataName_ = "";
}

// Destructor
Slice::~Slice()
{
}

// Copy constructor
Slice::Slice(const Slice& source)
{
	(*this) = source;
}

// Assignment operator
void Slice::operator=(const Slice& source)
{
	sourceFileName_ = source.sourceFileName_;
	dataName_ = source.dataName_;
	data_ = source.data_;
	transformedData_ = source.transformedData_;
	fileAssociated_ = source.fileAssociated_;
}

// Set source filename
void Slice::setSourceFileName(QString fileName)
{
	sourceFileName_ = fileName;
	fileAssociated_ = true;
}

// Return source filename
QString Slice::sourceFileName()
{
	return sourceFileName_;
}

// Set associated data name
void Slice::setDataName(QString dataName)
{
	dataName_ = dataName;
}

// Return data name
QString Slice::dataName()
{
	return dataName_;
}

// Load data from file
bool Slice::loadData(QDir sourceDir)
{
	// Check that a fileName is specified - otherwise there is nothing to do
	if (!fileAssociated_) return false;

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
Data2D& Slice::data()
{
	return data_;
}

// Transform original data with supplied transformers
void Slice::transform(Transformer& xTransformer, Transformer& yTransformer, Transformer& zTransformer)
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
Data2D& Slice::transformedData()
{
	return transformedData_;
}

/*
 * Extracted Slice
 */

// Constructor
ExtractedSlice::ExtractedSlice() : ListItem<ExtractedSlice>()
{
	title_ = "";
	axis_ = -1;
	axisValue_ = 0.0;
	group_ = NULL;
	treeItem_ = NULL;
}

// Destructor
ExtractedSlice::~ExtractedSlice()
{
}

// Copy constructor
ExtractedSlice::ExtractedSlice(const ExtractedSlice& source)
{
	(*this) = source;
}

// Assignment operator
void ExtractedSlice::operator=(const ExtractedSlice& source)
{
	title_ = source.title_;
	axis_ = source.axis_;
	axisValue_ = source.axisValue_;
	data_ = source.data_;
	transformedData_ = source.transformedData_;
	group_ = source.group_;
}

// Set title
void ExtractedSlice::setTitle(QString title)
{
	title_ = title;
}

// Return title
QString ExtractedSlice::title()
{
	return title_;
}

// Return original data
Data2D& ExtractedSlice::data()
{
	return data_;
}

// Transform original data
void ExtractedSlice::transformData(int xTransform, int yTransform)
{
	// Copy original data
	transformedData_ = data_;

	// Transform X axis data
	Array<double>& x = transformedData_.arrayX();
// 	for (int n=0; n<x.nItems(); ++n) x[n] = transformPoint
}

// Return transformed data
Data2D& ExtractedSlice::transformedData()
{
	return transformedData_;
}

// Return axis along which slice was generated
int ExtractedSlice::axis()
{
	return axis_;
}

// Return axis value at which slice was generated
double ExtractedSlice::axisValue()
{
	return axisValue_;
}

// Set group
void ExtractedSlice::setGroup(ExtractedSliceGroup* group)
{
	group_ = group;
}

// Return group
ExtractedSliceGroup* ExtractedSlice::group()
{
	return group_;
}

// Set tree node associated to the slice
void ExtractedSlice::setTreeItem(QTreeWidgetItem* item)
{
	treeItem_ = item;
}

// Return tree node associated to the slice
QTreeWidgetItem* ExtractedSlice::treeItem()
{
	return treeItem_;
}

/*
 * Extracted Slice Group
 */

// Static member
QVector<qreal> ExtractedSliceGroup::lineStyles_[] = {
	QVector<qreal>() << 10 << 1,
	QVector<qreal>() << 2 << 2,
	QVector<qreal>() << 5 << 2
};

// Constructor
ExtractedSliceGroup::ExtractedSliceGroup() : ListItem<ExtractedSliceGroup>()
{
	name_ = "New Group";
	xAxisTransform_ = 0;
	yAxisTransform_ = 0;
	lineStyle_ = ExtractedSliceGroup::SolidStyle;
	visible_ = true;
	treeItem_ = NULL;
}

// Destructor
ExtractedSliceGroup::~ExtractedSliceGroup()
{
}

// Set name
void ExtractedSliceGroup::setName(QString name)
{
	name_ = name;
}

// Return name
QString ExtractedSliceGroup::name()
{
	return name_;
}

// Add extracted slice to group
ExtractedSlice* ExtractedSliceGroup::addSlice(const ExtractedSlice& slice)
{
	ExtractedSlice* newSlice = extractedSlices_.add();
	(*newSlice) = slice;
	newSlice->setGroup(this);
	return newSlice;
}

// Return list of extracted slices in group
ExtractedSlice* ExtractedSliceGroup::extractedSlices()
{
	return extractedSlices_.first();
}

// Set associated line style
void ExtractedSliceGroup::setLineStyle(ExtractedSliceGroup::GroupLineStyle style)
{
	lineStyle_ = style;
}

// Return associated line dash pattern
const QVector<qreal>& ExtractedSliceGroup::dashes()
{
	return lineStyles_[lineStyle_];
}

// Set visibility of group
void ExtractedSliceGroup::setVisible(bool visible)
{
	visible_ = visible;
}

// Return visibility of group
bool ExtractedSliceGroup::visible()
{
	return visible_;
}

// Set tree node associated to the group
void ExtractedSliceGroup::setTreeItem(QTreeWidgetItem* item)
{
	treeItem_ = item;
}

// Return tree node associated to the group
QTreeWidgetItem* ExtractedSliceGroup::treeItem()
{
	return treeItem_;
}
