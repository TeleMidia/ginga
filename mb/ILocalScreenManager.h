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

#ifndef ILocalScreenManager_H_
#define ILocalScreenManager_H_

#include "IMBDefs.h"

#include "IWindow.h"
#include "ISurface.h"

#include "IContinuousMediaProvider.h"
#include "IFontProvider.h"
#include "IImageProvider.h"

#include "IInputManager.h"
#include "IInputEvent.h"
#include "IEventBuffer.h"

#include <vector>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class IScreenManager {
		public:
			virtual ~IScreenManager(){};

			virtual void releaseHandler()=0;

			virtual void setBackgroundImage(
					GingaScreenID screenId, string uri)=0;

			virtual int getDeviceWidth(GingaScreenID screenId)=0;

			virtual int getDeviceHeight(GingaScreenID screenId)=0;

			virtual void* getGfxRoot(GingaScreenID screenId)=0;

			virtual void releaseScreen(GingaScreenID screenId)=0;
			virtual void releaseMB(GingaScreenID screenId)=0;
			virtual void clearWidgetPools(GingaScreenID screenId)=0;

			virtual GingaScreenID createScreen(int numArgs, char** args)=0;
			virtual string getScreenName(GingaScreenID screenId)=0;

			virtual UnderlyingWindowID getScreenUnderlyingWindow(
					GingaScreenID screenId)=0;

			virtual IWindow* getIWindowFromId(
					GingaScreenID screenId, GingaWindowID winId)=0;

			virtual bool mergeIds(
					GingaScreenID screenId,
					GingaWindowID destId,
					vector<GingaWindowID>* srcIds)=0;

			virtual void blitScreen(
					GingaScreenID screenId, ISurface* destination)=0;

			virtual void blitScreen(GingaScreenID screenId, string fileUri)=0;
			virtual void refreshScreen(GingaScreenID screenId)=0;

      virtual bool isLocal () = 0;


			/* Interfacing output */

			virtual GingaWindowID createWindow(
					GingaScreenID screenId,
					int x, int y,
					int w, int h,
					float z)=0;

			virtual UnderlyingWindowID createUnderlyingSubWindow(
					GingaScreenID screenId,
					int x, int y,
					int w, int h,
					float z)=0;

			virtual bool hasWindow(
					GingaScreenID screenId, GingaWindowID window)=0;

			virtual void releaseWindow(
					GingaScreenID screenId, IWindow* window) = 0;

			virtual GingaSurfaceID createSurface(GingaScreenID screenId)=0;

			virtual GingaSurfaceID createSurface(
					GingaScreenID screenId, int w, int h)=0;

			virtual GingaSurfaceID createSurfaceFrom(
					GingaScreenID screenId, GingaSurfaceID underlyingSurface)=0;

			virtual bool hasSurface(
					const GingaScreenID &screenId, const GingaSurfaceID &surId)=0;

			virtual bool releaseSurface(
					GingaScreenID screenId, ISurface* sur)=0;

			/* Interfacing content */
			virtual IMediaProvider* getIMediaProviderFromId (
					const GingaProviderID& provId) = 0;
			virtual ISurface* getISurfaceFromId(const GingaSurfaceID &surfaceId) = 0;

			virtual GingaProviderID createContinuousMediaProvider(
					GingaScreenID screenId,
					const char* mrl,
					bool isRemote)=0;

			virtual void releaseContinuousMediaProvider(
					GingaScreenID screenId,
					GingaProviderID provider)=0;

			virtual GingaProviderID createFontProvider(
					GingaScreenID screenId,
					const char* mrl,
					int fontSize)=0;

			virtual void releaseFontProvider(
					GingaScreenID screenId, GingaProviderID provider)=0;

			virtual GingaProviderID createImageProvider(
					GingaScreenID screenId, const char* mrl)=0;

			virtual void releaseImageProvider(
					GingaScreenID screenId, GingaProviderID provider)=0;

			virtual GingaSurfaceID createRenderedSurfaceFromImageFile(
					GingaScreenID screenId, const char* mrl)=0;


			/* Interfacing input */
			virtual IInputManager* getInputManager(GingaScreenID screenId)=0;
			virtual IEventBuffer* createEventBuffer(GingaScreenID screenId)=0;

			virtual IInputEvent* createInputEvent(
					GingaScreenID screenId, void* event, const int symbol)=0;

			virtual IInputEvent* createApplicationEvent(
					GingaScreenID screenId, int type, void* data)=0;

			virtual int fromMBToGinga(GingaScreenID screenId, int keyCode)=0;
			virtual int fromGingaToMB(GingaScreenID screenId, int keyCode)=0;

			/* Methods created to isolate gingacc-mb */
			//Windows
			virtual void addWindowCaps (
					const GingaScreenID &screenId, const GingaWindowID &winId,
					int caps) = 0;

			virtual void setWindowCaps (
					const GingaScreenID &screenId, const GingaWindowID &, int caps) = 0;
			virtual int getWindowCap (
					const GingaScreenID &screenId, const GingaWindowID &winId,
					const string &cap) = 0;

			virtual void drawWindow (
					const GingaScreenID &screenId, const GingaWindowID &winId) = 0;

			virtual void setWindowBounds (
					const GingaScreenID &screenId, const GingaWindowID &winId,
			                              int x, int y, int w, int h) = 0;

			virtual void showWindow (
					const GingaScreenID &screenId, const GingaWindowID &winId) = 0;

			virtual void hideWindow (
					const GingaScreenID &screenId, const GingaWindowID &winId) = 0;

			virtual void raiseWindowToTop (
					const GingaScreenID &sreenId, const GingaWindowID &winId) = 0;

			virtual void renderWindowFrom(
					const GingaScreenID &screenId, const GingaWindowID &winId,
					const GingaSurfaceID &surId) = 0;

			virtual void setWindowBgColor (
					const GingaScreenID &screenId, const GingaWindowID &winId,
					int r, int g, int b, int alpha) = 0;

			virtual void setWindowBorder (
					const GingaScreenID &screenId, const GingaWindowID &winId,
					int r, int g, int b, int alpha, int width) = 0;

			virtual void setWindowCurrentTransparency (
					const GingaScreenID &screenId, const GingaWindowID &winId,
					int transparency) = 0;

			virtual void setWindowColorKey (
					const GingaScreenID &screenId, const GingaWindowID &winId,
					int r, int g, int b) = 0;

			virtual void setWindowX(
					const GingaScreenID &screenId, const GingaWindowID &winId, int x) = 0;
			virtual void setWindowY(
					const GingaScreenID &screenId, const GingaWindowID &winId, int y) = 0;
			virtual void setWindowW(
					const GingaScreenID &screenId, const GingaWindowID &winId, int w) = 0;
			virtual void setWindowH(
					const GingaScreenID &screenId, const GingaWindowID &winId, int h) = 0;
			virtual void setWindowZ (
					const GingaScreenID &screenId, const GingaWindowID &winId,
					float z) = 0;

			virtual void disposeWindow (
					const GingaScreenID &screenId, const GingaWindowID &winId) = 0;

			virtual void setGhostWindow(
					const GingaScreenID &screenId, const GingaWindowID &winId,
					bool ghost) = 0;

			virtual void validateWindow (
					const GingaScreenID &screenId, const GingaWindowID &winId) = 0;

			virtual int getWindowX (
					const GingaScreenID &screenId, const GingaWindowID &winId) = 0;
			virtual int getWindowY (
					const GingaScreenID &screenId, const GingaWindowID &winId) = 0;
			virtual int getWindowW (
					const GingaScreenID &screenId, const GingaWindowID &winId) = 0;
			virtual int getWindowH (
					const GingaScreenID &screenId, const GingaWindowID &winId) = 0;
			virtual float getWindowZ (
					const GingaScreenID &screenId, const GingaWindowID &winId)= 0;

			virtual int getWindowTransparencyValue(
					const GingaScreenID &screenId, const GingaWindowID &winId) = 0;

			virtual void resizeWindow(
					const GingaScreenID &screenId, const GingaWindowID &winId,
					int width, int height) = 0;

			virtual string getWindowDumpFileUri (
					const GingaScreenID &screenId, const GingaWindowID &winId,
					int quality, int dumpW, int dumpH) = 0;

			virtual void clearWindowContent (
					const GingaScreenID &screenId, const GingaWindowID &winId) = 0;

			virtual void lowerWindowToBottom (
					const GingaScreenID &screenId, const GingaWindowID &winId) = 0;

			virtual void setWindowMirrorSrc (
					const GingaScreenID &screenId, const GingaWindowID &winId,
					const GingaWindowID &mirrorSrc) = 0;

			virtual void revertWindowContent(
					const GingaScreenID &screenId, const GingaWindowID &winId) = 0;

			virtual void deleteWindow(
					const GingaScreenID &screenId, const GingaWindowID &winId) = 0;

			virtual void moveWindowTo(
					const GingaScreenID &screenId, const GingaWindowID &winId,
					int x, int y) = 0;


			//Surfaces
			virtual void registerSurface (ISurface*) = 0;

			//FIXME: Players should not use this method to completely isolate gingacc-mb.
			virtual void* getSurfaceContent(const GingaSurfaceID& surId) = 0;

			virtual GingaWindowID getSurfaceParentWindow(
					const GingaSurfaceID& surId) = 0;

			virtual void deleteSurface (const GingaSurfaceID &surId) = 0;

			virtual bool setSurfaceParentWindow (
					const GingaScreenID &screenId, const GingaSurfaceID &surId,
					const GingaWindowID &winId) = 0;

			virtual void clearSurfaceContent (const GingaSurfaceID &surId) = 0;

			virtual void getSurfaceSize (const GingaSurfaceID &surId,
			                             int* width, int* height) = 0;

			virtual void addSurfaceCaps(
					const GingaSurfaceID &surId, const int caps) = 0;
			virtual void setSurfaceCaps (
					const GingaSurfaceID &surId, const int caps) = 0;
			virtual int getSurfaceCap(
					const GingaSurfaceID &surId, const string &cap) = 0;
			virtual int getSurfaceCaps(const GingaSurfaceID &surId) = 0;

			virtual void setSurfaceBgColor(const GingaSurfaceID &surId,
			                               int r, int g, int b, int alpha) = 0;

			virtual void setSurfaceFont(const GingaSurfaceID &surId,
			                            GingaSurfaceID font) = 0;

			virtual void setColor(const GingaSurfaceID &surId, int r, int g, int b,
			                      int alpha) = 0;;

			virtual void setExternalHandler(
					const GingaSurfaceID &surId, bool extHandler) = 0;

			virtual void blitSurface (
					const GingaSurfaceID &surId,int x, int y, GingaSurfaceID src=0,
					int srcX=-1, int srcY=-1, int srcW=-1, int srcH=-1) = 0;

			virtual void flipSurface (const GingaSurfaceID &surId) = 0;

			virtual void setSurfaceContent(
					const GingaSurfaceID &surId, void* surface) = 0;

			virtual IColor* getSurfaceColor(const GingaSurfaceID &surId) = 0;

			virtual bool hasSurfaceExternalHandler(const GingaSurfaceID &surId) = 0;

			virtual void setSurfaceColor(
					const GingaSurfaceID &surId, int r, int g, int b, int alpha) = 0;


			//Providers
			virtual void setProviderSoundLevel (
					const GingaProviderID &provId, float level) = 0;

			virtual void getProviderOriginalResolution(
					const GingaProviderID &provId, int* width, int* height) = 0;

			virtual double getProviderTotalMediaTime(
					const GingaProviderID &provId) = 0;

			virtual double getProviderSoundLevel(const GingaProviderID &provId) = 0;

			virtual int64_t getProviderVPts(const GingaProviderID &provId) = 0;

			virtual void setProviderMediaTime(
					const GingaProviderID &provId, double pos) = 0;

			virtual double getProviderMediaTime(const GingaProviderID &provId) = 0;

			virtual void pauseProvider (const GingaProviderID &provId) = 0;

			virtual void stopProvider (const GingaProviderID &provId) = 0;

			virtual void resumeProvider (
					const GingaProviderID &provId, GingaSurfaceID surface) = 0;

			virtual void setProviderAVPid(
					const GingaProviderID &provId, int aPid, int vPid) = 0;

			virtual void feedProviderBuffers(const GingaProviderID &provId) = 0;

			virtual bool checkProviderVideoResizeEvent(
					const GingaProviderID &provId, const GingaSurfaceID &frame) = 0;

			virtual int getProviderStringWidth(
					const GingaProviderID &provId, const char* text, int textLength=0)=0;

			virtual void playProviderOver(
					const GingaProviderID &provId, const GingaSurfaceID &surface) = 0;

			virtual void playProviderOver(
					const GingaProviderID &provId, const GingaSurfaceID &surface,
					const char* text, int x, int y, short align) = 0;

			virtual int getProviderHeight(const GingaProviderID &provId) = 0;
	};
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::mb::IScreenManager*
		LocalScreenManagerCreator();

typedef void LocalScreenManagerDestroyer(
		::br::pucrio::telemidia::ginga::core::mb::
				IScreenManager* dm);

#endif /*ILocalScreenManager_H_*/
