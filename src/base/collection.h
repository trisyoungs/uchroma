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
#include "render/linestyle.h"
#include "render/primitivelist.h"
#include "templates/objectlist.h"

// Forward Declarations
class FitKernel;

class Collection : public ListItem<Collection>, public ObjectList<Collection>
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
	// Name of collection
	QString name_;
	// List of datasets
	List<DataSet> dataSets_;
	// Root directory for datafiles
	QDir dataFileDirectory_;
	// Extreme values of raw data
	Vec3<double> dataMin_, dataMax_;
	// Version counter for changes to data
	int dataVersion_;

	public:
	// Set name of collection
	void setName(QString title);
	// Return name
	QString name();
	// Return collection locator
	QString locator(QString currentPath = QString());
	// Add dataset
	DataSet* addDataSet();
	// Add dataset at specified z
	DataSet* addDataSet(double z);
	// Add dataset, copying from supplied data
	void addDataSet(DataSet* source);
	// Copy datasets from specified source collection
	void copyDataSets(Collection* source);
	// Return number of datasets
	int nDataSets();
	// Remove dataset
	void removeDataSet(DataSet* dataSet);
	// Set z value of specified dataset
	void setDataSetZ(DataSet* target, double z);
	// Set data for specified dataset (from arrays);
	void setDataSetData(DataSet* target, const Array<double>& x, const Array<double>& y);
	// Set data for specified dataste (from source DataSet)
	void setDataSetData(DataSet* target, DataSet& source);
	// Return first dataset in list
	DataSet* dataSets() const;
	// Return named dataset
	DataSet* dataSet(QString name);
	// Return last dataset in list
	DataSet* lastDataSet();
	// Return nth dataset in list
	DataSet* dataSet(int index);
	// Return index of specified dataset
	int dataSetIndex(const char* name);
	// Return unique name based on supplied basename
	QString uniqueDataSetName(QString baseName);
	// Return number of dataset with no data present
	int nEmptyDataSets();
	// Clear dataset data from collection
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
	// Return version counter for changes to data
	int dataVersion();


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
	// List of fits
	List<Collection> fits_;
	// List of extracted slices
	List<Collection> slices_;
	// Current slice data
	Collection* currentSlice_;
	// FitKernel (if a FitCollection)
	FitKernel* fitKernel_;

	private:
	// Return axis bin value of closest point to supplied value
	int closestBin(int axis, double value);
	// Get slice at specified axis and bin
	void getSlice(int axis, int bin);

	public:
	// Locate collection using parts specified
	Collection* locateCollection(const QStringList& parts, int offset);
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
	// Return icon string reflecting this Collection's type / status
	QString iconString(bool isUsed = true);
	// Return locally-unique fit name based on basename provided
	QString uniqueFitName(QString baseName);
	// Add fit to Collection
	Collection* addFit(QString name);
	// Remove specified fit from list
	void removeFit(Collection* collection);
	// Return fit data
	Collection* fits();
	// Return fit with name specified
	Collection* fit(QString name);
	// Return locally-unique slice name based on basename provided
	QString uniqueSliceName(QString baseName);
	// Add slice to Collection
	Collection* addSlice(QString name);
	// Remove specified slice
	void removeSlice(Collection* collection);
	// Return slices
	Collection* slices();
	// Return slice with name specified
	Collection* slice(QString name);
	// Update current slice based on specified axis and bin
	void updateCurrentSlice(int axis, double axisValue);
	// Extract current slice based on specified axis and bin
	void extractCurrentSlice(int axis, double axisValue);
	// Return current slice
	Collection* currentSlice();
	// Add FitKernel, if one does not exist
	void addFitKernel();
	// Return FitKernel
	FitKernel* fitKernel();


	/*
	 * Dependent Data / Signalling
	 */
	public:
	// Data changed signal
	enum CollectionSignal { CollectionCreatedSignal, CollectionDeletedSignal, CurrentSliceChangedSignal, DataChangedSignal, ExtractedDataAddedSignal };

	private:
	// List of notifications over all collections
	static RefList<Collection,CollectionSignal> collectionSignals_;
	// Register that something in this collection has changed
	void registerChange(Collection* source, Collection::CollectionSignal signal);

	public:
	// Return first signal in lists
	static RefListItem<Collection,CollectionSignal>* collectionSignals();
	// Delete specified signal and return next
	static RefListItem<Collection,CollectionSignal>* deleteCollectionSignal(RefListItem<Collection,CollectionSignal>* collectionSignal);


	/*
	 * Update
	 */
	private:
	// Data version at which limits and transforms were last updated
	int limitsAndTransformsVersion_;

	private:
	// Update data limits and transform data
	void updateLimitsAndTransforms();


	/*
	 * Colours
	 */
	public:
	// Available colourscale sources
	enum ColourSource { SingleColourSource, RGBGradientSource, HSVGradientSource, CustomGradientSource, nColourSources };
	// Convert text string to ColourSource
	static ColourSource colourSource(const char* s);
	// Convert ColourSource to text string
	static const char* colourSource(Collection::ColourSource cs);
	// Alpha control options
	enum AlphaControl { OwnAlpha, FixedAlpha, nAlphaControls };
	// Convert text string to AlphaControl
	static AlphaControl alphaControl(const char* s);
	// Convert AlphaControl to text string
	static const char* alphaControl(Collection::AlphaControl as);

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
	// Version for colourscale
	int colourVersion_;
	// Version at which the colourscale was last generated at
	bool colourScaleGeneratedAt_;

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
	// Return colour version
	int colourVersion();


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
	// Data version at which displayData_ was last generated
	int displayDataGeneratedAt_;
	// Abscissa values for display data
	Array<double> displayAbscissa_;
	// Display style of data
	DisplayStyle displayStyle_;
	// Line style
	LineStyle displayLineStyle_;
	// Style version
	int styleVersion_;

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
	// Return line style
	LineStyle& displayLineStyle();
	// Return style version
	int styleVersion();
};

#endif
