/*
	*** Main Window - Functions 
	*** src/gui/fqplot_funcs.cpp
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

#include "gui/fqplot.h"
#include "templates/reflist.h"
#include "version.h"

// Constructor
FQPlotWindow::FQPlotWindow(QMainWindow *parent) : QMainWindow(parent)
{
	// Initialise the icon resource
	Q_INIT_RESOURCE(icons);

	// Call the main creation function
	ui.setupUi(this);

	// Set variable defaults
	dataFileDirectory_ = getenv("CWD");
	clearData();
	refreshing_ = false;
}

// Destructor
FQPlotWindow::~FQPlotWindow()
{
}

// Window close event
void FQPlotWindow::closeEvent(QCloseEvent *event)
{
}

// Update all tabs
void FQPlotWindow::updateAllTabs()
{
	updateSourceDataTab();
	updateTransformTab();
	updateColourTab();
	updateViewTab();
}

// Update title bar
void FQPlotWindow::updateTitleBar()
{
	if (modified_) setWindowTitle("FQPlot v" + QString(FQPLOTVERSION) + " - " + inputFile_ + " (modified) ");
	else setWindowTitle("FQPlot v" + QString(FQPLOTVERSION) + " - " + inputFile_);
}

/*
// File Menu
*/

void FQPlotWindow::on_actionFileLoad_triggered(bool checked)
{
	if (modified_)
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning", "The current file has been modified.\nDo you want to save this data first?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		if (button == QMessageBox::Yes)
		{
			// Save file, and check modified_ status to make sure it wasn't cancelled.
			on_actionFileSave_triggered(false);
			if (modified_) return;
		}
	}

	QString fileName = QFileDialog::getOpenFileName(this, "Choose file to load", dataFileDirectory_.absolutePath(), "FQPlot files (*.fqp);;All files (*.*)");
	if (fileName.isEmpty()) return;

	clearData();
	if (loadData(fileName))
	{
		// Update data and transform limits
		calculateDataLimits();

		// Update GUI
		updateAllTabs();
		updateTitleBar();
	}
}

void FQPlotWindow::on_actionFileSave_triggered(bool checked)
{
	// Has an input filename already been chosen?
	if (inputFile_.isEmpty())
	{
		inputFile_ = QFileDialog::getSaveFileName(this, "Choose save file name", dataFileDirectory_.absolutePath(), "FQPlot files (*.fqp);;All files (*.*)");
		if (inputFile_.isEmpty()) return;
	}

	if (saveData(inputFile_)) modified_ = false;
	updateTitleBar();
}

void FQPlotWindow::on_actionFileSaveAs_triggered(bool checked)
{
	// Has an input filename already been chosen?
	QString newFileName = QFileDialog::getSaveFileName(this, "Choose save file name", dataFileDirectory_.absolutePath(), "FQPlot files (*.fqp);;All files (*.*)");
	if (newFileName.isEmpty()) return;
	
	inputFile_ = newFileName;
	if (saveData(inputFile_)) modified_ = false;
	updateTitleBar();
}

void FQPlotWindow::on_actionFileQuit_triggered(bool checked)
{
	QApplication::exit(0);
}

/*
// View Menu
*/

void FQPlotWindow::on_actionViewReset_triggered(bool checked)
{
	Matrix A;
	A[14] = -5.0;
	ui.MainView->setViewMatrix(A);
	ui.MainView->postRedisplay();
}

/*
 * Tabs - Source Data
*/

// Select source directory
void FQPlotWindow::on_SourceDirSelectButton_clicked(bool checked)
{
	QString dir = QFileDialog::getExistingDirectory(this, "TimeStamp Extraction", "Choose the directory containing the required files:");
	if (dir.isEmpty()) return;
	ui.SourceDirEdit->setText(dir);
	dataFileDirectory_ = dir;
	
	// Reload all data and update surface
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next) slice->loadData(dataFileDirectory_);
	setAsModified();
	updateSurface();
}

