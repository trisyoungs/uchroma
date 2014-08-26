/*
	*** View Pane
	*** src/base/viewpane.cpp
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

#include "base/viewpane.h"
#include "base/collection.h"
#include "base/viewlayout.h"
#include <algorithm>

// Constructor
ViewPane::ViewPane(ViewLayout& parent) : ListItem<ViewPane>(), parent_(parent), axes_(*this)
{
	// Geometry / position
	bottomEdge_ = 0;
	leftEdge_ = 0;
	height_ = 0;
	width_ = 0;
	aspectRatio_ = 1.0;

	// Projection / view	
	hasPerspective_ = false;
	perspectiveFieldOfView_ = 20.0;
	viewportMatrix_[0] = 0;
	viewportMatrix_[1] = 0;
	viewportMatrix_[2] = 0;
	viewportMatrix_[3] = 0;
	viewMatrix_[14] = -5.0;

	// Role
	role_ = ViewPane::StandardRole;
	twoDimensional_ = false;
	autoStretch3D_ = true;

	// Style
	boundingBox_ = ViewPane::NoBox;
	boundingBoxPlaneY_ = 0.0;
	labelPointSize_ = 10.0;
	titlePointSize_ = 12.0;
	textZScale_ = -1.0;

	// GL
	interactionPrimitive_.setNoInstances();
	interactionBoxPrimitive_.setNoInstances();
	boundingBoxPrimitive_.setNoInstances();
}

// Destructor
ViewPane::~ViewPane()
{
	// Notify all associated collections that we no longer exist
	for (RefListItem<Collection,bool>* ri = collections_.first(); ri != NULL; ri = ri->next) ri->item->setDisplayPane(NULL);
}

// Copy constructor
ViewPane::ViewPane(const ViewPane& source) : parent_(parent_), axes_(*this)
{
	(*this) = source;
}

// Assignment operator
void ViewPane::operator=(const ViewPane& source)
{
	// Geometry / position
	bottomEdge_ = source.bottomEdge_;
	leftEdge_ = source.leftEdge_;
	height_ = source.height_;
	width_ = source.width_;

	// Projection / view	
	hasPerspective_ = source.hasPerspective_;
	perspectiveFieldOfView_ = source.perspectiveFieldOfView_;
	viewportMatrix_[0] = source.viewportMatrix_[0];
	viewportMatrix_[1] = source.viewportMatrix_[1];
	viewportMatrix_[2] = source.viewportMatrix_[2];
	viewportMatrix_[3] = source.viewportMatrix_[3];
	viewMatrix_ = source.viewMatrix_;

	// Axes
	axes_ = source.axes_;

	// Style
	boundingBox_ = source.boundingBox_;
	boundingBoxPlaneY_ = source.boundingBoxPlaneY_;
	labelPointSize_ = source.labelPointSize_;
	titlePointSize_ = source.titlePointSize_;
}

/*
 * Parent
 */

// Set as modified (call parent routine)
void ViewPane::paneChanged()
{
	// Pass the modification notification upwards
	parent_.paneChanged(this);
}

/*
 * Position / Geometry
 */

// Set name of pane
void ViewPane::setName(QString name)
{
	name_ = name;

	paneChanged();
}

// Return name of pane
QString ViewPane::name()
{
	return name_;
}

// Set bottom left position
void ViewPane::setBottomLeft(int bottom, int left)
{
	bottomEdge_ = bottom;
	leftEdge_ = left;

	paneChanged();
}

// Return bottom edge of pane
int ViewPane::bottomEdge()
{
	return bottomEdge_;
}

// Return left edge of pane
int ViewPane::leftEdge()
{
	return leftEdge_;
}

// Set pane size (in terms of columns and rows)
void ViewPane::setSize(int w, int h)
{
	width_ = w;
	height_ = h;

	paneChanged();
}

// Return width of pane (in columns)
int ViewPane::width()
{
	return width_;
}

// Return height of pane (in rows)
int ViewPane::height()
{
	return height_;
}

// Move specified handle by specified amount
void ViewPane::moveHandle(PaneHandle handle, int deltaX, int deltaY)
{
	Vec4<int> newGeometry = geometryAfterHandleMove(handle, deltaX, deltaY);
	leftEdge_ = newGeometry.x;
	bottomEdge_ = newGeometry.y;
	width_ = newGeometry.z;
	height_ = newGeometry.w;

	paneChanged();
}

