/*
	*** View Pane
	*** src/base/viewpane.h
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

#ifndef UCHROMA_VIEWPANE_H
#define UCHROMA_VIEWPANE_H

#include "base/axes.h"
#include "math/matrix.h"
#include "templates/list.h"
#include "templates/reflist.h"
#include <GL/gl.h>
#include <QtCore/QString>

// Forward Declarations
class Collection;
class ViewLayout;

// ViewPane
class ViewPane : public ListItem<ViewPane>
{
	public:
	// Constructor / Destructor
	ViewPane();
	~ViewPane();
	// Copy constructor
	ViewPane(const ViewPane& source);
	// Assignment operator
	void operator=(const ViewPane& source);


	/*
	 * Position / Geometry
	 */
	private:
	// Layout in which this pane exists
	ViewLayout* parent_;
	// Name of pane
	QString name_;
	// Top edge position (row index)
	int bottomEdge_;
	// Left edge position (column index)
	int leftEdge_;
	// Width of pane (in columns)
	int width_;
	// Height of pane (in rows)
	int height_;
	// Viewport matrix for GL
	GLuint viewportMatrix_[4];

	public:
	// Set parent layout
	void setParent(ViewLayout* parent);
	// Set name of pane
	void setName(QString name);
	// Return name of pane
	QString name();
	// Set bottom left position
	void setBottomLeft(int bottom, int left);
	// Return bottom edge of pane
	int bottomEdge();
	// Return left edge of pane
	int leftEdge();
	// Set pane size (in terms of columns and rows)
	void setSize(int w, int h);
	// Return width of pane (in columns)
	int width();
	// Return height of pane (in rows)
	int height();
	// Recalculate viewport matrix based on grid pixel dimensions provided
	void recalculateViewport(int gridPixelWidth, int gridPixelHeight, int nColumns, int nRows, int widthRemainder, int heightRemainder);
	// Return viewport matrix
	GLuint* viewportMatrix();
	// Return whether the specified coordinate is in this pane
	bool containsPoint(int mouseX, int mouseY);


	/*
	 * Projection / View
	 */
	private:
	// Projection matrix for GL
	Matrix projectionMatrix_;
	// Whether projection has perspective
	bool hasPerspective_;
	// Field of view angle used in projectionMatrix_ when using perspective
	double perspectiveFieldOfView_;
	// View matrix for GL
	Matrix viewMatrix_;
	// View matrix inverse
	Matrix viewMatrixInverse_;

	private:
	// Return calculated projection matrix
	Matrix calculateProjectionMatrix(double zoom);

	public:
	// Return projection matrix
	Matrix projectionMatrix();
	// Set whether this pane uses perspective
	void setHasPerspective(bool perspective);
	// Return whether this pane uses perspective
	bool hasPerspective();
	// Update view matrix
	void setViewMatrix(Matrix& mat);
	// Update single column of view matrix
	void setViewMatrixColumn(int column, double x, double y, double z, double w);
	// Rotate view matrix about x and y by amounts specified
	void rotateView(double dx, double dy);
	// Translate view matrix by amounts specified
	void translateView(double dx, double dy, double dz);
	// Return view matrix
	Matrix viewMatrix();
	// Project given model coordinates into world coordinates
	Vec3<double> modelToWorld(Vec3<double> modelr);
	// Project given model coordinates into screen coordinates
	Vec4<double> modelToScreen(Vec3<double> modelr, double screenradius = -1.0);
	// Return zoom level, assuming orthogonally-aligned view matrix, to display coordinates supplied
	double calculateRequiredZoom(double xExtent, double yExtent, double fraction);
	// Convert screen coordinates into model space coordinates
	Vec3<double> screenToModel(int x, int y, double z);
	// Reset current view
	void resetView();
	// Set display limits to show all available data
	void showAllData();


	/*
	 * Collections / Axes
	 */
	private:
	// Reference list of collections displayed in this pane
	RefList<Collection,bool> collections_;
	// Axes for this pane
	Axes axes_;

	public:
	// Add reference to collection to be displayed in this pane
	void addCollection(Collection* collection);
	// Remove reference to collection
	void removeCollection(Collection* collection);
	// Return first collection reference
	RefListItem<Collection,bool>* collections();
	// Flag all collections for updating
	void flagCollectionDataInvalid();
	// Return absolute minimum transformed values over all associated collections
	Vec3<double> transformedDataMinima();
	// Return absolute maximum transformed values over all associated collections
	Vec3<double> transformedDataMaxima();
	// Return absolute minimum positive transformed values over all associated collections
	Vec3<double> transformedDataPositiveMinima();
	// Return absolute maximum positive transformed values over all associated collections
	Vec3<double> transformedDataPositiveMaxima();
	// Return axes for this pane
	Axes& axes();
	// Update axis limits to represent data extent of associated collections
	void updateAxisLimits();


	/*
	 * Style
	 */
	public:
	// Available Bounding Boxes
	enum BoundingBox { NoBox, PlaneBox, CubeBox, nBoundingBoxes };

	private:
	// Current bounding box type
	BoundingBox boundingBox_;
	// Y-intercept of bounding XZ plane
	double boundingBoxPlaneY_;
	// Font scaling for axis value labels
	double labelScale_;
	// Font scaling for titles
	double titleScale_;

	public:
	// Set current bounding box type
	void setBoundingBox(ViewPane::BoundingBox type);
	// Return current bounding box type
	ViewPane::BoundingBox boundingBox();
	// Set y intercept for plane bounding box
	void setBoundingBoxPlaneY(double value);
	// Return y intercept for plane bounding box
	double boundingBoxPlaneY();
	// Set font scaling for axis value labels
	void setLabelScale(double value);
	// Return font scaling for axis value labels
	double labelScale();
	// Return font scaling for titles
	void setTitleScale(double value);
	// Return font scaling for titles
	double titleScale();


	/*
	 * GL
	 */
	private:
	// Display primitives
	Primitive interactionPrimitive_, interactionBoxPrimitive_, boundingBoxPrimitive_;

	public:
	// Create bounding box
	void createBoundingBox(int type, double planeY);
	// Update interaction primitive
	void updateInteractionPrimitive(int axis);
	// Return interaction primitive
	Primitive& interactionPrimitive();
	// Return interaction box primitive
	Primitive& interactionBoxPrimitive();
	// Return bounding box primitive
	Primitive& boundingBoxPrimitive();

};

#endif