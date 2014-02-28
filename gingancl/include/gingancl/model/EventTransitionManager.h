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

#ifndef EventTransitionManager_H_
#define EventTransitionManager_H_

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "ncl/interfaces/RelativeTimeIntervalAnchor.h"
#include "ncl/interfaces/SampleIntervalAnchor.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "ncl/connectors/EventUtil.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "player/IPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "PresentationEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "BeginEventTransition.h"
#include "EndEventTransition.h"

#include <map>
#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace event {
namespace transition {
  class EventTransitionManager {
	private:
		map<short int, int> currentTransitionIndex;
		map<short int, int> startTransitionIndex;
		map<short int, vector<EventTransition*>*> transTable;
		pthread_mutex_t transMutex;

	public:
		EventTransitionManager();
		virtual ~EventTransitionManager();

	private:
		short int getType(PresentationEvent* event);
		vector<EventTransition*>* getTransitionEvents(short int type);

	public:
		void addPresentationEvent(PresentationEvent* event);

	private:
		void addEventTransition(EventTransition* transition, short int type);

	public:
		void removeEventTransition(PresentationEvent* event);

		void resetTimeIndex();
		void resetTimeIndexByType(short int type);
		void prepare(bool wholeContent, double startTime, short int type);
		void start(double offsetTime);
		void stop(double endTime, bool applicationType=false);
		void abort(double endTime, bool applicationType=false);

		void timeBaseNaturalEnd(
				int64_t timeValue,
				FormatterEvent* mainEvent,
				short int transType);

		void updateTransitionTable(
				double timeValue,
				IPlayer* player,
				FormatterEvent* mainEvent,
				short int transType);

		set<double>* getTransitionsValues(short int transType);
		EventTransition* getNextTransition(FormatterEvent* mainEvent);
  };
}
}
}
}
}
}
}
}

#endif /*EventTransitionManager_H_*/
