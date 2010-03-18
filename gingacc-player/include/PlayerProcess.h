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

#ifndef __PlayerProcess_h_
#define __PlayerProcess_h_

#include "Player.h"

#include "system/process/Process.h"
#include "system/process/IProcessListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::process;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	class PlayerProcess :
			public Player, public Process, public IProcessListener {

		private:
			map<string, string>* msgs;
			pthread_mutex_t msgMutex;

			bool isWaitingAns;
			pthread_mutex_t ansMutex;
			pthread_cond_t ansCond;
			int windowId;

		public:
			PlayerProcess(const char* objectName);
			virtual ~PlayerProcess();

		private:
			void init(const char* objName);

		public:
			void receiveProcessSignal(int sigType, int pSig, int ppid);
			void setMrl(string mrl, bool visible=true);
			void reset();
			void rebase();

		private:
			string getAnswer(string token, int howPatient);
			void waitAnswer(int milliseconds);

		public:
			void messageReceived(string msg);

			void setNotifyContentUpdate(bool notify);
			void addListener(IPlayerListener* listener);
			void removeListener(IPlayerListener* listener);
			void notifyListeners(
					short code, string paremeter, short type);

			int64_t getVPts();
			double getMediaTime();
			void setMediaTime(double newTime);
			bool setKeyHandler(bool isHandler);
			void setScope(
					string scope,
					short type, double begin=-1, double end=-1);

			void play();
			void stop();
			void abort();
			void pause();
			void resume();

			string getPropertyValue(string name);
			void setPropertyValue(
					string name,
					string value, double duration=-1, double by=-1);

			void setReferenceTimePlayer(IPlayer* player);
			void addTimeReferPlayer(IPlayer* referPlayer);
			void removeTimeReferPlayer(IPlayer* referPlayer);
			void notifyReferPlayers(int transition);
			void timebaseObjectTransitionCallback(int transition);
			void setTimeBasePlayer(IPlayer* timeBasePlayer);
			bool hasPresented();
			void setPresented(bool presented);
			bool isVisible();
			void setVisible(bool visible);
			bool immediatelyStart();
			void setImmediatelyStart(bool immediatelyStartVal);
			void forceNaturalEnd();
			bool isForcedNaturalEnd();
			bool setOutWindow(int windowId);

			/*Exclusive for ChannelPlayer*/
			IPlayer* getSelectedPlayer();
			void setPlayerMap(map<string, IPlayer*>* objs);
			map<string, IPlayer*>* getPlayerMap();
			IPlayer* getPlayer(string objectId);
			void select(IPlayer* selObject);

			/*Exclusive for Application Players*/
			void setCurrentScope(string scopeId);

			void timeShift(string direction);
	};
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::player::IPlayer* PlayerCreator(
		const char* mrl, bool hasVisual);

typedef void PlayerDestroyer(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* player);

typedef io::ISurface* SurfaceHelper(io::IImageProvider* provider, string mrl);
typedef io::ISurface* ImageRenderer(string mrl);
typedef int TextRenderer(
		io::ISurface* s,
		string text, string fontUri, int fontSize,
		::br::pucrio::telemidia::util::IColor* fontColor);

#endif /*__PlayerProcess_h_*/
