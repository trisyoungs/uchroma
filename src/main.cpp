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

int main(int argc, char *argv[])
{	
	/* Uncomment here for extra debug output */
// 	msg.addOutputType(Messenger::UndoRedo);

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

	/* Start a new, empty session */
	UChromaSession::startNewSession(true);

	/* Do we have CLI options? */
	if (argc > 1)
	{
		int n = 1;
		while (n < argc)
		{
			if (argv[n][0] != '-')
			{
				// Could be an input file, or could be a dataset
				QFileInfo info(argv[n]);
				if (info.suffix() == "ucr")
				{
					msg.print("Loading input file '%s'...\n", argv[n]);
					if (!UChromaSession::loadSession(argv[n])) return 1;
				}
				else
				{
					if (!UChromaSession::currentCollection())
					{
						msg.print("No collection exists to load data into.\n");
						return 1;
					}
					msg.print("Loading dataset '%s'...\n", argv[n]);
					if (!UChromaSession::currentCollection()->appendDataSet(argv[n])) return 1;
				}

				++n;
				continue;
			}

			bool missingArg = false;

			// Command-line switch
			switch (argv[n][1])
			{
				case ('h'):
					printf("UChroma revision %s, %s\n\nAvailable CLI options are:\n\n", UCHROMAREVISION, UCHROMADATE);
					printf("\t-h\t\tShow this help\n");
					printf("\t-a\tForce warnings generated during input file read to be treated as errors.\n");
					printf("\t-v\tEnable verbosity (for debugging purposes).\n");
					return 1;
					break;
				case ('a'):
					UChromaSession::setHardIOFail(true);
					break;
				case ('v'):
					msg.addOutputType(Messenger::Verbose);
					break;
				default:
					msg.print("Unrecognised command-line switch '%s'.\n", argv[n]);
					msg.print("Run with -h to see available switches.\n");
					return 1;
					break;
			}

			// Check for missing argument flag
			if (missingArg)
			{
				msg.print("Error: Argument expected but none was given for switch '%s'\n", argv[n]);
				return 1;
			}

			++n;
		}
	}

	/* Direct Messenger to the LogWindow */
	msg.setTextBrowser(mainWindow.logWindowBrowser());

	/* Update main window */
	mainWindow.updateGUI();

	/* Show the main window */
	mainWindow.show();

	/* Enter Qt's main events loop */
	return app.exec();
}

