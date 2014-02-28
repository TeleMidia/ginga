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

#include "gingancl/model/PresentationEvent.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace event {
  	//if the representation changes, update isUndefinedInstant method
	const double PresentationEvent::UNDEFINED_INSTANT= NaN();

	PresentationEvent::PresentationEvent(
		    string id,
		    void* executionObject,
		    ContentAnchor* anchor) :
		    	    AnchorEvent(
		    	    		id,
						    executionObject,
						    anchor) {

		typeSet.insert("PresentationEvent");

		numPresentations = 1;
		repetitionInterval = 0;

		if (anchor->instanceOf("IntervalAnchor")) {
			begin = ((IntervalAnchor*)anchor)->getBegin();
			end = ((IntervalAnchor*)anchor)->getEnd();
			duration = end - begin;

		} else {
			begin = PresentationEvent::UNDEFINED_INSTANT;
			end = PresentationEvent::UNDEFINED_INSTANT;
			duration = PresentationEvent::UNDEFINED_INSTANT;
		}
	}

	PresentationEvent::~PresentationEvent() {
		removeInstance(this);
	}

	bool PresentationEvent::stop() {
		if (currentState == EventUtil::ST_OCCURRING && numPresentations > 1) {
			numPresentations--;
		}

		return FormatterEvent::stop();
	}

	double PresentationEvent::getDuration() {
		return duration;
	}

	double PresentationEvent::getRepetitionInterval() {
		return repetitionInterval;
	}

	long PresentationEvent::getRepetitions() {
		return (numPresentations - 1);
	}

	void PresentationEvent::setDuration(double dur) {
		this->duration = dur;
	}

	void PresentationEvent::setEnd(double e) {
		bool isObjDur = IntervalAnchor::isObjectDuration(e);

		if (!isObjDur && isNaN(begin)) {
			begin = 0;
		}

		if (isObjDur || e >= begin) {
			end = e;
			if (isObjDur) {
				duration = end;

			} else {
				duration = end - begin;
			}
		}
	}

	void PresentationEvent::setRepetitionSettings(
		    long repetitions, double repetitionInterval) {

		if (repetitions >= 0) {
			this->numPresentations = repetitions + 1;

		} else {
			this->numPresentations = 1;
		}

		this->repetitionInterval = repetitionInterval;
	}

	double PresentationEvent::getBegin() {
		return begin;
	}

	double PresentationEvent::getEnd() {
		return end;
	}

	void PresentationEvent::incrementOccurrences() {
		occurrences++;
	}

	bool PresentationEvent::isUndefinedInstant(double value) {
		return isNaN(value);
	}
}
}
}
}
}
}
}
