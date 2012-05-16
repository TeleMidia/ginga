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

#ifndef IPLAYER_H_
#define IPLAYER_H_

#include <stdint.h>

#include "mb/interface/IWindow.h"
#include "mb/interface/ISurface.h"
#include "mb/interface/IImageProvider.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "IPlayerListener.h"

#include <map>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	class IPlayer {
		public:
			static const short PL_NOTIFY_START         =  0;
			static const short PL_NOTIFY_STOP          =  1;
			static const short PL_NOTIFY_PAUSE         =  2;
			static const short PL_NOTIFY_RESUME        =  3;
			static const short PL_NOTIFY_ABORT         =  4;
			static const short PL_NOTIFY_NCLEDIT       =  5;
			static const short PL_NOTIFY_UPDATECONTENT =  6;
			static const short PL_NOTIFY_OUTTRANS      =  7;

			static const short TYPE_PRESENTATION       = 10;
			static const short TYPE_ATTRIBUTION        = 11;
			static const short TYPE_SIGNAL             = 12;
			static const short TYPE_PASSIVEDEVICE      = 13;

			virtual ~IPlayer(){};

			virtual void flip()=0;
			virtual ISurface* getSurface()=0;
			virtual void setMrl(string mrl, bool visible=true)=0;
			virtual void reset()=0;
			virtual void rebase()=0;
			//virtual void setStandByState(bool standBy)=0;
			virtual void setNotifyContentUpdate(bool notify)=0;
			virtual void addListener(IPlayerListener* listener)=0;
			virtual void removeListener(IPlayerListener* listener)=0;
			virtual void notifyPlayerListeners(
					short code, string paremeter, short type, string value)=0;

			virtual int64_t getVPts()=0;
			virtual double getMediaTime()=0;
			virtual double getTotalMediaTime()=0;
			virtual void setMediaTime(double newTime)=0;
			virtual bool setKeyHandler(bool isHandler)=0;
			virtual void setScope(
					string scope,
					short type,
					double begin=-1, double end=-1, double outTransDur=-1)=0;

			virtual void play()=0;
			virtual void stop()=0;
			virtual void abort()=0;
			virtual void pause()=0;
			virtual void resume()=0;

			virtual string getPropertyValue(string name)=0;
			virtual void setPropertyValue(string name, string value)=0;

			virtual void setReferenceTimePlayer(IPlayer* player)=0;
			virtual void addTimeReferPlayer(IPlayer* referPlayer)=0;
			virtual void removeTimeReferPlayer(IPlayer* referPlayer)=0;
			virtual void notifyReferPlayers(int transition)=0;
			virtual void timebaseObjectTransitionCallback(int transition)=0;
			virtual void setTimeBasePlayer(IPlayer* timeBasePlayer)=0;
			virtual bool hasPresented()=0;
			virtual void setPresented(bool presented)=0;
			virtual bool isVisible()=0;
			virtual void setVisible(bool visible)=0;
			virtual bool immediatelyStart()=0;
			virtual void setImmediatelyStart(bool immediatelyStartVal)=0;
			virtual void forceNaturalEnd(bool forceIt)=0;
			virtual bool isForcedNaturalEnd()=0;
			virtual bool setOutWindow(GingaWindowID windowId)=0;

			/*Exclusive for ChannelPlayer*/
			virtual IPlayer* getSelectedPlayer()=0;
			virtual void setPlayerMap(map<string, IPlayer*>* objs)=0;
			virtual map<string, IPlayer*>* getPlayerMap()=0;
			virtual IPlayer* getPlayer(string objectId)=0;
			virtual void select(IPlayer* selObject)=0;

			/*Exclusive for Application Players*/
			virtual void setCurrentScope(string scopeId)=0;

			virtual void timeShift(string direction)=0;
	};
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::player::IPlayer* PlayerCreator(
		GingaScreenID screenId, const char* mrl, bool hasVisual);

typedef void PlayerDestroyer(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* player);

#endif /*IPLAYER_H_*/
