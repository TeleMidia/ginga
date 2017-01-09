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

#ifndef LINKASSESSMENT_H_
#define LINKASSESSMENT_H_

#include <string>
#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
	class LinkAssessment {
		protected:
			set<string> typeSet;
		public:
			LinkAssessment() {
				typeSet.insert("LinkAssessment");
			}
			virtual ~LinkAssessment() {}
			virtual string getValue()=0;
			bool instanceOf(string type) {
				return typeSet.count(type);
			}
	};
}
}
}
}
}
}
}

#endif /*LINKASSESSMENT_H_*/
