/*
	*** Main Window - Slices
	*** src/gui/uchroma_slices.cpp
	Copyright T. Youngs 2013

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

#include "gui/uchroma.h"

/*
 * Private Slots
 */

void UChromaWindow::on_SlicesClearButton_clicked(bool checked)
{
// 	ui.SlicesTree->clear();
// 	ui.AnalyseSurfaceSliceGraph->removeAllDataSets();
}

void UChromaWindow::on_SlicesSaveButton_clicked(bool checked)
{
	//if (ui.AnalyseSurfaceSliceList->currentIndex() == -1) return;
	
}

void UChromaWindow::on_SlicesList_currentRowChanged(int index)
{
}

/*
 * Public Functions
 */

// Update the Slices tab
void UChromaWindow::updateSlicesTab()
{
	// Update list of slices
	ui.SlicesTree->clear();
	ui.SlicesTree->setColumnCount(3);
	ui.SlicesTree->setHeaderLabels( QStringList() << "Column A" << "COlumn B" << "C" );
	int nDataSets = 0;
	// -- Add root node
	QTreeWidgetItem* topItem = new QTreeWidgetItem(ui.SlicesTree);
	topItem->setText(0, "All");
	topItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsTristate | Qt::ItemIsUserCheckable);
	// -- Add groups to root node
	for (ExtractedSliceGroup* group = extractedSliceGroups_.first(); group != NULL; group = group->next)
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

	// Update the axis titles on the necessary graphs
	if (sliceAxis_ == -1) ui.SlicesGraph->ui.Graph->setTitles("", "", "");
	else if (sliceAxis_ == 0) ui.SlicesGraph->ui.Graph->setTitles(ui.SliceSelectorLabel->text(), axisTitle_[2], axisTitle_[1]);
	else if (sliceAxis_ == 1) ui.SlicesGraph->ui.Graph->setTitles(ui.SliceSelectorLabel->text(), axisTitle_[0], axisTitle_[2]);
	else if (sliceAxis_ == 2) ui.SlicesGraph->ui.Graph->setTitles(ui.SliceSelectorLabel->text(), axisTitle_[0], axisTitle_[1]);
}

// Update the slice monitor widget
void UChromaWindow::updateSliceMonitor()
{
	// Update the axis titles on the necessary graphs
	if (sliceAxis_ == -1) sliceMonitorDialog_.ui.MonitorGraph->ui.Graph->setTitles("", "", "");
	else if (sliceAxis_ == 0) sliceMonitorDialog_.ui.MonitorGraph->ui.Graph->setTitles(ui.SliceSelectorLabel->text(), axisTitle_[2], axisTitle_[1]);
	else if (sliceAxis_ == 1) sliceMonitorDialog_.ui.MonitorGraph->ui.Graph->setTitles(ui.SliceSelectorLabel->text(), axisTitle_[0], axisTitle_[2]);
	else if (sliceAxis_ == 2) sliceMonitorDialog_.ui.MonitorGraph->ui.Graph->setTitles(ui.SliceSelectorLabel->text(), axisTitle_[0], axisTitle_[1]);
}