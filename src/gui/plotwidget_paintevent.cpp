/*
	*** PlotWidget PaintEvent
	*** src/plotwidget_paintevent.cpp
	Copyright T. Youngs 2012-2013

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

#include <QtOpenGL/QGLWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QMouseEvent>
#include "plotwidget.uih"
#include <math.h>
#include <algorithm>

/*!
 * \brief Widget PaintEvent
 */
void PlotWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	// Setup global transform for widget
	globalTransform_.reset();
	areaHeight_ = painter.device()->height();
	areaWidth_ = painter.device()->width();
	renderPlot(painter);

	// Done.
	painter.end();
}

// Create image of current graph
void PlotWidget::draw(QPainter& painter, QRect boundingRect)
{
	// Set up transform to shift image to correct place on graph
	globalTransform_.reset();
	globalTransform_.translate(boundingRect.left(), boundingRect.top());
	areaWidth_ = boundingRect.width();
	areaHeight_ = boundingRect.height();
	renderPlot(painter);
	painter.setTransform(QTransform());
}

// Draw plot on specified image
void PlotWidget::draw(QImage& image)
{
	QPainter painter(&image);
	globalTransform_.reset();
	areaWidth_ = image.width();
	areaHeight_ = image.height();
	renderPlot(painter);
	painter.end();
}

// Render plot
void PlotWidget::renderPlot(QPainter& painter)
{
	// Setup plot variables etc.
	plotSetup(painter);
	drawGridLines(painter);
	drawData(painter);
	// Reset transform to cover whole area again
	painter.setTransform(globalTransform_);
	painter.setClipping(false);
	painter.setPen(foregroundColour_);
	painter.setRenderHint(QPainter::Antialiasing, true);

	drawAxes(painter);
	drawTitles(painter);
	if (showLegend_) drawLegend(painter);
}

