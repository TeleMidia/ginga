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

#ifndef _DURATIONFACTORY_H_
#define _DURATIONFACTORY_H_

#include "ncl/time/flexibility/LinearTimeCostFunction.h"
#include "ncl/time/flexibility/TemporalFlexibilityFunction.h"
using namespace br::pucrio::telemidia::ncl::time::flexibility;

#include "UnflexibleDuration.h"
#include "LinearCostFunctionDuration.h"
#include "TimeMeasurement.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace time {
	class DurationFactory {
		public:
			static TimeMeasurement* createDuration(
				    double duration,
				    TemporalFlexibilityFunction* flexFunction);
	};
}
}
}
}
}
}
}

#endif //_DURATIONFACTORY_H_