// Add files button clicked
void FQPlotWindow::on_AddFilesButton_clicked(bool checked)
{
	QStringList files = QFileDialog::getOpenFileNames(this, "Select datafiles", dataFileDirectory_.path(), "MINT files (*.mint01);;MDCS files (*.mdcs01);;Text files (*.txt);;All files (*)");

	QProgressDialog progress("Loading Files...", "Abort Loading", 0, files.count(), this);
	progress.setWindowModality(Qt::WindowModal);

	int count = 0;
	foreach (QString fileName, files)
	{
		progress.setValue(count);
		if (progress.wasCanceled()) break;

		if (loadSlice(fileName)) ++count;
	}
	progress.setValue(files.count());

	// Update data limits and file list
	calculateDataLimits();
	updateSourceDataTab();
	updateTransformTab();
	setAsModified();
	
	// Need to update surface
	updateSurface();
}

// Remove files button clicked
void FQPlotWindow::on_RemoveFilesButton_clicked(bool checked)
{
	foreach(QTableWidgetItem* item, ui.SourceFilesTable->selectedItems())
	{
		printf("%i\n", item->row());
	}
	setAsModified();
}

// Source data item selection changed
void FQPlotWindow::on_SourceDataTable_itemSelectionChanged()
{
	ui.RemoveFilesButton->setEnabled(ui.SourceFilesTable->selectedItems().count() != 0);
}

// Retrieve relative Z values from timestamps
void FQPlotWindow::on_GetZFromTimeStampButton_clicked(bool checked)
{
	if (slices_.nItems() == 0) return;

// 	QString dir = QInputDialog::getText(this, "Choose File Location", "Select the location of the files that will be interrogated:", QLineEdit::Normal, 
	bool ok;
	QString extension = QInputDialog::getItem(this, "TimeStamp Extraction", "Select the type of file to look for:", QStringList() << "log" << "raw" << "nxs", 0, false, &ok);
	if (!ok) return;

	QString dirString = QFileDialog::getExistingDirectory(this, "TimeStamp Extraction", "Choose the directory containing the required files:");
	if (dirString.isEmpty()) return;
	QDir dir(dirString);
	
	// Load timestamp data from files - set offset in seconds from an arbitrary point to start with, then adjust afterwards
	QString s;
	double earliest = 0.0;
	QDateTime referenceTime(QDate(1970,1,1));
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next)
	{
		// Construct filename to search for
		s = dir.absoluteFilePath(slice->baseFileName()) + "." + extension;
		QFileInfo fileInfo(s);
		if (!fileInfo.exists())
		{
			QMessageBox::warning(this, "Failed to Open File", "The file '" + s + "' could not be found.");
			break;
		}
		slice->setZ(referenceTime.secsTo(fileInfo.lastModified()));
		if ((earliest == 0) || (slice->z() < earliest)) earliest = slice->z();
	}
	
	// Set correct offset
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next) slice->setZ(slice->z() - earliest);
	
	setAsModified();
	updateSourceDataTab();
	updateSurface();
}

// Update source data
void FQPlotWindow::updateSourceDataTab()
{
	refreshing_ = true;
	ui.SourceDirEdit->setText(dataFileDirectory_.absolutePath());

	ui.SourceFilesTable->clearContents();
	ui.SourceFilesTable->setRowCount(slices_.nItems());
	int count = 0;
	for (Slice* slice = slices_.first(); slice != NULL; slice = slice->next)
	{
		QTableWidgetItem* item = new QTableWidgetItem(slice->fileName());
		ui.SourceFilesTable->setItem(count, 0, item);
		item = new QTableWidgetItem(QString::number(slice->z()));
		ui.SourceFilesTable->setItem(count, 1, item);
		++count;
	}
	ui.SourceFilesTable->resizeColumnsToContents();
	
	refreshing_ = false;
}

/*
 * Tabs - Transform
 */

bool FQPlotWindow::transformTypeChanged(int axis, int index)
{
	if (refreshing_) return false;
	transformType_[axis] = (DataTransform) index;
	calculateTransformLimits();
	setAsModified();
	updateTransformTab();
	updateViewTab();
	updateSurface();
	return true;
}

bool FQPlotWindow::transformValueChanged(int axis, double value)
{
	if (refreshing_) return false;
	transformValue_[axis] = value;
	calculateTransformLimits();
	setAsModified();
	updateTransformTab();
	updateViewTab();
	updateSurface();
	return true;
}

