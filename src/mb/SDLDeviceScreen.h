/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

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

#ifndef SDLDEVICESCREEN_H_
#define SDLDEVICESCREEN_H_

#include "config.h"

#include "InputManager.h"
#include "SDLEventBuffer.h"
#include "SDLInputEvent.h"
#include "IContinuousMediaProvider.h"
#include "IFontProvider.h"
#include "IImageProvider.h"

#include "SDLWindow.h"
#include "SDLSurface.h"

#include <vector>
#include <string>
using namespace std;

#include "util/functions.h"
using namespace ::ginga::util;

#include "IDiscreteMediaProvider.h"

#include "SDL.h"

#include <pthread.h>
#include <time.h>

#include <set>
#include <map>
#include <iostream>
using namespace std;

#ifndef GINGA_PIXEL_FMT
#define GINGA_PIXEL_FMT SDL_PIXELFORMAT_RGB24
#endif

GINGA_MB_BEGIN


typedef struct {
	IMediaProvider* iDec;
	SDL_Surface* uSur;
	SDL_Texture* uTex;
} ReleaseContainer;

	class SDLDeviceScreen {
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

			GingaWindowID winIdRefCounter;
			map<GingaWindowID, SDLWindow*> windowRefs;
			set<SDLWindow*> windowPool;
			set<SDLSurface*> surfacePool;
			set<IContinuousMediaProvider*> cmpPool;
			set<IDiscreteMediaProvider*> dmpPool;

			GingaScreenID id;
			UnderlyingWindowID uParentId;
			UnderlyingWindowID uEmbedId;
			bool uEmbedFocused;
			bool mustGainFocus;

			InputManager* im;
			bool useStdin;

			bool waitingCreator;
			pthread_mutex_t condMutex;
			pthread_cond_t cond;

			SDLWindow* backgroundLayer;
			bool fullScreen;
			SDL_Window* screen;
			Uint32 sdlId;
			SDL_Renderer* renderer;

			static bool hasERC; //external renderer controller

			static map<int, int> gingaToSDLCodeMap;
			static map<int, int> sdlToGingaCodeMap;
			static map<string, int> sdlStrToSdlCode;

			static set<SDL_Surface*> uSurPool;
			static set<SDL_Texture*> uTexPool;
			static vector<ReleaseContainer*> releaseList;
			static map<GingaScreenID, map<float, set<SDLWindow*>*>*> renderMap;
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
					GingaScreenID myId, UnderlyingWindowID embedId,
					bool externalRenderer);

			virtual ~SDLDeviceScreen();

		private:
			static void checkMutexInit();

		public:
			static void lockSDL();
			static void unlockSDL();

			static void updateRenderMap(
					GingaScreenID screenId, SDLWindow* window,
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

			SDLWindow* getIWindowFromId(GingaWindowID winId);
			bool mergeIds(GingaWindowID destId, vector<GingaWindowID>* srcIds);
			void blitScreen(SDLSurface* destination);
			void blitScreen(string fileUri);

		private:
			void blitScreen(SDL_Surface* destination);
			void setInitScreenFlag();

		public:
			void refreshScreen();


			/* interfacing output */

			SDLWindow* createWindow(int x, int y, int w, int h, float z);

			UnderlyingWindowID createUnderlyingSubWindow(
					int x, int y, int w, int h, float z);

		private:
			UnderlyingWindowID createUnderlyingSubWindow(
					UnderlyingWindowID parent,
					string spec,
					int x, int y, int w, int h, float z);

		public:
			UnderlyingWindowID getScreenUnderlyingWindow();

			bool hasWindow(SDLWindow* win);
			void releaseWindow(SDLWindow* win);

			SDLSurface* createSurface();
			SDLSurface* createSurface(int w, int h);
			SDLSurface* createSurfaceFrom(void* underlyingSurface);
			bool hasSurface(SDLSurface* sur);
			bool releaseSurface(SDLSurface* sur);


			/* interfacing content */

			IContinuousMediaProvider* createContinuousMediaProvider(
					const char* mrl, bool isRemote);

			void releaseContinuousMediaProvider(
					IContinuousMediaProvider* provider);

			IFontProvider* createFontProvider(
					const char* mrl, int fontSize);

			void releaseFontProvider(IFontProvider* provider);

			IImageProvider* createImageProvider(const char* mrl);
			void releaseImageProvider(IImageProvider* provider);

			SDLSurface* createRenderedSurfaceFromImageFile(const char* mrl);

			static void addCMPToRendererList(IContinuousMediaProvider* cmp);
			static void removeCMPToRendererList(IContinuousMediaProvider* cmp);

			static void createReleaseContainer(
					SDL_Surface* uSur,
					SDL_Texture* uTex,
					IMediaProvider* iDec);

		private:
			static void checkSDLInit();
			static void notifyQuit();
			static void sdlQuit();

			static void checkWindowFocus(SDLDeviceScreen* s, SDL_Event* event);
			static bool notifyEvent(
					SDLDeviceScreen* screen,
					SDL_Event* event,
					bool capsOn,
					bool shiftOn);

			static void* checkStdin(void* ptr);
			static void processCmd(
					SDLDeviceScreen* s,
					string cmd,
					string type,
					string args);

			static bool checkEvents();
			static void* rendererT(void* ptr);

			static void refreshRC(SDLDeviceScreen* screen);
			static int refreshCMP(SDLDeviceScreen* screen);
			static void refreshWin(SDLDeviceScreen* screen);

			static void initEmbed(SDLDeviceScreen* s, UnderlyingWindowID uWin);
			static void forceInputFocus(
					SDLDeviceScreen* screen, UnderlyingWindowID uWin);

			static void initScreen(SDLDeviceScreen* screen);
			static void clearScreen(SDLDeviceScreen* screen);
			static void releaseScreen(SDLDeviceScreen* screen);

			static void releaseAll();

			static void initCMP(
					SDLDeviceScreen* screen, IContinuousMediaProvider* cmp);

			static bool blitFromWindow(SDLWindow* iWin, SDL_Surface* dest);

		public:

			/* interfacing input */

			InputManager* getInputManager();

			SDLEventBuffer* createEventBuffer();

			SDLInputEvent* createInputEvent(void* event, const int symbol);
			SDLInputEvent* createApplicationEvent(int type, void* data);

			int fromMBToGinga(int keyCode);
			int fromGingaToMB(int keyCode);


			/* interfacing underlying multimedia system */

			void* getGfxRoot();


			/* SDL MB internal use*/
		private:
			/* input */
			static int convertEventCodeStrToInt(string strEvent);
			static void initCodeMaps();
			static bool checkEventFocus(SDLDeviceScreen* s);

		public:
			/* output */
			static void renderMapInsertWindow(
					GingaScreenID screenId, SDLWindow* iWin, float z);

			static void renderMapRemoveWindow(
					GingaScreenID screenId, SDLWindow* iWin, float z);

//			static void updateWindowState(
//					GingaScreenID screenId, SDLWindow* win, short status);

		private:
//			static void updateWindowList(
//					vector<SDLWindow*>* windows, SDLWindow* win, short status);

			static void removeFromWindowList(
					vector<SDLWindow*>* windows, SDLWindow* win);

		public:
			static SDL_Window* getUnderlyingWindow(GingaWindowID winId);

		private:
			static bool drawSDLWindow(
					SDL_Renderer* renderer,
					SDL_Texture* texture,
					SDLWindow* iWin);

			static void insertWindowFromRenderList(
					SDLWindow* win, vector<SDLWindow*>* windows);

			static void removeWindowFromRenderList(
					SDLWindow* win, vector<SDLWindow*>* windows);

		public:
			/* CAUTION: call this method only from main SDL thread */
			static SDL_Texture* createTextureFromSurface(
					SDL_Renderer* renderer, SDL_Surface* surface);

		private:
			static SDL_Texture* createTexture(
					SDL_Renderer* renderer, int w, int h);

		public:
			static bool hasTexture(SDL_Texture* uTex);
			static void releaseTexture(SDL_Texture* uTex);

			static void addUnderlyingSurface(SDL_Surface* uSur);
			static SDL_Surface* createUnderlyingSurface(int width, int height);

			static SDL_Surface* createUnderlyingSurfaceFromTexture(
					SDL_Texture* texture);

			static bool hasUnderlyingSurface(SDL_Surface* uSur);

		private:
			static void releaseUnderlyingSurface(SDL_Surface* uSur);
	};

GINGA_MB_END
#endif /*SDLDEVICESCREEN_H_*/
