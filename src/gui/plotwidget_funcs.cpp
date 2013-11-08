/*
	*** PlotWidget Functions
	*** src/plotwidget_funcs.cpp
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
#include <QtGui/QMenu>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include "plotwidget.uih"
#include <math.h>

// Static Singletons
QColor PlotWidget::lineColours_[PlotWidget::nLineColours];

/*!
 * \brief Constructor
 */
PlotWidget::PlotWidget(QWidget* parent) : QWidget(parent)
{
	// Context Menu / Extra Widgets
	contextMenu_ = new QMenu(this);
	connect(contextMenu_->addAction("Show &All"), SIGNAL(triggered(bool)), this, SLOT(contextMenuShowAllClicked(bool)));
	connect(contextMenu_->addAction("Copy to Clipboard"), SIGNAL(triggered(bool)), this, SLOT(contextMenuCopyToClipboardClicked(bool)));
	coordinatesLabel_ = NULL;

	// Set plot defaults
	spacing_ = 4;
	backgroundColour_ = Qt::white;
	foregroundColour_ = Qt::black;
	font_.setPointSize(8);
	xMin_ = -10.0;
	xMax_ = 10.0;
	yMin_ = -10.0;
	yMax_ = 10.0;
	xScale_ = 1.0;
	yScale_ = 1.0;
	lastXScale_ = -1.0;
	lastYScale_ = -1.0;
	xAxisTickStart_ = -10.0;
	yAxisTickStart_ = -10.0;
	xAxisTickDelta_ = 2.0;
	yAxisTickDelta_ = 2.0;
	xLabelFormat_ = "%6.3f";
	yLabelFormat_ = "%6.3f";
	showLegend_ = true;
	verticalSpacing_ = 1.0;
	xMinLimit_ = 0.0;
	xMaxLimit_ = 0.0;
	yMinLimit_ = 0.0;
	yMaxLimit_ = 0.0;
	limitXMin_ = false;
	limitXMax_ = false;
	limitYMin_ = false;
	limitYMax_ = false;
	autoScale_ = true;
	nDataSetBlocksVisible_ = 0;
	
	// Style
	// -- Pre-defined colours
	lineColours_[PlotWidget::BlackColour] = Qt::black;
	lineColours_[PlotWidget::RedColour] = Qt::red;
	lineColours_[PlotWidget::BlueColour] = Qt::blue;
	lineColours_[PlotWidget::GreenColour].setRgb(32,114,53);
	lineColours_[PlotWidget::PurpleColour].setRgb(126,12,179);
	lineColours_[PlotWidget::OrangeColour].setRgb(224,133,25);
	lineColours_[PlotWidget::MetallicBlueColour].setRgb(61,129,160);
	lineColours_[PlotWidget::MintColour].setRgb(119,189,137);
	lineColours_[PlotWidget::GreyColour].setRgb(180,180,180);

	// Titles
	mainTitle_ = "NewGraph";
	xAxisTitle_ = "Time";
	yAxisTitle_ = "YAxis";
}

/*!
 * \brief Destructor
 */
PlotWidget::~PlotWidget()
{
}

/*
// Widgets / Slots / Reimplementations
*/

/*!
 * \brief Mouse press event
 */
void PlotWidget::mousePressEvent(QMouseEvent* event)
{
	buttons_ = event->buttons();
	clickedWidgetPosition_ = event->pos();

	if (buttons_&Qt::LeftButton)
	{
		// Where is mouse cursor (use original click position to check)?
		if (graphArea_.contains(clickedWidgetPosition_)) clickedDataPosition_ = widgetToGraph(event->pos());
		else clickedWidgetPosition_ = QPoint();
	}
	else if (buttons_&Qt::RightButton)
	{
		// Raise context menu...
		contextMenu_->exec(event->globalPos());
	}
	else if (buttons_&Qt::MiddleButton)
	{
		// Where is mouse cursor (use original click position to check)?
		if (graphArea_.contains(clickedWidgetPosition_)) clickedDataPosition_ = widgetToGraph(event->pos());
		else clickedWidgetPosition_ = QPoint();
	}
	
// 	printf("Clicked graph coords = %f,%f\n", clickedDataPosition_.x(), clickedDataPosition_.y());
}

/*!
 * \brief Mouse release event
 */
void PlotWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (buttons_&Qt::LeftButton)
	{
		// Set current mouse position (in graph coordinates)
		currentDataPosition_ = widgetToGraph(event->pos());
// 		printf("Unclicked graph coords = %f,%f\n", currentDataPosition_.x(), currentDataPosition_.y());

		// Where is mouse cursor (use original click position to check)?
		if (graphArea_.contains(clickedWidgetPosition_))
		{
			// If the box is too small in either direction, assume that we are translating to the point instead...
			bool smallArea = false;
			if (fabs(currentDataPosition_.x()-clickedDataPosition_.x()) < (xMax_-xMin_)*0.01) smallArea = true;
			if (fabs(currentDataPosition_.y()-clickedDataPosition_.y()) < (yMax_-yMin_)*0.01) smallArea = true;
			if (smallArea)
			{
				double dx = currentDataPosition_.x() - (xMin_+xMax_)*0.5;
				double dy = currentDataPosition_.y() - (yMin_+yMax_)*0.5;
				xMin_ += dx;
				xMax_ += dx;
				yMin_ += dy;
				yMax_ += dy;
				repaint();
			}
			else zoomToGraph(clickedDataPosition_.x(), clickedDataPosition_.y(), currentDataPosition_.x(), currentDataPosition_.y());
		}
	}
	else if (buttons_&Qt::MiddleButton)
	{
	}
	
	// Reset values
	buttons_ = 0;
	clickedWidgetPosition_ = QPoint();
}

/*!
 * \brief Mouse move event
 */
void PlotWidget::mouseMoveEvent(QMouseEvent* event)
{
	currentDataPosition_ = widgetToGraph(event->pos());
	setFocus();
	if (buttons_&Qt::MiddleButton)
	{
		if (!clickedWidgetPosition_.isNull())
		{
			double dx = currentDataPosition_.x() - clickedDataPosition_.x();
			double dy = currentDataPosition_.y() - clickedDataPosition_.y();
			xMin_ -= dx;
			xMax_ -= dx;
			yMin_ -= dy;
			yMax_ -= dy;

			clickedDataPosition_ = widgetToGraph(event->pos());
			repaint();
		}
	}
	
	// Set coordinates label (if it exists)
	if (coordinatesLabel_)
	{
		QString label;
		label.sprintf("x = %6.2e, y = %6.2e", currentDataPosition_.x(), currentDataPosition_.y());
		coordinatesLabel_->setText(label);
	}
}

/*!
 * \brief Mouse wheel event
 */
void PlotWidget::wheelEvent(QWheelEvent* event)
{
	// Get a fraction of the current y range
	double yBit = (yMax_ - yMin_) * 0.1;
	if (event->delta() < 0) yBit *= -1.0;

	// Shift y range of graph....
	yMin_ += yBit;
	yMax_ += yBit;

	repaint();
}

/*!
 * \brief Key press event
 */
void PlotWidget::keyPressEvent(QKeyEvent* event)
{
	bool accept = true;
	switch (event->key())
	{
		// Toggle autoscaling
		case (Qt::Key_A):
			autoScale_ = !autoScale_;
			update();
			break;
		// Fit graph to data (optionally obeying soft limits)
		case (Qt::Key_F):
			if (event->modifiers().testFlag(Qt::ShiftModifier)) fitData(false);
			else fitData(true);
			update();
			break;
		// Toggle X axis soft limits
		case (Qt::Key_X):
			if (event->modifiers().testFlag(Qt::ShiftModifier)) limitXMax_ = !limitXMax_;
			else limitXMin_ = !limitXMin_;
			fitData(true);
			update();
			break;
		// Toggle Y axis soft limits
		case (Qt::Key_Y):
			if (event->modifiers().testFlag(Qt::ShiftModifier)) limitYMax_ = !limitYMax_;
			else limitYMin_ = !limitYMin_;
			fitData(true);
			update();
			break;
		default:
			accept = false;
			break;
	}
	if (accept) event->accept();
	else event->ignore();
}

/*!
 * \brief Key release event
 */
void PlotWidget::keyReleaseEvent(QKeyEvent* event)
{
	event->ignore();
}

/*!
 * \brief Context Menu Show All clicked
 */
void PlotWidget::contextMenuShowAllClicked(bool checked)
{
	fitData(true);
}

// Context Menu CopyToClipboard clicked
void PlotWidget::contextMenuCopyToClipboardClicked(bool checked)
{
	// Create bitmap image of plot and then print image to QPainter
	const double inchesPerMetre = 39.3700787;
	int scaling = 1;
	QImage image(width()*scaling, height()*scaling, QImage::Format_RGB32);
// 	image.setDotsPerMeterX(inchesPerMetre * scaling*90);
// 	image.setDotsPerMeterY(inchesPerMetre * scaling*90);
	draw(image);

	// Copy image to clipboard
	QClipboard* clipboard = QApplication::clipboard();
	clipboard->setImage(image);
}

