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
			/* SDL Pending Tasks*/
			static const short SPT_NONE          = 0;
			static const short SPT_INIT          = 1;
			static const short SPT_CLEAR         = 2;
			static const short SPT_RELEASE       = 3;

		public:
			/* SDL Underlying Window Tasks*/
			static const short SUW_SHOW          = 0;
			static const short SUW_HIDE          = 1;
			static const short SUW_RAISETOTOP    = 2;
			static const short SUW_LOWERTOBOTTOM = 3;

			static const short SDS_FPS           = 35;
			static const int uSleepTime          = (int)(1000000/SDS_FPS);

		private:
			static bool hasRenderer;
			static bool mutexInit;
			static map<SDLDeviceScreen*, short> sdlScreens;

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
			set<IWindow*> windowPool;
			set<ISurface*> surfacePool;
			set<IContinuousMediaProvider*> cmpPool;
			set<IDiscreteMediaProvider*> dmpPool;

			GingaScreenID id;
			GingaWindowID uParentId;
			GingaWindowID uEmbedId;
			bool uEmbedFocused;
			bool mustGainFocus;

			IInputManager* im;

			bool waitingCreator;
			pthread_mutex_t condMutex;
			pthread_cond_t cond;

			IWindow* backgroundLayer;
			SDL_Window* screen;
			Uint32 sdlId;
			SDL_Renderer* renderer;

			static bool hasERC; //external renderer controller

			static map<int, int> gingaToSDLCodeMap;
			static map<int, int> sdlToGingaCodeMap;

			static set<SDL_Surface*> uSurPool;
			static set<SDL_Texture*> uTexPool;
			static vector<ReleaseContainer*> releaseList;
			static map<GingaScreenID, map<float, set<IWindow*>*>*> renderMap;
			static set<IContinuousMediaProvider*> cmpRenderList;

			static pthread_mutex_t sdlMutex; //mutex for SDL structures
			static pthread_mutex_t sieMutex; //mutex for SDL input event Map
			static pthread_mutex_t renMutex; //mutex for C++ STL SDL Render Map
			static pthread_mutex_t scrMutex; //mutex for C++ STL SDL Screens
			static pthread_mutex_t recMutex; //mutex for C++ STL release structures
			static pthread_mutex_t winMutex; //mutex for C++ STL Window
			static pthread_mutex_t surMutex; //mutex for C++ STL Surface
			static pthread_mutex_t proMutex; //mutex for C++ STL Providers
			static pthread_mutex_t cstMutex; //mutex for the others C++ STL structures

		public:
			SDLDeviceScreen(
					int numArgs, char** args,
					GingaScreenID myId, GingaWindowID embedId,
					bool externalRenderer);

			virtual ~SDLDeviceScreen();

		private:
			static void checkMutexInit();

		public:
			static void lockSDL();
			static void unlockSDL();

			static void updateRenderMap(
					GingaScreenID screenId, IWindow* window,
					float oldZIndex, float newZIndex);

			void releaseScreen();

			void releaseMB();

			void clearWidgetPools();

			string getScreenName();

		private:
			void setEmbedFromParent(string parentCoords);

		public:
			void setBackgroundImage(string uri);

			unsigned int getWidthResolution();
			void setWidthResolution(unsigned int wRes);
			unsigned int getHeightResolution();
			void setHeightResolution(unsigned int hRes);

			void setColorKey(int r, int g, int b);

			void mergeIds(GingaWindowID destId, vector<GingaWindowID>* srcIds);
			void blitScreen(ISurface* destination);
			void blitScreen(string fileUri);

		private:
			void blitScreen(SDL_Surface* destination);
			void setInitScreenFlag();

		public:
			void refreshScreen();


			/* interfacing output */

			IWindow* createWindow(int x, int y, int w, int h, float z);

			GingaWindowID createUnderlyingSubWindow(
					int x, int y, int w, int h, float z);

		private:
			GingaWindowID createUnderlyingSubWindow(
					GingaWindowID parent,
					string spec,
					int x, int y, int w, int h, float z);

		public:
			GingaWindowID getScreenUnderlyingWindow();

			IWindow* createWindowFrom(GingaWindowID underlyingWindow);
			bool hasWindow(IWindow* win);
			void releaseWindow(IWindow* win);

			ISurface* createSurface();
			ISurface* createSurface(int w, int h);
			ISurface* createSurfaceFrom(void* underlyingSurface);
			bool hasSurface(ISurface* sur);
			bool releaseSurface(ISurface* sur);


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

			static void createReleaseContainer(
					SDL_Surface* uSur,
					SDL_Texture* uTex,
					IMediaProvider* iDec);

		private:
			static void checkSDLInit();
			static void sdlQuit();
			static void* rendererT(void* ptr);

			static void refreshRC(SDLDeviceScreen* screen);
			static int refreshCMP(SDLDeviceScreen* screen);
			static void refreshWin(SDLDeviceScreen* screen);

			static void initEmbed(SDLDeviceScreen* s, GingaWindowID uWin);
			static void forceInputFocus(
					SDLDeviceScreen* screen, GingaWindowID uWin);

			static void initScreen(SDLDeviceScreen* screen);
			static void clearScreen(SDLDeviceScreen* screen);
			static void releaseScreen(SDLDeviceScreen* screen);

			static void releaseAll();

			static void initCMP(
					SDLDeviceScreen* screen, IContinuousMediaProvider* cmp);

			static void blitFromWindow(IWindow* iWin, SDL_Surface* dest);

		public:

			/* interfacing input */

			IInputManager* getInputManager();

			IEventBuffer* createEventBuffer();

			IInputEvent* createInputEvent(void* event, const int symbol);
			IInputEvent* createApplicationEvent(int type, void* data);

			int fromMBToGinga(int keyCode);
			int fromGingaToMB(int keyCode);


			/* interfacing underlying multimedia system */

			void* getGfxRoot();


			/* SDL MB internal use*/
		private:
			/* input */
			static void initCodeMaps();
			static bool checkEventFocus(SDLDeviceScreen* s);

		public:
			/* output */
			static void renderMapInsertWindow(
					GingaScreenID screenId, IWindow* iWin, float z);

			static void renderMapRemoveWindow(
					GingaScreenID screenId, IWindow* iWin, float z);

