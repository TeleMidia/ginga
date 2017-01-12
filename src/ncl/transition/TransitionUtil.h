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

#ifndef TRANSITIONUTIL_H_
#define TRANSITIONUTIL_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include <string>
using namespace std;

#include "Transition.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace transition {
	class TransitionUtil {
		public:
			static int getTypeCode(string type);
			static string getTypeName(int type);
			static int getSubtypeCode(int type, string subtype);
			static string getSubtypeName(int type, int subtype);
			static short getDirectionCode(string direction);
			static string getDirectionName(short direction);
			static int getDefaultSubtype(int type);
	};
}
}
}
}
}

#endif /*TRANSITIONUTIL_H_*/
