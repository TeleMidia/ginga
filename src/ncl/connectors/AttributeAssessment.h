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

#ifndef ATTRIBUTEASSESSMENT_H_
#define ATTRIBUTEASSESSMENT_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "Assessment.h"
#include "EventUtil.h"
#include "Role.h"

//#include <limits>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {
	class AttributeAssessment : public Assessment, public Role {
		private:
			short attributeType;
			string key;
			string offset;

		public:
			AttributeAssessment(string role);
			virtual ~AttributeAssessment() {};
			void setMaxCon(int max);
			void setMinCon(int min);
			short getAttributeType();
			void setAttributeType(short attribute);
			string getKey();
			void setKey(string key);
			string getOffset();
			void setOffset(string offset);
			string toString();
			bool instanceOf(string type) {
				return Assessment::instanceOf(type);
			}
	};
}
}
}
}
}

#endif /*ATTRIBUTEASSESSMENT_H_*/
