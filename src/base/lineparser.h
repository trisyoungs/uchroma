/*
	*** Line Parsing Routines
	*** src/base/lineparser.h
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

#ifndef UCHROMA_LINEPARSER_H
#define UCHROMA_LINEPARSER_H

#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

// Forward Declarations
/* none */

// Line Parser
class LineParser
{
	public:
	// Constructors / Destructor
	LineParser(QString fileName, bool writeFile = false);
	~LineParser();
	/// Parse Options Enum
	enum ParseOption
	{
		Defaults = 0,		/**< Default parsing behaviour */
		StripComments = 1,	/**< Automatically strip comments (beginning # or //) from files */
		UseQuotes = 2,		/**< Quoted text in files is retained as a single delimited argument */
		SkipBlanks = 4,		/**< Skip blank lines (those containing nothing or only whitespace) */
		StripBrackets = 8,	/**< Remove parentheses during parsing */
		NoEscapes = 16,		/**< Don't convert excaped characters */
		UseBraces = 32,		/**< Text inside curly brackets is retaind as a single argument */
		NormalCommas = 64,	/**< Don't treat commas as delimiting marks in files */
		DummyOption = 128,
		nParseOptions = 9
	};


	/*
	 * Source
	 */
	private:
	// Whether we are reading or writing
	bool writing_;
	// Target filename
	QString fileName_;
	// File object
	QFile file_;
	// Stream for reading / writing
	QTextStream* stream_;

	public:
	// Close file(s)
	void closeFiles();
	// Return whether current file / stream are ready for use
	bool ready() const;
	// Return whether the end of the input stream has been reached
	bool atEnd() const;


	/*
	 * Write Routines
	 */
	public:
	// Write line to file
	bool writeLine(QString s);
	// Write formatter line to file
	bool writeLineF(const char* fmt, ...);


	/*
	 * Read Routines / Data
	 */
	private:
	// Current source line
	QString line_;
	// Length of current source line
	int lineLength_;
	// Current character position in line
	int linePos_;
	// Parsed arguments
	QStringList arguments_;
	// Whether we are at the end of the current line
	bool endOfLine_;

	public:
	// Skip 'n' lines from internal file
	   bool skipLines(int nSkip);
	// Gets next delimited arg from internal line
	bool getNextArg(int optionMask, QString& destarg);
	// Read line from file and do delimited parse
	bool getArgs(int optionMask);
	// Returns number of arguments grabbed from last parse
	int nArgs() const;
	// Returns the specified argument as a QString
	const QString& argString(int i);
	// Returns the specified argument as a char string
	const char* argChar(int i);
	// Returns the specified argument as an integer
	int argi(int i);
	// Returns the specified argument as a double
	double argd(int i);
	// Returns the specified argument as a bool
	bool argb(int i);
	// Returns the specified argument as a float
	float argf(int i);
	// Returns whether the specified argument exists
	bool hasArg(int i) const;
};

#endif
