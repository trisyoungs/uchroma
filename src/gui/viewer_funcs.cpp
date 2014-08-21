/*
	*** Viewer Functions
	*** src/gui/viewer_funcs.cpp
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

#include "gui/viewer.uih"
#include "gui/uchroma.h"
#include "base/messenger.h"
#include "render/glextensions.h"
#include "render/fontinstance.h"

/*
 * Image Formats
 */

// Available image export formats
const char *imageFormatFilters[Viewer::nImageFormats] = { "Windows Image (*.bmp)", "Joint Photographic Experts Group (*.jpg)", "Portable Network Graphics (*.png)", "Portable Pixmap (*.ppm)", "X11 Bitmap (*.xbm)", "X11 Pixmap (*.xpm)" };
const char *imageFormatExtensions[Viewer::nImageFormats] = { "bmp", "jpg", "png", "ppm", "xbm", "xpm" };
Viewer::ImageFormat Viewer::imageFormat(const char *s)
{
	for (int n=0; n<Viewer::nImageFormats; ++n) if (strcmp(s, imageFormatExtensions[n]) == 0) return (Viewer::ImageFormat) n;
	return Viewer::nImageFormats;
}
Viewer::ImageFormat Viewer::imageFormatFromFilter(const char *s)
{
	for (int n=0; n<Viewer::nImageFormats; ++n) if (strcmp(s, imageFormatFilters[n]) == 0) return (Viewer::ImageFormat) n;
	return Viewer::nImageFormats;
}
const char *Viewer::imageFormatFilter(Viewer::ImageFormat bf)
{
        return imageFormatFilters[bf];
}
const char *Viewer::imageFormatExtension(Viewer::ImageFormat bf)
{
        return imageFormatExtensions[bf];
}

// Constructor
Viewer::Viewer(QWidget *parent) : QGLWidget(parent)
{
	// Setup context format
	QGLFormat format;
	format.setSampleBuffers(true);
	format.setDirectRendering(true);
	setFormat(format);

	uChroma_ = NULL;

	// Character / Setup
	contextWidth_ = 0;
	contextHeight_ = 0;
	valid_ = false;
	drawing_ = false;
	renderingOffscreen_ = false;

	// Engine Setup
	correctTransparency_ = false;
	useFrameBuffer_ = false;
	lineWidth_ = 2.0;

	// Prevent QPainter from autofilling widget background
	setAutoFillBackground(false);
}

// Destructor
Viewer::~Viewer()
{
}

// Set UChromaWindow pointer
void Viewer::setUChroma(UChromaWindow* ptr)
{
	uChroma_ = ptr;
}

/*
// Protected Qt Virtuals
*/

// Initialise context widget (when created by Qt)
void Viewer::initializeGL()
{
	// Initialize GL
	msg.enter("Viewer::initializeGL");
	valid_ = true;

	// Create a GLExtensions object to probe features and give when pushing instances etc.
	GLExtensions* extensions = extensionsStack_.add();

	// Check for vertex buffer extensions
	if ((!extensions->hasVBO()) && (PrimitiveInstance::globalInstanceType() == PrimitiveInstance::VBOInstance))
	{
		printf("VBO extension is requested but not available, so reverting to display lists instead.\n");
		PrimitiveInstance::setGlobalInstanceType(PrimitiveInstance::ListInstance);
	}

	// Create an instance for each defined user primitive - we do this in every call to initialiseGL so
	// that, when saving a bitmap using QGLWidget::renderPixmap(), we automatically create new display list
	// objects, rather than having to worry about context sharing etc. Slow, but safer and more compatible.
// 	msg.print("In Viewer::initializeGL, pushing instances for %i primitives...\n", primitiveList_.nItems());
	for (RefListItem<Primitive,int> *ri = primitiveList_.first(); ri != NULL; ri = ri->next) ri->item->pushInstance(context(), extensions);

	// Recreate surface primitives (so that images are saved correctly)
	for (Collection* collection = uChroma_->collections(); collection != NULL; collection = collection->next) updateSurfacePrimitive(collection, true);

	// Recalculate view layouts
	uChroma_->recalculateViewLayout(contextWidth_, contextHeight_);

	msg.exit("Viewer::initializeGL");
}