// Setup plot for drawing
void PlotWidget::plotSetup(QPainter& painter)
{
	// Get text line height
	QRectF tempRect = painter.boundingRect(QRectF(), Qt::AlignLeft, "xyz");
	textHeight_ = tempRect.height();

	// Set font point size
	font_.setBold(false);
	font_.setItalic(false);
	painter.setFont(font_);

	// Determine maximal label widths for each axis.
	QString testText;
	// --- X
	xValueRect_ = painter.boundingRect(QRectF(), Qt::AlignHCenter, "000:00:00");
	// --- Y
	testText = QString::number(yMin_*1.01, 'g', 4);
	yValueRect_ = painter.boundingRect(QRectF(), Qt::AlignRight | Qt::AlignVCenter, testText);
	testText = QString::number(yMax_*1.01, 'g', 4);
	tempRect = painter.boundingRect(QRectF(), Qt::AlignRight | Qt::AlignVCenter, testText);
	if (tempRect.width() > yValueRect_.width()) { yValueRect_.setLeft(tempRect.left()); yValueRect_.setRight(tempRect.right()); }

	// Work out areas
	xAxisArea_.setRect(spacing_, areaHeight_ - 2*spacing_ - 2*textHeight_, areaWidth_-2*spacing_, 2*textHeight_ + 2*spacing_);
	yAxisArea_.setRect(spacing_, spacing_, yValueRect_.width() + 2*spacing_ + textHeight_, areaHeight_-2*spacing_);
	graphArea_.setRect(yAxisArea_.right(), 2*spacing_+textHeight_, areaWidth_-2*spacing_-yAxisArea_.width(), areaHeight_-2*spacing_-textHeight_-xAxisArea_.height());
	xScale_ = graphArea_.width()/(xMax_-xMin_);
	yScale_ = graphArea_.height()/(yMax_-yMin_);

	// If graph area has changed since last redraw then the scales will also have changed, so must regenerate QPainterPaths and recalculate tick deltas
	if ((graphArea_ != lastGraphArea_) || (fabs(xScale_-lastXScale_) > 1e-5) || (fabs(yScale_-lastYScale_) > 1e-5)) calculateTickDeltas(10);
	
	// Loop over all datasets and recreate painterpaths (if necessary)
        for (PlotData* pd = dataSets_.first(); pd != NULL; pd = pd->next)
        {
                if (!pd->visible()) continue;
                pd->generatePainterPaths(xScale_, yScale_);
        }
        staticDataSet_.generatePainterPaths(xScale_, yScale_);

        lastGraphArea_ = graphArea_;
	lastXScale_ = xScale_;
	lastYScale_ = yScale_;

	// Setup a 'data space' transform to give 0,0 at llh corner of graphArea_
	dataTransform_ = globalTransform_;
	dataTransform_.translate(graphArea_.left(), graphArea_.bottom());
	dataTransform_.scale(xScale_, -yScale_);
	dataTransform_.translate(-xMin_, -yMin_);

	// Setup a 'local' transformation matrix which will use local widget coordinates
	localTransform_ = globalTransform_;
	localTransform_.translate(graphArea_.left(), graphArea_.bottom());
	localTransform_.scale(1.0,-1.0);

	// Calculate axis start values, based on current tick deltas
	xAxisTickStart_ = int(xMin_ / xAxisTickDelta_) + (xMin_ > 0.0 ? 1 : 0);
	xAxisTickStart_ *= xAxisTickDelta_;
	yAxisTickStart_ = int(yMin_ / yAxisTickDelta_) + (yMin_ > 0.0 ? 1 : 0);
	yAxisTickStart_ *= yAxisTickDelta_;

	// Setup brushes
	backgroundBrush_ = QBrush(backgroundColour_, Qt::SolidPattern);
	
	// Setup painter
	painter.setBackground(backgroundBrush_);
	painter.setRenderHint(QPainter::TextAntialiasing);

	// Clear background
	painter.setTransform(globalTransform_);
	painter.setBrush(backgroundBrush_);
	painter.drawRect(0, 0, areaWidth_, areaHeight_);
	
	// Draw simple outline around graph area, and set up clipping on this area
	painter.drawRect(graphArea_);
	painter.setClipRect(graphArea_);
}

// Draw gridlines
void PlotWidget::drawGridLines(QPainter& painter) 
{
	// Move to data space
	painter.setTransform(dataTransform_);

	// Draw axis gridlines
	QPen grayPen(Qt::gray);
	grayPen.setStyle(Qt::DashLine);
	painter.setPen(grayPen); 
	double xpos, ypos, absxpos;
	QLineF line;
	int n;

	// --- X
	xpos = xAxisTickStart_;
	do
	{
		line.setLine(xpos, yMin_, xpos, yMax_);
		painter.drawLine(line);
		xpos += xAxisTickDelta_;
	} while (xpos <= xMax_);
	
	// --- Y
	ypos = yAxisTickStart_;
	do
	{
		line.setLine(xMin_, ypos, xMax_, ypos);
		painter.drawLine(line);
		ypos += yAxisTickDelta_;
	} while (ypos <= yMax_);

	// Draw 'zero' axis gridlines
	painter.setPen(Qt::black);
	painter.setBrush(Qt::NoBrush);
	// -- Zero Y
	line.setLine(xMin_, 0.0, xMax_, 0.0);
	painter.drawLine(line);
	// -- Zero X
	line.setLine(0.0, yMin_, 0.0, yMax_);
	painter.drawLine(line);
}

