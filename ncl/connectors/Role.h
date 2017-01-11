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

#ifndef ROLE_H_
#define ROLE_H_

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {
	class Role {
		protected:
			string label;
			short eventType;
			int maxCon, minCon;

		public:
			Role();
			virtual ~Role();

			static const int UNBOUNDED = 2^30;

			virtual short getEventType();
			virtual string getLabel();
			virtual void setEventType(short type);
			virtual void setLabel(string id);
			
			virtual int getMinCon();
			virtual int getMaxCon();
			
			virtual void setMinCon(int minCon);
			virtual void setMaxCon(int maxCon);
			virtual bool instanceOf(string type) {
				if (type == "Role") {
					return true;
				}

				return false;
			};
	};
}
}
}
}
}

#endif /*ROLE_H_*/
