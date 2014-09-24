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
#include "math/cuboid.h"
#include <algorithm>

// Static Members
template<class ViewPane> RefList<ViewPane,bool> ObjectList<ViewPane>::objects_;

// Constructor
ViewPane::ViewPane(ViewLayout& parent) : ListItem<ViewPane>(), parent_(parent), axes_(*this), ObjectList<ViewPane>(this), zOffset_(-10.0)
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
	viewportVersion_ = 0;
	viewTranslation_.set(0.0, 0.0, zOffset_);
	viewViewportUsedAt_ = -1;
	viewAxesUsedAt_ = -1;

	// Role
	role_ = ViewPane::StandardRole;
	viewType_ = ViewPane::NormalView;

	// Style
	boundingBox_ = ViewPane::NoBox;
	boundingBoxPlaneY_ = 0.0;
	labelPointSize_ = 16.0;
	titlePointSize_ = 20.0;
	textZScale_ = -1.0;

	// GL
	interactionPrimitive_.setNoInstances();
	interactionBoxPrimitive_.setNoInstances();
	boundingBoxPrimitive_.setNoInstances();
}

// Destructor
ViewPane::~ViewPane()
{
}

// Copy constructor
ViewPane::ViewPane(const ViewPane& source) : parent_(parent_), axes_(*this), ObjectList<ViewPane>(NULL), zOffset_(zOffset_)
{
	(*this) = source;
}

// Assignment operator
void ViewPane::operator=(const ViewPane& source)
{
	// Geometry / position
	name_ = source.name_;
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
	viewportVersion_ = 0;
	viewRotation_ = source.viewRotation_;
	viewTranslation_ = source.viewTranslation_;
	viewViewportUsedAt_ = -1;
	viewAxesUsedAt_ = -1;

	// Collection / Axes
	axes_ = source.axes_;

	// Style
	boundingBox_ = source.boundingBox_;
	boundingBoxPlaneY_ = source.boundingBoxPlaneY_;
	labelPointSize_ = source.labelPointSize_;
	titlePointSize_ = source.titlePointSize_;
	textZScale_ = source.textZScale_;

	// Role
	role_ = source.role_;
	viewType_ = source.viewType_;
	// -- Don't perform a literal copy of the paneTargets_. Re-add them instead...
	paneTargets_.clear();
// 	RefList<ViewPane,bool> paneTargets_;	TODO
	// -- Don't perform a literal copy of the collectionTargets_. Re-add them instead...
	collectionTargets_.clear();
	for (TargetData* target = source.collectionTargets_.first(); target != NULL; target = target->next) addCollectionTarget(target->collection());

	// Additional / Generated data
	// -- Should be handled by performing re-addition of target collections above
}

/*
 * Parent
 */

// Return parent
ViewLayout& ViewPane::parent()
{
	return parent_;
}

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
	++viewportVersion_;

	// Add on the remainder if the top or right edges are extreme
	if ((leftEdge_+width_) == nColumns) viewportMatrix_[2] += widthRemainder;
	if ((bottomEdge_+height_) == nRows) viewportMatrix_[3] += heightRemainder;

	// Recalculate projection matrix and view
	projectionMatrix_ = calculateProjectionMatrix(zOffset_);
	calculateFontScaling();
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
const char* RoleKeywords[ViewPane::nPaneRoles] = { "SliceMonitor", "Standard" };

// Convert text string to PaneRole
ViewPane::PaneRole ViewPane::paneRole(QString s)
{
	for (int n=0; n<ViewPane::nPaneRoles; ++n) if (s == RoleKeywords[n]) return (ViewPane::PaneRole) n;
	return ViewPane::nPaneRoles;
}

