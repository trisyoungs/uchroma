/*
	*** Two-Dimensional Data
	*** src/base/data2d.h
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

#ifndef UCHROMA_DATA2D_H
#define UCHROMA_DATA2D_H

#include <QString>
#include "templates/array.h"

#define OPTOLERANCE 1.0e-6

// Data2D
class Data2D : public ListItem<Data2D>
{
	public:
	// Constructor
	Data2D();
	// Destructor
	~Data2D();
	// Copy Constructor
	Data2D(const Data2D& source);
	// Clear data
	void clear();


	/*!
	 * \name Data
	 */
	private:
	// Array of x values
	Array<double> x_;
	// Array of y values
	Array<double> y_;
	// Z value
	double z_;
	// Name
	QString name_;
	
	private:
	// Resize arrays
	void resize(int size);

	public:
	// Reset arrays to zero
	void reset();
	// Initialise arrays to specified size
	void initialise(int size);
	// Return current array size
	int arraySize() const;
	// Set data point 
	void setPoint(int index, double x, double y);
	// Return number of defined datapoints
	int nPoints() const;
	// Set x value
	void setX(int index, double x);
	// Add to x value
	void addX(int index, double delta);
	// Return x value specified
	double x(int index) const;
	// Return x Array
	Array<double>& arrayX();
	// Return const x Array
	const Array<double>& constArrayX() const;
	// Set y value
	void setY(int index, double y);
	// Add to y value
	void addY(int index, double delta);
	// Add to y array
	bool addY(const Array<double>& source, double factor = 1.0);
	// Multiply y value
	void multiplyY(int index, double factor);
	// Return y value specified
	double y(int index) const;
	// Return y Array
	Array<double>& arrayY();
	// Return const y Array
	const Array<double>& constArrayY() const;
	// Add new data point
	void addPoint(double x, double y);
	// Set z data
	void setZ(double z);
	// Return z data
	double z() const;
	// Add to z value
	void addZ(double value);
	// Set name of data
	void setName(QString name);
	// Return name of data
	QString name() const;


	/*!
	 * \name Operators
	 */
	public:
	// Assignment Operator
	void operator=(const Data2D& source);
	// Operator +
	Data2D operator+(const Data2D& source) const;
	// Operator +=
	void operator+=(const Data2D& source);
	// Operator +=
	void operator+=(const double dy);
	// Operator -
	Data2D operator-(const Data2D& source) const;
	// Operator -=
	void operator-=(const Data2D& source);
	// Operator -=
	void operator-=(const double dy);
	// Operator *=
	void operator*=(const double factor);
	// Operator /=
	void operator/=(const double factor);


	/*!
	 * \name Fourier Transforms
	 */
	public:
	/// Window Functions
	enum WindowFunction
	{
		NoWindow,		/**> Don't use any window */
		BartlettWindow,		/**> Bartlett (triangular) window */
		GaussianWindow,		/**> Gaussian window */
		LanczosWindow,		/**> Lanczos window */
		NuttallWindow,		/**> Nuttall window (continuous first derivatives over range */
		SineWindow,		/**> Sine Window */
		nWindowFunctions	/**> Number of defined WindowFunctions */
	};
	// Convert text string to WindowFunction
	static WindowFunction windowFunction(const char* s);
	// Covert WindowFunction to text string
	static const char* windowFunction(Data2D::WindowFunction wf);

	private:
	// Make some checks before doing transform
	bool checkBeforeTransform();
	// Return value of window function at specified position (in range 0 - 1.0)
	double window(Data2D::WindowFunction wf, double pos);

	public:
	// Perform plain Fourier transform of real data
	bool fourierTransformReal(bool forwardTransform = true, Data2D::WindowFunction wf = Data2D::NoWindow);
	// Transform g(r) to S(Q)
	bool transformRDF(double atomicDensity, Data2D::WindowFunction wf = Data2D::NoWindow);
	// Transform g(r) to S(Q), applying instrumental broadening functions
	bool transformBroadenedRDF(double atomicDensity, double qStep, double fwhm, double fwhmq, Data2D::WindowFunction wf = Data2D::NoWindow);
	// Transform S(Q) to g(r)
	bool transformSQ(double atomicDensity, Data2D::WindowFunction wf = Data2D::NoWindow);
	// Calculate S(Q) correlation function
	bool correlateSQ(double atomicDensity);


	/*!
	 * \name Spline Interpolation
	 */
	private:
	// Array of parameters for spline fit (if created)
	Array<double> splineA_, splineB_, splineC_, splineD_, splineH_;
	// Interval of last interpolated point
	int splineInterval_;
	// Whether a constrained spline fit was used
	bool constrainedSpline_;

	public:
	// Calculate natural spline interpolation of current data
	void interpolate(bool constrained = true);
	// Return spline interpolated y value for supplied x
	double interpolated(double xvalue);
	// Smooth data
	void smooth(int avgSize, int skip = 0);
	// Add interpolated data
	void addInterpolated(Data2D& source);
	// Subtract interpolated data
	void subtractInterpolated(Data2D& source);


	/*!
	 * \name General Functions
	 */
	public:
	// Return minumum x value in data
	double xMin() const;
	// Return maximum x value in data
	double xMax() const;
	// Return minumum y value in data
	double yMin() const;
	// Return maximum y value in data
	double yMax() const;
	// Compute integral of the data
	double integral();
	// Compute absolute integral of the data
	double absIntegral();
	// Apply median filter to data
	void medianFilter(int length);
	// Convolute this data with the supplied data, by products
	bool convoluteProduct(Data2D& data);
	// Trim data to X-range specified
	void trim(double minX, double maxX);
	// Calculate average y value over x range specified
	double averageY(double xMin, double xMax) const;


	/*!
	 * \name File I/O
	 */
	public:
	// Load data from specified file
	bool load(const char* fileName);
	// Save data to specified file
	bool save(const char* fileName) const;
	// Save data and interpolation to specified file
	bool saveWithInterpolation(const char* fileName);
};

#endif
