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

#ifndef _FORMATTEREVENT_H_
#define _FORMATTEREVENT_H_

#include <pthread.h>

#include "ncl/connectors/EventUtil.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "IFormatterEvent.h"

#include <string>
#include <iostream>
#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace event {
  class FormatterEvent : public IFormatterEvent {
	private:
		static const short ST_ABORTED = 50;

	protected:
		string id;
		short currentState;
		short previousState;
		long occurrences;
		void* executionObject;
		set<IEventListener*> coreListeners;
		set<IEventListener*> linksListeners;
		set<IEventListener*> objectsListeners;
		set<string> typeSet;
		bool deleting;
		short eventType;
		pthread_mutex_t mutex;

		static set<FormatterEvent*> instances;
		static bool init;
		static pthread_mutex_t iMutex;

	public:
		FormatterEvent(string id, void* executionObject);
		virtual ~FormatterEvent();

	private:
		virtual void destroyListeners();

	public:
		static bool hasInstance(FormatterEvent* event, bool remove);

	private:
		static void addInstance(FormatterEvent* event);

	protected:
		static bool removeInstance(FormatterEvent* event);

	public:
		bool instanceOf(string s);

		static bool hasNcmId(FormatterEvent* event, string anchorId);

		void setEventType(short eventType);
		virtual short getEventType();
		void setId(string id);
		void addEventListener(IEventListener* listener);
		bool containsEventListener(IEventListener* listener);
		void removeEventListener(IEventListener* listener);

	protected:
		short getNewState(short transition);
		short getTransition(short newState);

	public:
		bool abort();
		virtual bool start();
		virtual bool stop();
		bool pause();
		bool resume();
		void setCurrentState(short newState);

	protected:
		bool changeState(short newState, short transition);

	public:
		short getCurrentState();
		short getPreviousState();
		static short getTransistion(short previousState, short newState);

		void* getExecutionObject();
		void setExecutionObject(void* object);
		string getId();
		long getOccurrences();
		static string getStateName(short state);
  };
}
}
}
}
}
}
}

#endif //_FORMATTEREVENT_H_