bool FQPlotWindow::transformShiftChanged(int axis, bool pre, double value)
{
	if (refreshing_) return false;
	if (pre) preTransformShift_[axis] = value;
	else postTransformShift_[axis] = value;
	calculateTransformLimits();
	setAsModified();
	updateTransformTab();
	updateViewTab();
	updateSurface();
	return true;
}

bool FQPlotWindow::transformLimitChanged(int axis, bool minLim, double value)
{
	if (refreshing_) return false;
	if (minLim) limitMin_[axis] = value;
	else limitMax_[axis] = value;
	calculateTransformLimits();
	setAsModified();
	updateTransformTab();
	updateViewTab();
	updateSurface();
	return true;
}

void FQPlotWindow::on_TransformXTypeCombo_currentIndexChanged(int index)
{
	if (transformTypeChanged(0, index)) ui.TransformXValueSpin->setEnabled(index < 2);
}

void FQPlotWindow::on_TransformYTypeCombo_currentIndexChanged(int index)
{
	if (transformTypeChanged(1, index)) ui.TransformYValueSpin->setEnabled(index < 2);
}

void FQPlotWindow::on_TransformZTypeCombo_currentIndexChanged(int index)
{
	if (transformTypeChanged(2, index)) ui.TransformZValueSpin->setEnabled(index < 2);
}

void FQPlotWindow::on_TransformXValueSpin_valueChanged(double value)
{
	transformValueChanged(0, value);
}

void FQPlotWindow::on_TransformYValueSpin_valueChanged(double value)
{
	transformValueChanged(0, value);
}

void FQPlotWindow::on_TransformZValueSpin_valueChanged(double value)
{
	transformValueChanged(0, value);
}

void FQPlotWindow::on_TransformXPreShiftSpin_valueChanged(double value)
{
	transformShiftChanged(0, true, value);
}

void FQPlotWindow::on_TransformYPreShiftSpin_valueChanged(double value)
{
	transformShiftChanged(1, true, value);
}

void FQPlotWindow::on_TransformZPreShiftSpin_valueChanged(double value)
{
	transformShiftChanged(2, true, value);
}

void FQPlotWindow::on_TransformXPostShiftSpin_valueChanged(double value)
{
	transformShiftChanged(0, false, value);
}

void FQPlotWindow::on_TransformYPostShiftSpin_valueChanged(double value)
{
	transformShiftChanged(1, false, value);
}

void FQPlotWindow::on_TransformZPostShiftSpin_valueChanged(double value)
{
	transformShiftChanged(2, false, value);
}

void FQPlotWindow::on_LimitXMinSpin_valueChanged(double value)
{
	transformLimitChanged(0, true, value);
}

void FQPlotWindow::on_LimitYMinSpin_valueChanged(double value)
{
	transformLimitChanged(1, true, value);
}

void FQPlotWindow::on_LimitZMinSpin_valueChanged(double value)
{
	transformLimitChanged(2, true, value);
}

void FQPlotWindow::on_LimitXMaxSpin_valueChanged(double value)
{
	transformLimitChanged(0, false, value);
}

void FQPlotWindow::on_LimitYMaxSpin_valueChanged(double value)
{
	transformLimitChanged(1, false, value);
}

void FQPlotWindow::on_LimitZMaxSpin_valueChanged(double value)
{
	transformLimitChanged(2, false, value);
}

