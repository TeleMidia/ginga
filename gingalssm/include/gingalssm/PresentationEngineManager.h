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

#ifndef PRESENTATIONENGINEMANAGER_H_
#define PRESENTATIONENGINEMANAGER_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "mb/IInputManager.h"
#include "mb/ILocalScreenManager.h"
#include "mb/interface/IInputEventListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "player/IShowButton.h"
#include "player/IProgramAV.h"
#include "player/IPlayerListener.h"
#include "player/IApplicationPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "ncl/layout/LayoutRegion.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "ncl/components/ContentTypeManager.h"
#include "ncl/NclDocument.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "gingancl/privatebase/IPrivateBaseManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl;

#include "IPresentationEngineManager.h"

#include <pthread.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace lssm {
  class PresentationEngineManager :
					public IPresentationEngineManager,
		  			public IPlayerListener,
					public IInputEventListener, 
					public ICmdEventListener, 
					Thread {

	private:
		static const short UC_BACKGROUND  = 0;
		static const short UC_PRINTSCREEN = 1;
		static const short UC_STOP        = 2;
		static const short UC_PAUSE       = 3;
		static const short UC_RESUME      = 4;
		static const short UC_SHIFT       = 5;

		int devClass;
		int x;
		int y;
		int w;
		int h;
		bool enableGfx;

		GingaScreenID myScreen;
		IPrivateBaseManager* privateBaseManager;
		map<string, INCLPlayer*> formatters;
		set<INCLPlayer*> formattersToRelease;
		bool paused;
		string iconPath;
		bool isEmbedded;
		bool standAloneApp;
		bool isLocalNcl;
		bool disableAV;
		void* dsmccListener;
		void* tuner;
		bool closed;
		bool hasTMPNotification;
		bool hasInteractivity;

        bool enableMulticast;

		bool exitOnEnd;
		bool disableFKeys;

		ITimeBaseProvider* timeBaseProvider;
		int currentPrivateBaseId;
		static bool autoProcess;
		vector<string> commands;
		static ILocalScreenManager* dm;
		IInputManager* im;
		IShowButton* sb;
		bool debugWindow;

	public:
		PresentationEngineManager(
				int devClass,
				int xOffset,
				int yOffset,
				int width,
				int height,
				bool disableGfx,
                bool useMulticast,
				GingaScreenID screenId);

		virtual ~PresentationEngineManager();

		void setDebugWindow(bool debugWindow);
		void setExitOnEnd(bool exitOnEnd);
		void setDisableFKeys(bool disableFKeys);

		set<string>* createPortIdList(string nclFile);
		short getMappedInterfaceType(string nclFile, string portId);

		void disableMainAV(bool disableAV);
		void autoMountOC(bool autoMountIt);
		void setCurrentPrivateBaseId(unsigned int baseId);
		void setTimeBaseProvider(ITimeBaseProvider* tmp);

	private:
		void printGingaWindows();
		bool nclEdit(string nclEditApi);
		bool editingCommand(string commandTag, string commandPayload);

	public:
		bool editingCommand(string editingCmd);
		void setBackgroundImage(string uri);

	private:
		void close();
		void registerKeys();
		void setTimeBaseInfo(INCLPlayer* nclPlayer);

	public:
		void getScreenShot();
		bool getIsLocalNcl();
		void setEmbedApp(bool isEmbedded);
		void setIsLocalNcl(bool isLocal, void* tuner=NULL);
		void setInteractivityInfo(bool hasInt);

	private:
		INCLPlayer* createNclPlayer(string baseId, string fname);

	public:
		NclPlayerData* createNclPlayerData();

		void addPlayerListener(string nclFile, IPlayerListener* listener);
		void removePlayerListener(string nclFile, IPlayerListener* listener);

		bool openNclFile(string nclFile);
		bool startPresentation(string nclFile, string interfId);
		bool pausePresentation(string nclFile);
		bool resumePresentation(string nclFile);
		bool stopPresentation(string nclFile);
		bool setPropertyValue(string nclFile, string interfaceId, string value);
		string getPropertyValue(string nclFile, string interfaceId);
		bool stopAllPresentations();
		bool abortPresentation(string nclFile);

	private:
		//#if HAVE_GINGAJ
		void openNclDocument(string docUri, int x, int y, int w, int h);

		//#if HAVE_DSMCC
		void* getDsmccListener();
		void pausePressed();

	public:
		void setCmdFile(string cmdFile);
		static void* processAutoCmd(void* ptr);
		void waitUnlockCondition();

	private:
		void presentationCompleted(string formatterId);
		void releaseFormatter(string formatterId);
		bool checkStatus();

		void updateStatus(
				short code, string parameter, short type, string value);

		bool userEventReceived(IInputEvent* ev);
		bool cmdEventReceived(string command, string args);

		static void* eventReceived(void* ptr);
		void readCommand(string command);

		bool getNclPlayer(string docLocation, INCLPlayer** player);
		bool getNclPlayer(string baseId, string docId, INCLPlayer** p);

		void updateFormatters(short command, string parameter="");
		void run();
  };
}
}
}
}
}

#endif /*PRESENTATIONENGINEMANAGER_H_*/
