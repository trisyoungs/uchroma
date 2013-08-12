/*
	*** Viewer Functions
	*** src/gui/viewer_funcs.cpp
	Copyright T. Youngs 2013

	This file is part of FQPlot.

	FQPlot is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	FQPlot is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FQPlot.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/viewer.uih"
#include "base/messenger.h"

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

	// Character / Setup
	contextWidth_ = 0;
	contextHeight_ = 0;
	valid_ = false;
	drawing_ = false;
	renderingOffscreen_ = false;
	hasPerspective_ = false;
	sliceSelector_.set(false, false, false);
	sliceAxisValue_ = 0.0;

	// Engine Setup
	triangleChopper_.initialise(0.0, 1000, 0.2);
	createPrimitives();
	viewMatrix_[14] = -5.0;
	font_ = NULL;
	clipPlaneDelta_ = 0.0001;
	clipPlaneBottom_[0] = 0.0;
	clipPlaneBottom_[1] = 1.0;
	clipPlaneBottom_[2] = 0.0;
	clipPlaneBottom_[3] = 0.0;
	clipPlaneTop_[0] = 0.0;
	clipPlaneTop_[1] = -1.0;
	clipPlaneTop_[2] = 0.0;
	clipPlaneTop_[3] = 0.0;
	clipPlaneYMin_ = 0.0;
	clipPlaneYMax_ = 0.0;
	correctTransparency_ = true;
	useFrameBuffer_ = false;
	lineWidth_ = 2.0;

	// Prevent QPainter from autofilling widget background
	setAutoFillBackground(false);
}

// Destructor
Viewer::~Viewer()
{
}

/*
// Protected Qt Virtuals
*/

// Initialise context widget (when created by Qt)
void Viewer::initializeGL()
{
	// Initialize GL
	msg.enter("Viewer::initializeGL");
	valid_ = TRUE;
	
	// Create an instance for each defined user primitive - we do this in every call to initialiseGL so
	// that, when saving a bitmap using QGLWidget::renderPixmap(), we automatically create new display list
	// objects, rather than having to worry about context sharing etc. Slow, but safer and more compatible.
	msg.print("In Viewer::initializeGL, pushing instances for %i primitives...\n", primitiveList_.nItems());
	for (RefListItem<Primitive,int> *ri = primitiveList_.first(); ri != NULL; ri = ri->next) ri->item->pushInstance(context());

	msg.exit("Viewer::initializeGL");
}

