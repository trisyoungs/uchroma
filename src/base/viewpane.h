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
#include "base/collection.h"
#include "base/targetdata.h"
#include "math/matrix.h"
#include "templates/list.h"
#include "templates/reflist.h"
#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/gl3.h>
#endif
#include <QtCore/QString>

// Forward Declarations
class ViewLayout;

// ViewPane
class ViewPane : public ListItem<ViewPane>, public ObjectList<ViewPane>
{
	public:
	// Constructor / Destructor
	ViewPane(ViewLayout& parent);
	~ViewPane();
	// Copy constructor
	ViewPane(const ViewPane& source);
	// Assignment operator
	void operator=(const ViewPane& source);
	// Pane Handles
	enum PaneHandle { BottomLeftHandle, BottomMiddleHandle, BottomRightHandle, MiddleLeftHandle, MiddleRightHandle, TopLeftHandle, TopMiddleHandle, TopRightHandle, nHandles };


	/*
	 * Parent
	 */
	private:
	// Layout in which this pane exists
	ViewLayout& parent_;

	public:
	// Return parent
	ViewLayout& parent();
	// Notify parent that this pane has been changed
	void paneChanged();


	/*
	 * Position / Geometry
	 */
	private:
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
	// Aspect ratio of pane
	double aspectRatio_;
	// Viewport matrix for GL
	GLuint viewportMatrix_[4];

	public:
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
	// Move specified handle by specified amount
	void moveHandle(PaneHandle handle, int deltaX, int deltaY);
	// Return geometry that would result after moving the specified handle
	Vec4<int> geometryAfterHandleMove(PaneHandle handle, int deltaX, int deltaY);
	// Recalculate viewport matrix based on grid pixel dimensions provided
	void recalculateViewport(int gridPixelWidth, int gridPixelHeight, int nColumns, int nRows, int widthRemainder, int heightRemainder);
	// Return viewport matrix
	GLuint* viewportMatrix();
	// Return whether the specified coordinate is in this pane
	bool containsCoordinate(int mouseX, int mouseY);
	// Return whether the specified grid reference is in this pane
	bool containsGridReference(int gridX, int gridY);


	/*
	 * Role / Targets
	 */
	public:
	// Role of pane
	enum PaneRole { FitResultsRole, ExtractionRole, SliceMonitorRole, StandardRole, nPaneRoles };
	// Convert text string to PaneRole
	static PaneRole paneRole(const char* s);
	// Convert PaneRole to text string
	static const char* paneRole(PaneRole role);

	private:
	// Role of this pane
	PaneRole role_;
	// Target pane(s) for role, if relevant
	RefList<ViewPane,bool> paneTargets_;
	// Target collection(s) for role
	RefList<Collection,bool> collectionTargets_;

	public:
	// Set role of this pane
	void setRole(PaneRole role);
	// Return role of this pane
	PaneRole role();
	// Add target pane for role
	void addPaneTarget(ViewPane* pane);
	// Remove target pane for role
	void removePaneTarget(ViewPane* pane);
	// Return whether specified pane is a target
	bool paneIsTarget(ViewPane* pane);
	// Return first target pane for role
	RefListItem<ViewPane,bool>* paneTargets();
	// Add target collection for role
	void addCollectionTarget(Collection* collection);
	// Remove target collection for role
	void removeCollectionTarget(Collection* collection);
	// Return whether specified collection is a target
	bool collectionIsTarget(Collection* collection);
	// Return first target collection for role
	RefListItem<Collection,bool>* collectionTargets();
	// Process supplied Collection changed/update signal if it is relevant to this pane
	bool processUpdate(Collection* source, Collection::CollectionSignal signal);


	/*
	 * Generated/Derived Data
	 */
	private:
	// Display targets for pane
	List<TargetData> displayTargets_;

	private:
	// Add display target
	void addDisplayTarget(Collection* collection);
	// Remove display target
	void removeDisplayTarget(Collection* collection);


	/*
	 * Projection / View
	 */
	public:
	// Autoscaling for 2D plots
	enum AutoScaleMethod { NoAutoScale, ExpandingAutoScale, FullAutoScale, nAutoScaleMethods };
	// Convert text string to AutoScaleMethod
	static AutoScaleMethod autoScaleMethod(const char* s);
	// Convert AutoScaleMethod to text string
	static const char* autoScaleMethod(AutoScaleMethod scale);
	// View type
	enum ViewType { NormalView, AutoStretchedView, FlatXYView, FlatXZView, FlatYZView, LinkedView, nViewTypes };
	// Convert text string to ViewType
	static ViewType viewType(const char* s);
	// Convert ViewType to text string
	static const char* viewType(ViewType vt);