// Return geometry that would result after moving the specified handle
Vec4<int> ViewPane::geometryAfterHandleMove(PaneHandle handle, int deltaX, int deltaY)
{
	// Result contains leftEdge_, bottomEdge_, width_, height_
	Vec4<int> result(leftEdge_, bottomEdge_, width_, height_);
	switch (handle)
	{
		case (ViewPane::BottomLeftHandle):
			if ((deltaX < 0) || ((result.z - deltaX) > 0)) { result.x += deltaX; result.z -= deltaX; }
			if ((deltaY < 0) || ((result.w - deltaY) > 0)) { result.y += deltaY; result.w -= deltaY; }
			break;
		case (ViewPane::BottomMiddleHandle):
			if ((deltaY < 0) || ((result.w - deltaY) > 0)) { result.y += deltaY; result.w -= deltaY; }
			break;
		case (ViewPane::BottomRightHandle):
			if ((deltaX > 0) || ((result.z + deltaX) > 0)) result.z += deltaX;
			if ((deltaY < 0) || ((result.w - deltaY) > 0)) { result.y += deltaY; result.w -= deltaY; }
			break;
		case (ViewPane::MiddleLeftHandle):
			if ((deltaX < 0) || ((result.z - deltaX) > 0)) { result.x += deltaX; result.z -= deltaX; }
			break;
		case (ViewPane::MiddleRightHandle):
			if ((deltaX > 0) || ((result.z + deltaX) > 0)) result.z += deltaX;
			break;
		case (ViewPane::TopLeftHandle):
			if ((deltaX < 0) || ((result.z - deltaX) > 0)) { result.x += deltaX; result.z -= deltaX; }
			if ((deltaY > 0) || ((result.w + deltaY) > 0)) result.w += deltaY;
			break;
		case (ViewPane::TopMiddleHandle):
			if ((deltaY > 0) || ((result.w + deltaY) > 0)) result.w += deltaY;
			break;
		case (ViewPane::TopRightHandle):
			if ((deltaX > 0) || ((result.z + deltaX) > 0)) result.z += deltaX;
			if ((deltaY > 0) || ((result.w + deltaY) > 0)) result.w += deltaY;
			break;
	}

	return result;
}

// Recalculate viewport matrix based on grid pixel dimensions provided
void ViewPane::recalculateViewport(int gridPixelWidth, int gridPixelHeight, int nColumns, int nRows, int widthRemainder, int heightRemainder)
{
	viewportMatrix_[0] = gridPixelWidth * leftEdge_;
	viewportMatrix_[1] = gridPixelHeight * bottomEdge_;
	viewportMatrix_[2] = gridPixelWidth * width_;
	viewportMatrix_[3] = gridPixelHeight * height_;
	aspectRatio_ = double(viewportMatrix_[2]) / double(viewportMatrix_[3]);

	// Add on the remainder if the top or right edges are extreme
	if ((leftEdge_+width_) == nColumns) viewportMatrix_[2] += widthRemainder;
	if ((bottomEdge_+height_) == nRows) viewportMatrix_[3] += heightRemainder;

	// Recalculate projection matrix and view matrix (if 2D)
	projectionMatrix_ = calculateProjectionMatrix(viewMatrix_[14]);
	resetView();
}

// Return viewport matrix
GLuint* ViewPane::viewportMatrix()
{
	return viewportMatrix_;
}

// Return whether the specified coordinate is in this pane
bool ViewPane::containsCoordinate(int layoutX, int layoutY)
{
	if (layoutX < viewportMatrix_[0]) return false;
	if (layoutX > (viewportMatrix_[0]+viewportMatrix_[2])) return false;
	if (layoutY < viewportMatrix_[1]) return false;
	if (layoutY > (viewportMatrix_[1]+viewportMatrix_[3])) return false;
	return true;
}

// Return whether the specified grid reference is in this pane
bool ViewPane::containsGridReference(int gridX, int gridY)
{
	if (gridX < leftEdge_) return false;
	if (gridX >= (leftEdge_+width_)) return false;
	if (gridY < bottomEdge_) return false;
	if (gridY >= (bottomEdge_+height_)) return false;
	return true;
}

/*
 * Role
 */

// Role of pane
const char* RoleKeywords[ViewPane::nPaneRoles] = { "FitResults", "Extraction", "SliceMonitor", "Standard" };

// Convert text string to PaneRole
ViewPane::PaneRole ViewPane::paneRole(const char* s)
{
	for (int n=0; n<ViewPane::nPaneRoles; ++n) if (strcmp(s,RoleKeywords[n]) == 0) return (ViewPane::PaneRole) n;
	return ViewPane::nPaneRoles;
}

// Convert InputBlock to text string
const char* ViewPane::paneRole(ViewPane::PaneRole role)
{
	return RoleKeywords[role];
}

// AutoScale methods
const char* AutoScaleKeywords[ViewPane::nAutoScaleMethods] = { "None", "Expanding", "Full" };

// Convert text string to AutoScale2D
ViewPane::AutoScaleMethod ViewPane::autoScaleMethod(const char* s)
{
	for (int n=0; n<ViewPane::nAutoScaleMethods; ++n) if (strcmp(s,AutoScaleKeywords[n]) == 0) return (ViewPane::AutoScaleMethod) n;
	return ViewPane::nAutoScaleMethods;
}

// Convert InputBlock to text string
const char* ViewPane::autoScaleMethod(ViewPane::AutoScaleMethod scale)
{
	return AutoScaleKeywords[scale];
}

// Set role of this pane
void ViewPane::setRole(ViewPane::PaneRole role)
{
	role_ = role;

	paneChanged();
}

// Return role of this pane
ViewPane::PaneRole ViewPane::role()
{
	return role_;
}

// Set autoscaling method employed
void ViewPane::setAutoScale(ViewPane::AutoScaleMethod method)
{
	autoScale_ = method;

	paneChanged();
}