void Viewer::paintGL()
{
	msg.enter("Viewer::paintGL");

	// Do nothing if the canvas is not valid, or we are still drawing from last time, or the uChroma pointer has not been set
	if ((!valid_) || drawing_ || (!uChroma_))
	{
		msg.exit("Viewer::paintGL");
		return;
	}

	// Set the drawing flag so we don't have any rendering clashes
	drawing_ = true;

	// Grab topmost GLExtensions pointer
	GLExtensions* extensions = extensionsStack_.last();

	// Update / recreate axes, display data and surface primitives if necessary
	//uChroma_->updateAxesPrimitives();
	uChroma_->updateDisplayData();
	int nUpdated = 0;
	for (Collection* collection = uChroma_->collections(); collection != NULL; collection = collection->next) if (updateSurfacePrimitive(collection)) ++nUpdated;
	if (nUpdated > 0) emit(surfacePrimitivesUpdated());

	// Setup basic GL stuff
	setupGL();

	// Set colour mode
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);

	// Clear view
	msg.print(Messenger::Verbose, " --> Clearing context, background, and setting pen colour\n");
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Create a query object to get timing information
	GLuint timeQuery = 0;
	if (extensions->hasQueries())
	{
		extensions->glGenQueries(1, &timeQuery);
		extensions->glBeginQuery(GL_TIME_ELAPSED, timeQuery);
	}
	
	// Loop over defined viewpanes
	GLdouble clipPlaneBottom[4] = { 0.0, 1.0, 0.0, 0.0 }, clipPlaneTop[4] = { 0.0, -1.0, 0.0, 0.0 };
	for (ViewPane* pane = uChroma_->viewLayout()->panes(); pane != NULL; pane = pane->next)
	{
		// Set viewport
		glViewport(pane->viewportMatrix()[0], pane->viewportMatrix()[1], pane->viewportMatrix()[2], pane->viewportMatrix()[3]);

		// Setup an orthographic matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, pane->viewportMatrix()[2], 0, pane->viewportMatrix()[3], -10, 10);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_LIGHTING);
		GLfloat colourGray[4] = { 0.8, 0.8, 0.8, 1.0 };
		GLfloat colourBlue[4] = { 0.88, 0.95, 1.0, 1.0 };
		GLfloat colourWhite[4] = { 1.0, 1.0, 1.0, 1.0 };

		// Draw graduated background for current pane
		if (pane == uChroma_->currentViewPane())
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
		glColor4fv(colourGray);
		glBegin(GL_LINE_LOOP);
		glVertex3i(0, 0, 1);
		glVertex3i(0, pane->viewportMatrix()[3]-1, 1);
		glVertex3i(pane->viewportMatrix()[2]-1, pane->viewportMatrix()[3]-1, 1);
		glVertex3i(pane->viewportMatrix()[2]-1, 0, 1);
		glEnd();

		// Set projection matrix
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixd(pane->projectionMatrix().matrix());
		glClear(GL_DEPTH_BUFFER_BIT);

		// Set modelview matrix as target for the remainder of the routine
		glMatrixMode(GL_MODELVIEW);

		// Set up our transformation matrix
		Vec3<double> centreTranslation = -pane->axes().axesCoordCentre();
		Matrix A, viewMatrix = pane->viewMatrix();
		A.setIdentity();
		A.setTranslation(centreTranslation);
		A = viewMatrix * A;
		
		// Send axis primitives to the display first
		glLoadMatrixd(A.matrix());
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		GLfloat colourBlack[4] = { 0.0, 0.0, 0.0, 1.0 };
		glColor4fv(colourBlack);
		int maxAxis = (pane->twoDimensional() ? 2 : 3);

		// -- Render axis text
		if (FontInstance::fontOK())
		{
			FontInstance::font()->FaceSize(1);
			for (int n=0; n<maxAxis; ++n) if (pane->axes().axisVisible(n)) pane->axes().axisTextPrimitive(n).renderAll(viewMatrix, centreTranslation, FontInstance::font(), uChroma_->labelCorrectOrientation(), pane->textZScale());
		}

		// -- Render axis lines
		glEnable(GL_LINE_SMOOTH);
		glLoadMatrixd(A.matrix());
		glLineWidth(lineWidth_);
		glDisable(GL_LIGHTING);
		for (int axis=0; axis<maxAxis; ++axis) if (pane->axes().axisVisible(axis)) pane->axes().axisPrimitive(axis).sendToGL();
		glEnable(GL_LIGHTING);
		glDisable(GL_LINE_SMOOTH);

		// Render bounding box
		pane->boundingBoxPrimitive().sendToGL();

		// Render current selection marker
		glLoadMatrixd(A.matrix());
		int sliceAxis = uChroma_->interactionAxis();
		if ((pane == uChroma_->currentViewPane()) && (sliceAxis != -1))
		{
			// Note - we do not need to check for inverted or logarithmic axes here, since the transformation matrix A takes care of that
			Vec3<double> v;

			// Draw starting interaction point (if we are interacting)
			if (uChroma_->interacting())
			{
				v[sliceAxis] = uChroma_->clickedInteractionCoordinate() * pane->axes().axisStretch(sliceAxis);
				glTranslated(v.x, v.y, v.z);
				glColor4d(0.0, 0.0, 1.0, 0.5);
				pane->interactionPrimitive().sendToGL();
				pane->interactionBoxPrimitive().sendToGL();
			}

			// Draw current selection position
			v[sliceAxis] = 	uChroma_->currentInteractionCoordinate() * pane->axes().axisStretch(sliceAxis) - v[sliceAxis];
			glTranslated(v.x, v.y, v.z);
			glColor4d(1.0, 0.0, 0.0, 0.5);
			pane->interactionPrimitive().sendToGL();
			pane->interactionBoxPrimitive().sendToGL();
		}

		// Setup clip planes to enforce Y-axis limits
		glLoadMatrixd(A.matrix());
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

		// Render relevant data to this pane
		switch (pane->role())
		{
			// Slice Monitor Pane - Render monitor slices for all target collections
			case (ViewPane::SliceMonitorRole):
				for (RefListItem<Collection,bool>* ri = pane->roleTargetCollections(); ri != NULL; ri = ri->next)
				{
					Collection* collection = ri->item;
					if (collection->currentSlice())
					{
						collection->currentSlice()->sendToGL();
						printf("XXXX : %i  %i\n", collection->currentSlice()->displayPrimitivesValid(), collection->currentSlice()->displayPrimitives().nDefinedVertices());
						printf("Sent current slice for collecion '%s' to the display...\n", qPrintable(collection->title()));
					}
				}
				break;
			// Standard Pane - Render all associated collections
			case (ViewPane::StandardRole):
				for (RefListItem<Collection,bool>* ri = pane->collections(); ri != NULL; ri = ri->next) ri->item->sendToGL();
// 					glDisable(GL_MULTISAMPLE);
				break;
			default:
				msg.print("No data to be rendered for target pane?\n");
				break;
		}

		// Disable current clip planes
		glDisable(GL_CLIP_PLANE0);
		glDisable(GL_CLIP_PLANE1);

	}

	// End timer query
	if (extensions->hasQueries())
	{
		extensions->glEndQuery(GL_TIME_ELAPSED);

		// Wait for all results to become available
		GLint available = 0;
		GLuint64 timeElapsed = 0;
		while (!available) { extensions->glGetQueryObjectiv(timeQuery, GL_QUERY_RESULT_AVAILABLE, &available); }

		// Time taken is in ns, so convert to ms and write to a string
		extensions->glGetQueryObjectui64v(timeQuery, GL_QUERY_RESULT, &timeElapsed);
		double ms = timeElapsed / 1.0e6;
		renderTime_.sprintf("%0.2f ms", ms);
		emit(renderComplete(renderTime_));
	}
	else renderTime_ = "";

	// Set the rendering flag to false
	drawing_ = false;

	// If we were rendering offscreen, we may delete the topmost primitive instance and GLExtensions objects here
	if (renderingOffscreen_)
	{
// 		msg.print("In Viewer::PaintGL, popping instances for %i primitives...\n", primitiveList_.nItems());
		for (RefListItem<Primitive,int> *ri = primitiveList_.first(); ri != NULL; ri = ri->next) ri->item->popInstance(context());
		extensionsStack_.removeLast();
	}
	
	msg.exit("Viewer::paintGL");
}