// Convert PaneRole to text string
const char* ViewPane::paneRole(ViewPane::PaneRole role)
{
	return RoleKeywords[role];
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

// Add target pane for role
void ViewPane::addPaneTarget(ViewPane* pane)
{
	paneTargets_.add(pane);

	paneChanged();
}

// Remove target pane for role
void ViewPane::removePaneTarget(ViewPane* pane)
{
	paneTargets_.remove(pane);

	paneChanged();
}

// Return whether specified pane is a target
bool ViewPane::paneIsTarget(ViewPane* pane)
{
	return paneTargets_.contains(pane);
}

// Return target panes for role
RefListItem<ViewPane,bool>* ViewPane::paneTargets()
{
	return paneTargets_.first();
}

// Add target collection for role
void ViewPane::addCollectionTarget(Collection* collection)
{
	TargetData* target = collectionTargets_.add(*this);
	target->initialise(collection);

	paneChanged();
}

// Remove target collection for role
void ViewPane::removeCollectionTarget(Collection* collection)
{
	TargetData* target = collectionIsTarget(collection);
	if (!target)
	{
		msg.print(Messenger::Verbose, "Internal Error: Tried to remove collection '%s' from pane '%s', but it is not a target there.\n", qPrintable(collection->name()), qPrintable(name_));
		return;
	}

	// Remove the target
	collectionTargets_.remove(target);

	paneChanged();
}

// Return whether specified collection is a target
TargetData* ViewPane::collectionIsTarget(Collection* collection)
{
	for (TargetData* target = collectionTargets_.first(); target != NULL; target = target->next) if (target->collection() == collection) return target;
	return NULL;
}

// Return target collections for role
TargetData* ViewPane::collectionTargets()
{
	return collectionTargets_.first();
}

// Process supplied Collection changed/update signal if it is relevant to this pane
bool ViewPane::processUpdate(Collection* source, Collection::CollectionSignal signal)
{
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
			if (!collectionIsTarget(source)) return false;
			removeCollectionTarget(source);
			break;
		// Current Slice Changed
		// -- Role Relevance : SliceMonitorRole
		case (Collection::CurrentSliceChangedSignal):
			if (role_ != ViewPane::SliceMonitorRole) return false;
			if (!collectionIsTarget(source)) return false;
			// Update axes limits
			updateAxisLimits();
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
			msg.print("ViewPane::processUpdate - Unrecognised signal %i sent from collection '%s'\n", signal, qPrintable(source->name()));
			return false;
	}

	return true;
}

/*
 * Projection / View
 */

// View types
const char* ViewTypeKeywords[ViewPane::nViewTypes] = { "Normal", "AutoStretched", "FlatXY", "FlatXZ", "FlatYZ", "Linked" };

// Convert text string to ViewType
ViewPane::ViewType ViewPane::viewType(QString s)
{
	for (int n=0; n<ViewPane::nViewTypes; ++n) if (s == ViewTypeKeywords[n]) return (ViewPane::ViewType) n;
	return ViewPane::nViewTypes;
}

// Convert ViewType to text string
const char* ViewPane::viewType(ViewPane::ViewType vt)
{
	return ViewTypeKeywords[vt];
}

