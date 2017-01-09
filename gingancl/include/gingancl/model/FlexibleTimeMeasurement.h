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

#ifndef _FLEXIBLETIMEMEASUREMENT_H_
#define _FLEXIBLETIMEMEASUREMENT_H_

#include "TimeMeasurement.h"

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace time {
	class FlexibleTimeMeasurement : public TimeMeasurement {
		protected:
			double computedValue;
			double minimumValue; // duration lower bound
			double maximumValue; // duration upper bound
			double minFeasibleValue;
			double maxFeasibleValue;

		public:
			FlexibleTimeMeasurement(double expectedValue, double minValue,
		    	double maxValue);

		    virtual ~FlexibleTimeMeasurement(){};
		    double getComputedValue();
		    double getOptimumValue();
		    void setComputedValue(double value);
		    void setOptimumValue(double optValue);
		    double getMaximumValue();
		    double getMaximumFeasibleValue();
		    double getMinimumFeasibleValue();
		    double getMinimumValue();
		    bool isPredictable();
		    bool isComputed();
		    bool isUpperBounded();

		private:
			void setBoundaryValues(double minValue, double maxValue);

		protected:
			virtual void overwrite(FlexibleTimeMeasurement* dur);

		public:
			TimeMeasurement* duplicate();
			string toString();
			double getValue();
			void setMinimumValue(double value);
			void setMaximumValue(double value);
			void setMinimumFeasibleValue(double value);
			void setMaximumFeasibleValue(double value);
	};
}
}
}
}
}
}
}

#endif //_FLEXIBLETIMEMEASUREMENT_H_
