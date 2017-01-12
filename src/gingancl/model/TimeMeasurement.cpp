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

#include "TimeMeasurement.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace time {
	TimeMeasurement::TimeMeasurement(double value) {
		expectedValue = value;
		actualValue = NaN();
	}

	double TimeMeasurement::getExpectedValue() {
		return expectedValue;
	}

	double TimeMeasurement::getComputedValue() {
		return expectedValue;
	}

	double TimeMeasurement::getActualValue() {
		return actualValue;
	}

	void TimeMeasurement::setExpectedValue(double value) {
		expectedValue = value;
	}

	void TimeMeasurement::setActualValue(double value) {
		actualValue = value;
	}

	bool TimeMeasurement::isPredictable() {
		if (expectedValue >= 0 || actualValue >= 0)
			return true;
		else
			return false;
	}

	bool TimeMeasurement::isComputed() {
		return true;
	}

	string TimeMeasurement::toString() {
		return (
		  "expected value: "
		  + itos(expectedValue)
		  + "; actual value: "
		  + (isNaN(actualValue) ?
		  "UNDEFINED" : ""
		  + itos(actualValue)));
	}

	void TimeMeasurement::overwrite(TimeMeasurement* time) {
		expectedValue = time->getExpectedValue();
		actualValue = time->getActualValue();
	}

	TimeMeasurement* TimeMeasurement::duplicate() {
		TimeMeasurement* newMeas;

		newMeas = new TimeMeasurement(expectedValue);
		newMeas->overwrite(this);
		return newMeas;
	}

	double TimeMeasurement::getValue() {
		if (!isNaN(actualValue)) {
			return actualValue;
		} else {
			return expectedValue;
		}
	}

	void TimeMeasurement::fromString(string str) {
		//TODO
	}
}
}
}
}
}
}
}
