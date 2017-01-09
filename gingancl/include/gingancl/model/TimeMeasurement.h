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

#ifndef _TIMEMEASUREMENT_H_
#define _TIMEMEASUREMENT_H_

#include "util/functions.h"
using namespace br::pucrio::telemidia::util;

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace time {
	class TimeMeasurement {
		protected:
			double expectedValue;
			double actualValue;

		public:
			TimeMeasurement(double value);
			virtual ~TimeMeasurement(){};
			double getExpectedValue();
			virtual double getComputedValue();
			virtual void setComputedValue(double value){};
			double getActualValue();
			void setExpectedValue(double value);
			void setActualValue(double value);
			virtual bool isPredictable();
			virtual bool isComputed();
			string toString();

		protected:
			virtual void overwrite(TimeMeasurement* time);

		public:
			virtual TimeMeasurement* duplicate();
			virtual double getValue();
			void fromString(string str);
	};
}
}
}
}
}
}
}

#endif //_TIMEMEASUREMENT_H_
