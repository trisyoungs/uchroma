/*
	*** uChroma Main
	*** src/main.cpp
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

#include "version.h"
#include "gui/uchroma.h"

#include "parser/tree.h"

int main(int argc, char *argv[])
{
	// Uncomment here for extra debug output
// 	msg.addOutputType(Messenger::Verbose);

	/* Create the main QApplication */
	QApplication app(argc, argv, QApplication::GuiClient);
	QCoreApplication::setOrganizationName("uChroma");
	QCoreApplication::setOrganizationDomain("www.projectaten.net");
	QCoreApplication::setApplicationName("uChroma");

	#if QT_VERSION >= 0x040600
	QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
	#endif

	/* Tweak the default QGLFormat */
	QGLFormat::defaultFormat().setSampleBuffers(true);

	/* Create the main window */
	UChromaWindow mainWindow;

	/* Was an input file supplied? */
	if (argc == 2)
	{
		if (!mainWindow.loadInputFile(argv[1])) return 1;
	}

	/* Update main window */
	mainWindow.updateGUI();

	/* Show the main window */
	mainWindow.show();

	/* Set status to not modified, if we did not load a file */
	if (argc != 2) mainWindow.setAsModified(false);

	/* Enter Qt's main events loop */
	return app.exec();
}

