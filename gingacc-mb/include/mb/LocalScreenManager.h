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

#ifndef LocalScreenManager_H_
#define LocalScreenManager_H_

#include "interface/IDeviceScreen.h"
#include "ILocalScreenManager.h"
#include "IInputManager.h"

#include <pthread.h>

#include "system/thread/Thread.h"
using namespace br::pucrio::telemidia::ginga::core::system::thread;

#include <string>
#include <iostream>
#include <map>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class LocalScreenManager : public ILocalScreenManager {
		public:
			/* Ginga defining its Multimedia Backend System Types (GMBST)    */
										     /* System Description  String   */
			static const short GMBST_DFLT;   /* Default system:      dflt    */
			static const short GMBST_DFB;    /* DirectFB:            dfb     */
			static const short GMBST_DX;     /* DirectX:             dx      */
			static const short GMBST_SDL;    /* SDL:                 sdl     */
			static const short GMBST_TERM;   /* Teminal:             term    */

			/* Ginga defining its Multimedia Backend SubSystem Types (GMBSST)*/
											 /* System Description  String   */
			static const short GMBSST_DFLT;  /* Default subsystem:  dflt     */
			static const short GMBSST_FBDEV; /* Frame buffer:       fbdev    */
			static const short GMBSST_X11;   /* X11:                x11      */
			static const short GMBSST_HWND;  /* MS-W Window Handle: hwnd     */
			static const short GMBSST_SDL;   /* SDL:                sdl      */
			static const short GMBSST_COCOA; /* COCOA:              cocoa    */

		private:
			map<GingaScreenID, IDeviceScreen*> screens;
			pthread_mutex_t mapMutex;
			pthread_mutex_t genMutex;

			static set<IInputEventListener*> iListeners;
			static pthread_mutex_t ilMutex;

			static set<IMotionEventListener*> mListeners;
			static pthread_mutex_t mlMutex;

			static bool initMutex;

			vector<short> sortSys;
			map<string, short> sysNames;
			pthread_mutex_t sysMutex;

			bool running;

			bool isWaiting;
			pthread_cond_t wsSignal;
			pthread_mutex_t wsMutex;
			GingaScreenID waitingRefreshScreen;

			static LocalScreenManager* _instance;

			LocalScreenManager();
			virtual ~LocalScreenManager();

			static void checkInitMutex();

		public:
			void releaseHandler();

			static void addIEListenerInstance(
					IInputEventListener* listener);

			static void removeIEListenerInstance(
					IInputEventListener* listener);

			static bool hasIEListenerInstance(
					IInputEventListener* listener, bool removeInstance=false);

			static void addMEListenerInstance(
					IMotionEventListener* listener);

			static void removeMEListenerInstance(
					IMotionEventListener* listener);

			static bool hasMEListenerInstance(
					IMotionEventListener* listener, bool removeInstance=false);

			void setBackgroundImage(GingaScreenID screenId, string uri);

			static LocalScreenManager* getInstance();

			int getDeviceWidth(GingaScreenID screenId);

			int getDeviceHeight(GingaScreenID screenId);

			void* getGfxRoot(GingaScreenID screenId);

			void releaseScreen(GingaScreenID screenId);
			void releaseMB(GingaScreenID screenId);
			void clearWidgetPools(GingaScreenID screenId);

			GingaScreenID createScreen(int argc, char** args);

		private:
			GingaScreenID createScreen(
					string vSystem,
					string vSubSystem,
					string vMode,
					string vParent,
					string vEmbed,
					string aSystem,
					bool externalRenderer,
					bool useStdin);

		public:
			string getScreenName(GingaScreenID screenId);
			GingaWindowID getScreenUnderlyingWindow(GingaScreenID screenId);

		private:
			short getMBSystemType(string mbSystemName);

			bool isAvailable(short mbSysType);
			void lockSysNames();
			void unlockSysNames();

		public:
			IWindow* getIWindowFromId(
					GingaScreenID screenId, GingaWindowID winId);

			bool mergeIds(
					GingaScreenID screenId,
					GingaWindowID destId,
					vector<GingaWindowID>* srcIds);

			void blitScreen(GingaScreenID screenId, ISurface* destination);
			void blitScreen(GingaScreenID screenId, string fileUri);
			void refreshScreen(GingaScreenID screenId);


			/* Interfacing output */
			IWindow* createWindow(
					GingaScreenID screenId,
					int x, int y,
					int w, int h,
					float z);

			GingaWindowID createUnderlyingSubWindow(
					GingaScreenID screenId,
					int x, int y,
					int w, int h,
					float z);

			IWindow* createWindowFrom(
					GingaScreenID screenId, GingaWindowID underlyingWindow);

			bool hasWindow(GingaScreenID screenId, IWindow* window);
			void releaseWindow(GingaScreenID screenId, IWindow* window);

			ISurface* createSurface(GingaScreenID screenId);

			ISurface* createSurface(GingaScreenID screenId, int w, int h);

			ISurface* createSurfaceFrom(
					GingaScreenID screenId, void* underlyingSurface);

			bool hasSurface(GingaScreenID screenId, ISurface* surface);
			bool releaseSurface(GingaScreenID screenId, ISurface* surface);


			/* Interfacing content */
			IContinuousMediaProvider* createContinuousMediaProvider(
					GingaScreenID screenId,
					const char* mrl,
					bool* hasVisual,
					bool isRemote);

			void releaseContinuousMediaProvider(
					GingaScreenID screenId,
					IContinuousMediaProvider* provider);

			IFontProvider* createFontProvider(
					GingaScreenID screenId,
					const char* mrl,
					int fontSize);

			void releaseFontProvider(
					GingaScreenID screenId, IFontProvider* provider);

			IImageProvider* createImageProvider(
					GingaScreenID screenId, const char* mrl);

			void releaseImageProvider(
					GingaScreenID screenId, IImageProvider* provider);

			ISurface* createRenderedSurfaceFromImageFile(
					GingaScreenID screenId, const char* mrl);


			/* interfacing input */
			IInputManager* getInputManager(GingaScreenID screenId);
			IEventBuffer* createEventBuffer(GingaScreenID screenId);
			IInputEvent* createInputEvent(
					GingaScreenID screenId, void* event, const int symbol);

			IInputEvent* createApplicationEvent(
					GingaScreenID screenId, int type, void* data);

			int fromMBToGinga(GingaScreenID screenId, int keyCode);
			int fromGingaToMB(GingaScreenID screenId, int keyCode);

			/* and finally some private stuff */
		private:
			void addScreen(
					GingaScreenID screenId, IDeviceScreen* screen);

			short getNumOfScreens();

			bool getScreen(
					GingaScreenID screenId, IDeviceScreen** screen);

			bool removeScreen(GingaScreenID screenId);

			void lockScreenMap();
			void unlockScreenMap();

			void lock();
			void unlock();
	};
}
}
}
}
}
}

#endif /*LocalScreenManager_H_*/
