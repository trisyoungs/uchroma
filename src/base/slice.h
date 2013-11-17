/*
	*** Slice
	*** src/base/slice.h
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

#ifndef UCHROMA_SLICE_H
#define UCHROMA_SLICE_H

#include "base/data2d.h"
#include "base/dnchar.h"
#include "base/transformer.h"
#include "templates/list.h"
#include <QtCore/QDir>

// Forward Declarations
/* None */

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


	/*
	// Data
	*/
	private:
	// Source file, if one is specified (not including path)
	QString sourceFileName_;
	// Whether a file has been associated to this slice
	bool fileAssociated_;
	// Data name
	QString dataName_;
	// Data
	Data2D data_;
	// Transformed data
	Data2D transformedData_;

	public:
	// Set source filename
	void setSourceFileName(QString fileName);
	// Return source filename
	QString sourceFileName();
	// Set data name
	void setDataName(QString dataName);
	// Return data name
	QString dataName();
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
};

// Extracted Slice Group
class ExtractedSliceGroup : public ListItem<ExtractedSliceGroup>
{
	public:
	// Constructor / Destructor
	ExtractedSliceGroup();
	~ExtractedSliceGroup();


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

	public:
	// Set name
	void setName(QString name);
	// Return name
	QString name();
	// Return list of extracted slices in group
	ExtractedSlice* extractedSlices();
};

#endif