void Viewer::paintGL()
{
	msg.enter("Viewer::paintGL");

	// Do nothing if the canvas is not valid, or we are still drawing from last time.
	if ((!valid_) || drawing_)
	{
		msg.exit("Viewer::paintGL");
		return;
	}

	// Set the drawing flag so we don't have any rendering clashes
	drawing_ = true;

	// Setup basic GL stuff
	setupGL();

	// Clear view
	msg.print(Messenger::Verbose, " --> Clearing context, background, and setting pen colour\n");
	glViewport(0,0,contextWidth_,contextHeight_);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	GLfloat colourBlack[4] = { 0.0, 0.0, 0.0, 1.0 };

	// Set colour mode
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);

	// Prepare for model rendering
	glMatrixMode(GL_PROJECTION);
	setProjectionMatrix();
	glLoadMatrixd(projectionMatrix_.matrix());
	glMatrixMode(GL_MODELVIEW);

	// Set up our transformation matrix
	Matrix A;
	A.setIdentity();
	A.setTranslation(-surfaceCenter_.x, -surfaceCenter_.y, -surfaceCenter_.z);
	A = viewMatrix_ * A;
	
	// Send axis primitives to the display first
	glLoadMatrixd(A.matrix());
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glColor4fv(colourBlack);
	// -- Render axis text
	if (font_)
	{
		font_->FaceSize(1);
		textPrimitives_.renderAll(viewMatrix_, -surfaceCenter_, font_);
		axisTextPrimitives_[0].renderAll(viewMatrix_, -surfaceCenter_, font_);
		axisTextPrimitives_[1].renderAll(viewMatrix_, -surfaceCenter_, font_);
		axisTextPrimitives_[2].renderAll(viewMatrix_, -surfaceCenter_, font_);
	}

	// -- Render axis lines
	glEnable(GL_LINE_SMOOTH);
	glLoadMatrixd(A.matrix());
	glLineWidth(lineWidth_);
	glDisable(GL_LIGHTING);
	for (int axis=0; axis<3; ++axis) if (axisVisible_[axis]) axisPrimitives_[axis].sendToGL();
	glEnable(GL_LIGHTING);
	glDisable(GL_LINE_SMOOTH);

	// Render bounding box
	boundingBoxPrimitive_.sendToGL();

	// Render locator slices
	glLoadMatrixd(A.matrix());
	slicePrimitive_.clear();
	glColor4f(0.0, 0.0, 0.0, 0.5);
	Vec3<double> u, v, s, t, normal, extension = (axisMax_-axisMin_)*0.25;
	for (int axis=0; axis<3; ++axis)
	{
		if (!sliceSelector_[axis]) continue;

		v = axisMin_ - extension;
		u = axisMax_ + extension;
		v[axis] = sliceAxisValue_[axis];
		u[axis] = sliceAxisValue_[axis];
		s = u;
		s[(axis+1)%3] = v[(axis+1)%3];
		t = u;
		t[(axis+2)%3] = v[(axis+2)%3];
		normal.zero();
		normal[axis] = 1.0;
		slicePrimitive_.defineVertex(u.x, u.y, u.z, normal.x, normal.y, normal.z, true);
		slicePrimitive_.defineVertex(v.x, v.y, v.z, normal.x, normal.y, normal.z, true);
		slicePrimitive_.defineVertex(s.x, s.y, s.z, normal.x, normal.y, normal.z, true);
		slicePrimitive_.defineVertex(u.x, u.y, u.z, normal.x, normal.y, normal.z, true);
		slicePrimitive_.defineVertex(v.x, v.y, v.z, normal.x, normal.y, normal.z, true);
		slicePrimitive_.defineVertex(t.x, t.y, t.z, normal.x, normal.y, normal.z, true);
		slicePrimitive_.sendToGL();
	}

	// Render main surface
	// -- Setup clip planes to enforce Y-axis limits
	glLoadMatrixd(A.matrix());
	glPushMatrix();
	glTranslated(0.0, clipPlaneYMin_, 0.0);
	glClipPlane (GL_CLIP_PLANE0, clipPlaneBottom_);
	glEnable(GL_CLIP_PLANE0);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0.0, clipPlaneYMax_, 0.0);
	glClipPlane (GL_CLIP_PLANE1, clipPlaneTop_);
	glEnable(GL_CLIP_PLANE1);
	glPopMatrix();
	
// 	triangleChopper_.emptyTriangles();
// 	triangleChopper_.storeTriangles(&surface_, A);
// 	glLoadIdentity();
// 	glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
// 	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
// 	triangleChopper_.sendToGL();
// 	glPopClientAttrib();

	surfacePrimitive_.sendToGL();
	
	glDisable(GL_MULTISAMPLE);
	glDisable(GL_CLIP_PLANE0);
	glDisable(GL_CLIP_PLANE1);

	// Send all other listed primitives to the display
// 	sortAndSendGL();
// 		// Transform and render each transparent primitive in each list, unless correctTransparency_ is off.
// 	if (correctTransparency_)
// 	{
// 		triangleChopper_.emptyTriangles();
// 		for (PrimitiveInfo *pi = transparentPrimitives_.first(); pi != NULL; pi = pi->next)
// 		{
// 			triangleChopper_.storeTriangles(pi, viewMatrix_);
// 		}
// 		glLoadIdentity();
// 		glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
// 		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
// 		triangleChopper_.sendToGL();
// 		glPopClientAttrib();
// 	}
// 	else for (PrimitiveInfo *pi = transparentPrimitives_.first(); pi != NULL; pi = pi->next)
// 	{
// 		// Grab primitive pointer
// 		prim = pi->primitive();
// 		if (prim == NULL) continue;
// 		if (!prim->colouredVertexData()) glColor4fv(pi->colour());
// 		A = viewMatrix_ * pi->localTransform();
// 		glLoadMatrixd(A.matrix());
// 		prim->sendToGL();
// 	}
	
	// Set the rendering flag to false
	drawing_ = false;

	// If we were rendering offscreen, we may delete the topmost primitive instance here
	if (renderingOffscreen_)
	{
		msg.print("In Viewer::PaintGL, popping instances for %i primitives...\n", primitiveList_.nItems());
		for (RefListItem<Primitive,int> *ri = primitiveList_.first(); ri != NULL; ri = ri->next) ri->item->popInstance(context());
	}
	
	msg.exit("Viewer::paintGL");
}

// Resize function
void Viewer::resizeGL(int newwidth, int newheight)
{
	// Store the new width and height of the widget
	contextWidth_ = (GLsizei) newwidth;
	contextHeight_ = (GLsizei) newheight;
	setProjectionMatrix();
}

/*
// Character / Setup
*/

