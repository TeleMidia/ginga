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
