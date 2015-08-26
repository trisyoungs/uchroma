/*
	*** Messaging routines
	*** src/base/messenger.h
	Copyright T. Youngs 2013-2015

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

#ifndef UCHROMA_MESSENGER_H
#define UCHROMA_MESSENGER_H

// Forward Declarations
class QTextBrowser;

// Global messaging and program output levels
class Messenger
{
	public:
	// Constructor
	Messenger();
	// Messaging output types
	enum OutputType { All, Calls, Force, Verbose, UndoRedo, nOutputTypes };
	// Convert text string to debug mode
	static OutputType outputType(const char* name, bool reportError = 0);
	// Convert debug mode to text string
	static const char* outputType(OutputType ot);


	/*
	 * Output Modes
	 */
	private:
	// Bit-vector of active output levels
	int outputTypes_;
	// Call level, used to provide indent for enter/exit info
	int callLevel_;
	// Quiet mode
	bool quiet_;

	public:
	// Add an output type to the output bit-vector
	void addOutputType(Messenger::OutputType);
	// Remove an output type from the output bit-vector
	void removeOutputType(Messenger::OutputType);
	// Returns whether the specified output type is active
	bool isOutputActive(Messenger::OutputType) const;
	// Set status of quiet mode
	void setQuiet(bool b);
	// Return status of quiet mode
	bool isQuiet() const;


	/*
	 * Messaging functions
	 */
	private:
	// Target QTextBrowser (if any)
	QTextBrowser* textBrowser_;

	public:
	// Set target QTextBrowser
	void setTextBrowser(QTextBrowser* browser);
	// Print normal message
	void print(const char*, ...) const;
	// Print message in specific output level
	void print(Messenger::OutputType, const char*, ...) const;
	// Entrances / exits to / from subroutines
	void enter(const char* callname);
	void exit(const char* callname);
};

extern Messenger msg;

#endif
