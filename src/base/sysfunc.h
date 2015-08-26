/*
	*** System functions
	*** src/base/sysfunc.h
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

#ifndef UCHROMA_SYSFUNC_H
#define UCHROMA_SYSFUNC_H

#include <QString>

// Enum search and print
int enumSearch(QString name, int nitems, const char* *list, QString query, bool reportError = 1);
void enumPrintValid(int nitems, const char* *list);

#endif
