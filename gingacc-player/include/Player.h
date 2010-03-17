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

#ifndef PLAYER_H_
#define PLAYER_H_

#include "system/io/IGingaLocatorFactory.h"
#include "system/io/IInputManager.h"
#include "system/io/interface/input/IInputEventListener.h"
#include "system/io/interface/input/CodeMap.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::io;

#include "IPlayer.h"

#include <set>
#include <vector>
#include <map>
#include <string>
using namespace std;

#include "../config.h"

typedef struct lockedPlayerLitenerAction {
	::br::pucrio::telemidia::ginga::core::player::IPlayerListener* l;
	bool isAdd;
} LockedPlayerListener;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	class Player : public IPlayer {
		private:
			pthread_mutex_t listM;
			pthread_mutex_t lockedListM;
			pthread_mutex_t referM;
			map<string, string>* properties;

			bool notifying;
			set<IPlayerListener*>* listeners;
			vector<LockedPlayerListener*>* lockedListeners;

		protected:
			short status;
			static const short NONE = 0;
			static const short PLAY = 1;
			static const short PAUSE = 2;
			static const short STOP = 3;

			string mrl;
			ISurface* surface;
			IWindow* outputWindow;
			double initTime, elapsedTime, elapsedPause, pauseTime;
			set<IPlayer*>* referredPlayers;
			IPlayer* timeBasePlayer;
			bool presented;
			bool visible;
			bool immediatelyStartVar;
			bool forcedNaturalEnd;
			bool notifyContentUpdate;
			string scope;
			double scopeInitTime;
			double scopeEndTime;

		public:
			Player(string mrl);
			virtual ~Player();

			virtual void setMrl(string mrl, bool visible=true);
			//virtual void setStandByState(bool standBy){};
			virtual void setNotifyContentUpdate(bool notify);
			virtual void addListener(IPlayerListener* listener);
			void removeListener(IPlayerListener* listener);

		private:
			void performLockedListenersRequest();

		public:
			void notifyListeners(
					short code, string paremeter="",
					short type=TYPE_PRESENTATION);

			virtual void setSurface(ISurface* surface);
			virtual ISurface* getSurface();

			virtual void setMediaTime(double newTime);
			virtual int64_t getVPts(){return 0;};

#if HAVE_CLOCKTIME
			double getMediaTime();
#else
			virtual double getMediaTime();
#endif

			virtual bool setKeyHandler(bool isHandler);
			virtual void setScope(
					string scope,
					short type=TYPE_PRESENTATION,
					double begin=-1, double end=-1);

			virtual void play();
			virtual void stop();
			virtual void abort();
			virtual void pause();
			virtual void resume();
			virtual string getPropertyValue(string name);
			virtual void setPropertyValue(
					string name,
					string value, double duration=-1, double by=-1);

			virtual void setReferenceTimePlayer(IPlayer* player){};

			void addTimeReferPlayer(IPlayer* referPlayer);
			void removeTimeReferPlayer(IPlayer* referPlayer);
			void notifyReferPlayers(int transition);
			void timebaseObjectTransitionCallback(int transition);
			void setTimeBasePlayer(IPlayer* timeBasePlayer);
			virtual bool hasPresented();
			void setPresented(bool presented);
			bool isVisible();
			void setVisible(bool visible);
			bool immediatelyStart();
			void setImmediatelyStart(bool immediatelyStartVal);
			void forceNaturalEnd();
			bool isForcedNaturalEnd();
			virtual bool setOutWindow(int windowId);

			/*Exclusive for ChannelPlayer*/
			virtual IPlayer* getSelectedPlayer(){return NULL;};
			virtual void setPlayerMap(map<string, IPlayer*>* objs){};
			virtual map<string, IPlayer*>* getPlayerMap(){return NULL;};
			virtual IPlayer* getPlayer(string objectId){return NULL;};
			virtual void select(IPlayer* selObject){};

			/*Exclusive for Application Players*/
			virtual void setCurrentScope(string scopeId){};

			virtual void timeShift(string direction){};
	};
}
}
}
}
}
}

using namespace ::br::pucrio::telemidia::ginga::core::player;

struct notify {
	IPlayerListener* listener;
	short code;
	string param;
	short type;
};

#endif /*PLAYER_H_*/
