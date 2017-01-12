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

#ifndef _TEMPORALFLEXIBILITYFUNCTION_H_
#define _TEMPORALFLEXIBILITYFUNCTION_H_

#include <iostream>
#include <string>
#include <set>
using namespace std;

#include "../Entity.h"
using namespace br::pucrio::telemidia::ncl;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace time {
namespace flexibility {
	class TemporalFlexibilityFunction : public Entity {
		protected:
			string id;
			double shrinkingFactor;
			double stretchingFactor;
			set<string> typeSet;
		public:
			TemporalFlexibilityFunction(
							string id,
							double shrinkingFactor,
							double stretchingFactor);

			virtual ~TemporalFlexibilityFunction(){};
			bool instanceOf(string s);
			double getShrinkingFactor();
			double getStretchingFactor();
			void setShrinkingFactor(double factor);
			void setStretchingFactor(double factor);
	};
}
}
}
}
}
}

#endif //_TEMPORALFLEXIBILITYFUNCTION_H_
