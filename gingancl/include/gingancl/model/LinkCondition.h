/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

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

#ifndef _LINKCONDITION_H_
#define _LINKCONDITION_H_

#include <vector>
#include <set>
#include <string>
#include <iostream>
using namespace std;

#include "FormatterEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
	class LinkCondition {
		protected:
			set<string>typeSet;

		public:
			LinkCondition() {
				typeSet.insert("LinkCondition");
			};

			virtual ~LinkCondition(){};

			bool instanceOf(string s) {
				if (!typeSet.empty()) {
					return (typeSet.find(s) != typeSet.end());

				} else {
					return false;
				}
			};
			virtual vector<FormatterEvent*>* getEvents()=0;
	};
}
}
}
}
}
}
}

#endif //_LINKCONDITION_H_
