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

#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_

#include "../thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "IInputManager.h"

#include <iostream>
#include <vector>
#include <set>
#include <map>
using namespace std;

typedef struct lockedLitenerAction {
	::br::pucrio::telemidia::ginga::core::system::io::IInputEventListener* l;
	bool isAdd;
	set<int>* events;
} LockedAction;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	class InputManager : public IInputManager, public Thread {
		private:
			static InputManager* _instance;
			map<IInputEventListener*, set<int>*>* eventListeners;
			vector<LockedAction*>* actionsToInpListeners;
			set<IInputEventListener*>* applicationListeners;
			vector<LockedAction*>* actionsToAppListeners;
			bool running;
			bool notifying;
			bool notifyingApp;
			IEventBuffer* eventBuffer;
			double lastEventTime;
			double imperativeIntervalTime;
			double declarativeIntervalTime;

			int currentXAxis;
			int currentYAxis;
			int maxX;
			int maxY;

			pthread_mutex_t actAppMutex;
			pthread_mutex_t actInpMutex;

			pthread_mutex_t appMutex;

			InputManager();
			virtual ~InputManager();

			void initializeInputIntervalTime();

		public:
			void release();
			static InputManager* getInstance();
			void addInputEventListener(
					IInputEventListener* listener, set<int>* events=NULL);

			void removeInputEventListener(IInputEventListener* listener);

		private:
			void performInputLockedActions();
			void performApplicationLockedActions();
			bool dispatchEvent(IInputEvent* keyEvent);
			bool dispatchApplicationEvent(IInputEvent* keyEvent);

		public:
			void addApplicationInputEventListener(IInputEventListener* listener);
			void removeApplicationInputEventListener(
					IInputEventListener* listener);

			void postEvent(IInputEvent* event);
			void postEvent(int keyCode);

			void setAxisValues(int x, int y, int z);
			void setAxisBoundaries(int x, int y, int z);
			int getCurrentXAxisValue();
			int getCurrentYAxisValue();

		private:
			void run();

#ifdef _WIN32
			pthread_mutex_t mutex_event_buffer;
#endif
	};
}
}
}
}
}
}
}

#endif /*INPUTMANAGER_H_*/