// Return autoscaling method employed
ViewPane::AutoScaleMethod ViewPane::autoScale()
{
	return autoScale_;
}

// Set whether this pane is a 2D plot
void ViewPane::setTwoDimensional(bool b)
{
	twoDimensional_ = b;

	// Recalculate viewmatrix
	resetView();

	paneChanged();
}

// Return whether this pane is a 2D plot
bool ViewPane::twoDimensional()
{
	return twoDimensional_;
}

// Set whether autostretching of 3D axes is enabled for this pane
void ViewPane::setAutoStretch3D(bool b)
{
	autoStretch3D_ = b;
	
	paneChanged();
}

// Return whether autostretching of 3D axes is enabled for this pane
bool ViewPane::autoStretch3D()
{
	return autoStretch3D_;
}

// Add target pane for role
void ViewPane::addRoleTargetPane(ViewPane* pane)
{
	roleTargetPanes_.add(pane);

	paneChanged();
}

// Remove target pane for role
void ViewPane::removeRoleTargetPane(ViewPane* pane)
{
	roleTargetPanes_.remove(pane);

	paneChanged();
}

// Return whether specified pane is a target
bool ViewPane::roleIsTargetPane(ViewPane* pane)
{
	return roleTargetPanes_.contains(pane);
}

// Return target panes for role
RefListItem<ViewPane,bool>* ViewPane::roleTargetPanes()
{
	return roleTargetPanes_.first();
}

// Add target collection for role
void ViewPane::addRoleTargetCollection(Collection* collection)
{
	RefListItem<Collection,TargetData>* ri = roleTargetCollections_.add(collection);
	ri->data.setParent(this);

	paneChanged();
}

// Remove target collection for role
void ViewPane::removeRoleTargetCollection(Collection* collection)
{
	roleTargetCollections_.remove(collection);

	paneChanged();
}

// Return whether specified collection is a target
RefListItem<Collection,TargetData>* ViewPane::roleIsTargetCollection(Collection* collection)
{
	return roleTargetCollections_.contains(collection);
}

// Return target collections for role
RefListItem<Collection,TargetData>* ViewPane::roleTargetCollections()
{
	return roleTargetCollections_.first();
}

// Process supplied Collection changed/update signal if it is relevant to this pane
bool ViewPane::processUpdate(Collection* source, Collection::CollectionSignal signal)
{
	RefListItem<Collection,TargetData>* ri;
	Collection* collection;
	switch (signal)
	{
		// Collection Created
		// -- Role Relevance : ???
		case (Collection::CollectionCreatedSignal):
			return false;
			break;
		// Collection Deleted
		// -- Role Relevance : All
		case (Collection::CollectionDeletedSignal):
			// If the emitting collection is a current target, remove it from our list
			if (!roleIsTargetCollection(source)) return false;
			removeRoleTargetCollection(source);
			break;
		// Current Slice Changed
		// -- Role Relevance : SliceMonitorRole
		case (Collection::CurrentSliceChangedSignal):
			if (role_ != ViewPane::SliceMonitorRole) return false;
			ri = roleIsTargetCollection(source);
			if (!ri) return false;
			// Grab slice storage and copy over data 
			collection = ri->data.addCollectionData(TargetData::SliceData);
			collection->clearDataSets();
			collection->copyDataSets(source->currentSlice());
			// Update axes limits if autoscaling?????? XXX TODO
			break;
		// Data Changed
		// -- Role Relevance : ???
		case (Collection::DataChangedSignal):
			return false;
			break;
		// Extracted Data Added
		// -- Role Relevance : ???
		case (Collection::ExtractedDataAddedSignal):
			return false;
			break;
		default:
			msg.print("ViewPane::processUpdate - Unrecognised signal %i sent from collection '%s'\n", signal, qPrintable(source->title()));
			return false;
	}

	return true;
}

/*
 * Projection / View
 */

