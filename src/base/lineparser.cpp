/*
	*** Line Parsing Routines
	*** src/base/lineparser.cpp
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

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "base/messenger.h"
#include <string.h>
#include <stdarg.h>

// Constructors
LineParser::LineParser(QString fileName, bool writeFile)
{
	fileName_ = fileName;
	writing_ = writeFile;

	// Attempt to open the file and, if successful, create a QTextStream for it
	file_.setFileName(fileName_);
	stream_ = NULL;
	if (writing_)
	{
		file_.open(QFile::WriteOnly);
		if (file_.isOpen() && (file_.isWritable())) stream_ = new QTextStream(&file_);
	}
	else
	{
		file_.open(QFile::ReadOnly);
		if (file_.isOpen() && (file_.isReadable())) stream_ = new QTextStream(&file_);
	}
}

// Destructor
LineParser::~LineParser()
{
	file_.close();
	if (stream_) delete stream_;
}

/*
 * Source
 */

// Close file(s)
void LineParser::closeFiles()
{
	file_.close();
	if (stream_)
	{
		delete stream_;
		stream_ = NULL;
	}
}

// Return whether current file / stream are ready for use
bool LineParser::ready() const
{
	if (writing_) return (file_.isOpen() && (file_.isWritable()));
	else return (file_.isOpen() && (file_.isReadable()));
}

// Return whether the end of the input stream has been reached
bool LineParser::atEnd() const
{
	if (!stream_) return false;
	return stream_->atEnd();
}

/*
 * Write
 */



// Write line to file
bool LineParser::writeLine(QString s)
{
	if ((!writing_) || (!stream_)) return false;

	(*stream_) << s;

	return true;
}

// Write formatter line to file
bool LineParser::writeLineF(const char* fmt, ...)
{
	if ((!writing_) || (!stream_)) return false;

	// Construct line
	va_list arguments;
	static char s[8096];
	s[0] = '\0';
	// Parse the argument list (...) and internally write the output string into s[]
	va_start(arguments,fmt);
	vsprintf(s,fmt,arguments);
	va_end(arguments);

	(*stream_) << s;
	
	return true;
}

/*
 * Read
 */

// Skip 'n' lines from file
bool LineParser::skipLines(int nSkip)
{
	if (writing_ || (!stream_)) return false;
	
	for (int n=0; n<nSkip; n++)
	{
		line_ = stream_->readLine();
		if (line_.isNull()) return false;
	}
	return true;

}

// Gets next delimited arg from internal line
bool LineParser::getNextArg(int optionMask, QString& destArg)
{
	// Get the next input chunk from the internal string and put into argument specified.
	msg.enter("LineParser::getNextArg");
	bool done, hadquotes;
	char c, quotechar = '\0';
	done = false;
	hadquotes = false;
	endOfLine_ = false;

	// Reset destArg
	destArg = QString();

	// If we are already at the end of the string, return false
	if ((linePos_ == lineLength_) || (line_.at(linePos_) == QChar('\0')))
	{
		endOfLine_ = true;
		return false;
	}

	while (linePos_ < lineLength_)
	{
		c = line_.at(linePos_).toLatin1();
		switch (c)
		{
			// End of line markers
			case (10):	// Line feed (\n)
			case (13):	// Carriage Return
				done = true;
				endOfLine_ = true;
				break;
			// Delimiters
			// If we encounter one and arg length != 0 this signals the end of the argument.
			case (','):	// Comma
				if (optionMask&LineParser::NormalCommas)
				{
					destArg += c;
					break;
				}
			case (9):	// Horizontal Tab
			case (' '):	// Space
				if (quotechar != '\0') destArg += c;
				else if (!destArg.isEmpty()) done = true;
				break;
			// Quote marks
			// If LineParser::UseQuotes, keep delimiters and other quote marks inside the quoted text.
			case (34):	// Double quotes
			case (39):	// Single quotes
				if (!(optionMask&LineParser::UseQuotes)) break;
				if (quotechar == '\0') quotechar = c;
				else if (quotechar == c)
				{
					quotechar = '\0';
					hadquotes = true;
					done = true;
				}
				else destArg += c;
				break;
			// Curly brackets - treat in the same way as quotes
			case ('{'):
			case ('}'):
				// If explicitly not useing braces, add as normal character
				if (!(optionMask&LineParser::UseBraces)) destArg += c;
				else
				{
					// If the quotechar is a left brace and we have a right brace, stop quoting
					if ((quotechar == '{') && (c == '}'))
					{
						quotechar = '\0';
						break;
					}
					// If we are already quoting by some other means, add character and exit
					if (quotechar != '\0') destArg += c;
					// No previous quoting, so begin quoting if '{'
					if (c == '{') quotechar = c;
				}
				break;
			// Parentheses
			case ('('):	// Left parenthesis
			case (')'):	// Right parenthesis
				if (optionMask&LineParser::StripBrackets) break;
				destArg += c;
				break;
			// Comment markers
			case ('#'):	// "#" Rest/all of line is a comment
				endOfLine_ = true;
				done = true;
				break;
			// Some other character
			default: 
				destArg += c;
				break;
		}

		// Increment line position
		++linePos_;
		if (done) break;
	}

	if (linePos_ == lineLength_) endOfLine_ = true;

	return (destArg.isEmpty() ? (hadquotes ? true : false) : true);
}

