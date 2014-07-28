/*
	*** Data Collection
	*** src/base/collection.h
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

#ifndef UCHROMA_COLLECTION_H
#define UCHROMA_COLLECTION_H

#include "base/dataset.h"
#include "base/displaydataset.h"
#include "base/transformer.h"
#include "base/colourscale.h"
#include "render/primitivelist.h"

// Forward Declarations
class ViewPane;

class Collection : public ListItem<Collection>
{
	public:
	// Constructor
	Collection();
	// Destructor
	~Collection();
	// Copy constructor
	Collection(const Collection& source);
	// Assignment operator
	void operator=(const Collection& source);


	/*
	 * Data
	 */
	private:
	// Title
	QString title_;
	// List of datasets
	List<DataSet> dataSets_;
	// Root directory for datafiles
	QDir dataFileDirectory_;
	// Extreme values of raw data
	Vec3<double> dataMin_, dataMax_;

	public:
	// Set title
	void setTitle(QString title);
	// Return title
	QString title();
	// Add dataset
	DataSet* addDataSet();
	// Add slice at specified z
	DataSet* addDataSet(double z);
	// Add slice, copying from supplied data
	void addDataSet(DataSet* source);
	// Return number of datasets
	int nDataSets();
	// Remove dataset
	void removeDataSet(DataSet* dataSet);
	// Set z value of specified slice
	void setDataSetZ(DataSet* target, double z);
	// Set data for specified slice
	void setDataSetData(DataSet* target, Data2D* newData);
	// Return first slice in list
	DataSet* dataSets() const;
	// Return last slice in list
	DataSet* lastDataSet();
	// Return nth slice in list
	DataSet* dataSet(int index);
	// Return number of dataset with no data present
	int nEmptyDataSets();
	// Clear slice data from collection
	void clearDataSets();
	// Return total number of points across all dataset
	int nDataPoints();
	// Set root directory for datafiles
	void setDataFileDirectory(QDir directory);
	// Return root directory for datafiles
	QDir dataFileDirectory();
	// Load specified dataset
	bool loadDataSet(DataSet* dataSet);
	// Reload data for all dataset
	int loadAllDataSets();
	// Return data minima, calculating if necessary
	Vec3<double> dataMin();
	// Return data maxima, calculating if necessary
	Vec3<double> dataMax();


	/*
	 * Transform
	 */
	private:
	// Extreme values of transformed data 
	Vec3<double> transformMin_, transformMax_;
	// Extreme positive values of transformed data
	Vec3<double> transformMinPositive_, transformMaxPositive_;
	// Transform for data
	Transformer transforms_[3];
	// Interpolation flags
	Vec3<bool> interpolate_, interpolateConstrained_;
	// Interpolation step sizes
	Vec3<double> interpolationStep_;

	public:
	// Return transformed data minima, calculating if necessary
	Vec3<double> transformMin();
	// Return transformed data maxima, calculating if necessary
	Vec3<double> transformMax();
	// Return transformed positive data minima
	Vec3<double> transformMinPositive();
	// Return transformed positive data maxima
	Vec3<double> transformMaxPositive();
	// Set transform equation for data
	void setTransformEquation(int axis, QString transformEquation);
	// Return transform equation for data
	QString transformEquation(int axis);
	// Return whether specified transform equation is valid
	bool transformEquationValid(int axis);
	// Set whether specified transform is enabled
	void setTransformEnabled(int axis, bool enabled);
	// Return whether specified transform is enabled
	bool transformEnabled(int axis);
	// Set whether interpolation is enabled
	void setInterpolate(int axis, bool enabled);
	// Return whether interpolation is enabled
	bool interpolate(int axis);
	// Set whether interpolation is constrained
	void setInterpolateConstrained(int axis, bool enabled);
	// Return whether interpolation is constrained
	bool interpolateConstrained(int axis);
	// Set interpolation step size
	void setInterpolationStep(int axis, double step);
	// Return interpolation step size
	double interpolationStep(int axis);


	/*
	 * Associated Data
	 */
	public:
	// Collection Type
	enum CollectionType
	{
		MasterCollection,
		FitCollection,
		ExtractedCollection,
		CurrentSliceCollection
	};

	private:
	// Parent Collection
	Collection* parent_;
	// Type of this collection
	CollectionType type_;
	// List of fits made to parent
	List<Collection> fitData_;
	// List of slices extracted from parent
	List<Collection> extractedData_;
	// Current slice data
	Collection* currentSlice_;	

	private:
	// Return axis bin value of closest point to supplied value
	int closestBin(int axis, double value);
	// Get slice at specified axis and bin
	void getSlice(int axis, int bin);

	public:
	// Find collection with name specified
	Collection* findCollection(QString name);
	// Return next logical collection in lists
	Collection* nextCollection(bool descend);
	// Return previous logical collection in lists
	Collection* previousCollection(bool descend);
	// Set parent Collection
	void setParent(Collection* parent);
	// Return parent Collection
	Collection* parent();
	// Return type of this collection
	CollectionType type();
	// Add fit data to Collection
	Collection* addFitData(QString title = QString());
	// Remove specified fitdata from list
	void removeFitData(Collection* collection);
	// Return fit data in Collection
	Collection* fitData();
	// Add extracted data to Collection
	Collection* addExtractedData(QString title = QString());
	// Return extracted data in Collection
	Collection* extractedData();
	// Remove specified extracted data from list
	void removeExtractedData(Collection* collection);
	// Update current slice based on specified axis and bin
	void updateCurrentSlice(int axis, double axisValue);
	// Extract current slice based on specified axis and bin
	void extractCurrentSlice(int axis, double axisValue);
	// Return current slice
	Collection* currentSlice();


	/*
	 * Dependent Data / Signalling
	 */
	public:
	// Data changed signal
	enum CollectionSignal { CollectionCreatedSignal, CollectionDeletedSignal, CurrentSliceChangedSignal, DataChangedSignal, ExtractedDataAddedSignal };

	private:
	// List of notifications over all collections
	static RefList<Collection,CollectionSignal> collectionSignals_;
	// Notify dependents that something in this collection has changed
	void notifyDependents(Collection* source, Collection::CollectionSignal signal);

	public:
	// Return first signal in lists
	static RefListItem<Collection,CollectionSignal>* collectionSignals();
	// Delete specified signal and return next
	static RefListItem<Collection,CollectionSignal>* deleteCollectionSignal(RefListItem<Collection,CollectionSignal>* collectionSignal);


	/*
	 * Update
	 */
	private:
	// Flag indicating if slice data has been changed in any way
	bool dataChanged_;

	private:
	// Update data limits and transform data
	void updateLimitsAndTransforms();

	public:
	// Flag that slice data has been changed
	void setDataChanged();


	/*
	 * Colours
	 */
	public:
	// Available colourscale sources
	enum ColourSource { SingleColourSource, RGBGradientSource, HSVGradientSource, CustomGradientSource, nColourSources };
	// Convert text string to ColourSource
	static ColourSource colourSource(const char* s);
	// Convert ColourSource to text string
	static const char* colourSource(ColourSource kwd);
	// Alpha control options
	enum AlphaControl { OwnAlpha, FixedAlpha, nAlphaControls };
	// Convert text string to AlphaControl
	static AlphaControl alphaControl(const char* s);
	// Convert AlphaControl to text string
	static const char* alphaControl(AlphaControl kwd);

	private:
	// Current colourscale source to use
	ColourSource colourSource_;
	// Points for SingleColour, RGBGradient and HSVGradient sources
	ColourScalePoint colourSinglePoint_;
	ColourScalePoint colourRGBGradientAPoint_, colourRGBGradientBPoint_;
	ColourScalePoint colourHSVGradientAPoint_, colourHSVGradientBPoint_;
	// Actual ColourScale used by surface
	ColourScale colourScale_;
	// Custom ColourScale source
	ColourScale customColourScale_;
	// Current alpha control
	AlphaControl alphaControl_;
	// Fixed alpha value (for FixedAlpha option)
	double fixedAlpha_;
	// Flag indicating that colourscale is valid (and doesn't need to be regenerated)
	bool colourScaleValid_;

	public:
	// Update colour scale
	void updateColourScale();
	// Set colourscale source to use
	void setColourSource(ColourSource source);
	// Return colourscale source to use
	ColourSource colourSource();
	// Set colourscale point colour
	void setColourScalePoint(ColourSource source, QColor colour, double value = 0.0, int index = -1);
	// Return colourscale point specified
	const ColourScalePoint* colourScalePoint(ColourSource source, int index = -1);
	// Return colour of point specified
	QColor colourScalePointColour(ColourSource source, int index = -1);
	// Retu5rn value of point specified
	double colourScalePointValue(ColourSource source, int index = -1);
	// Add empty point to end of custom colourscale
	void addCustomColourScalePoint();
	// Add point to custom colourscale
	void addCustomColourScalePoint(double value, QColor colour);
	// Return number of custom colourscale points
	int nCustomColourScalePoints();
	// Return first custom colourscale point in list
	ColourScalePoint* customColourScalePoints();
	// Return custom colourscale point with index specified
	ColourScalePoint* customColourScalePoint(int id);
	// Remove specified colourscale point
	void removeCustomColourScalePoint(ColourScalePoint* point);
	// Set alpha control
	void setAlphaControl(AlphaControl alpha);
	// Return current alpha control
	AlphaControl alphaControl();
	// Set fixed alpha value
	void setFixedAlpha(double alpha);
	// Return fixed alpha value
	double fixedAlpha();
	// Return current colourscale
	const ColourScale& colourScale();
	// Return whether colourscale is valid
	bool colourScaleValid();


	/*
	 * Display
	 */
	public:
	// Display types enum
	enum DisplayStyle { LineXYStyle, LineZYStyle, GridStyle, SurfaceStyle, nDisplayStyles };
	// Convert text string to DisplayStyle
	static DisplayStyle displayStyle(const char* s);
	// Convert DisplayStyle to text string
	static const char* displayStyle(DisplayStyle kwd);

	private:
	// Whether data is visible
	bool visible_;
	// Transformed data to display
	List<DisplayDataSet> displayData_;
	// Abscissa values for display data
	Array<double> displayAbscissa_;
	// Display style of data
	DisplayStyle displayStyle_;
	// Line width (for line styles)
	double displayLineWidth_;
	// Flag indicating whether display data is valid (and don't need to be regenerated)
	bool displayDataValid_;
	// PrimitiveList containing GL display data
	PrimitiveList displayPrimitives_;
	// Flag indicating whether display primitives are valid (and don't need to be regenerated)
	bool displayPrimitivesValid_;
	// View pane on which this data is being displayed
	ViewPane* displayPane_;

	public:
	// Set whether data is visible
	bool setVisible(bool visible);
	// Return hether data is visible
	bool visible();
	// Return transformed display abscissa for data
	const Array<double>& displayAbscissa() const;
	// Return transformed data to display
	List<DisplayDataSet>& displayData();
	// Set display style of data
	void setDisplayStyle(DisplayStyle style);
	// Return display style of data
	DisplayStyle displayStyle();
	// Set line width (for line styles)
	void setDisplayLineWidth(double width);
	// Return Line width (for line styles)
	double displayLineWidth();
	// Manually set the flag to force regeneration of surface data
	void setDisplayDataInvalid();
	// Flag that the primitive has been updated
	void setDisplayPrimitiveValid();
	// Return whether primitives are valid
	bool displayPrimitivesValid();
	// Return list of display primitives
	PrimitiveList& displayPrimitives();
	// Update display data and surface if necessary
	void updateDisplayData();
	// Set view pane on which this data is being displayed
	void setDisplayPane(ViewPane* pane);
	// Return view pane on which this data is being displayed
	ViewPane* displayPane();
	// Send collection data to GL, including any associated fit and extracted data
	void sendToGL();
};

#endif
