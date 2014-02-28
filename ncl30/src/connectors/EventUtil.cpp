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

#include "ncl/connectors/EventUtil.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {
	short EventUtil::getTypeCode(string typeName) {
		if (typeName == "presentation") {
			return EventUtil::EVT_PRESENTATION;

		} else if (typeName == "selection") {
			return EventUtil::EVT_SELECTION;

		} else if (typeName == "attribution") {
			return EventUtil::EVT_ATTRIBUTION;

		} else if(typeName == "composition") {
			return EventUtil::EVT_COMPOSITION;
		}

		return EventUtil::EVT_PRESENTATION;
	}

	string EventUtil::getTypeName(short type) {
		switch (type) {
			case EventUtil::EVT_PRESENTATION:
				return "presentation";

			case EventUtil::EVT_SELECTION:
				return "selection";

			case EventUtil::EVT_ATTRIBUTION:
				return "attribution";

			case EventUtil::EVT_COMPOSITION:
				return "composition";

			default:
				return "";
		}
	}

	short EventUtil::getStateCode(string stateName) {
		if (stateName == "occurring") {
			return EventUtil::ST_OCCURRING;

		} else if (stateName == "paused") {
			return EventUtil::ST_PAUSED;
		}

		// "sleeping"
		return EventUtil::ST_SLEEPING;
	}

	string EventUtil::getStateName(short state) {
		switch (state) {
			case EventUtil::ST_OCCURRING:
				return "occurring";

			case EventUtil::ST_PAUSED:
				return "paused";

			case EventUtil::ST_SLEEPING:
				return "sleeping";

			default:
				return "";
		}
	}

	string EventUtil::getTransitionName(short transition) {
		switch (transition) {
			case EventUtil::TR_STARTS:
				return "starts";

			case EventUtil::TR_STOPS:
				return "stops";

			case EventUtil::TR_PAUSES:
				return "pauses";

			case EventUtil::TR_RESUMES:
				return "resumes";

			case EventUtil::TR_ABORTS:
				return "aborts";

			default:
				return "";
		}
	}

	short EventUtil::getTransitionCode(string transition) {
		if (transition == "starts") {
			return EventUtil::TR_STARTS;

		} else if (transition == "stops") {
			return EventUtil::TR_STOPS;

		} else if (transition == "pauses") {
			return EventUtil::TR_PAUSES;

		} else if (transition == "resumes") {
			return EventUtil::TR_RESUMES;

		} else if (transition == "aborts") {
			return EventUtil::TR_ABORTS;
		}

		return -1;
	}

	short EventUtil::getAttributeTypeCode(string attTypeName) {
		if (attTypeName == "occurrences") {
			return EventUtil::ATT_OCCURRENCES;

		} else if (attTypeName == "nodeProperty") {
			return EventUtil::ATT_NODE_PROPERTY;

		} else if (attTypeName == "repetitions") {
			return EventUtil::ATT_REPETITIONS;

		} else if (attTypeName == "state") {
				return EventUtil::ATT_STATE;
		}

		return -1;
	}

	string EventUtil::getAttributeTypeName(short type) {
		switch(type) {
			case EventUtil::ATT_OCCURRENCES:
				return "occurrences";

			case EventUtil::ATT_NODE_PROPERTY:
				return "nodeProperty";

			case EventUtil::ATT_REPETITIONS:
				return "repetitions";

			case EventUtil::ATT_STATE:
				return "state";

			default:
				return "nodeProperty";
		}
	}
}
}
}
}
}
