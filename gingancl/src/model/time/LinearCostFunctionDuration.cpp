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

#include "gingancl/model/LinearCostFunctionDuration.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace time {
	LinearCostFunctionDuration::LinearCostFunctionDuration(
				double expectedVal,
				double minVal,
				double maxVal,
				LinearTimeCostFunction* function) :
				   CostFunctionDuration(expectedVal,
				                         minVal,
				                         maxVal,
				                         function) {

	}

	LinearCostFunctionDuration::LinearCostFunctionDuration(
				double expectedVal,
				LinearTimeCostFunction* function) :
				   CostFunctionDuration(expectedVal, function) {

	}

	double LinearCostFunctionDuration::getShrinkingCostRate() {
		return ((LinearTimeCostFunction*)CostFunctionDuration::
			    costFunction)->getMaxShrinkingCost() /
			    (expectedValue - minimumValue);
	}

	double LinearCostFunctionDuration::getStretchingCostRate() {
		if (maximumValue == infinity())
			return ((LinearTimeCostFunction*)CostFunctionDuration::
				    costFunction)->getMaxStretchingCost();
		else
			return ((LinearTimeCostFunction*)CostFunctionDuration::
				    costFunction)->getMaxStretchingCost() /
				    (maximumValue - expectedValue);
	}

	double LinearCostFunctionDuration::getCostValue(double value) {
		if (expectedValue < 0)
			return 0;

		if (value >= expectedValue)
			return (value - (expectedValue)) * getStretchingCostRate();
		else
			return ((expectedValue) - value) * getShrinkingCostRate();
	}

	TimeMeasurement* LinearCostFunctionDuration::duplicate() {
		LinearCostFunctionDuration* newDuration;

		newDuration =
		  new LinearCostFunctionDuration(
		    expectedValue,
		    minimumValue,
		    maximumValue,
		    (LinearTimeCostFunction*)costFunction);
		newDuration->overwrite(this);
		return newDuration;
	}

	string LinearCostFunctionDuration::toString() {
		return (
			(
			"expected value: "
			+ itos(expectedValue)
			+ "; actual value: "
			+ (!isNaN(actualValue) ? "UNDEFINED" :
			"" + itos(actualValue))) +
			("; minimum value: "
			+ itos(minimumValue)
			+ "; maximum value : "
			+ (isInfinity(maximumValue) ?
			"INFINITY" : "" + itos(maximumValue))
			+ "; computed value: "
			+ (!isNaN(computedValue) ? "UNDEFINED"
			: "" + itos(computedValue)))
			+ "; shrinking cost rate: "
			+ itos(getShrinkingCostRate())
			+ "; stretching cost rate: "
			+ itos(getStretchingCostRate()));
	}
}
}
}
}
}
}
}
