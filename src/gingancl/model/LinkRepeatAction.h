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

#ifndef _LINKREPEATACTION_H_
#define _LINKREPEATACTION_H_

#include "LinkSimpleAction.h"

#include "PresentationEvent.h"
#include "FormatterEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include <set>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
	class LinkRepeatAction : public LinkSimpleAction {
		private:
			long repetitions;
			double repetitionInterval;

			virtual void run();

		public:
			LinkRepeatAction(FormatterEvent* event, short actionType);
			virtual ~LinkRepeatAction();
			long getRepetitions();
			double getRepetitionInterval();
			void setRepetitions(long repetitions);
			void setRepetitionInterval(double delay);
	};
}
}
}
}
}
}
}

#endif //_LINKREPEATACTION_H_
