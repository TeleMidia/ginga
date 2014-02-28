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

#ifndef NOMINALEVENTMONITOR_H_
#define NOMINALEVENTMONITOR_H_

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "system/time/ITimeBaseListener.h"
#include "system/time/ITimeBaseProvider.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::time;

#include "player/IPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "ncl/interfaces/ContentAnchor.h"
#include "ncl/interfaces/LambdaAnchor.h"
#include "ncl/interfaces/IntervalAnchor.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "gingancl//model/PresentationEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "gingancl//model/EventTransition.h"
#include "gingancl//model/TransitionDispatcher.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event::transition;

#include "gingancl//model/ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "IPlayerAdapter.h"

#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
   class NominalEventMonitor : public Thread, public ITimeBaseListener {
	private:
		double sleepTime;
		double expectedSleepTime;
		ExecutionObject* executionObject;
		IPlayerAdapter* adapter;
		int timeBaseId;
		ITimeBaseProvider* timeBaseProvider;

		static const double DEFAULT_SLEEP_TIME; 
		static const double DEFAULT_ERROR;

		bool running;
		bool paused;
		bool stopped;

	public:
		NominalEventMonitor(ExecutionObject* obj, IPlayerAdapter* player);
		virtual ~NominalEventMonitor();

	private:
		void unregisterFromTimeBase();

   public:
		void setTimeBaseProvider(ITimeBaseProvider* timeBaseProvider);

	private:
		void prepareNptTransitionsEvents();

	public:
		void updateTimeBaseId(
				unsigned char oldContentId,
				unsigned char newContentId);

		void valueReached(unsigned char timeBaseId, double timeValue);

		void timeBaseNaturalEnd(unsigned char contentId, double timeValue);

		void loopDetected();

		void startMonitor();
		void pauseMonitor();
		void resumeMonitor();
		void stopMonitor();

		virtual void run();
   };
}
}
}
}
}
}

#endif /*NOMINALEVENTMONITOR_H_*/