	private:
	// Autoscaling method employed
	AutoScaleMethod autoScale_;
	// Type of view to use
	ViewType viewType_;
	// Projection matrix for GL
	Matrix projectionMatrix_;
	// Whether projection has perspective
	bool hasPerspective_;
	// Field of view angle used in projectionMatrix_ when using perspective
	double perspectiveFieldOfView_;
	// View matrix for GL
	Matrix viewRotation_;
	// Current translation of view
	Vec3<double> viewTranslation_;
	// Standard zOffset for translation matrix
	const double zOffset_;

	private:
	// Return calculated projection matrix
	Matrix calculateProjectionMatrix(double zoom);
	// Update primitive
	void updatePrimitive(Collection* collection, PrimitiveList& primitive, bool forcePrimitiveUpdate = false, bool dontPopInstance = false);

	public:
	// Set autoscaling method employed
	void setAutoScale(ViewPane::AutoScaleMethod method);
	// Return autoscaling method employed
	ViewPane::AutoScaleMethod autoScale();
	// Set view type
	void setViewType(ViewPane::ViewType vt);
	// Return view type
	ViewPane::ViewType viewType();
	// Return projection matrix
	Matrix projectionMatrix();
	// Set whether this pane uses perspective
	void setHasPerspective(bool perspective);
	// Return whether this pane uses perspective
	bool hasPerspective();
	// Update view matrix
	void setViewRotation(Matrix& mat);
	// Update single column of view matrix
	void setViewRotationColumn(int column, double x, double y, double z);
	// Rotate view matrix about x and y by amounts specified
	void rotateView(double dx, double dy);
	// Return view rotation
	Matrix viewRotation();
	// Set view translation
	void setViewTranslation(double x, double y, double z);
	// Translate view matrix by amounts specified
	void translateView(double dx, double dy, double dz);
	// Return current view translation
	Vec3<double> viewTranslation();
	// Return full view matrix (rotation + translation)
	Matrix viewMatrix();
	// Project given model coordinates into world coordinates
	Vec3<double> modelToWorld(Vec3<double> modelr);
	// Project given model coordinates into screen coordinates
	Vec3<double> modelToScreen(Vec3<double> modelr);
	// Project given model coordinates into screen coordinates using supplied rotation matrix and translation vector
	Vec3<double> modelToScreen(Vec3<double> modelr, Matrix rotationMatrix, Vec3<double> translation = Vec3<double>());
	// Return z translation necessary to display coordinates supplied, assuming the identity view matrix
	double calculateRequiredZoom(double xMax, double yMax, double fraction);
	// Convert screen coordinates into model space coordinates
	Vec3<double> screenToModel(int x, int y, double z);
	// Recalculate current view parameters (e.g. for 2D, autostretched 3D etc.)
	void recalculateView();
	// Reset view matrix to face XY plane
	void resetViewMatrix();
	// Set display limits to show all available data
	void showAllData();
	// Render all data associated with this pane
	void renderData(const QGLContext* context, GLExtensions* extensions, bool forcePrimitiveUpdate = false, bool dontPopInstance = false);


	/*
	 * Axes
	 */
	private:
	// Axes for this pane
	Axes axes_;

	public:
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
	// Update current slices for all collections displayed in this pane
	void collectionsUpdateCurrentSlices(int axis, double axisValue);


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
	double labelPointSize_;
	// Font scaling for titles
	double titlePointSize_;
	// Text z scaling factor
	double textZScale_;

	private:
	// Calculate font scaling factor
	void calculateFontScaling();

	public:
	// Set current bounding box type
	void setBoundingBox(ViewPane::BoundingBox type);
	// Return current bounding box type
	ViewPane::BoundingBox boundingBox();
	// Set y intercept for plane bounding box
	void setBoundingBoxPlaneY(double value);
	// Return y intercept for plane bounding box
	double boundingBoxPlaneY();
	// Set font point size for axis value labels
	void setLabelPointSize(double value);
	// Return font point size for axis value labels
	double labelPointSize();
	// Return font point size for titles
	void setTitlePointSize(double value);
	// Return font point size for titles
	double titlePointSize();
	// Return text z scaling factor
	double textZScale();


	/*
	 * Interaction
	 */
	public:
	// Return axis title at specified coordinates (if any)
	int axisTitleAt(int screenX, int screenY);


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
