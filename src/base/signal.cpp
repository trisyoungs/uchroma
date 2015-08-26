/*
	*** Signal
	*** src/base/signal.cpp
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

#include "base/signal.h"
#include "gui/uchroma.h"

// Static Declarations
UChromaWindow* UChromaSignal::uChroma_ = NULL;


// Set UChromaWindow pointer
void UChromaSignal::setMainWindow(UChromaWindow* ptr)
{
	uChroma_ = ptr;
}

// Send Collection-related signal
UChromaSignal::SignalAction UChromaSignal::send(UChromaSignal::CollectionSignal signal, Collection* collection)
{
	if (!uChroma_) return UChromaSignal::IgnoreSignal;

	// Send to ViewPanes in current ViewLayout....
	int nAccepted = 0;
	bool unique = false;
	for (ViewPane* pane = UChromaSession::viewLayout().panes(); pane != NULL; pane = pane->next)
	{
		UChromaSignal::SignalAction result = pane->processCollectionSignal(signal, collection);
		if (result == UChromaSignal::IgnoreSignal) continue;
		++nAccepted;

		// If the object wants sole acceptance of this signal, break now
		if (result == UChromaSignal::UniqueAcceptSignal)
		{
			unique = true;
			break;
		}
	}
	if (nAccepted == 0) return UChromaSignal::IgnoreSignal;
	else return (unique ? UChromaSignal::UniqueAcceptSignal : UChromaSignal::AcceptSignal);
}