// Return calculated projection matrix
Matrix ViewPane::calculateProjectionMatrix(double orthoZoom)
{
	Matrix result;

	GLdouble top, bottom, right, left;
	GLdouble nearClip = 0.5, farClip = 2000.0;

	if (hasPerspective_ && (!twoDimensional_))
	{
		// Use reversed top and bottom values so we get y-axis (0,1,0) pointing up
		top = tan(perspectiveFieldOfView_ / DEGRAD) * 0.5;
		bottom = -top;
		left = -aspectRatio_*top;
		right = aspectRatio_*top;
		result.setColumn(0, (nearClip*2.0) / (right-left), 0.0, 0.0, 0.0);
		result.setColumn(1, 0.0, (nearClip*2.0) / (top-bottom), 0.0, 0.0);
		result.setColumn(2, (right+left)/(right-left), (top+bottom)/(top-bottom), -(farClip+nearClip)/(farClip-nearClip), -1.0);
		result.setColumn(3, 0.0, 0.0, -(2.0*nearClip*farClip) / (farClip-nearClip), 0.0);
		// Equivalent to the following code:
		// glMatrixMode(GL_PROJECTION);
		// glLoadIdentity();
		// top = tan(prefs.perspectiveFov() / DEGRAD) * prefs.clipNear();
		// bottom = -top;
		// glFrustum(aspect*bottom, aspect*top, bottom, top, prefs.clipNear(), prefs.clipFar());
		// glGetDoublev(GL_PROJECTION_MATRIX, modelProjectionMatrix_.matrix());
	}
	else
	{
		top = -tan(perspectiveFieldOfView_ / DEGRAD) * orthoZoom;
		bottom = -top;
		left = -aspectRatio_*top;
		right = aspectRatio_*top;

		result.setColumn(0, 2.0 / (right-left), 0.0, 0.0, (right+left)/(right-left));
		result.setColumn(1, 0.0, 2.0 / (top-bottom), 0.0, (top+bottom)/(top-bottom));
		result.setColumn(2, 0.0, 0.0, -1.0/farClip, 0.0);
		result.setColumn(3, 0.0, 0.0, 0.0, 1.0);
		// Equivalent to the following code:
		// glMatrixMode(GL_PROJECTION);
		// glLoadIdentity();
		// top = tan(prefs.perspectiveFov() / DEGRAD) * prefs.clipNear();
		// bottom = -top;
		// glOrtho(aspect*top, aspect*bottom, top, bottom, -prefs.clipFar(), prefs.clipFar());
		// glGetDoublev(GL_PROJECTION_MATRIX, modelProjectionMatrix_.matrix());
	}

	return result;
}

// Return projection matrix
Matrix ViewPane::projectionMatrix()
{
	return projectionMatrix_;
}

// Set whether this pane uses perspective
void ViewPane::setHasPerspective(bool perspective)
{
	hasPerspective_ = perspective;

	projectionMatrix_ = calculateProjectionMatrix(viewMatrix_[14]);
}

// Return whether this pane uses perspective
bool ViewPane::hasPerspective()
{
	return hasPerspective_;
}

// Update transformation (view) matrix
void ViewPane::setViewMatrix(Matrix &mat)
{
	// If this is a two-dimensional graph, ignore the request
	if (twoDimensional_) return;

	// Projection matrix needs to be updated if the zoom value has changed and we are *not* using perspective
	if ((!hasPerspective_) && (fabs(mat[14] - viewMatrix_[14]) > 0.01)) projectionMatrix_ = calculateProjectionMatrix(mat[14]);

	viewMatrix_ = mat;
}

// Update single column of view matrix
void ViewPane::setViewMatrixColumn(int column, double x, double y, double z, double w)
{
	// If this is a two-dimensional graph, ignore the request
	if (twoDimensional_) return;

	// Projection matrix needs to be updated if the zoom value has changed and we are *not* using perspective
	if ((!hasPerspective_) && (column == 3) && (fabs(z - viewMatrix_[14]) > 0.01)) projectionMatrix_ = calculateProjectionMatrix(z);
	
	viewMatrix_.setColumn(column, x, y, z, w);
}

// Rotate view matrix about x and y by amounts specified
void ViewPane::rotateView(double dx, double dy)
{
	// If this is a two-dimensional graph, ignore the request
	if (twoDimensional_) return;

	Matrix A;
	A.createRotationXY(dx, dy);
	A.copyTranslationAndScaling(viewMatrix_);
	viewMatrix_.removeTranslationAndScaling();
	viewMatrix_ = A * viewMatrix_;
}

// Translate view matrix by amounts specified
void ViewPane::translateView(double dx, double dy, double dz)
{
	// If this is a two-dimensional graph, ignore the request
	if (twoDimensional_) return;

	viewMatrix_.adjustColumn(3, dx, dy, dz, 0.0);
	if ((!hasPerspective_) && (fabs(dz) > 1.0e-4)) projectionMatrix_ = calculateProjectionMatrix(viewMatrix_[14]);
}

// Return view matrix
Matrix ViewPane::viewMatrix()
{
	return viewMatrix_;
}

// Project given model coordinates into world coordinates
Vec3<double> ViewPane::modelToWorld(Vec3< double > modelr)
{
	msg.enter("Viewer::modelToWorld");
	Vec3<double> worldr;
	Matrix vmat;
	Vec4<double> pos, temp;

	// Projection formula is : worldr = P x M x modelr
	pos.set(modelr, 1.0);
	// Get the world coordinates of the atom - Multiply by modelview matrix 'view'
	vmat = viewMatrix_;
//	vmat.applyTranslation(-cell_.centre().x, -cell_.centre().y, -cell_.centre().z);
	temp = vmat * pos;
	worldr.set(temp.x, temp.y, temp.z);
	msg.exit("Viewer::modelToWorld");
	return worldr;
}