// Read line from file
bool LineParser::getLine(QString& destination)
{
	if (writing_)
	{
		msg.print("Internal Error: Tried to read from a LineParser create to write.\n");
		return false;
	}
	if (!stream_)
	{
		msg.print("Internal Error: No valid stream in LineParser::getLine().\n");
		return false;
	}

	destination = stream_->readLine();

	if (destination.isNull() || atEnd()) return false;
	return true;
}

// Parse delimited (from file)
bool LineParser::getArgs(int optionMask)
{
	if (writing_)
	{
		msg.print("Internal Error: Tried to read from a LineParser create to write.\n");
		return false;
	}
	if (!stream_)
	{
		msg.print("Internal Error: No valid stream in LineParser::getArgs().\n");
		return false;
	}

	bool done = false;
	QString tempArg;
	int result;
	// Returns : 0=ok, 1=error, -1=eof
	do
	{
		// Read line from file and parse it
		line_ = stream_->readLine();
		if (line_.isNull()) return false;

		// Assume that we will finish after parsing the line we just read in
		done = true;

		// Parse the current line
		arguments_.clear();
		linePos_ = 0;
		lineLength_ = line_.length();
		endOfLine_ = false;
		while (!endOfLine_)
		{
			// Get next delimiter argument in the line
			if (getNextArg(optionMask, tempArg))
			{
		// 			printf("getAllArgsDelim arg=%i [%s]\n", arguments_.nItems(), arg->get());
				arguments_ << tempArg;
			}
		}

		// To check for blank lines, do the parsing and then check nargs()
		if ((optionMask&LineParser::SkipBlanks) && (nArgs() == 0)) done = false;
	} while (!done);

	return true;
}

// Returns number of arguments grabbed from last parse
int LineParser::nArgs() const
{
	return arguments_.size();
}


// Returns the specified argument as a QString
const QString& LineParser::argString(int i)
{
	static QString dummy("NULL");
	if ((i < 0) || (i >= nArgs()))
	{
		printf("Warning: Argument %i is out of range - returning \"NULL\"...\n", i);
		return dummy;
	}
	return arguments_.at(i);
}

// Returns the specified argument as a char string
const char* LineParser::argChar(int i)
{
	if ((i < 0) || (i >= nArgs()))
	{
		printf("Warning: Argument %i is out of range - returning \"NULL\"...\n", i);
		return "NULL";
	}
	return qPrintable(arguments_.at(i));
}

// Returns the specified argument as an integer
int LineParser::argi(int i)
{
	if ((i < 0) || (i >= nArgs()))
	{
		printf("Warning: Argument %i is out of range - returning 0...\n", i);
		return 0;
	}
	return arguments_.at(i).toInt();
}

// Returns the specified argument as a double
double LineParser::argd(int i)
{
	if ((i < 0) || (i >= nArgs()))
	{
		printf("Warning: Argument %i is out of range - returning 0.0...\n", i);
		return 0.0;
	}
	return arguments_.at(i).toDouble();
}

// Returns the specified argument as a bool
bool LineParser::argb(int i)
{
	if ((i < 0) || (i >= nArgs()))
	{
		printf("Warning: Argument %i is out of range - returning false...\n", i);
		return false;
	}

	// Convert string to boolean
	QString lcase(arguments_.at(i).toLower());
	if (lcase == "off") return false;
	else if (lcase == "on") return true;
	else if (lcase == "no") return false;
	else if (lcase == "yes") return true;
	else if (lcase == "false") return false;
	else if (lcase == "true") return true;

	msg.print("Character constant '%s' doesn't translate directly to a boolean value - false assumed.\n", qPrintable(lcase));
	return false;
}

// Returns the specified argument as a float
float LineParser::argf(int i)
{
	if ((i < 0) || (i >= nArgs()))
	{
		printf("Warning: Argument %i is out of range - returning 0.0f...\n", i);
		return 0.0f;
	}
	return arguments_.at(i).toFloat();
}

// Returns whether the specified argument exists (has been provided)
bool LineParser::hasArg(int i) const
{
	if ((i < 0) || (i >= nArgs())) return false;
	return true;
}