// Update Transform tab
void FQPlotWindow::updateTransformTab()
{
	refreshing_ = true;

	// Transform type / value
	ui.TransformXTypeCombo->setCurrentIndex(transformType_[0]);
	ui.TransformYTypeCombo->setCurrentIndex(transformType_[1]);
	ui.TransformZTypeCombo->setCurrentIndex(transformType_[2]);
	ui.TransformXValueSpin->setEnabled(transformType_[0] < 2);
	ui.TransformYValueSpin->setEnabled(transformType_[1] < 2);
	ui.TransformZValueSpin->setEnabled(transformType_[2] < 2);
	ui.TransformXValueSpin->setValue(transformValue_.x);
	ui.TransformYValueSpin->setValue(transformValue_.y);
	ui.TransformZValueSpin->setValue(transformValue_.z);

	// Shifts
	ui.TransformXPreShiftSpin->setValue(preTransformShift_.x);
	ui.TransformYPreShiftSpin->setValue(preTransformShift_.y);
	ui.TransformZPreShiftSpin->setValue(preTransformShift_.z);
	ui.TransformXPostShiftSpin->setValue(postTransformShift_.x);
	ui.TransformYPostShiftSpin->setValue(postTransformShift_.y);
	ui.TransformZPostShiftSpin->setValue(postTransformShift_.z);

	// Limits
	ui.LimitXMinSpin->setRange(transformMin_.x, transformMax_.x);
	ui.LimitYMinSpin->setRange(transformMin_.y, transformMax_.y);
	ui.LimitZMinSpin->setRange(transformMin_.z, transformMax_.z);
	ui.LimitXMaxSpin->setRange(transformMin_.x, transformMax_.x);
	ui.LimitYMaxSpin->setRange(transformMin_.y, transformMax_.y);
	ui.LimitZMaxSpin->setRange(transformMin_.z, transformMax_.z);
	ui.LimitXMinSpin->setValue(limitMin_.x);
	ui.LimitYMinSpin->setValue(limitMin_.y);
	ui.LimitZMinSpin->setValue(limitMin_.z);
	ui.LimitXMaxSpin->setValue(limitMax_.x);
	ui.LimitYMaxSpin->setValue(limitMax_.y);
	ui.LimitZMaxSpin->setValue(limitMax_.z);
	ui.LimitXMinLabel->setText(QString::number(transformMin_.x));
	ui.LimitXMaxLabel->setText(QString::number(transformMax_.x));
	ui.LimitYMinLabel->setText(QString::number(transformMin_.y));
	ui.LimitYMaxLabel->setText(QString::number(transformMax_.y));
	ui.LimitZMinLabel->setText(QString::number(transformMin_.z));
	ui.LimitZMaxLabel->setText(QString::number(transformMax_.z));

	refreshing_ = false;
}

/*
 * Tabs - Colour
 */

void FQPlotWindow::on_ColourScaleTable_itemSelectionChanged()
{
	if (refreshing_) return;
	bool selected = (ui.ColourScaleTable->selectedItems().count() != 0);
	ui.RemoveColourScalePointButton->setEnabled(selected);
}

void FQPlotWindow::on_ColourScaleTable_cellDoubleClicked(int row, int column)
{
	if (refreshing_ || (column == 0)) return;

	// Get item data (contains ColourScalePoint index)
	QTableWidgetItem* item = ui.ColourScaleTable->item(row, column);
	if (!item) return;
	int cspId = item->data(Qt::UserRole).toInt();
	
        // Request a colour dialog
        QColor newcol = QColorDialog::getColor(item->backgroundColor().rgba(), this, "Select new colour", QColorDialog::ShowAlphaChannel);
        if (!newcol.isValid()) return;
	
	ui.ColourScaleWidget->setPointColour(cspId, newcol);

	// Update table and refresh surface
	item->setBackgroundColor(newcol);
	updateSurface(false);
	setAsModified();
}

void FQPlotWindow::on_ColourScaleTable_cellChanged(int row, int column)
{
	if (refreshing_ || (column == 1)) return;
	
	// Get item data (contains ColourScalePoint index)
	QTableWidgetItem* item = ui.ColourScaleTable->item(row, column);
	if (!item) return;
	int cspId = item->data(Qt::UserRole).toInt();
	
	// Value of this point has changed, so must update the list and refresh the surface and necessary widgets
	ui.ColourScaleWidget->setPointValue(cspId, item->text().toDouble());

	// Refresh table
	updateColourTab();
	updateSurface(false);
	setAsModified();
}

void FQPlotWindow::on_AddColourScalePointButton_clicked(bool checked)
{
	ui.ColourScaleWidget->addPoint(ui.ColourScaleWidget->lastPoint() ? ui.ColourScaleWidget->lastPoint()->value() + 1.0 : 0.0, Qt::white);

	// Refresh table
	updateColourTab();
	updateSurface(false);
	setAsModified();
}

