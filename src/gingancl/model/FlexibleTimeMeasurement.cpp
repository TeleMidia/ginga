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

#include "FlexibleTimeMeasurement.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace time {
	FlexibleTimeMeasurement::FlexibleTimeMeasurement(
		    double expectedValue, double minValue, double maxValue) :
		     TimeMeasurement(expectedValue) {

		computedValue = NaN();
		minFeasibleValue = NaN();
		maxFeasibleValue = NaN();
		setBoundaryValues(minValue, maxValue);
	}

    double FlexibleTimeMeasurement::getComputedValue() {
    	return computedValue;
    }

    double FlexibleTimeMeasurement::getOptimumValue() {
    	return getComputedValue();
    }

    void FlexibleTimeMeasurement::setComputedValue(double value) {
    	computedValue = value;
    }

    void FlexibleTimeMeasurement::setOptimumValue(double optValue) {
    	setComputedValue(optValue);
    }

    double FlexibleTimeMeasurement::getMaximumValue() {
    	return maximumValue;
    }

    double FlexibleTimeMeasurement::getMaximumFeasibleValue() {
    	return maxFeasibleValue;
    }

    double FlexibleTimeMeasurement::getMinimumFeasibleValue() {
    	return minFeasibleValue;
    }

    double FlexibleTimeMeasurement::getMinimumValue() {
    	return minimumValue;
    }

    bool FlexibleTimeMeasurement::isPredictable() {
    	if (TimeMeasurement::isPredictable() || computedValue >= 0)
			return true;
		else
			return false;
    }

    bool FlexibleTimeMeasurement::isComputed() {
    	if (!isNaN(computedValue))
			return true;
		else
			return false;
    }

    bool FlexibleTimeMeasurement::isUpperBounded() {
    	if (maximumValue >= 0)
			return true;
		else
			return false;
    }

	void FlexibleTimeMeasurement::setBoundaryValues(
			    double minValue,
			    double maxValue) {

		if (minValue > maxValue)
			minValue = maxValue;

		if (minValue < 0)
			minimumValue = 0;

		else if (!isNaN(expectedValue)
			    && minValue > TimeMeasurement::expectedValue)

			minimumValue = expectedValue;
		else
			minimumValue = minValue;

		if (maxValue < 0)
			maximumValue = infinity();

		else if ((!isNaN(
			    TimeMeasurement::expectedValue)) &&
				(maxValue < TimeMeasurement::expectedValue))

			maximumValue = expectedValue;
		else
			maximumValue = maxValue;
	}

	void FlexibleTimeMeasurement::overwrite(FlexibleTimeMeasurement* dur) {
		TimeMeasurement::overwrite(dur);

		minimumValue = dur->getMinimumValue();
		maximumValue = dur->getMaximumValue();
		minFeasibleValue = dur->getMinimumFeasibleValue();
		maxFeasibleValue = dur->getMaximumFeasibleValue();
		computedValue = dur->getComputedValue();
	}

	TimeMeasurement* FlexibleTimeMeasurement::duplicate() {
		FlexibleTimeMeasurement* newDuration;

		newDuration = new FlexibleTimeMeasurement(
			    TimeMeasurement::expectedValue,
			    minimumValue,
			    maximumValue);

		newDuration->overwrite(this);
		return newDuration;
	}

	string FlexibleTimeMeasurement::toString() {
		return (
			  "expected value: "
			  + itos(expectedValue)
			  + "; actual value: "
			  + (isNaN(actualValue) ? "UNDEFINED" :
			  "" + itos(actualValue))) + (
			  "; minimum value: "
			  + itos(minimumValue)
			  + "; maximum value : "
			  + (isInfinity(maximumValue) ?
			  "INFINITY" : ""
			  + itos(maximumValue))
			  + "; computed value: "
			  + (isNaN(computedValue) ? "UNDEFINED"
			  : "" + itos(computedValue)));
	}

	double FlexibleTimeMeasurement::getValue() {
		if (!isNaN(actualValue)) {
			return actualValue;

		} else if (!isNaN(computedValue)) {
			return computedValue;

		} else {
			return expectedValue;
		}
	}

	void FlexibleTimeMeasurement::setMinimumValue(double value) {
		minimumValue = value;
	}

	void FlexibleTimeMeasurement::setMaximumValue(double value) {
		maximumValue = value;
	}

	void FlexibleTimeMeasurement::setMinimumFeasibleValue(double value) {
		minFeasibleValue = value;
	}

	void FlexibleTimeMeasurement::setMaximumFeasibleValue(double value) {
		maxFeasibleValue = value;
	}
}
}
}
}
}
}
}
