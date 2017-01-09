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

#include "gingancl/model/CostFunctionDuration.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace time {
	CostFunctionDuration::CostFunctionDuration(
		    double expectedValue,
		    double minValue,
		    double maxValue,
		    TemporalFlexibilityFunction* function) : FlexibleTimeMeasurement(
		    	    expectedValue,
		    	    minValue,
		    	    maxValue) {

	    this->costFunction = function;
    }

	CostFunctionDuration::CostFunctionDuration(double expectedValue,
		    TemporalFlexibilityFunction* function) : FlexibleTimeMeasurement(
		    	    expectedValue,
		    	    expectedValue,
		    	    expectedValue) {

	    this->costFunction = function;
		updateDurationInterval();
    }

	TemporalFlexibilityFunction* CostFunctionDuration::getCostFunction() {
		return costFunction;
	}

	void CostFunctionDuration::setCostFunction(
		    TemporalFlexibilityFunction* function) {

		costFunction = function;
		updateDurationInterval();
	}

	void CostFunctionDuration::overwrite(CostFunctionDuration* dur) {
		FlexibleTimeMeasurement::overwrite(dur);
		costFunction = dur->getCostFunction();
	}

	void CostFunctionDuration::updateDurationInterval() {
		if (!isNaN(expectedValue)) {
			if (costFunction->getShrinkingFactor() > 0
				    && costFunction->getShrinkingFactor() <= 1) {

				minimumValue = (long)((1 - costFunction->
					    getShrinkingFactor()) * expectedValue);
			}

			if (costFunction->getStretchingFactor() >= 0) {
				maximumValue = (long)((1 + costFunction->
					    getStretchingFactor()) * expectedValue);

			} else {
				maximumValue = infinity();
			}

		} else {
			minimumValue = NaN();
			maximumValue = NaN();
		}
	}

	double CostFunctionDuration::getCostValue(double value) {
		return value;
	}
}
}
}
}
}
}
}