// Draw data
void PlotWidget::drawData(QPainter& painter)
{
	// Draw data
	painter.setRenderHint(QPainter::Antialiasing);
	QTransform modifiedTransform;
	QPen dataPen;
	dataPen.setWidthF(1.5);
	int count = 0;
	for (PlotData* pd = dataSets_.first(); pd != NULL; pd = pd->next)
	{
		// Is dataset visible?
		if (!pd->visible()) continue;
		
		// Take copy of the current transformation matrix, and modify it as necessary
		modifiedTransform = localTransform_;
// 		dataTransform.translate(0.0, verticalSpacing_*pd->verticalOffset()); TODO
		// -- Set correct x-origin for data
		modifiedTransform.translate(-xMin_*xScale_, -yMin_*yScale_);

		// Update Painter transform, pen, and draw the dataset path
		painter.setTransform(modifiedTransform);
		pd->stylePen(dataPen);
		dataPen.setColor(lineColour(count));
		painter.setPen(dataPen);
		painter.setBrush(Qt::NoBrush);
		painter.drawPath(pd->linePath());
		++count;
	}

	// Static data
	// Take copy of the current transformation matrix, and modify it as necessary
	modifiedTransform = localTransform_;
// 		dataTransform.translate(0.0, verticalSpacing_*pd->verticalOffset()); TODO
	// -- Set correct x-origin for data
	modifiedTransform.translate(-xMin_*xScale_, -yMin_*yScale_);

	// Update Painter transform, pen, and draw the dataset path
	painter.setTransform(modifiedTransform);
	staticDataSet_.stylePen(dataPen);

	painter.setPen(dataPen);
	painter.setBrush(Qt::NoBrush);
	painter.drawPath(staticDataSet_.linePath());
}

// Draw axis tick marks / labels
void PlotWidget::drawAxes(QPainter& painter)
{
	// -- Draw X tick marks and labels
	QLineF line;
	double lastPos = -1e6, deviceXpos, deviceYpos;
	double xpos = xAxisTickStart_;
	deviceXpos = (xpos - xMin_)*xScale_ + graphArea_.left();
	deviceYpos = xAxisArea_.top();
	QRectF rect1 = xValueRect_;
	rect1.translate(deviceXpos, deviceYpos+spacing_);
	QRectF rect2 = rect1.translated(0.0, spacing_+textHeight_);
	lastPos = rect1.right() - 2.0*rect1.width();
	do
	{
		// Check for overlap between this label and the last one
		if ((rect1.left()-5) > lastPos)
		{
			// Draw larger tick mark since there is a value here
			line.setLine(deviceXpos, deviceYpos, deviceXpos, deviceYpos+spacing_);
			painter.drawText(rect1, Qt::AlignCenter, QString::number(xpos, 'g', 4));
			
			lastPos = rect1.right();
		}
		else line.setLine(deviceXpos, deviceYpos, deviceXpos, deviceYpos+spacing_*0.5);

		// Draw tick mark
		painter.drawLine(line);
		
		// Move text rectangle(s) and tick position
		rect1.translate(xAxisTickDelta_*xScale_, 0.0);
		rect2.translate(xAxisTickDelta_*xScale_, 0.0);
		deviceXpos += xAxisTickDelta_*xScale_;
		xpos += xAxisTickDelta_;
	} while (xpos <= xMax_);

	// -- Draw Y tick marks and labels
	double ypos = yAxisTickStart_;
	lastPos = -1e6;
	deviceYpos = graphArea_.bottom() - (ypos - yMin_)*yScale_;
	deviceXpos = yAxisArea_.right() - 2.0;
	rect1 = yValueRect_;
	rect1.translate(deviceXpos, deviceYpos);
	lastPos = rect1.top() + 2.0*rect1.height();
	do
	{
		// Draw tick mark
		line.setLine(deviceXpos, deviceYpos, deviceXpos+2.0, deviceYpos);
		painter.drawLine(line);
		
		// Check for overlap between this label and the last one
		if (rect1.bottom() < lastPos)
		{
			painter.drawText(rect1, Qt::AlignRight, QString::number(ypos, 'g', 4));
			lastPos = rect1.top();
		}
		
		// Move text rectangle and tick position
		rect1.translate(0.0, -yAxisTickDelta_*yScale_);
		deviceYpos -= yAxisTickDelta_*yScale_;
		ypos += yAxisTickDelta_;
	} while (ypos <= yMax_);
}

