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

#ifdef _WIN32
#if	_EXP_LOCALSCREENHANDLER_API == 0
#define LOCALSCREENHANDLER_API	__declspec(dllexport)
#else
#define LOCALSCREENHANDLER_API	__declspec(dllimport)
#endif
#else
#define LOCALSCREENHANDLER_API
#endif

#include "interface/IDeviceScreen.h"
#include "ILocalScreenManager.h"

#include <pthread.h>

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
	class LOCALSCREENHANDLER_API LocalScreenManager :
			public ILocalScreenManager {

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
			map<GingaScreenID, IDeviceScreen*>* screens;
			pthread_mutex_t scrMutex;

			vector<short> sortSys;
			map<string, short>* sysNames;
			pthread_mutex_t sysMutex;

			static LocalScreenManager* _instance;

			LocalScreenManager();
			virtual ~LocalScreenManager();

		public:
			void releaseHandler();

			void setParentScreen(GingaScreenID screenId, void* parentId);

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
					string mbSystem,
					string mbSubSystem,
					string mbMode,
					string mbParent);

			void getMBSystemType(string mbSystemName, short* mbSystemType);

			bool isAvailable(short mbSysType);
			void lockSysNames();
			void unlockSysNames();

		public:
			void mergeIds(
					GingaScreenID screenId,
					GingaWindowID destId,
					vector<GingaWindowID>* srcIds);


			/* Interfacing output */
			IWindow* createWindow(
					GingaScreenID screenId, int x, int y, int w, int h);

			IWindow* createWindowFrom(
					GingaScreenID screenId, GingaWindowID underlyingWindow);

			void releaseWindow(GingaScreenID screenId, IWindow* window);

			ISurface* createSurface(GingaScreenID screenId);

			ISurface* createSurface(GingaScreenID screenId, int w, int h);

			ISurface* createSurfaceFrom(
					GingaScreenID screenId, void* underlyingSurface);

			void releaseSurface(GingaScreenID screenId, ISurface* surface);


			/* Interfacing content */
			IContinuousMediaProvider* createContinuousMediaProvider(
					GingaScreenID screenId,
					const char* mrl,
					bool hasVisual,
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

		private:
			void addScreen(
					GingaScreenID screenId, IDeviceScreen* screen);

			short getNumOfScreens();

			bool getScreen(
					GingaScreenID screenId, IDeviceScreen** screen);

			void lockScreens();
			void unlockScreens();
	};
}
}
}
}
}
}

#endif /*LocalScreenManager_H_*/