void FQPlotWindow::on_RemoveColourScalePointButton_clicked(bool checked)
{
	if (ui.ColourScaleTable->selectedItems().count() == 0) return;

	// Get list of items to delete
	int lastId = -1;
	RefList<ColourScalePoint,int> toDelete;
	foreach(QTableWidgetItem* item, ui.ColourScaleTable->selectedItems())
	{
		// Get point id from item
		int cspId = item->data(Qt::UserRole).toInt();
		if (lastId == cspId) continue;
		lastId = cspId;
		toDelete.addUnique(ui.ColourScaleWidget->point(cspId));
	}

	// Now delete the points
	for (RefListItem<ColourScalePoint,int>* ri = toDelete.first(); ri != NULL; ri = ri->next) ui.ColourScaleWidget->removePoint(ri->item);

	// Refresh table
	updateColourTab();
	updateSurface(false);
	setAsModified();
}

// Update Colour tab
void FQPlotWindow::updateColourTab()
{
	refreshing_ = true;

	// ColourScale - add points in reverse order so higher values appear at the top of the list
	ui.ColourScaleTable->clearContents();
	ui.ColourScaleTable->setRowCount(ui.ColourScaleWidget->nPoints());
	QTableWidgetItem *item;
	int count = 0;
	for (ColourScalePoint *csp = ui.ColourScaleWidget->lastPoint(); csp != NULL; csp = csp->prev)
	{
		item = new QTableWidgetItem(QString::number(csp->value()));
		item->setData(Qt::UserRole, ui.ColourScaleWidget->nPoints() - (count+1));
		ui.ColourScaleTable->setItem(count, 0, item);
		item = new QTableWidgetItem();
		item->setBackgroundColor(csp->colour());
		item->setData(Qt::UserRole, ui.ColourScaleWidget->nPoints() - (count+1));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable	);
		ui.ColourScaleTable->setItem(count, 1, item);
		++count;
	}
	// Select first item in list
	ui.ColourScaleTable->setCurrentItem(0);
	
	refreshing_ = false;
}

/*
 * Tabs - View
 */

bool FQPlotWindow::viewAxisCrossChanged(int axis, int dir, double value)
{
	if (refreshing_) return false;
	axisPosition_[axis].set(dir,value);
	setAsModified();
	updateSurface(false);
	return true;
}

bool FQPlotWindow::viewAxisTicksChanged(int axis, bool start, double value)
{
	if (refreshing_) return false;
	if (start) axisFirstTick_[axis] = value;
	else axisTickDelta_[axis] = value;
	setAsModified();
	updateSurface(false);
	return true;
}

bool FQPlotWindow::viewAxisOrientationChanged(int axis, int dir, bool direction, double value)
{
	if (refreshing_) return false;
	if (direction) axisLabelDirection_[axis].set(dir, value);
	else axisLabelRotation_[axis] = value;
	setAsModified();
	updateSurface(false);
	return true;
}

void FQPlotWindow::on_ViewInvertZCheck_clicked(bool checked)
{
	if (refreshing_) return;
	invertZAxis_ = checked;
	setAsModified();
	ui.MainView->setInvertZ(invertZAxis_);
	ui.MainView->update();
}

void FQPlotWindow::on_ViewXAxisCrossAtYSpin_valueChanged(double value)
{
	viewAxisCrossChanged(0, 1, value);
}

void FQPlotWindow::on_ViewXAxisCrossAtZSpin_valueChanged(double value)
{
	viewAxisCrossChanged(0, 2, value);
}

void FQPlotWindow::on_ViewXAxisAutoTicksCheck_clicked(bool checked)
{
	axisAutoTicks_.x = checked;
	updateViewTab();
	updateSurface(false);
}

void FQPlotWindow::on_ViewXAxisTicksStartSpin_valueChanged(double value)
{
	viewAxisTicksChanged(0, true, value);
}

void FQPlotWindow::on_ViewXAxisTicksDeltaSpin_valueChanged(double value)
{
	viewAxisTicksChanged(0, false, value);
}

void FQPlotWindow::on_ViewXAxisDirectionXSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(0, 0, true, value);
}

void FQPlotWindow::on_ViewXAxisDirectionYSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(0, 1, true, value);
}

void FQPlotWindow::on_ViewXAxisDirectionZSpin_valueChanged(double value)
{
	viewAxisOrientationChanged(0, 2, true, value);
}