// Draw axis titles
void PlotWidget::drawTitles(QPainter& painter)
{
	// Draw X-axis title
	xAxisArea_.setLeft( graphArea_.left() );
	xAxisArea_.moveTop( xAxisArea_.top() + (spacing_ + textHeight_) );
	painter.drawText(xAxisArea_, Qt::AlignHCenter | Qt::AlignTop, xAxisTitle_);
	
	// Draw Y-axis title(s)
	QTransform transform = globalTransform_;
	QRectF tempRect;
	transform.translate(0, graphArea_.center().y());
	transform.rotate(-90);
	painter.setTransform(transform);
	tempRect.setRect(-0.5*graphArea_.height(), 0.5*(textHeight_+spacing_), graphArea_.height(), textHeight_);
	// First, loop over group data to work out maximum rect width required
	yAxisTitle_ = "";
	for (PlotDataBlock* pdb = dataSetBlocks_.first(); pdb != NULL; pdb = pdb->next)
	{
		if (pdb->visible())
		{
			if (!yAxisTitle_.isEmpty()) yAxisTitle_ += " / ";
			yAxisTitle_ += pdb->blockName();
		}
	}

	painter.drawText(tempRect, Qt::AlignHCenter | Qt::AlignVCenter, yAxisTitle_);
}

// Draw legend
void PlotWidget::drawLegend(QPainter& painter)
{
	painter.setTransform(globalTransform_);

	// Loop over datasets and determine maximum extent needed for titles
	QRectF textRect, tempRect;
	int nDataSets = 0;
	for (PlotData* pd = dataSets_.first(); pd != NULL; pd = pd->next)
	{
		// Is dataset to be drawn?
		if (!pd->visible()) continue;

		tempRect = painter.boundingRect(QRectF(), Qt::AlignLeft, pd->name());
		if (tempRect.width() > textRect.width()) textRect = tempRect;
		++nDataSets;
	}
	
	if (nDataSets == 0) return;

	// Create rectangle for legend area
	int legendSpacing = 2, legendLineLength = 20;
	QRectF legendRect = textRect;
	legendRect.setWidth( textRect.width() + 3 * legendSpacing + legendLineLength );
	legendRect.moveRight(width()-spacing_);
	legendRect.moveTop(graphArea_.top());
	legendRect.setHeight( textRect.height() * nDataSets + (nDataSets+1) * legendSpacing );
	textRect.moveRight(width() - legendSpacing - spacing_);
	textRect.moveTop(legendRect.top() + legendSpacing);

	// Draw semi-transparent box to underpin legend
	QBrush legendBrush(backgroundColour_, Qt::SolidPattern);
	painter.setBrush(legendBrush);
	painter.drawRect(legendRect);
	QLine legendLine(legendRect.left() + legendSpacing, legendRect.top()+legendSpacing+textRect.height()/2, legendRect.left() + legendSpacing + legendLineLength, legendRect.top()+legendSpacing+textRect.height()/2);

	// Draw on dataSet names and representative lines...
	QPen pen;
	int count = 0;
	pen.setWidth(2);
	for (PlotData* pd = dataSets_.first(); pd != NULL; pd = pd->next)
	{
		// Is dataset visible?
		if (!pd->visible()) continue;

		painter.setPen(Qt::black);
		painter.drawText(textRect, Qt::AlignLeft, pd->name());
		textRect.moveTop( textRect.top() + textRect.height() + legendSpacing);
		
		if (pd->block() != NULL) pen.setDashPattern(pd->block()->dashes());
		pen.setColor(lineColour(count));
		painter.setPen(pen);
		painter.drawLine(legendLine);
		legendLine.translate(0, textRect.height()+legendSpacing);
		++count;
	}
}