//			static void updateWindowState(
//					GingaScreenID screenId, IWindow* win, short status);

		private:
//			static void updateWindowList(
//					vector<IWindow*>* windows, IWindow* win, short status);

			static void removeFromWindowList(
					vector<IWindow*>* windows, IWindow* win);

		public:
			static SDL_Window* getUnderlyingWindow(GingaWindowID winId);

		private:
			static bool drawWindow(
					SDL_Renderer* renderer,
					SDL_Texture* texture,
					IWindow* iWin);

			static void insertWindowFromRenderList(
					IWindow* win, vector<IWindow*>* windows);

			static void removeWindowFromRenderList(
					IWindow* win, vector<IWindow*>* windows);

		public:
			/* CAUTION: call this method only from main SDL thread */
			static SDL_Texture* createTextureFromSurface(
					SDL_Renderer* renderer, SDL_Surface* surface);

		private:
			static SDL_Texture* createTexture(
					SDL_Renderer* renderer, int w, int h);

		public:
			/* CAUTION: call this method only from main SDL thread */
			static void releaseTexture(SDL_Texture* uTex);

			static void addUnderlyingSurface(SDL_Surface* uSur);
			static SDL_Surface* createUnderlyingSurface(int width, int height);

		private:
			static SDL_Surface* createUnderlyingSurfaceFromTexture(
					SDL_Texture* texture);

		public:
			static bool hasUnderlyingSurface(SDL_Surface* uSur);

		private:
			static void releaseUnderlyingSurface(SDL_Surface* uSur);

		public:
			static void getRGBAMask(
					int depth,
					Uint32* rmask,
					Uint32* gmask,
					Uint32* bmask,
					Uint32* amask);
	};
}
}
}
}
}
}

#endif /*SDLDEVICESCREEN_H_*/
