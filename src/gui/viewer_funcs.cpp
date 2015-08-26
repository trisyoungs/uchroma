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

	int axis;

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

	// Set colour mode
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);

	// Clear view
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Set some colours
	GLfloat colourBlack[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat colourGray[4] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat colourBlue[4] = { 0.88, 0.95, 1.0, 1.0 };
	GLfloat colourWhite[4] = { 1.0, 1.0, 1.0, 1.0 };

	// Loop over defined viewpanes
	GLdouble clipPlaneBottom[4] = { 0.0, 1.0, 0.0, 0.0 }, clipPlaneTop[4] = { 0.0, -1.0, 0.0, 0.0 };
	for (ViewPane* pane = UChromaSession::viewLayout().panes(); pane != NULL; pane = pane->next)
	{
		// Before we do anything else, make sure the view is up to date
		pane->recalculateView();

		// Set viewport
		glViewport(pane->viewportMatrix()[0], pane->viewportMatrix()[1], pane->viewportMatrix()[2], pane->viewportMatrix()[3]);
// 		printf("Viewport for pane '%s' is %i %i %i %i\n" , qPrintable(pane->name()), pane->viewportMatrix()[0], pane->viewportMatrix()[1], pane->viewportMatrix()[2], pane->viewportMatrix()[3]);

		// Setup an orthographic matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, pane->viewportMatrix()[2], 0, pane->viewportMatrix()[3], -10, 10);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_LIGHTING);

		// Draw graduated background for current pane (only if rendering on-screen)
		if ((pane == UChromaSession::currentViewPane()) && (!renderingOffScreen_))
		{
			glBegin(GL_QUADS);
			glColor4fv(colourBlue);
			glVertex3i(0, 0, 0);
			glVertex3i(pane->viewportMatrix()[2]-1, 0, 0);
			glColor4fv(colourWhite);
			glVertex3i(pane->viewportMatrix()[2]-1, pane->viewportMatrix()[3]*0.5, 0);
			glVertex3i(0, pane->viewportMatrix()[3]*0.5, 0);
			glEnd();
		}

		// Draw a box around the pane
		if (!renderingOffScreen_)
		{
			glColor4fv(colourGray);
			glBegin(GL_LINE_LOOP);
			glVertex3i(0, 0, 1);
			glVertex3i(0, pane->viewportMatrix()[3]-1, 1);
			glVertex3i(pane->viewportMatrix()[2]-1, pane->viewportMatrix()[3]-1, 1);
			glVertex3i(pane->viewportMatrix()[2]-1, 0, 1);
			glEnd();
		}

		// Set projection matrix
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixd(pane->projectionMatrix().matrix());

		// Set modelview matrix as target for the remainder of the routine
		glMatrixMode(GL_MODELVIEW);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Get the pane's view matrix
		Matrix viewMatrix = pane->viewMatrix();

		// Send axis primitives to the display first
		glLoadMatrixd(viewMatrix.matrix());
		glColor4fv(colourBlack);
		int skipAxis = -1;
		if (pane->viewType() == ViewPane::FlatXYView) skipAxis = 2;
		else if (pane->viewType() == ViewPane::FlatXZView) skipAxis = 1;
		else if (pane->viewType() == ViewPane::FlatZYView) skipAxis = 0;

		// -- Render axis text
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		if (FontInstance::fontOK())
		{
			FontInstance::font()->FaceSize(1);
			for (axis=0; axis<3; ++axis) if (pane->axes().visible(axis) && (axis != skipAxis))
			{
				pane->axes().labelPrimitive(axis).renderAll(viewMatrix, pane->flatLabels(), pane->textZScale());
				if (updateQueryDepth()) setQueryObject(Viewer::AxisTickLabelObject, QString::number(axis));
				pane->axes().titlePrimitive(axis).renderAll(viewMatrix, pane->flatLabels(), pane->textZScale());
				if (updateQueryDepth()) setQueryObject(Viewer::AxisTitleLabelObject, QString::number(axis));
			}
		}

		// -- Render axis (and grid) lines
		glLoadMatrixd(viewMatrix.matrix());
		glDisable(GL_LIGHTING);
		glEnable(GL_LINE_SMOOTH);
		for (axis=0; axis<3; ++axis) if (pane->axes().visible(axis) && (axis != skipAxis))
		{
			pane->axes().gridLineMinorStyle(axis).apply();
			pane->axes().gridLineMinorPrimitive(axis).sendToGL();
			if (updateQueryDepth()) setQueryObject(Viewer::GridLineMinorObject, QString::number(axis));
		}
		for (axis=0; axis<3; ++axis) if (pane->axes().visible(axis) && (axis != skipAxis))
		{
			pane->axes().gridLineMajorStyle(axis).apply();
			pane->axes().gridLineMajorPrimitive(axis).sendToGL();
			if (updateQueryDepth()) setQueryObject(Viewer::GridLineMajorObject, QString::number(axis));
		}
		LineStyle::revert();
		for (axis=0; axis<3; ++axis) if (pane->axes().visible(axis) && (axis != skipAxis))
		{
			pane->axes().axisPrimitive(axis).sendToGL();
			if (updateQueryDepth()) setQueryObject(Viewer::AxisLineObject, QString::number(axis));
		}
		glEnable(GL_LIGHTING);
		glDisable(GL_LINE_SMOOTH);

		// Render bounding box
		pane->boundingBoxPrimitive().sendToGL();

		// Render selection markers (if needed)
		glLoadMatrixd(viewMatrix.matrix());
		int interactionAxis = uChromaWindow_->interactionAxis();
		if ((pane == UChromaSession::currentViewPane()) && (interactionAxis != -1))
		{
			// Note - we do not need to check for inverted or logarithmic axes here, since the transformation matrix A takes care of that
			Vec3<double> v;

			// Draw starting interaction point (if the interaction has been started)
			if (uChromaWindow_->interactionStarted())
			{
				v[interactionAxis] = uChromaWindow_->clickedInteractionCoordinate() * pane->axes().stretch(interactionAxis);
				glTranslated(v.x, v.y, v.z);
				glColor4d(0.0, 0.0, 1.0, 0.5);
				pane->interactionPrimitive().sendToGL();
				pane->interactionBoxPrimitive().sendToGL();
			}

			// Draw current selection position
			v[interactionAxis] = 	uChromaWindow_->currentInteractionCoordinate() * pane->axes().stretch(interactionAxis) - v[interactionAxis];
			glTranslated(v.x, v.y, v.z);
			glColor4d(1.0, 0.0, 0.0, 0.5);
			pane->interactionPrimitive().sendToGL();
			pane->interactionBoxPrimitive().sendToGL();
		}

		// Setup clip planes to enforce Y-axis limits
		glLoadMatrixd(viewMatrix.matrix());
		glPushMatrix();
		glTranslated(0.0, pane->axes().clipPlaneYMin(), 0.0);
		glClipPlane(GL_CLIP_PLANE0, clipPlaneBottom);
		glEnable(GL_CLIP_PLANE0);
		glPopMatrix();
		glPushMatrix();
		glTranslated(0.0, pane->axes().clipPlaneYMax(), 0.0);
		glClipPlane (GL_CLIP_PLANE1, clipPlaneTop);
		glEnable(GL_CLIP_PLANE1);
		glPopMatrix();

		// Render pane data - loop over collection targets
		for (TargetData* target = pane->collectionTargets(); target != NULL; target = target->next)
		{
			// If this is the collection to highlight, set color to transparent grey and disable material colouring....
			if (target->collection() == highlightCollection_)
			{
				glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
				glDisable(GL_COLOR_MATERIAL);
			}

			// Set shininess for collection
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, target->collection()->displaySurfaceShininess());

			// Loop over display primitives in this target...
			for (TargetPrimitive* primitive = target->displayPrimitives(); primitive != NULL; primitive = primitive->next)
			{
				// Make sure the primitive is up to date and send it to GL
				primitive->updateAndSendPrimitive(pane->axes(), renderingOffScreen_, renderingOffScreen_, context());
			}

			// Update query
			if (updateQueryDepth()) setQueryObject(Viewer::CollectionObject, target->collection()->locator());

			glEnable(GL_COLOR_MATERIAL);
		}

		// Disable current clip planes
		glDisable(GL_CLIP_PLANE0);
		glDisable(GL_CLIP_PLANE1);

		// Render toolbar?
// 		// Setup an orthographic matrix
// 		glMatrixMode(GL_PROJECTION);
// 		glLoadIdentity();
// 		glOrtho(0, pane->viewportMatrix()[2], 0, pane->viewportMatrix()[3], -10, 10);
// 		glClear(GL_DEPTH_BUFFER_BIT);
// 
// 		glMatrixMode(GL_MODELVIEW);
// 		glLoadIdentity();
// 		glColor4fv(colourTransparentGray);
// 		glBegin(GL_LINE_LOOP);
// 		glVertex2i(pane->viewportMatrix()[2]-2, pane->viewportMatrix()[3]-2);
// 		glVertex2i(pane->viewportMatrix()[2]-2, pane->viewportMatrix()[3]-16);
// 		glVertex2i(pane->viewportMatrix()[2]-16, pane->viewportMatrix()[3]-16);
// 		glVertex2i(pane->viewportMatrix()[2]-16, pane->viewportMatrix()[3]-2);
// 		glEnd();
// 		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
// 		glBegin(GL_LINE_LOOP);
// 		glVertex2i(pane->viewportMatrix()[2]-2, pane->viewportMatrix()[3]-2);
// 		glVertex2i(pane->viewportMatrix()[2]-2, pane->viewportMatrix()[3]-6);
// 		glVertex2i(pane->viewportMatrix()[2]-16, pane->viewportMatrix()[3]-6);
// 		glVertex2i(pane->viewportMatrix()[2]-16, pane->viewportMatrix()[3]-2);
// 		glEnd();
	}

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