// Set coordinates label
void PlotWidget::setCoordinatesLabel(QLabel* label)
{
	coordinatesLabel_ = label;
}

/*
 * Style
 */

/*!
 * \brief Return nth pre-defined colour
 */
QColor PlotWidget::lineColour(int n)
{
	return lineColours_[n%PlotWidget::nLineColours];
}

/*
 * Plot Area
 */

/*!
 * \brief Determine suitable tick deltas based on current graph limits
 * \details Given the current min and max values of the x and y axes, determine a 'best-fit' tick delta, given the constraints that it must
 * be exactly one of several basic values, multiplied by some power of 10.
 */
void PlotWidget::calculateTickDeltas(int maxTicks)
{
	const int nBaseValues = 5, maxIterations = 10;
	int power = 1, baseValues[nBaseValues] = { 1, 2, 3, 4, 5 }, baseValueIndex = 0, nTicks, iteration, minTicks = maxTicks/2;

	// Y axis first
	baseValueIndex = 0;
	power = int(log10((yMax_-yMin_) / maxTicks) - 1);
	iteration = 0;
	
	if ((yMax_-yMin_) > 1.0e-10)
	{
		do
		{
			// Calculate current tickDelta
			yAxisTickDelta_ = baseValues[baseValueIndex]*pow(10.0,power);

			// Get first tickmark value
			yAxisTickStart_ = int(yMin_ / yAxisTickDelta_) * yAxisTickDelta_;
			if (yAxisTickStart_ < yMin_) yAxisTickStart_ += yAxisTickDelta_;
			
			// How many ticks now fit between the firstTick and max value?
			// Add 1 to get total ticks for this delta (i.e. including firstTick)
			nTicks = int((yMax_-yAxisTickStart_) / yAxisTickDelta_);
			++nTicks;
			
			// Check n...
			if (nTicks > maxTicks)
			{
				++baseValueIndex;
				if (baseValueIndex == nBaseValues) ++power;
				baseValueIndex = baseValueIndex%nBaseValues;
			}
			else if (nTicks < minTicks)
			{
				--baseValueIndex;
				if (baseValueIndex == -1)
				{
					--power;
					baseValueIndex += nBaseValues;
				}
			}
			
			++iteration;
			if (iteration == maxIterations) break;
			
		} while ((nTicks > maxTicks) || (nTicks < minTicks));
	}
	else
	{
		yAxisTickStart_ = yMin_;
		yAxisTickDelta_ = 1.0;
	}
	
	// Now x (time) axis
	// The exact 'units' we use will depend on the timespan of the axis
	const int nTimeDeltas = 16, timeDeltas[] = { 1, 2, 3, 4, 5, 10, 30, 60, 120, 180, 240, 300, 600, 1200, 1800, 3600 };
	int range = int(floor(xMax_) - ceil(xMin_));
	int snappedXMin;

	// Loop over timeDeltas, and search for a sensible division
	nTicks = 0;
	int requiredTicks;
	for (int n=0; n<100; ++n)
	{
		if (n < nTimeDeltas) xAxisTickDelta_ = timeDeltas[n];
		else xAxisTickDelta_ = (1+n-nTimeDeltas) * 3600;

		// Get 'snapped' value for xMin - i.e. first time value divisible by this time delta
		snappedXMin = int(ceil(xMin_) / xAxisTickDelta_) * xAxisTickDelta_;
		xAxisTickStart_ = snappedXMin;
		
		// Calculate number of ticks needed between here and the maximum value
		nTicks = int((xMax_ - snappedXMin) / xAxisTickDelta_);
		if (nTicks < maxTicks) break;
	}
}

/*!
 * \brief Set main title
 */
void PlotWidget::setMainTitle(QString title)
{
	mainTitle_ = title;
	repaint();
}

/*!
 * \brief Set x-axis title
 */
void PlotWidget::setXAxisTitle(QString title)
{
	xAxisTitle_ = title;
	repaint();
}

/*!
 * \brief Set y-axis title
 */
void PlotWidget::setYAxisTitle(QString title)
{
	yAxisTitle_ = title;
	repaint();
}

/*!
 * \brief Set x and y axis titles
 */
void PlotWidget::setTitles(QString mainTitle, QString xTitle, QString yTitle)
{
	mainTitle_ = mainTitle;
	xAxisTitle_ = xTitle;
	yAxisTitle_ = yTitle;
	update();
}