// Project given model coordinates into screen coordinates
Vec4<double> ViewPane::modelToScreen(Vec3<double> modelr, double screenradius)
{
	msg.enter("Viewer::modelToScreen");
	Vec4<double> screenr, tempscreen;
	Vec4<double> worldr;
	Matrix vmat;
	Vec4<double> pos;

	// Projection formula is : worldr = P x M x modelr
	pos.set(modelr, 1.0);

	// Get the world coordinates of the point - Multiply by modelview matrix 'view'
	vmat = viewMatrix_;
	vmat.applyTranslation(-axes_.axesCoordCentre());
	worldr = vmat * pos;

	screenr = projectionMatrix_ * worldr;
	screenr.x /= screenr.w;
	screenr.y /= screenr.w;
	screenr.x = viewportMatrix_[0] + viewportMatrix_[2]*(screenr.x+1)*0.5;
	screenr.y = viewportMatrix_[1] + viewportMatrix_[3]*(screenr.y+1)*0.5;
	screenr.z = screenr.z / screenr.w;

	// Calculate 2D 'radius' around the point - Multiply world[x+delta] coordinates by P
	if (screenradius > 0.0)
	{
		worldr.x += screenradius;
		tempscreen = projectionMatrix_ * worldr;
		tempscreen.x /= tempscreen.w;
		screenr.w = fabs( (viewportMatrix_[0] + viewportMatrix_[2]*(tempscreen.x+1)*0.5) - screenr.x);
	}
	msg.exit("Viewer::modelToScreen");
	return screenr;
}

// Return zoom level, assuming orthogonally-aligned view matrix, to display coordinates supplied
double ViewPane::calculateRequiredZoom(double xExtent, double yExtent, double fraction)
{
	// The supplied x and y extents should indicate the number of units in those directions
	// from the origin that are to be displaye on-screen. The 'fraction' indicates how much of the
	// available range on-screen to use, allowing a margin to be added. A value of '1.0' would
	// put the extent with the highest units on the very edge of the display.

	Vec4<double> rScreen, rProjected;
	Vec4<double> worldr;
	Vec4<double> pos;

	// Sanity check
	if (viewportMatrix_[2] == 0) return 1.0;
	if (viewportMatrix_[3] == 0) return 1.0;

	// Calculate target screen coordinate
	int targetX = (1.0 + fraction) * viewportMatrix_[2] * 0.5;
	int targetY = (1.0 + fraction) * viewportMatrix_[3] * 0.5;
	int screenX = targetX, screenY = targetY;

	double zoom = 0.0;
	int count = 0;
	do
	{
		// Increase zoom distance
		zoom -= std::max( std::max(screenX / targetX, screenY / targetY), 1);

		// Get the world coordinates of the point - viewmatrix is assumed to be the identity matrix, plus the zoom factor in array index [14]
		worldr.x = xExtent;
		worldr.y = yExtent;
		worldr.z = 0.0;

		// Get projection matrix for this zoom level (orthogonal projection only)
		Matrix projectionMatrix = calculateProjectionMatrix(zoom);

// 		rProjected = projectionMatrix * worldr;

		// Multiply by view matrix to get screen coordinates
// 		rScreen.x = viewportMatrix_[0] + viewportMatrix_[2]*((rProjected.x / rProjected.w)+1)*0.5;
// 		rScreen.y = viewportMatrix_[1] + viewportMatrix_[3]*((rProjected.y / rProjected.w)+1)*0.5;

		// Calculate screen X coordinate
		screenX = viewportMatrix_[0] + viewportMatrix_[2]*((
			(worldr.x * projectionMatrix[0] + worldr.z * projectionMatrix[8])
			/ 
			(worldr.x * projectionMatrix[3] + worldr.z * projectionMatrix[11] + projectionMatrix[15])
			)+1)*0.5;
		screenY = viewportMatrix_[1] + viewportMatrix_[3]*((
			(worldr.y * projectionMatrix[5] + worldr.z * projectionMatrix[8])
			/ 
			(worldr.y * projectionMatrix[7] + worldr.z * projectionMatrix[11] + projectionMatrix[15])
			)+1)*0.5;

		// Limit the number of iterations so we can never get into an infinite loop
		if (++count == 20) break;

	} while ((screenX > targetX) || (screenY > targetY));
	return zoom;
}

// Convert screen coordinates into model space coordinates
Vec3<double> ViewPane::screenToModel(int x, int y, double z)
{
	msg.enter("Viewer::screenToModel");
	static Vec3<double> modelr;
	Vec4<double> temp, worldr;
	int newx, newy;
	double dx, dy;

	// Grab transformation matrix, apply translation correction, and invert
	Matrix itransform = viewMatrix_;
//	itransform.applyTranslation(-cell_.centre().x, -cell_.centre().y, -cell_.centre().z);
	itransform.invert();

	// Mirror y-coordinate
	y = viewportMatrix_[3] - y;

	// Project points at guide z-position and two other points along literal x and y to get scaling factors for screen coordinates
	worldr.set(0.0,0.0,z, 1.0);
	temp = projectionMatrix_ * worldr;
	newx = viewportMatrix_[0] + viewportMatrix_[2]*(temp.x / temp.w + 1.0)*0.5;
	newy = viewportMatrix_[1] + viewportMatrix_[3]*(temp.y / temp.w + 1.0)*0.5;

	for (int n=0; n<10; ++n)
	{
		// Determine new (better) coordinate from a yardstick centred at current world coordinates
		temp = projectionMatrix_ * Vec4<double>(worldr.x+1.0, worldr.y+1.0, worldr.z, worldr.w);
		dx = viewportMatrix_[0] + viewportMatrix_[2]*(temp.x / temp.w + 1.0)*0.5 - newx;
		dy = viewportMatrix_[1] + viewportMatrix_[3]*(temp.y / temp.w + 1.0)*0.5 - newy;

		worldr.add((x-newx)/dx, (y-newy)/dy, 0.0, 0.0);
// 		printf ("N=%i", n); worldr.print();
		temp = projectionMatrix_ * worldr;
		newx = viewportMatrix_[0] + viewportMatrix_[2]*(temp.x / temp.w + 1.0)*0.5;
		newy = viewportMatrix_[1] + viewportMatrix_[3]*(temp.y / temp.w + 1.0)*0.5;
// 		printf("NEW dx = %f, dy = %f, wantedxy = %f, %f\n", newx, newy, x, y);
		if ((x == newx) && (y == newy)) break;
	}

	// Finally, invert to model coordinates
	modelr = itransform * Vec3<double>(worldr.x, worldr.y, worldr.z);

	msg.exit("Viewer::screenToModel");
	return modelr;
}

