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

#ifndef EVENTTRANSITION_H_
#define EVENTTRANSITION_H_

#include "PresentationEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include <string>
#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace event {
namespace transition {
  class EventTransition {
	protected:
		set<string> typeSet;

	private:
		PresentationEvent* event;
		double time;

	public:
		EventTransition(double time, PresentationEvent* event);
		virtual ~EventTransition();

		int compareTo(EventTransition* object);

	private:
		int compareType(EventTransition* otherEntry);

	public:
		bool equals(EventTransition* object);
		PresentationEvent* getEvent();
		double getTime();
		bool instanceOf(string s);
  };
}
}
}
}
}
}
}
}

#endif /*EVENTTRANSITION_H_*/
