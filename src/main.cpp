/*
	*** FQPlot Main
	*** src/main.cpp
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

#include "version.h"
#include "gui/fqplot.h"

int main(int argc, char *argv[])
{
	/* Create the main QApplication */
	QApplication app(argc, argv, QApplication::GuiClient);
	QCoreApplication::setOrganizationName("FQPlot");
	QCoreApplication::setOrganizationDomain("www.projectaten.net");
	QCoreApplication::setApplicationName("FQPlot");

	#if QT_VERSION >= 0x040600
        QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
        #endif
	
	/* Tweak the default QGLFormat */
	QGLFormat::defaultFormat().setSampleBuffers(true);

	/* Create the main window */
	FQPlotWindow mainWindow;

	/* Was an input file supplied? */
	if (argc == 2)
	{
		if (!mainWindow.loadData(argv[1])) return 1;
	}

	/* Update main window */
	// TODO Put these functions in an 'updateAfterLoad()' function
	mainWindow.calculateDataLimits();
	mainWindow.updateColourScale();
	mainWindow.updateAllTabs();
	mainWindow.updateTitleBar();
		
	/* Show the main window */
	mainWindow.show();

	/* Enter Qt's main events loop */
	return app.exec();
}

