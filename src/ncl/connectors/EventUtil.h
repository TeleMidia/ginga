/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

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

#ifndef _EVENTUTIL_H_
#define _EVENTUTIL_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {
  class EventUtil {
	public:
		// tipos de evento
		static const short EVT_SELECTION    = 0;
		static const short EVT_PRESENTATION = 1;
		static const short EVT_ATTRIBUTION  = 2;
		static const short EVT_COMPOSITION  = 4;

		// Transicoes de estados de um evento
		static const short TR_STARTS  = 0;
		static const short TR_STOPS   = 1;
		static const short TR_PAUSES  = 2;
		static const short TR_RESUMES = 3;
		static const short TR_ABORTS  = 4;

		// estados
		static const short ST_SLEEPING  = 0;
		static const short ST_OCCURRING = 1;
		static const short ST_PAUSED    = 2;

		//Tipos de Atributos
		static const short ATT_OCCURRENCES   = 0;
		static const short ATT_REPETITIONS  = 1;
		static const short ATT_STATE         = 2;
		static const short ATT_NODE_PROPERTY = 3;

		//funcoes adicionais
		static short getTypeCode(string typeName); 
		static string getTypeName(short type);
		static short getStateCode(string stateName);
		static string getStateName(short state);
		static string getTransitionName(short transition);
		static short getTransitionCode(string transition);
		static short getAttributeTypeCode(string attTypeName);
		static string getAttributeTypeName(short type);
  };
}
}
}
}
}

#endif //_EVENTUTIL_H_