// Return calculated projection matrix
Matrix ViewPane::calculateProjectionMatrix(double orthoZoom)
{
	Matrix result;

	GLdouble top, bottom, right, left;
	GLdouble nearClip = 0.5, farClip = 2000.0;

	if (hasPerspective_ && (viewType_ <= ViewPane::AutoStretchedView))
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

// Set view type
void ViewPane::setViewType(ViewPane::ViewType vt)
{
	viewType_ = vt;
}

// Return view type
ViewPane::ViewType ViewPane::viewType()
{
	return viewType_;
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

	projectionMatrix_ = calculateProjectionMatrix(viewTranslation_.z);
	calculateFontScaling();
}

// Return whether this pane uses perspective
bool ViewPane::hasPerspective()
{
	return hasPerspective_;
}

// Update transformation (view) matrix
void ViewPane::setViewRotation(Matrix& mat)
{
	// If this is a two-dimensional or linked view, ignore the request
	if ((viewType_ != ViewPane::NormalView) && (viewType_ != ViewPane::AutoStretchedView)) return;

	viewRotation_ = mat;
}

// Update single column of view matrix
void ViewPane::setViewRotationColumn(int column, double x, double y, double z)
{
	// If this is a two-dimensional or linked view, ignore the request
	if ((viewType_ != ViewPane::NormalView) && (viewType_ != ViewPane::AutoStretchedView)) return;
	
	viewRotation_.setColumn(column, x, y, z, 0.0);
}

// Rotate view matrix about x and y by amounts specified
void ViewPane::rotateView(double dx, double dy)
{
	// If this is a two-dimensional or linked view, ignore the request
	if ((viewType_ != ViewPane::NormalView) && (viewType_ != ViewPane::AutoStretchedView)) return;

	Matrix A;
	A.createRotationXY(dx, dy);
	viewRotation_ *= A;
}

// Return view rotation
Matrix ViewPane::viewRotation()
{
	return viewRotation_;
}

// Set view translation
void ViewPane::setViewTranslation(double x, double y, double z)
{
	viewTranslation_.set(x, y, z);
	if (!hasPerspective_) projectionMatrix_ = calculateProjectionMatrix(viewTranslation_.z);
	calculateFontScaling();
}

// Translate view matrix by amounts specified
void ViewPane::translateView(double dx, double dy, double dz)
{
	// If this is a two-dimensional or linked view, ignore the request
	if ((viewType_ != ViewPane::NormalView) && (viewType_ != ViewPane::AutoStretchedView)) return;

	viewTranslation_.add(dx, dy, dz);
	if ((!hasPerspective_) && (fabs(dz) > 1.0e-4)) projectionMatrix_ = calculateProjectionMatrix(viewTranslation_.z);
	calculateFontScaling();
}

// Return current view translation
Vec3<double> ViewPane::viewTranslation()
{
	return viewTranslation_;
}

// Return full view matrix (rotation + translation)
Matrix ViewPane::viewMatrix()
{
	Matrix viewMatrix;

	// Apply translation to centre of axes coordinates
	viewMatrix.createTranslation(-axes().coordCentre());

	// Apply rotation matrix about this local centre
	viewMatrix *= viewRotation_;

	// Apply translation to apply view shift and zoom (the latter only if using perspective)
// 	viewMatrix.applyTranslation(viewTranslation_.x, viewTranslation_.y, hasPerspective_ ? viewTranslation_.z : 0.0 );
	viewMatrix.applyTranslation(viewTranslation_.x, viewTranslation_.y, viewTranslation_.z);

	return viewMatrix;
}

// Project given model coordinates into world coordinates
Vec3<double> ViewPane::modelToWorld(Vec3<double> modelr)
{
	msg.enter("Viewer::modelToWorld");
	Vec3<double> worldr;
	Matrix vmat;
	Vec4<double> pos, temp;

	// Projection formula is : worldr = P x M x modelr
	pos.set(modelr, 1.0);
	// Get the world coordinates of the atom - Multiply by modelview matrix 'view'
	vmat = viewMatrix();
	temp = vmat * pos;
	worldr.set(temp.x, temp.y, temp.z);
	msg.exit("Viewer::modelToWorld");
	return worldr;
}

// Project given model coordinates into screen coordinates
Vec3<double> ViewPane::modelToScreen(Vec3<double> modelr)
{
	msg.enter("Viewer::modelToScreen");
	Vec4<double> screenr, tempscreen;
	Vec4<double> worldr;
	Matrix vmat;
	Vec4<double> pos;

	// Projection formula is : worldr = P x M x modelr
	pos.set(modelr, 1.0);

	// Get the world coordinates of the point - Multiply by modelview matrix 'view'
	vmat = viewMatrix();
	worldr = vmat * pos;
	screenr = projectionMatrix_ * worldr;
	screenr.x /= screenr.w;
	screenr.y /= screenr.w;
	screenr.x = viewportMatrix_[0] + viewportMatrix_[2]*(screenr.x+1)*0.5;
	screenr.y = viewportMatrix_[1] + viewportMatrix_[3]*(screenr.y+1)*0.5;
	screenr.z = screenr.z / screenr.w;

	msg.exit("Viewer::modelToScreen");
	return Vec3<double>(screenr.x, screenr.y, screenr.z);
}

// Project given model coordinates into screen coordinates using supplied rotation matrix and translation vector
Vec3<double> ViewPane::modelToScreen(Vec3<double> modelr, Matrix rotationMatrix, Vec3<double> translation)
{
	msg.enter("Viewer::modelToScreen");
	Vec4<double> screenr, tempscreen;
	Vec4<double> worldr;
	Matrix vmat;
	Vec4<double> pos;

	// Projection formula is : worldr = P x M x modelr
	pos.set(modelr, 1.0);

	// Get the world coordinates of the point - Multiply by modelview matrix 'view'
	vmat = rotationMatrix;
	vmat.applyTranslation(translation);
	worldr = vmat * pos;
	screenr = projectionMatrix_ * worldr;
	screenr.x /= screenr.w;
	screenr.y /= screenr.w;
	screenr.x = viewportMatrix_[0] + viewportMatrix_[2]*(screenr.x+1)*0.5;
	screenr.y = viewportMatrix_[1] + viewportMatrix_[3]*(screenr.y+1)*0.5;
	screenr.z = screenr.z / screenr.w;

	msg.exit("Viewer::modelToScreen");
	return Vec3<double>(screenr.x, screenr.y, screenr.z);
}

// Return z translation necessary to display coordinates supplied, assuming the identity view matrix
double ViewPane::calculateRequiredZoom(double xMax, double yMax, double fraction)
{
	// The supplied x and y extents should indicate the number of units in those directions
	// from the origin that are to be displaye on-screen. The 'fraction' indicates how much of the
	// available range on-screen to use, allowing a margin to be added. A value of '1.0' would
	// put the extent with the highest units on the very edge of the display.

	Matrix viewMatrix, projectionMatrix = calculateProjectionMatrix(zOffset_);
	Vec4<double> rScreen, rWorld, rModel(xMax, yMax, 0.0, 1.0);
	Vec3<double> translation(0.0, 0.0, -1.0);

	// Sanity check
	if (viewportMatrix_[2] == 0) return 1.0;
	if (viewportMatrix_[3] == 0) return 1.0;

	// Calculate target screen coordinate
	int targetX = viewportMatrix_[0] + (1.0 + fraction) * viewportMatrix_[2] * 0.5;
	int targetY = viewportMatrix_[1] + (1.0 + fraction) * viewportMatrix_[3] * 0.5;

	int count = 0;
	do
	{
		// If not using perspective, must recalculate the projection matrix
		if (!hasPerspective_) projectionMatrix = calculateProjectionMatrix(translation.z);

		// Project the point : worldr = P x M x modelr
		viewMatrix.setIdentity();
		viewMatrix.applyTranslation(translation);
		rWorld = viewMatrix * rModel;
		rScreen = projectionMatrix * rWorld;
		rScreen.x /= rScreen.w;
		rScreen.y /= rScreen.w;
		rScreen.x = viewportMatrix_[0] + viewportMatrix_[2]*(rScreen.x+1)*0.5;
		rScreen.y = viewportMatrix_[1] + viewportMatrix_[3]*(rScreen.y+1)*0.5;
		rScreen.z = rScreen.z / rScreen.w;

		// Increase zoom distance
		translation.z -= std::max( std::max(rScreen.x / targetX, rScreen.y / targetY), 1.0);

		// Limit the number of iterations so we can never get into an infinite loop
		if (++count == 1000) break;

	} while ((rScreen.x > targetX) || (rScreen.y > targetY));
	return translation.z;
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
	Matrix itransform = viewMatrix();
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

// Recalculate current view parameters (e.g. for 2D, autoStretched 3D etc.)
void ViewPane::recalculateView(bool force)
{
	// Check viewport size to try and avoid nan errors
	if ((viewportMatrix_[2] == 0) || (viewportMatrix_[3] == 0)) return;

	// If the view is neither flat nor autoStretched, there is nothing to do here...
	if (viewType_ == ViewPane::NormalView) return;

	// If we are already up-to-date (w.r.t. the associated axes) then we can also return now
	bool upToDate = true;
	if (force) upToDate = false;
	else if (viewAxesUsedAt_ != axes().axesVersion()) upToDate = false;
	else if (viewViewportUsedAt_ != viewportVersion_) upToDate = false;

	if (upToDate) return;

	int axis;

	// To begin, set the stretch factors to our best first estimate, dividing the pane width by the range of the axes
	// Doing this first will allow us to get much better values for the pixel overlaps we need later on
	// -- Project a point one unit each along X and Y and subtract off the viewport centre coordinate in order to get literal 'pixels per unit' for (screen) X and Y 
	Vec3<double> unit = modelToScreen(Vec3<double>(1.0, 1.0, 0.0), Matrix());
	unit.x -= viewportMatrix_[0] + viewportMatrix_[2]/2.0;
	unit.y -= viewportMatrix_[1] + viewportMatrix_[3]/2.0;
	unit.z = unit.y;

	// Get axis min/max, accounting for logarithmic axes
	Vec3<double> axisMin, axisMax;
	for (axis=0; axis<3; ++axis)
	{
		axisMin[axis] = axes_.logarithmic(axis) ? log10(axes_.min(axis)) : axes_.min(axis);
		axisMax[axis] = axes_.logarithmic(axis) ? log10(axes_.max(axis)) : axes_.max(axis);
	}

	// Decide how we will set stretch factors for each axis (initially set to standard xyy)
	int axisX = 0, axisY = 1;
	Vec3<int> axisDir(0,1,1);
	if (viewType_ == ViewPane::FlatXZView) axisY = 2;
	else if (viewType_ == ViewPane::FlatYZView)
	{
		axisDir.set(1,0,1);
		axisX = 2;
	}

	// Set axis stretch factors to fill available pixel width/height
	for (axis=0; axis<3; ++axis) axes_.setStretch(axis, viewportMatrix_[axisDir[axis]+2] / (unit[axisDir[axis]] * (axisMax[axis] - axisMin[axis])));

	const double margin = 10.0;
	Matrix viewMat, B;
	double tempMin, tempMax;
	Vec3<double> coordMin[3], coordMax[3], labelMin, labelMax, a, b, globalMin, globalMax;

	// Iterate for a few cycles
	for (int cycle = 0; cycle < 5; ++cycle)
	{
// 		printf("Pane '%s' Cycle = %i\n", qPrintable(name_), cycle);
		// We will now calculate more accurate stretch factors to apply to the X and Y axes.
		// Project the axis limits on to the screen using the relevant viewmatrix + coordinate centre translation
		viewMat.createTranslation(-axes().coordCentre());
		if (viewType_ == ViewPane::FlatXZView) viewMat.applyRotationX(-90.0);
		else if (viewType_ == ViewPane::FlatYZView) viewMat.applyRotationY(90.0);

		// Calculate coordinates and global extremes over axes and labels
		globalMin.set(1e9,1e9,1e9);
		globalMax = -globalMin;
		labelMin = globalMin;
		labelMax = -labelMin;
// 		printf("VP = %i %i %i %i\n", viewportMatrix_[0], viewportMatrix_[1], viewportMatrix_[2], viewportMatrix_[3]);
		for (axis=0; axis<3; ++axis)
		{
			// Skip third (i.e. 'z') axis
			if ((axis != axisX) && (axis != axisY)) continue;

			// Project axis min/max coordinates onto screen
			a = modelToScreen(axes_.coordMin(axis), viewMat);
			b = modelToScreen(axes_.coordMax(axis), viewMat);
			coordMin[axis].set(std::min(a.x,b.x), std::min(a.y,b.y),  std::min(a.z,b.z)); 
			coordMax[axis].set(std::max(a.x,b.x), std::max(a.y,b.y),  std::max(a.z,b.z)); 

			// Update global min/max
// 			printf("Projected %i Coord Min = ", axis); coordMin[axis].print();
// 			printf("Projected %i Coord Max = ", axis); coordMax[axis].print();
			for (int n=0; n<3; ++n)
			{
				if (coordMin[axis][n] < globalMin[n]) globalMin[n] = coordMin[axis][n];
				if (coordMax[axis][n] > globalMax[n]) globalMax[n] = coordMax[axis][n];
			}

			// Get bounding cuboid for axis text
			Cuboid cuboid;
			cuboid = axes_.labelPrimitive(axis).boundingCuboid(*this, false, textZScale_);
			cuboid = axes_.titlePrimitive(axis).boundingCuboid(*this, false, textZScale_, cuboid);

			// Project cuboid extremes and store projected coordinates
			a = modelToScreen(cuboid.minima(), viewMat);
			b = modelToScreen(cuboid.maxima(), viewMat);
// 			printf("Label %i minima = ", axis); a.print();
// 			printf("Label %i maxima = ", axis); b.print();

			// Update global and label min/max
			for (int n=0; n<3; ++n)
			{
				tempMin = std::min(a[n],b[n]);
				tempMax = std::max(a[n],b[n]);
				if (tempMin < globalMin[n]) globalMin[n] = tempMin;
				if (tempMax > globalMax[n]) globalMax[n] = tempMax;
				if (tempMin < labelMin[n]) labelMin[n] = tempMin;
				if (tempMax > labelMax[n]) labelMax[n] = tempMax;
			}
		}

		// Now have screen coordinates of all necessary objects (axes and labels)
// 		printf("GlobalMinMax: X = %f/%f, Y=%f/%f\n", globalMin.x, globalMax.x, globalMin.y, globalMax.y);

		// Calculate total width and height of objects as they are arranged
		double globalWidth = globalMax.x - globalMin.x;
		double globalHeight = globalMax.y - globalMin.y;
		double axisWidth = coordMax[axisX].x - coordMin[axisX].x;
		double axisHeight = coordMax[axisY].y - coordMin[axisY].y;
		double labelWidth = labelMax.x - labelMin.x;
		double labelHeight = labelMax.y - labelMin.y;
// 		printf("Global w/h = %f/%f\n", globalWidth, globalHeight);

		// Now, we know the width and height of the axis on its own, and the extra 'added' by the labels, so work out how much we need to shrink the axis by
		double deltaWidth = (viewportMatrix_[2] - 2*margin) - globalWidth;
		double deltaHeight = (viewportMatrix_[3] - 2*margin) - globalHeight;
// 		printf("deltas on width and height are %f and %f\n", deltaWidth, deltaHeight);

		// So, need to lose deltaWidth and deltaHeight pixels from the axis exents - we'll do this by scaling the stretchfactor
		double factor = axisWidth / (axisWidth - deltaWidth);
// 		printf("Factor on axisX stretch = %f\n", factor);
		axes_.setStretch(axisX, axes_.stretch(axisX) * factor);
		factor = axisHeight / (axisHeight - deltaHeight);
// 		printf("Factor on axisY stretch = %f\n", factor);
		axes_.setStretch(axisY, axes_.stretch(axisY) * factor);

// 		printf("New stretch factors at end of cycle %i are %f %f %f\n", cycle, axes_.stretch(0), axes_.stretch(1), axes_.stretch(2));
	}

	// Set new rotation matrix
	viewRotation_.setIdentity();
	if (viewType_ == ViewPane::FlatXZView) viewRotation_.applyRotationX(-90.0);
	else if (viewType_ == ViewPane::FlatYZView) viewRotation_.applyRotationY(90.0);

	// Set a translation in order to set the margins as requested
	// The viewTranslation_ is applied in 'normal' coordinate axes, so viewTranslation_.x is along screen x etc.
// 	printf("Final globalMin = "); globalMin.print();
// 	printf("Final globalMax = "); globalMax.print();
// 	printf("Viewport is %i %i %i %i\n", viewportMatrix_[0], viewportMatrix_[1], viewportMatrix_[2], viewportMatrix_[3]);
	viewTranslation_.zero();
	viewTranslation_[0] = (margin - (globalMin.x - viewportMatrix_[0])) / unit.x;
	viewTranslation_[1] = (margin - (globalMin.y - viewportMatrix_[1])) / unit.y;
// 	viewTranslation_.print();

	// Store new versions of view
	viewAxesUsedAt_ = axes().axesVersion();
	viewViewportUsedAt_ = viewportVersion_;
}

// Reset view
void ViewPane::resetViewMatrix()
{
	// Reset view matrix only if a normal or autostretched view
	if (viewType_ <= ViewPane::AutoStretchedView)
	{
		viewRotation_.setIdentity();
		viewTranslation_.set(0.0, 0.0, zOffset_);

		// Calculate zoom to show all data
		viewTranslation_.z = calculateRequiredZoom(axes_.range(0)*0.5, axes_.range(1)*0.5, 0.9);

		// Recalculate projection matrix
		projectionMatrix_ = calculateProjectionMatrix(viewTranslation_.z);
	}
	else
	{
		// Recalculate projection matrix
		projectionMatrix_ = calculateProjectionMatrix(zOffset_);
	}

	calculateFontScaling();
}

// Set display limits to show all available data
void ViewPane::showAllData()
{
	updateAxisLimits();

	for (int axis = 0; axis < 3; ++axis)
	{
		axes_.setToLimit(axis, true);
		axes_.setToLimit(axis, false);
	}
}

// Render all data associated with this pane
void ViewPane::renderData(const QGLContext* context, GLExtensions* extensions, bool forcePrimitiveUpdate, bool pushAndPop)
{
	// Loop over displayTargets_ list...
	for (TargetData* target = collectionTargets_.first(); target != NULL; target = target->next)
	{
		// Loop over display primitives in this target...
		for (TargetPrimitive* primitive = target->displayPrimitives(); primitive != NULL; primitive = primitive->next)
		{
			// Make sure the primitive is up to date and send it to GL
			primitive->updateAndSendPrimitive(axes_, forcePrimitiveUpdate, pushAndPop, context, extensions);
		}
	}
}

/*
 * Axes
 */

// Return absolute minimum transformed values over all associated collections
Vec3<double> ViewPane::transformedDataMinima()
{
	if (collectionTargets_.nItems() == 0) return Vec3<double>(0.0,0.0,0.0);

	// Set starting values from first display collection we find
	bool first = true;
	Vec3<double> v, minima;
	for (TargetData* target = collectionTargets_.first(); target != NULL; target = target->next)
	{
		// Loop over display targets
		for (TargetPrimitive* prim = target->displayPrimitives(); prim != NULL; prim = prim->next)
		{
			if (first)
			{
				minima = prim->collection()->transformMin();
				first = false;
			}
			else
			{
				v = target->collection()->transformMin();
				if (v.x < minima.x) minima.x = v.x;
				if (v.y < minima.y) minima.y = v.y;
				if (v.z < minima.z) minima.z = v.z;
			}
		}
	}
	return minima;
}

// Return absolute maximum transformed values over all associated collections
Vec3<double> ViewPane::transformedDataMaxima()
{
	if (collectionTargets_.nItems() == 0) return Vec3<double>(0.0,0.0,0.0);

	// Set starting values from first display collection we find
	bool first = true;
	Vec3<double> v, maxima;
	for (TargetData* target = collectionTargets_.first(); target != NULL; target = target->next)
	{
		// Loop over display targets
		for (TargetPrimitive* prim = target->displayPrimitives(); prim != NULL; prim = prim->next)
		{
			if (first)
			{
				maxima = prim->collection()->transformMax();
				first = false;
			}
			else
			{
				v = target->collection()->transformMax();
				if (v.x > maxima.x) maxima.x = v.x;
				if (v.y > maxima.y) maxima.y = v.y;
				if (v.z > maxima.z) maxima.z = v.z;
			}
		}
	}
	return maxima;
}

// Return absolute minimum positive transformed values over all associated collections
Vec3<double> ViewPane::transformedDataPositiveMinima()
{
	if (collectionTargets_.nItems() == 0) return Vec3<double>(0.0,0.0,0.0);

	// Set starting values from first display collection we find
	bool first = true;
	Vec3<double> v, minima;
	for (TargetData* target = collectionTargets_.first(); target != NULL; target = target->next)
	{
		// Loop over display targets
		for (TargetPrimitive* prim = target->displayPrimitives(); prim != NULL; prim = prim->next)
		{
			if (first)
			{
				minima = prim->collection()->transformMinPositive();
				first = false;
			}
			else
			{
				v = target->collection()->transformMinPositive();
				if (v.x < minima.x) minima.x = v.x;
				if (v.y < minima.y) minima.y = v.y;
				if (v.z < minima.z) minima.z = v.z;
			}
		}
	}
	return minima;
}

// Return absolute maximum positive transformed values over all associated collections
Vec3<double> ViewPane::transformedDataPositiveMaxima()
{
	if (collectionTargets_.nItems() == 0) return Vec3<double>(0.0,0.0,0.0);

	// Set starting values from first display collection we find
	bool first = true;
	Vec3<double> v, maxima;
	for (TargetData* target = collectionTargets_.first(); target != NULL; target = target->next)
	{
		// Loop over display targets
		for (TargetPrimitive* prim = target->displayPrimitives(); prim != NULL; prim = prim->next)
		{
			if (first)
			{
				maxima = prim->collection()->transformMaxPositive();
				first = false;
			}
			else
			{
				v = target->collection()->transformMaxPositive();
				if (v.x > maxima.x) maxima.x = v.x;
				if (v.y > maxima.y) maxima.y = v.y;
				if (v.z > maxima.z) maxima.z = v.z;
			}
		}
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
		if (axes_.logarithmic(axis))
		{
			axes_.setLimitMin(axis, dataMinPositive[axis]);
			axes_.setLimitMax(axis, dataMaxPositive[axis]);
		}
		else
		{
			axes_.setLimitMin(axis, dataMin[axis]);
			axes_.setLimitMax(axis, dataMax[axis]);
		}
	}
}

// Update current slices for all collections displayed in this pane
void ViewPane::collectionsUpdateCurrentSlices(int axis, double axisValue)
{
	for (TargetData* target = collectionTargets_.first(); target != NULL; target = target->next)
	{
		for (TargetPrimitive* prim = target->displayPrimitives(); prim != NULL; prim = prim->next)
		{
			prim->collection()->updateCurrentSlice(axis, axisValue);
		}
	}
}

/*
 * Style
 */

// Calculate font scaling factor
void ViewPane::calculateFontScaling()
{
	// Calculate text scaling factor
	Vec3<double> unit = modelToScreen(Vec3<double>(0.0, 1.0, viewTranslation_.z), Matrix(), Vec3<double>(0.0, 0.0, zOffset_));
	unit.y -= viewportMatrix_[1] + viewportMatrix_[3]*0.5;
	textZScale_ = unit.y;
}

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
 * Interaction
 */

// Return axis title at specified coordinates (if any)
int ViewPane::axisTitleAt(int screenX, int screenY)
{
	printf("Coords=%i %i\n", screenX, screenY);
	Vec3<double> labelMin, labelMax;
	for (int axis=0; axis<3; ++axis)
	{
		// Reset bounding box extreme values
		labelMin.set(1e9,1e9,1e9);
		labelMax = -labelMin;

		// Calculate orthogonal bounding cuboid for this axis title (local coordinates)
		Cuboid cuboid = axes_.titlePrimitive(axis).boundingCuboid(*this, false, textZScale_);

		// Determine whether the screen point specified is within the projected cuboid
		if (cuboid.isPointWithinProjection(screenX, screenY, viewMatrix(), projectionMatrix_, viewportMatrix_)) return axis;
	}
	return -1;
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
	axisMinA[(axis+1)%3] = axes_.coordMin((axis+1)%3)[(axis+1)%3];
	axisMaxA[(axis+1)%3] = axes_.coordMax((axis+1)%3)[(axis+1)%3];
	axisMinB[(axis+2)%3] = axes_.coordMin((axis+2)%3)[(axis+2)%3];
	axisMaxB[(axis+2)%3] = axes_.coordMax((axis+2)%3)[(axis+2)%3];
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
