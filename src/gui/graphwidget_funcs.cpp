/*
	*** GraphWidget Functions
	*** src/graphwidget_funcs.cpp
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

#include "gui/graphwidget.h"
#include "uchroma.h"

// Static singletons
UChromaWindow* GraphWidget::uChroma_ = NULL;

/*!
 * \brief Constructor
 */
GraphWidget::GraphWidget(QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);

	// Connect signals/slots to PlotWidget
	connect(ui.AutoScaleCheck, SIGNAL(clicked(bool)), ui.Graph, SLOT(setAutoScale(bool)));
	connect(ui.Graph, SIGNAL(autoScaleChanged(bool)), ui.AutoScaleCheck, SLOT(setChecked(bool)));
	connect(ui.ShowLegendCheck, SIGNAL(clicked(bool)), ui.Graph, SLOT(setShowLegend(bool)));
	connect(ui.Graph, SIGNAL(showLegendChanged(bool)), ui.ShowLegendCheck, SLOT(setChecked(bool)));

	// Ensure that initial widget state represents the PlotWidget
	ui.AutoScaleCheck->setChecked(ui.Graph->autoScale());
	ui.ShowLegendCheck->setChecked(ui.Graph->showLegend());
}

/*!
 * \brief Destructor
 */
GraphWidget::~GraphWidget()
{
}

/*
 * Link to UChroma
 */

// Set UChromaWindow pointer
void GraphWidget::setUChroma(UChromaWindow* ptr)
{
	uChroma_ = ptr;
}

/*
 * Widgets / Slots / Reimplementations
 */

// Source static data has changed, so update it
void GraphWidget::staticDataChanged()
{
	// Change the static data in PlotWidget for that currently in uChroma
	if (!uChroma_) return;
	ui.Graph->setStaticData(uChroma_->currentSlice());
}

/*
 * Public Functions
 */

// Add data to graph
void GraphWidget::addData(ExtractedSlice* data)
{
	ui.Graph->addDataSet(data);
}
