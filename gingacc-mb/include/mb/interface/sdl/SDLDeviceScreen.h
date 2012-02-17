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

#ifndef SDLDEVICESCREEN_H_
#define SDLDEVICESCREEN_H_

#include "config.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#if HAVE_COMPSUPPORT
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#endif

#include "mb/interface/IDiscreteMediaProvider.h"
#include "mb/interface/IDeviceScreen.h"

#include "SDL.h"

#include <pthread.h>

#include <set>
#include <map>
#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {

typedef struct {
	IMediaProvider* iDec;
	SDL_Surface* uSur;
	SDL_Texture* uTex;
} ReleaseContainer;

	class SDLDeviceScreen : public IDeviceScreen {
		public:
			static const unsigned int DSA_UNKNOWN;
			static const unsigned int DSA_4x3;
			static const unsigned int DSA_16x9;

		private:
			static bool hasRenderer;
			static map<SDLDeviceScreen*, bool> sdlScreens;
			static pthread_mutex_t sMutex;

			string aSystem;
			string mbMode;
			string mbSubSystem;
			unsigned int aspect;
			unsigned int hSize;
			unsigned int vSize;
			int hRes;
			int wRes;

#if HAVE_COMPSUPPORT
			static IComponentManager* cm;
#endif
			set<IWindow*>* windowPool;
			set<ISurface*>* surfacePool;
			set<IContinuousMediaProvider*>* cmpPool;
			set<IDiscreteMediaProvider*>* dmpPool;

			GingaScreenID id;
			GingaWindowID uId;
			IInputManager* im;

			SDL_Surface* uSur;
			int uSurW;
			int uSurH;
			pthread_mutex_t uSurMutex;
			bool uSurPending;

			bool waitingCreator;
			pthread_mutex_t cMutex;
			pthread_cond_t cond;

			pthread_mutex_t winMutex;
			pthread_mutex_t surMutex;
			pthread_mutex_t cmpMutex;
			pthread_mutex_t dmpMutex;

			SDL_Window* screen;
			SDL_Renderer* renderer;

			static pthread_mutex_t ieMutex;
			static map<int, int>* gingaToSDLCodeMap;
			static map<int, int>* sdlToGingaCodeMap;

			static set<ReleaseContainer*> releaseList;
			static pthread_mutex_t rlMutex;

			static set<IContinuousMediaProvider*> cmpRenderList;
			static set<IDiscreteMediaProvider*> dmpRenderList;
			static pthread_mutex_t mplMutex;

		public:
			SDLDeviceScreen(
					int numArgs, char** args,
					GingaScreenID myId, GingaWindowID parentId);

			virtual ~SDLDeviceScreen();

			void releaseScreen();

			void releaseMB();

			void clearWidgetPools();

			void setParentScreen(GingaWindowID parentId);

			void setBackgroundImage(string uri);

			unsigned int getWidthResolution();
			void setWidthResolution(unsigned int wRes);
			unsigned int getHeightResolution();
			void setHeightResolution(unsigned int hRes);

			void setColorKey(int r, int g, int b);

			void mergeIds(GingaWindowID destId, vector<GingaWindowID>* srcIds);


			/* interfacing output */

			IWindow* createWindow(int x, int y, int w, int h);
			IWindow* createWindowFrom(GingaWindowID underlyingWindow);
			bool hasWindow(IWindow* win);
			void releaseWindow(IWindow* win);

			ISurface* createSurface();
			ISurface* createSurface(int w, int h);
			ISurface* createSurfaceFrom(void* underlyingSurface);
			bool hasSurface(ISurface* sur);
			void releaseSurface(ISurface* sur);


			/* interfacing content */

			IContinuousMediaProvider* createContinuousMediaProvider(
					const char* mrl, bool hasVisual, bool isRemote);

			void releaseContinuousMediaProvider(
					IContinuousMediaProvider* provider);

			IFontProvider* createFontProvider(
					const char* mrl, int fontSize);

			void releaseFontProvider(IFontProvider* provider);

			IImageProvider* createImageProvider(const char* mrl);
			void releaseImageProvider(IImageProvider* provider);

			ISurface* createRenderedSurfaceFromImageFile(const char* mrl);

			static void addCMPToRendererList(IContinuousMediaProvider* cmp);
			static void removeCMPToRendererList(IContinuousMediaProvider* cmp);

			static void addDMPToRendererList(IDiscreteMediaProvider* dmp);
			static void removeDMPToRendererList(IDiscreteMediaProvider* dmp);

			static void createReleaseContainer(
					SDL_Surface* uSur,
					SDL_Texture* uTex,
					IMediaProvider* iDec);

		private:
			static bool checkTasks(SDLDeviceScreen* screen);
			static void refreshRC(SDLDeviceScreen* screen);
			static void refreshCMP(SDLDeviceScreen* screen);
			static void refreshDMP(SDLDeviceScreen* screen);
			static void refreshWin(SDLDeviceScreen* screen);
			static void* rendererT(void* ptr);
			static void initScreen(SDLDeviceScreen* screen);
			static void initCMP(
					SDLDeviceScreen* screen, IContinuousMediaProvider* cmp);

		public:

			/* interfacing input */

			IInputManager* getInputManager();
			void setInputManager(IInputManager* im);

			IEventBuffer* createEventBuffer();
			IInputEvent* createInputEvent(void* event, const int symbol);
			IInputEvent* createApplicationEvent(int type, void* data);

			int fromMBToGinga(int keyCode);
			int fromGingaToMB(int keyCode);


			/* interfacing underlying multimedia system */

			void* getGfxRoot();


			/* SDL MB internal use*/

			/* input */
			static void initCodeMaps();

			/* output */
			static SDL_Window* getUnderlyingWindow(GingaWindowID winId);

		private:
			static SDL_Texture* createTexture(
					SDL_Renderer* renderer, SDL_Surface* surface);

			static void releaseTexture(SDL_Texture* uTex);

			SDL_Surface* createUnderlyingSurface(int width, int height);

			static void releaseUnderlyingSurface(SDL_Surface* uSur);

			bool getRenderList(vector<IWindow*>* renderList);

			void waitSurfaceCreator();
			bool surfaceCreator();
	};
}
}
}
}
}
}

#endif /*SDLDEVICESCREEN_H_*/
