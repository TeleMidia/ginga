/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef _PRESENTATIONEVENT_H_
#define _PRESENTATIONEVENT_H_

#include "ncl/interfaces/ContentAnchor.h"
#include "ncl/interfaces/IntervalAnchor.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "ncl/connectors/EventUtil.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "AnchorEvent.h"

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace event {
  class PresentationEvent : public AnchorEvent {
	public:
		static const double UNDEFINED_INSTANT;

	private:
		double begin;
		double end;
		double duration;
		long numPresentations;
		double repetitionInterval;

	public:
		PresentationEvent(
			    string id,
			    void* executionObject,
			    ContentAnchor* anchor);

		virtual ~PresentationEvent();

		bool stop();
		double getDuration();
		double getRepetitionInterval();
		long getRepetitions();
		void setDuration(double dur);
		void setEnd(double e);
		void setRepetitionSettings(
			    long repetitions, double repetitionInterval);

		double getBegin();
		double getEnd();
		void incrementOccurrences();
		static bool isUndefinedInstant(double value);
  };
}
}
}
}
}
}
}

#endif //_PRESENTATIONEVENT_H_