void FQPlotWindow::on_ViewYAxisCrossAtXSpin_valueChanged(double value)
{
	viewAxisCrossChanged(1, 0, value);
}

void FQPlotWindow::on_ViewYAxisCrossAtZSpin_valueChanged(double value)
{
	viewAxisCrossChanged(1, 2, value);
}

void FQPlotWindow::on_ViewYAxisAutoTicksCheck_clicked(bool checked)
{
	axisAutoTicks_.y = checked;
	updateViewTab();
	updateSurface(false);
}

void FQPlotWindow::on_ViewYAxisTicksStartSpin_valueChanged(double value)
{
	viewAxisTicksChanged(1, true, value);
}

void FQPlotWindow::on_ViewYAxisTicksDeltaSpin_valueChanged(double value)
{
	viewAxisTicksChanged(1, false, value);
}

void FQPlotWindow::on_ViewZAxisCrossAtXSpin_valueChanged(double value)
{
	viewAxisCrossChanged(2, 0, value);
}

void FQPlotWindow::on_ViewZAxisCrossAtYSpin_valueChanged(double value)
{
	viewAxisCrossChanged(2, 1, value);
}

void FQPlotWindow::on_ViewZAxisAutoTicksCheck_clicked(bool checked)
{
	axisAutoTicks_.z = checked;
	updateViewTab();
	updateSurface(false);
}

void FQPlotWindow::on_ViewZAxisTicksStartSpin_valueChanged(double value)
{
	viewAxisTicksChanged(2, true, value);
}

void FQPlotWindow::on_ViewZAxisTicksDeltaSpin_valueChanged(double value)
{
	viewAxisTicksChanged(2, false, value);
}

// Update View tab
void FQPlotWindow::updateViewTab()
{
	refreshing_ = true;

	// Axis positions
	// -- X
	ui.ViewXAxisCrossAtYSpin->setRange(limitMin_.y, limitMax_.y);
	ui.ViewXAxisCrossAtZSpin->setRange(limitMin_.z, limitMax_.z);
	ui.ViewXAxisCrossAtYSpin->setValue(axisPosition_[0].y);
	ui.ViewXAxisCrossAtZSpin->setValue(axisPosition_[0].z);
	// -- Y
	ui.ViewYAxisCrossAtXSpin->setRange(limitMin_.x, limitMax_.x);
	ui.ViewYAxisCrossAtZSpin->setRange(limitMin_.z, limitMax_.z);
	ui.ViewYAxisCrossAtXSpin->setValue(axisPosition_[0].x);
	ui.ViewYAxisCrossAtZSpin->setValue(axisPosition_[0].z);
	// -- Z
	ui.ViewZAxisCrossAtXSpin->setRange(limitMin_.x, limitMax_.x);
	ui.ViewZAxisCrossAtYSpin->setRange(limitMin_.y, limitMax_.y);
	ui.ViewZAxisCrossAtXSpin->setValue(axisPosition_[0].x);
	ui.ViewZAxisCrossAtYSpin->setValue(axisPosition_[0].y);
	
	// AxisTicks
	// -- X
	ui.ViewXAxisAutoTicksCheck->setChecked(axisAutoTicks_.x);
	ui.ViewXAxisTicksWidget->setEnabled(!axisAutoTicks_.x);
	ui.ViewXAxisTicksStartSpin->setValue(axisFirstTick_.x);
	ui.ViewXAxisTicksDeltaSpin->setValue(axisTickDelta_.x);
	// -- Y
	ui.ViewYAxisAutoTicksCheck->setChecked(axisAutoTicks_.y);
	ui.ViewYAxisTicksWidget->setEnabled(!axisAutoTicks_.y);
	ui.ViewYAxisTicksStartSpin->setValue(axisFirstTick_.y);
	ui.ViewYAxisTicksDeltaSpin->setValue(axisTickDelta_.y);
	// -- Z
	ui.ViewZAxisAutoTicksCheck->setChecked(axisAutoTicks_.z);
	ui.ViewZAxisTicksWidget->setEnabled(!axisAutoTicks_.z);
	ui.ViewZAxisTicksStartSpin->setValue(axisFirstTick_.z);
	ui.ViewZAxisTicksDeltaSpin->setValue(axisTickDelta_.z);
	
	refreshing_ = false;
}
