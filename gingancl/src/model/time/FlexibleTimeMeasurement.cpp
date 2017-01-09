/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#include "gingancl/model/FlexibleTimeMeasurement.h"

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
