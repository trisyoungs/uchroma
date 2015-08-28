/*
	*** Viewer Functions
	*** src/gui/viewer_funcs.cpp
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

#include "gui/viewer.hui"
#include "gui/uchroma.h"
#include "base/messenger.h"
#include "render/fontinstance.h"

// Constructor
Viewer::Viewer(QWidget *parent) : QOpenGLWidget(parent)
{
	uChromaWindow_ = NULL;

	// Character / Setup
	contextWidth_ = 0;
	contextHeight_ = 0;
	valid_ = false;
	drawing_ = false;
	renderingOffScreen_ = false;
	highlightCollection_ = NULL;

	// Query
	objectQueryX_ = -1;
	objectQueryY_ = -1;
	depthAtQueryCoordinates_ = 1.0;
	objectAtQueryCoordinates_ = Viewer::NoObject;

	// Prevent QPainter from autofilling widget background
	setAutoFillBackground(false);
}

// Destructor
Viewer::~Viewer()
{
}

// Set UChromaWindow pointer
void Viewer::setUChromaWindow(UChromaWindow* ptr)
{
	uChromaWindow_ = ptr;
}

/*
 * Protected Qt Virtuals
 */

// Initialise context widget (when created by Qt)
void Viewer::initializeGL()
{
	msg.enter("Viewer::initializeGL");

	// Setup function pointers to OpenGL extension functions
	initializeOpenGLFunctions();

	// Setup offscreen context
	msg.print(Messenger::Verbose, "Setting up offscreen context and surface...");
        offscreenContext_.setShareContext(context());
        offscreenContext_.setFormat(context()->format());
        offscreenContext_.create();
        offscreenSurface_.setFormat(context()->format());
	offscreenSurface_.create();
	msg.print(Messenger::Verbose, "Done.");

	// Check for vertex buffer extensions
        if ((!hasOpenGLFeature(QOpenGLFunctions::Buffers)) && (PrimitiveInstance::globalInstanceType() == PrimitiveInstance::VBOInstance))
	{
		printf("VBO extension is requested but not available, so reverting to display lists instead.\n");
		PrimitiveInstance::setGlobalInstanceType(PrimitiveInstance::ListInstance);
	}

	valid_ = true;

	// Recalculate view layouts
	UChromaSession::recalculateViewLayout(contextWidth_, contextHeight_);

	msg.exit("Viewer::initializeGL");
}

void Viewer::paintGL()
{
	msg.enter("Viewer::paintGL");

	// Do nothing if the canvas is not valid, or we are still drawing from last time, or the uChroma pointer has not been set
	if ((!valid_) || drawing_ || (!uChromaWindow_))
	{
		msg.exit("Viewer::paintGL");
		return;
	}

	// Set the drawing flag so we don't have any rendering clashes
	drawing_ = true;

	// Setup basic GL stuff
	setupGL();

	// Render full scene
	renderFullScene();

	// Reset query coordinate
	objectQueryX_ = -1;
	objectQueryY_ = -1;

	// Set the rendering flag to false
	drawing_ = false;
	
	msg.exit("Viewer::paintGL");
}

// Resize function
void Viewer::resizeGL(int newwidth, int newheight)
{
	// Store the new width and height of the widget
	contextWidth_ = (GLsizei) newwidth;
	contextHeight_ = (GLsizei) newheight;

	// Recalculate view layout
	UChromaSession::recalculateViewLayout(contextWidth_, contextHeight_);
}

/*
// Character / Setup
*/

// Return the current height of the drawing area
GLsizei Viewer::contextHeight() const
{
	return contextHeight_;
}

// Return the current width of the drawing area
GLsizei Viewer::contextWidth() const
{
	return contextWidth_;
}

// Check for GL error
void Viewer::checkGlError()
{
	GLenum glerr = GL_NO_ERROR;
	do
	{
		switch (glGetError())
		{
			case (GL_INVALID_ENUM): msg.print(Messenger::Verbose, "GLenum argument out of range\n"); break;
			case (GL_INVALID_VALUE): msg.print(Messenger::Verbose, "Numeric argument out of range\n"); break;
			case (GL_INVALID_OPERATION): msg.print(Messenger::Verbose, "Operation illegal in current state\n"); break;
			case (GL_STACK_OVERFLOW): msg.print(Messenger::Verbose, "Command would cause a stack overflow\n"); break;
			case (GL_STACK_UNDERFLOW): msg.print(Messenger::Verbose, "Command would cause a stack underflow\n"); break;
			case (GL_OUT_OF_MEMORY): msg.print(Messenger::Verbose, "Not enough memory left to execute command\n"); break;
			case (GL_NO_ERROR): msg.print(Messenger::Verbose, "No GL error\n"); break;
			default:
				msg.print(Messenger::Verbose, "Unknown GL error?\n");
				break;
		}
	} while (glerr != GL_NO_ERROR);
}

// Refresh widget / scene
void Viewer::postRedisplay()
{
	if ((!valid_) || drawing_) return;
	update();
}

/*
 * Object Querying
 */

// Update depth at query coordinates, returning whether it is closer
bool Viewer::updateQueryDepth()
{
	// Return immediately if we are not querying
	if (objectQueryX_ == -1) return false;

	// Sample a small area centred at the supplied position
	GLfloat depth[9];
	
	glReadPixels(objectQueryX_, objectQueryY_, objectQueryWidth_, objectQueryHeight_, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

	bool result = false;
	for (int i=0; i<objectQueryWidth_*objectQueryHeight_; ++i)
	{
		if (depth[i] < depthAtQueryCoordinates_)
		{
			depthAtQueryCoordinates_ = depth[i];
			result = true;
		}
	}
	// 	printf("Current averageDepth at %i,%i is now %f\n", objectQueryX_, objectQueryY_, averageDepth);

	return result;
}

// Set information of query object
void Viewer::setQueryObject(Viewer::ViewObject objectType, QString info)
{
	objectAtQueryCoordinates_ = objectType;
	infoAtQueryCoordinates_ = info;
}

// Set coordinates to query at next redraw
void Viewer::setQueryCoordinates(int mouseX, int mouseY)
{
	depthAtQueryCoordinates_ = 1.0;
	objectAtQueryCoordinates_ = Viewer::NoObject;
	infoAtQueryCoordinates_.clear();

	// Check for invalid coordinates
	if ((mouseX < 0) || (mouseX >= width()) || (mouseY < 0) || (mouseY >= height()))
	{
		objectQueryX_ = -1;
		objectQueryY_ = -1;
		return;
	}

	// Setup area to sample around central pixel
	objectQueryWidth_ = 3;
	objectQueryX_ = mouseX-1;
	if (mouseX == 0)
	{
		--objectQueryWidth_;
		++mouseX;
	}
	else if (mouseX == (width()-1)) --objectQueryWidth_;

	objectQueryHeight_ = 3;
	objectQueryY_ = mouseY-1;
	if (mouseY == 0)
	{
		--objectQueryHeight_;
		++mouseY;
	}
	else if (mouseY == (height()-1)) --objectQueryHeight_;
}

// Return object type at query coordinates
Viewer::ViewObject Viewer::objectAtQueryCoordinates()
{
	return objectAtQueryCoordinates_;
}

// Info for object at query coordinates
QString Viewer::infoAtQueryCoordinates()
{
	return infoAtQueryCoordinates_;
}
