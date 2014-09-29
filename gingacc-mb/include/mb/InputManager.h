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

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "interface/InputEventFactory.h"

#include "IMBDefs.h"
#include "IInputManager.h"

#include <iostream>
#include <vector>
#include <set>
#include <map>
using namespace std;

typedef struct lockedLitenerAction {
	::br::pucrio::telemidia::ginga::core::mb::IInputEventListener* l;
	bool isAdd;
	set<int>* events;
} LockedAction;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class InputManager : public IInputManager, public Thread {
		protected:
			map<IInputEventListener*, set<int>*> eventListeners;
			vector<LockedAction*> actionsToInpListeners;
			set<IInputEventListener*> applicationListeners;
			vector<LockedAction*> actionsToAppListeners;

			set<IMotionEventListener*> motionListeners;

			ICmdEventListener* cmdListener;

			pthread_mutex_t mlMutex;

			bool running;
			bool notifying;
			bool notifyingApp;
			IEventBuffer* eventBuffer;
			double lastEventTime;
			double imperativeIntervalTime;
			double declarativeIntervalTime;
			InputEventFactory* ief;
			GingaScreenID myScreen;

			int currentXAxis;
			int currentYAxis;
			int currentZAxis;
			int maxX;
			int maxY;

			pthread_mutex_t actAppMutex;
			pthread_mutex_t actInpMutex;

			pthread_mutex_t appMutex;

		public:
			InputManager(GingaScreenID screenId);
			~InputManager();

		protected:
			void initializeInputIntervalTime();

			void release();

		public:
			void addMotionEventListener(IMotionEventListener* listener);
			void removeMotionEventListener(IMotionEventListener* listener);

		protected:
			virtual void notifyMotionListeners(int x, int y, int z);

		public:
			void addInputEventListener(
					IInputEventListener* listener, set<int>* events=NULL);

			void removeInputEventListener(IInputEventListener* listener);

		public:
			void setCommandEventListener(ICmdEventListener* listener);

		protected:
			void performInputLockedActions();
			void performApplicationLockedActions();
			virtual bool dispatchEvent(IInputEvent* keyEvent);
			virtual bool dispatchApplicationEvent(IInputEvent* keyEvent);

		public:
			void addApplicationInputEventListener(IInputEventListener* listener);
			void removeApplicationInputEventListener(
					IInputEventListener* listener);

			void postInputEvent(IInputEvent* event);
			void postInputEvent(int keyCode);
			void postCommand(string cmd, string args);

			void setAxisValues(int x, int y, int z);
			void setAxisBoundaries(int x, int y, int z);
			int getCurrentXAxisValue();
			int getCurrentYAxisValue();

			IEventBuffer* getEventBuffer();

		protected:
			virtual void run();
			void handleInputEvent (
				IInputEvent *inputEvent, int& pLastCode, int& lastCode, 
				double& pTimeStamp, double& timeStamp, int& mouseX, int& mouseY);
	};
}
}
}
}
}
}

#endif /*INPUTMANAGER_H_*/