// Resize function
void Viewer::resizeGL(int newwidth, int newheight)
{
	// Store the new width and height of the widget
	contextWidth_ = (GLsizei) newwidth;
	contextHeight_ = (GLsizei) newheight;

	// Recalculate viewlayouts
	uChroma_->recalculateViewLayout(contextWidth_, contextHeight_);
}

// Setup basic GL properties (called each time before renderScene())
void Viewer::setupGL()
{
	msg.enter("Viewer::setupGL");

	// Define colours etc.
	GLfloat backgroundColour[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat spotlightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat spotlightDiffuse[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat spotlightSpecular[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat spotlightPosition[4] = { 100.0f, 100.0f, 0.0f, 0.0f };
	GLfloat specularColour[4] = { 0.9f, 0.9f, 0.9f, 1.0f };

	// Clear (background) colour
	glClearColor(backgroundColour[0], backgroundColour[1], backgroundColour[2], backgroundColour[3]);

	// Perspective hint
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);

	// Smooth shading
	glShadeModel(GL_SMOOTH);

	// Auto-normalise surface normals
	glEnable(GL_NORMALIZE);	

	// Set up the light model
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, spotlightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, spotlightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spotlightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, spotlightPosition);
	glEnable(GL_LIGHT0);

	// Set specular reflection colour
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColour);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 127);

	// Configure antialiasing
	glDisable(GL_MULTISAMPLE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	// Configure fog effects
//	glFogi(GL_FOG_MODE, GL_LINEAR);
//	prefs.copyColour(Prefs::BackgroundColour, col);
//	glFogfv(GL_FOG_COLOR, col);
//	glFogf(GL_FOG_DENSITY, 0.35f);
//	glHint(GL_FOG_HINT, GL_NICEST);
//	glFogi(GL_FOG_START, prefs.depthNear());
//	glFogi(GL_FOG_END, prefs.depthFar());
//	glEnable(GL_FOG);
	glDisable(GL_FOG);

	// Configure face culling
// 	glCullFace(GL_BACK);
// 	glEnable(GL_CULL_FACE);

	msg.exit("Viewer::setupGL");
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

// Probe features
void Viewer::probeFeatures()
{
	QGLFormat fmt = context()->format();
	// Probe this format!
	printf(" QGLFormat: Alpha buffer is %s.\n", fmt.alpha() ? "enabled" : "disabled");
	printf(" QGLFormat: Accumulation buffer is %s.\n", fmt.accum() ? "enabled" : "disabled");
	printf(" QGLFormat: Depth buffer is %s.\n", fmt.depth() ? "enabled" : "disabled");
	printf(" QGLFormat: Double-buffering is %s.\n", fmt.doubleBuffer() ? "enabled" : "disabled");
	printf(" QGLFormat: Direct rendering is %s.\n", fmt.directRendering() ? "enabled" : "disabled");
	printf(" QGLFormat: RGBA colour mode is %s.\n", fmt.rgba() ? "enabled" : "disabled");
	printf(" QGLFormat: Multisample buffer is %s.\n", fmt.sampleBuffers() ? "enabled" : "disabled");
	printf(" QGLFormat: Stencil buffer is %s.\n", fmt.stencil() ? "enabled" : "disabled");
	printf(" QGLWidget: Autoswap buffers is %s.\n", autoBufferSwap() ? "enabled" : "disabled");
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

// Render or grab image
QPixmap Viewer::generateImage(int w, int h)
{
	renderingOffscreen_ = true;
	if (useFrameBuffer_)
	{
		postRedisplay();
		QImage image = grabFrameBuffer();

		renderingOffscreen_ = false;
		return QPixmap::fromImage(image);
	}
	else
	{
		// Scale current line width to reflect size of exported image
		double oldLineWidth = lineWidth_;
		lineWidth_ *= double(w) / width();

		// Generate offscreen bitmap (a temporary context will be created)
		QPixmap pixmap = renderPixmap(w, h, false);
		
		// Ensure correct widget context size is stored
		contextWidth_ = (GLsizei) width();
		contextHeight_ = (GLsizei) height();

		// Reset everything back to normal
		lineWidth_ = oldLineWidth;
		renderingOffscreen_ = false;

		return pixmap;
	}
}