/*!
 * \brief Set soft X limits for plot area
 */
void PlotWidget::setXLimits(bool setMinLimit, bool applyMinLimit, double minLimit, bool setMaxLimit, bool applyMaxLimit, double maxLimit)
{
	if (setMinLimit)
	{
		limitXMin_ = applyMinLimit;
		xMinLimit_ = minLimit;
	}
	if (setMaxLimit)
	{
		limitXMax_ = applyMaxLimit;
		xMaxLimit_ = maxLimit;
	}
	fitData(true);
}

/*!
 * \brief Set soft Y limits for plot area
 */
void PlotWidget::setYLimits(bool setMinLimit, bool applyMinLimit, double minLimit, bool setMaxLimit, bool applyMaxLimit, double maxLimit)
{
	if (setMinLimit)
	{
		limitYMin_ = applyMinLimit;
		yMinLimit_ = minLimit;
	}
	if (setMaxLimit)
	{
		limitYMax_ = applyMaxLimit;
		yMaxLimit_ = maxLimit;
	}
	fitData(true);
}

// Return whether legend is visible
bool PlotWidget::showLegend()
{
	return showLegend_;
}

// Return whether autoscaling is on
bool PlotWidget::autoScale()
{
	return autoScale_;
}

// Set whether legend is visible
void PlotWidget::setShowLegend(bool enabled)
{
	showLegend_ = enabled;
	update();
}

/*!
 * \brief Enable/disable autoscaling
 */
void PlotWidget::setAutoScale(bool enabled)
{
	autoScale_ = enabled;
	update();
}

/*!
 * \brief Enable/disable soft x minimum limit
 */
void PlotWidget::setXMinLimit(bool enabled)
{
	limitXMin_ = enabled;
	fitData(true);
}

/*!
 * \brief Enable/disable soft x maxiimum limit
 */
void PlotWidget::setXMaxLimit(bool enabled)
{
	limitXMax_ = enabled;
	fitData(true);
}

/*!
 * \brief Enable/disable soft y minimum limit
 */
void PlotWidget::setYMinLimit(bool enabled)
{
	limitYMin_ = enabled;
	fitData(true);
}

/*!
 * \brief Enable/disable soft y maxiimum limit
 */
void PlotWidget::setYMaxLimit(bool enabled)
{
	limitYMax_ = enabled;
	fitData(true);
}

/*
// Source Data
*/

/*!
 * \brief Add data to Plot (local Data2D)
 */
PlotData* PlotWidget::addDataSet(ExtractedSlice& data, QString name, QString blockName, int yOffset)
{
	PlotData* pd = dataSets_.add();
	pd->setData(data, name);
	pd->setVerticalOffset(yOffset);
	
	// Determine limits for this data, and adjust parent's relative limits if necessary
	pd->determineLimits();

	// Set group index - does the group exist already?
	PlotDataBlock* pdb;
	for (pdb = dataSetBlocks_.first(); pdb != NULL; pdb = pdb->next) if (pdb->blockName() == blockName) break;
	if (pdb == NULL)
	{
		pdb = new PlotDataBlock(blockName);
		dataSetBlocks_.own(pdb);
	}
	pd->setBlock(pdb);

	// Autoscale view if requested
	if (autoScale_) fitData(true);

	// Repaint widget
	repaint();

	return pd;
}

// Set static data
void PlotWidget::setStaticData(ExtractedSlice data, QString name)
{
	// Take copy of data
	staticDataSet_.setData(data, name);

	// Transform it according to the current settings // TODO
	staticDataSet_.data().transformData(0, 0);

	staticDataSet_.determineLimits();

	// Autoscale view if requested
	if (autoScale_) fitData(true);
	
	// Repaint widget
	repaint();
}

/*!
 * \brief Remove all data from plot
 */
void PlotWidget::removeAllDataSets()
{
	dataSets_.clear();
	staticDataSet_.data().originalData().clear();

	repaint();
}

// Return dataset list
const List<PlotData>& PlotWidget::dataSets()
{
	return dataSets_;
}

// Determine dataset limits
void PlotWidget::determineDataSetLimits()
{
	for (PlotData* pd = dataSets_.first(); pd != NULL; pd = pd->next) pd->determineLimits();
	staticDataSet_.determineLimits();
}

// Return list of data set groups
const List<PlotDataBlock>& PlotWidget::dataSetBlocks()
{
	return dataSetBlocks_;
}