// Reset view
void ViewPane::resetView()
{
	Vec4<double> unit;

	// Reset the current view matrix
	viewMatrix_.setIdentity();

	// Calculate text scaling factor
	viewMatrix_[14] = -10.0;
	projectionMatrix_ = calculateProjectionMatrix(viewMatrix_[14]);
	unit = modelToScreen(Vec3<double>(0.0, 1.0+axes_.axesCoordCentre().y, 0.0));
	unit.y -= viewportMatrix_[1] + viewportMatrix_[3]*0.5;
	textZScale_ = unit.y*viewMatrix_[14];
	
	if (twoDimensional_)
	{
		// Set a 'default' zoom for the 2D view
		viewMatrix_[14] = -10.0;

		// Project a point one unit each along X and Y, accounting for the axes global centre
		unit = modelToScreen(Vec3<double>(1.0+axes_.axesCoordCentre().x, 1.0+axes_.axesCoordCentre().y,0.0));

		// Subtract off the viewpoint centre coordinate in order to get literal 'pixels per unit' for X and Y axes range
		unit.x -= viewportMatrix_[0] + viewportMatrix_[2]/2.0;
		unit.y -= viewportMatrix_[1] + viewportMatrix_[3]/2.0;

		// Setup some variables
		// -- Margins (in pixels)
		int marginLeft = 50, marginRight = 10;
		int marginTop = 10, marginBottom = (labelPointSize_ + titlePointSize_) * 1.5;
		// -- Available width for axes, accounting for margins
		int availableWidth = viewportMatrix_[2] - (marginLeft+marginRight);
		int availableHeight = viewportMatrix_[3] - (marginTop+marginBottom);

		// Set scaling factors so that axes reach extreme coordinates
		viewMatrix_[0] = availableWidth / (unit.x * axes_.axisRange(0));
		viewMatrix_[5] = availableHeight / (unit.y * axes_.axisRange(1));

		// Set a translation in order to set the margins as requested
		viewMatrix_[12] = 0.5 * (marginLeft - marginRight) / unit.x;
		viewMatrix_[13] = 0.5 * (marginBottom - marginTop) / unit.y;
	}
	else
	{
		viewMatrix_[14] = calculateRequiredZoom((axes_.axisMax(0) - axes_.axisMin(0))*0.5, (axes_.axisMax(1) - axes_.axisMin(1))*0.5, 0.9);
	}

	// Recalculate projection matrix
	projectionMatrix_ = calculateProjectionMatrix(viewMatrix_[14]);
}

// Set display limits to show all available data
void ViewPane::showAllData()
{
	updateAxisLimits();

	for (int axis = 0; axis < 3; ++axis)
	{
		axes_.setAxisToLimit(axis, true);
		axes_.setAxisToLimit(axis, false);
	}

	// Flag that all surfaces and axes should be regenerated (data and primitive)
	flagCollectionDataInvalid();
}

/*
 * Collections / Axes
 */

// Add reference to collection to be displayed in this pane
void ViewPane::addCollection(Collection* collection)
{
	if (collections_.contains(collection)) msg.print("Internal Error: Pane '%s' already contains collection '%s'\n", qPrintable(name_), qPrintable(collection->title()));
	else collections_.add(collection);

	// Tell the collection that we now are displaying it
	collection->setDisplayPane(this);

	// Update data limits for axes
	updateAxisLimits();
}

// Remove reference to collection
void ViewPane::removeCollection(Collection* collection)
{
	if (!collections_.contains(collection)) msg.print("Internal Error: Pane '%s' does not contain collection '%s'\n", qPrintable(name_), qPrintable(collection->title()));
	else collections_.remove(collection);
}

// Return first collection reference
RefListItem<Collection,bool>* ViewPane::collections()
{
	return collections_.first();
}

// Flag all collections for updating
void ViewPane::flagCollectionDataInvalid()
{
	for (RefListItem<Collection,bool>* ri = collections_.first(); ri != NULL; ri = ri->next) ri->item->setDisplayDataInvalid();
}