// Setup font specified
bool Viewer::setupFont(QString fontName)
{
	// If the current font is valid, and matches the name of the new font supplied, do nothing
	if (font_ && (fontFile_ == fontName)) return true;

	// Prevent anything from being drawn while we change the font
	drawing_ = true;
	
	if (font_) delete font_;
	font_ = NULL;
	fontFile_ = fontName;

	// Check if the file exists
	if (!QFile::exists(fontFile_))
	{
		QMessageBox::warning(this, "Font Error", "The specified font file '" + fontFile_ + "' does not exist.");
		drawing_ = false;
		return false;
	}

	FTPolygonFont* newFont = new FTPolygonFont(qPrintable(fontName));
	if (newFont->Error())
	{
		QMessageBox::warning(this, "Font Error", "Error creating primitives from '" + fontFile_ + "'.");
		delete newFont;
		fontBaseHeight_ = 1.0;
	}
	else
	{
		font_ = newFont;
// 		font_->Depth(3.0);
// 		font_->Outset(-.5, 1.5);
		font_->FaceSize(1);
		FTBBox boundingBox = font_->BBox("Hello");
		fontBaseHeight_ = boundingBox.Upper().Yf() - boundingBox.Lower().Yf();

	}

	drawing_ = false;
	return (font_ != NULL);
}

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
			case (GL_INVALID_VALUE): msg.print(Messenger::Verbose, "N(umeric argument out of range\n"); break;
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

// Setup projection matrix
void Viewer::setProjectionMatrix(double perspectiveFov)
{
	// Create projection matrix for model
	GLdouble top, bottom, right, left, aspect = (GLdouble) contextWidth_ / (GLdouble) contextHeight_;
	GLdouble nearClip = 0.5, farClip = 2000.0;

	// Set viewport matrix here
	viewportMatrix_[0] = 0;
	viewportMatrix_[1] = 0;
	viewportMatrix_[2] = contextWidth_;
	viewportMatrix_[3] = contextHeight_;

	if (hasPerspective_)
	{
		// Use reversed top and bottom values so we get y-axis (0,1,0) pointing up
		top = tan(perspectiveFov / DEGRAD) * 0.5;
		bottom = -top;
		left = -aspect*top;
		right = aspect*top;
		projectionMatrix_.setColumn(0, (nearClip*2.0) / (right-left), 0.0, 0.0, 0.0);
		projectionMatrix_.setColumn(1, 0.0, (nearClip*2.0) / (top-bottom), 0.0, 0.0);
		projectionMatrix_.setColumn(2, (right+left)/(right-left), (top+bottom)/(top-bottom), -(farClip+nearClip)/(farClip-nearClip), -1.0);
		projectionMatrix_.setColumn(3, 0.0, 0.0, -(2.0*nearClip*farClip) / (farClip-nearClip), 0.0);
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
		top = -tan(perspectiveFov / DEGRAD) * viewMatrix_[14];
		bottom = -top;
		left = -aspect*top;
		right = aspect*top;

		projectionMatrix_.setColumn(0, 2.0 / (right-left), 0.0, 0.0, (right+left)/(right-left));
		projectionMatrix_.setColumn(1, 0.0, 2.0 / (top-bottom), 0.0, (top+bottom)/(top-bottom));
		projectionMatrix_.setColumn(2, 0.0, 0.0, -1.0/farClip, 0.0);
		projectionMatrix_.setColumn(3, 0.0, 0.0, 0.0, 1.0);
		// Equivalent to the following code:
		// glMatrixMode(GL_PROJECTION);
		// glLoadIdentity();
		// top = tan(prefs.perspectiveFov() / DEGRAD) * prefs.clipNear();
		// bottom = -top;
		// glOrtho(aspect*top, aspect*bottom, top, bottom, -prefs.clipFar(), prefs.clipFar());
		// glGetDoublev(GL_PROJECTION_MATRIX, modelProjectionMatrix_.matrix());
	}
}
                
// Update transformation (view) matrix
void Viewer::setViewMatrix(Matrix &mat)
{
	viewMatrix_ = mat;
}
// Return transformation (view) matrix
Matrix Viewer::viewMatrix()
{
	return viewMatrix_;
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

// Project given model coordinates into world coordinates
Vec3<double> Viewer::modelToWorld(Vec3<double> &modelr)
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
Vec4<double> Viewer::modelToScreen(Vec3<double> &modelr, double screenradius)
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
	vmat.applyTranslation(-surfaceCenter_.x, -surfaceCenter_.y, -surfaceCenter_.z);
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

// Convert screen coordinates into model space coordinates
Vec3<double> Viewer::screenToModel(int x, int y, double z)
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
