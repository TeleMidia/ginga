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

#ifndef IEPGLISTENER_H_
#define IEPGLISTENER_H_

#include "tsparser/IEventInfo.h"
using namespace br::pucrio::telemidia::ginga::core::tsparser::si;

#include <string>
#include <map>
using namespace std;

/*
 * A Lua Node can register himself to handle EPG events (class='si', type='epg').
 * Doing this, his associated Lua Player becames an IEPGListener. The standard
 * defines 3 main types that a Lua Node can request EPG events.
 *
 * 1) stage='current', fields={field_1, field_2,...field_j}
 * 2) stage='next', eventId=<number>, fields={field_1, field_2,...field_j}
 * 3) stage='schedule', startTime=<date>, endTime=<date>, fields={field_1,
 * 		field_2,...field_j}
 *
 * The struct Request was create to model a lua node request for EPG events.
 * That request has to be stored associated with her N IEPGListeners.
 */

struct Request{
	string stage;
	/* 3 possible values: current, next or schedule*/

	unsigned short eventId;
	/* only if stage==next, requesting the next event of the event with this
	 *  eventId. If is not specified, the request is for the next event of the
	 *  current event*/

	string startTime;
	string endTime;
	/* only if stage==schedule, requesting events with startTime and endTime in
	* the range specified by this startTime and this endTime*/

	string fields;
	/*requesting specified metadata fields for each event. If is not specified
	 * the request is for all possible metadada fields*/
};

struct Field{
	string str;
	map<string, struct Field> table;
};

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace epg {
	class IEPGListener {
		public:
			virtual ~IEPGListener(){};
			virtual void pushEPGEvent(map<string, struct Field> event)=0;
			virtual void addAsEPGListener()=0;

	};
}
}
}
}
}
}
}

#endif /*IEPGLISTENER_H_*/