// Return absolute minimum transformed values over all associated collections
Vec3<double> ViewPane::transformedDataMinima()
{
	if (collections_.nItems() == 0) return Vec3<double>(0.0,0.0,0.0);

	// Set starting values from first collection
	Vec3<double> v, minima = collections_.first()->item->transformMin();
	for (RefListItem<Collection,bool>* ri = collections_.first()->next; ri != NULL; ri = ri->next)
	{
		v =  ri->item->transformMin();
		if (v.x < minima.x) minima.x = v.x;
		if (v.y < minima.y) minima.y = v.y;
		if (v.z < minima.z) minima.z = v.z;
	}
	return minima;
}

// Return absolute maximum transformed values over all associated collections
Vec3<double> ViewPane::transformedDataMaxima()
{
	if (collections_.nItems() == 0) return Vec3<double>(10.0,10.0,10.0);

	// Set starting values from first collection
	Vec3<double> v, maxima = collections_.first()->item->transformMax();
	for (RefListItem<Collection,bool>* ri = collections_.first()->next; ri != NULL; ri = ri->next)
	{
		v =  ri->item->transformMax();
		if (v.x > maxima.x) maxima.x = v.x;
		if (v.y > maxima.y) maxima.y = v.y;
		if (v.z > maxima.z) maxima.z = v.z;
	}
	return maxima;
}

// Return absolute minimum positive transformed values over all associated collections
Vec3<double> ViewPane::transformedDataPositiveMinima()
{
	if (collections_.nItems() == 0) return Vec3<double>(0.1,0.1,0.1);

	// Set starting values from first collection
	Vec3<double> v, minima = collections_.first()->item->transformMinPositive();
	for (RefListItem<Collection,bool>* ri = collections_.first()->next; ri != NULL; ri = ri->next)
	{
		v =  ri->item->transformMinPositive();
		if (v.x < minima.x) minima.x = v.x;
		if (v.y < minima.y) minima.y = v.y;
		if (v.z < minima.z) minima.z = v.z;
	}
	return minima;
}

// Return absolute maximum positive transformed values over all associated collections
Vec3<double> ViewPane::transformedDataPositiveMaxima()
{
	if (collections_.nItems() == 0) return Vec3<double>(10.0, 10.0, 10.0);

	// Set starting values from first collection
	Vec3<double> v, maxima = collections_.first()->item->transformMaxPositive();
	for (RefListItem<Collection,bool>* ri = collections_.first()->next; ri != NULL; ri = ri->next)
	{
		v = ri->item->transformMaxPositive();
		if (v.x > maxima.x) maxima.x = v.x;
		if (v.y > maxima.y) maxima.y = v.y;
		if (v.z > maxima.z) maxima.z = v.z;
	}
	return maxima;
}

// Return axes for this pane
Axes& ViewPane::axes()
{
	return axes_;
}

// Update axis limits to represent data extent of associated collections
void ViewPane::updateAxisLimits()
{
	// Get transformed data extents
	Vec3<double> dataMin = transformedDataMinima();
	Vec3<double> dataMax = transformedDataMaxima();
	Vec3<double> dataMinPositive = transformedDataPositiveMinima();
	Vec3<double> dataMaxPositive = transformedDataPositiveMaxima();

	// Loop over axes
	for (int axis = 0; axis < 3; ++axis)
	{
		// Set allowable range to avoid negative numbers if axis is now logarithmic
		if (axes_.axisLogarithmic(axis))
		{
			axes_.setAxisLimitMin(axis, dataMinPositive[axis]);
			axes_.setAxisLimitMax(axis, dataMaxPositive[axis]);
		}
		else
		{
			axes_.setAxisLimitMin(axis, dataMin[axis]);
			axes_.setAxisLimitMax(axis, dataMax[axis]);
		}
	}
}

// Update current slices for all collections displayed in this pane
void ViewPane::collectionsUpdateCurrentSlices(int axis, double axisValue)
{
	for (RefListItem<Collection,bool>* ri = collections_.first(); ri != NULL; ri = ri->next) ri->item->updateCurrentSlice(axis, axisValue);
}

/*
 * Style
 */

// Set current bounding box type
void ViewPane::setBoundingBox(ViewPane::BoundingBox type)
{
	boundingBox_ = type;

	// Bounding boxes are generated as part of the axes primitives, so need to regenerate them
	// TODO regenerateAxes_ = true;
}

// Return current bounding box type
ViewPane::BoundingBox ViewPane::boundingBox()
{
	return boundingBox_;
}

// Set y intercept for plane bounding box
void ViewPane::setBoundingBoxPlaneY(double value)
{
	boundingBoxPlaneY_ = value;

	// Bounding boxes are generated as part of the axes primitives, so need to regenerate them
	// TODO 0regenerateAxes_ = true;
}

// Return y intercept for plane bounding box
double ViewPane::boundingBoxPlaneY()
{
	return boundingBoxPlaneY_;
}

// Set font point size for axis value labels
void ViewPane::setLabelPointSize(double value)
{
	labelPointSize_ = value;

	axes_.setPrimitivesInvalid();
}

// Return font point size for axis value labels
double ViewPane::labelPointSize()
{
	return labelPointSize_;
}

