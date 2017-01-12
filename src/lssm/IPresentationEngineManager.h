/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef _IPRESENTATIONENGINEMANAGER_H_
#define _IPRESENTATIONENGINEMANAGER_H_

#include "mb/IMBDefs.h"

#include "system/ITimeBaseProvider.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::time;

#include "player/INCLPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include <set>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace lssm {
  class IPresentationEngineManager {
	public:
		virtual ~IPresentationEngineManager(){};

		virtual void setExitOnEnd(bool exitOnEnd)=0;
		virtual void setDisableFKeys(bool disableFKeys)=0;

		virtual set<string>* createPortIdList(string nclFile)=0;
		virtual short getMappedInterfaceType(string nclFile, string portId)=0;

		virtual void autoMountOC(bool autoMountIt)=0;
		virtual void setCurrentPrivateBaseId(unsigned int baseId)=0;
		virtual void setBackgroundImage(string uri)=0;
		virtual void getScreenShot()=0;
		virtual NclPlayerData* createNclPlayerData()=0;

		virtual void addPlayerListener(
				string nclFile, IPlayerListener* listener)=0;

		virtual void removePlayerListener(
				string nclFile, IPlayerListener* listener)=0;

		virtual bool openNclFile(string nclFile)=0;
		virtual bool startPresentation(string nclFile, string interfId)=0;
		virtual bool pausePresentation(string nclFile)=0;
		virtual bool resumePresentation(string nclFile)=0;
		virtual bool stopPresentation(string nclFile)=0;

		virtual bool setPropertyValue(
				string nclFile, string interfaceId, string value)=0;

		virtual string getPropertyValue(string nclFile, string interfaceId)=0;

		virtual bool stopAllPresentations()=0;
		virtual bool abortPresentation(string nclFile)=0;

		virtual void waitUnlockCondition()=0;
		virtual void setEmbedApp(bool isEmbedded)=0;
		virtual void setIsLocalNcl(bool isLocal, void* tuner)=0;
		virtual void setInteractivityInfo(bool hasInt)=0;
		virtual void* getDsmccListener()=0;
		virtual void setTimeBaseProvider(ITimeBaseProvider* tmp)=0;
		virtual bool editingCommand(string editingCommand)=0;
		virtual void setCmdFile(string cmdFile)=0;
		virtual void setDebugWindow(bool debugWindow)=0;
  };
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::lssm::IPresentationEngineManager*
		PEMCreator(
				int devClass,
				int xOffset, int yOffset, int w, int h,
				bool disableGfx,
                bool useMulticast,
				GingaScreenID screenId);

typedef void PEMDestroyer(
		::br::pucrio::telemidia::ginga::lssm::IPresentationEngineManager* pem);

#endif /*_IPRESENTATIONENGINEMANAGER_H_*/