// Hide all datasets
void PlotWidget::hideAllDataSets()
{
	for (PlotDataBlock* pdb = dataSetBlocks_.first(); pdb != NULL; pdb = pdb->next) pdb->setVisible(false);
	nDataSetBlocksVisible_ = 0;
}

// Show all datasets containing specified block data
void PlotWidget::setBlockVisible(QString blockName, bool visible)
{
	nDataSetBlocksVisible_ = 0;
	for (PlotDataBlock* pdb = dataSetBlocks_.first(); pdb != NULL; pdb = pdb->next)
	{
		if (pdb->blockName() == blockName)
		{
			pdb->setVisible(visible);
		}
		
		// Set block linestyle
		if (pdb->visible())
		{
			pdb->setLineStyle( (PlotDataBlock::BlockLineStyle) (nDataSetBlocksVisible_%PlotDataBlock::nBlockLineStyles) );
			++nDataSetBlocksVisible_;
		}
	}

	fitData(true);
	update();
}

/*
// Functions
*/

/*!
 * \brief Convert widget coordinates to graph coordinates
 */
QPointF PlotWidget::widgetToGraph(QPoint pos)
{
	return QPointF((pos.x() - graphArea_.left()) / xScale_ + xMin_, (graphArea_.bottom() - pos.y()) / yScale_ + yMin_ );
}

/*!
 * \brief Zoom to specified graph coordinates
 */
void PlotWidget::zoomToGraph(double x1, double y1, double x2, double y2)
{
	// Set range
	xMin_ = x1;
	xMax_ = x2;
	yMin_ = y1;
	yMax_ = y2;

	// Swap values if necessary
	double temp;
	if (xMin_ > xMax_)
	{
		temp = xMin_;
		xMin_ = xMax_;
		xMax_ = temp;
	}
	if (yMin_ > yMax_)
	{
		temp = yMin_;
		yMin_ = yMax_;
		yMax_ = temp;
	}

	msg.print("Zoom To  %f/%f and %f/%f\n", xMin_, xMax_, yMin_, yMax_);
	
	repaint();
}

/*!
 * \brief Rescale axes to fit all current data
 */
void PlotWidget::fitData(bool obeySoftLimits)
{
	// Static data first
	// -- X axis first
	xMin_ = staticDataSet_.xMin();
	xMax_ = staticDataSet_.xMax();

	// -- Now Y
	yMin_ = staticDataSet_.yMin();
	yMax_ = staticDataSet_.yMax();
	
	// Set initial values
	for (PlotData* pd = dataSets_.first(); pd != NULL; pd = pd->next)
	{
		// If dataset isn't currently shown, move on
		if (!pd->visible()) continue;
		
		// If this is the first visible dataset encountered, set initial limits - otherwise, compare
		double off = verticalSpacing_ * pd->verticalOffset();
		// X axis first
		if (pd->xMin() < xMin_) xMin_ = pd->xMin();
		if (pd->xMax() > xMax_) xMax_ = pd->xMax();

		// Now Y
		if ((pd->yMin() + off) < yMin_) yMin_ = pd->yMin() + off;
		if ((pd->yMax() + off) > yMax_) yMax_ = pd->yMax() + off;
	}

	// Increase y limits by 5% of difference (or actual value if difference is 'zero')
	double diff = ((yMax_ - yMin_) < yMax_*1.0e-3 ? yMax_*0.05 : (yMax_ - yMin_) * 0.05);
	if (diff < 1.0e-8)
	{
		yMin_ -= 1.0;
		yMax_ += 1.0;
	}
	else
	{
		if (fabs(yMin_) > 1.0e-8) yMin_ -= diff;
		if (fabs(yMax_) > 1.0e-8) yMax_ += diff;
	}

	// Obey soft limits?
	if (obeySoftLimits)
	{
		if (limitXMin_ && (xMin_ < xMinLimit_)) xMin_ = xMinLimit_;
		if (limitXMax_ && (xMax_ > xMaxLimit_)) xMax_ = xMaxLimit_;
		if (limitYMin_ && (yMin_ < yMinLimit_)) yMin_ = yMinLimit_;
		if (limitYMax_ && (yMax_ > yMaxLimit_)) yMax_ = yMaxLimit_;
	}

// 	msg.print("New axis limits are %f/%f and %f/%f (scales = %f/%f), %i/%i\n", xMin_, xMax_, yMin_, yMax_, xScale_, yScale_, graphArea_.width(), graphArea_.height());

	repaint();
}