// Return font point size for titles
void ViewPane::setTitlePointSize(double value)
{
	titlePointSize_ = value;

	axes_.setPrimitivesInvalid();
}

// Return font point size for titles
double ViewPane::titlePointSize()
{
	return titlePointSize_;
}

// Return text z scaling factor
double ViewPane::textZScale()
{
	return textZScale_;
}

/*
 * GL
 */

// Create bounding box
void ViewPane::createBoundingBox(int type, double planeY)
{
	boundingBoxPrimitive_.forgetAll();
	
	if (type == 0) return;
	else if (type == 1)
	{
		// Plane in XZ, spanning data range   // TODO
// 		boundingBoxPrimitive_.plotLine(Vec3<double>(axisCoordMin_[0].x, planeY, axisCoordMin_[2].z), Vec3<double>(axisCoordMin_[0].x, planeY, axisCoordMax_[2].z));
// 		boundingBoxPrimitive_.plotLine(Vec3<double>(axisCoordMin_[0].x, planeY, axisCoordMax_[2].z), Vec3<double>(axisCoordMax_[0].x, planeY, axisCoordMax_[2].z));
// 		boundingBoxPrimitive_.plotLine(Vec3<double>(axisCoordMax_[0].x, planeY, axisCoordMax_[2].z), Vec3<double>(axisCoordMax_[0].x, planeY, axisCoordMin_[2].z));
// 		boundingBoxPrimitive_.plotLine(Vec3<double>(axisCoordMax_[0].x, planeY, axisCoordMin_[2].z), Vec3<double>(axisCoordMin_[0].x, planeY, axisCoordMin_[2].z));
	}
}


// Update interaction primitive
void ViewPane::updateInteractionPrimitive(int axis)
{
	const int nPoints = 16;
	
	interactionPrimitive_.initialise(nPoints*nPoints*4, nPoints*nPoints*6, GL_TRIANGLES, false);
	interactionPrimitive_.forgetAll();
	interactionBoxPrimitive_.initialise(4, 8, GL_LINES, false);
	interactionBoxPrimitive_.forgetAll();

	if (axis == -1) return;

	// Grab axes, and knock out values in the supplied vectors which correspond to the activated axis
	Vec3<double> axisMinA, axisMinB, axisMaxA, axisMaxB;
	axisMinA[(axis+1)%3] = axes_.axisCoordMin((axis+1)%3)[(axis+1)%3];
	axisMaxA[(axis+1)%3] = axes_.axisCoordMax((axis+1)%3)[(axis+1)%3];
	axisMinB[(axis+2)%3] = axes_.axisCoordMin((axis+2)%3)[(axis+2)%3];
	axisMaxB[(axis+2)%3] = axes_.axisCoordMax((axis+2)%3)[(axis+2)%3];
	axisMinA[axis] = 0.0;
	axisMaxA[axis] = 0.0;
	axisMinB[axis] = 0.0;
	axisMaxB[axis] = 0.0;

	// Create 'bounding box' for slice primitive
	Vec3<double> normal(0.0, 0.0, 1.0);
	
	interactionBoxPrimitive_.defineVertex(axisMinA + axisMinB, normal);
	interactionBoxPrimitive_.defineVertex(axisMinA + axisMaxB, normal);
	interactionBoxPrimitive_.defineVertex(axisMaxA + axisMaxB, normal);
	interactionBoxPrimitive_.defineVertex(axisMaxA + axisMinB, normal);
	interactionBoxPrimitive_.defineIndices(0,1);
	interactionBoxPrimitive_.defineIndices(1,2);
	interactionBoxPrimitive_.defineIndices(2,3);
	interactionBoxPrimitive_.defineIndices(3,0);

	// Work out deltas for each direction
	Vec3<double> deltaA, deltaB, pos;
	deltaA = (axisMaxA - axisMinA) / nPoints;
	deltaB = (axisMaxB - axisMinB) / nPoints;

	// Set normal
	normal.zero();
	normal[axis] = 1.0;

	// Construct plane
	GLuint a, b, c, d;
	for (int n=0; n<nPoints; ++n)
	{
		pos = axisMinA + axisMinB + deltaA*n;
		for (int m=0; m<nPoints; ++m)
		{
			a = interactionPrimitive_.defineVertex(pos, normal);
			b = interactionPrimitive_.defineVertex(pos + deltaA, normal);
			c = interactionPrimitive_.defineVertex(pos + deltaA + deltaB, normal);
			d = interactionPrimitive_.defineVertex(pos + deltaB, normal);
			interactionPrimitive_.defineIndices(a, b, c);
			interactionPrimitive_.defineIndices(c, d, a);
			pos += deltaB;
		}
	}
}

// Return interaction primitive
Primitive& ViewPane::interactionPrimitive()
{
	return interactionPrimitive_;
}

// Return interaction box primitive
Primitive& ViewPane::interactionBoxPrimitive()
{
	return interactionBoxPrimitive_;
}

// Return bounding box primitive
Primitive& ViewPane::boundingBoxPrimitive()
{
	return boundingBoxPrimitive_;
}
