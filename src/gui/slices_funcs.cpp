/*
	*** uChroma Slices Window
	*** src/gui/slices_funcs.cpp
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

#include "gui/slices.h"
#include "uchroma.h"
#include "templates/reflist.h"

/*
 * Window Functions
 */

// Constructor
SlicesWindow::SlicesWindow(UChromaWindow& parent) : QWidget(&parent), uChroma_(parent)
{
	ui.setupUi(this);

	QWidget::setWindowFlags(Qt::Tool);

	refreshing_ = false;
}

// Destructor
SlicesWindow::~SlicesWindow()
{
}

// Window close event
void SlicesWindow::closeEvent(QCloseEvent *event)
{
	emit(windowClosed(false));
}

/*
 * Convenience Functions
 */

/*
 * Slots
 */

void SlicesWindow::on_SlicesClearButton_clicked(bool checked)
{
// 	ui.SlicesTree->clear();
// 	ui.AnalyseSurfaceSliceGraph->removeAllDataSets();
}

void SlicesWindow::on_SlicesSaveButton_clicked(bool checked)
{
	//if (ui.AnalyseSurfaceSliceList->currentIndex() == -1) return;
	
}

void SlicesWindow::on_SlicesList_currentRowChanged(int index)
{
}

/*
 * Update
 */

// Add slice data to graph
void SlicesWindow::addSlice(ExtractedSlice* slice)
{
}

// Update controls and show window
void SlicesWindow::updateAndShow()
{
	updateControls();
	show();
	move(uChroma_.centrePos() - QPoint(width()/2, height()/2));
}

// Update controls
void SlicesWindow::updateControls()
{
	// If the window isn't visible, do nothing...
	if (!isVisible()) return;

	refreshing_ = true;

	// Update list of slices
	ui.SlicesTree->clear();
	ui.SlicesTree->setColumnCount(3);
	ui.SlicesTree->setHeaderLabels( QStringList() << "Column A" << "Column B" << "C" );
	int nDataSets = 0;
	// -- Add root node
	QTreeWidgetItem* topItem = new QTreeWidgetItem(ui.SlicesTree);
	topItem->setText(0, "All");
	topItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsTristate | Qt::ItemIsUserCheckable);
	// -- Add groups to root node
	for (ExtractedSliceGroup* group = uChroma_.extractedSliceGroups(); group != NULL; group = group->next)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem(topItem);
		item->setText(0, group->name());
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsTristate | Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
		group->setTreeItem(item);

		// Loop over slices in this group, adding to the tree
		for (ExtractedSlice* slice = group->extractedSlices(); slice != NULL; slice = slice->next)
		{
			QTreeWidgetItem* item = new QTreeWidgetItem(group->treeItem());
			item->setText(0, slice->title());
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsTristate | Qt::ItemIsUserCheckable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
			slice->setTreeItem(item);
		}
	}
	topItem->setExpanded(true);
	for (int n=0; n<3; ++n) ui.SlicesTree->resizeColumnToContents(n);

	// Update the axis titles on the necessary graphs  TODO
// 	if (uChroma_.sliceAxis() == -1) ui.SlicesGraph->ui.Graph->setTitles("", "", "");
// 	else if (uChroma_.sliceAxis() == 0) ui.SlicesGraph->ui.Graph->setTitles(ui.SliceSelectorLabel->text(), axisTitle_[2], axisTitle_[1]);
// 	else if (uChroma_.sliceAxis() == 1) ui.SlicesGraph->ui.Graph->setTitles(ui.SliceSelectorLabel->text(), axisTitle_[0], axisTitle_[2]);
// 	else if (uChroma_.sliceAxis() == 2) ui.SlicesGraph->ui.Graph->setTitles(ui.SliceSelectorLabel->text(), axisTitle_[0], axisTitle_[1]);

	refreshing_ = false;
}
