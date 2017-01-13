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

#ifndef IDEVICESCREEN_H_
#define IDEVICESCREEN_H_

#include "IMBDefs.h"
#include "IInputManager.h"

#include "IContinuousMediaProvider.h"
#include "IFontProvider.h"
#include "IImageProvider.h"

#include "IWindow.h"
#include "ISurface.h"

#include <vector>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class IDeviceScreen {
		public:
			/*
			 * Attention:
			 *
			 * If you are exiting from your program or if you are
			 * removing all screens of a specific multimedia backend, call
			 * releaseMB() before delete a device screen,
			 */
			virtual ~IDeviceScreen(){};

			virtual void releaseScreen()=0;
			virtual void releaseMB()=0;
			virtual void clearWidgetPools()=0;

			virtual string getScreenName()=0;
			virtual UnderlyingWindowID getScreenUnderlyingWindow()=0;

			virtual void setBackgroundImage(string uri)=0;

			virtual unsigned int getWidthResolution()=0;
			virtual void setWidthResolution(unsigned int wRes)=0;
			virtual unsigned int getHeightResolution()=0;
			virtual void setHeightResolution(unsigned int hRes)=0;

			virtual void setColorKey(int r, int g, int b)=0;

			virtual IWindow* getIWindowFromId(GingaWindowID winId)=0;

			virtual bool mergeIds(
					GingaWindowID destId, vector<GingaWindowID>* srcIds)=0;

			virtual void blitScreen(ISurface* destination)=0;
			virtual void blitScreen(string fileUri)=0;

			virtual void refreshScreen()=0;


			/* interfacing output */

			virtual IWindow* createWindow(
					int x, int y, int w, int h, float z)=0;

			virtual UnderlyingWindowID createUnderlyingSubWindow(
					int x, int y, int w, int h, float z)=0;

			virtual bool hasWindow(IWindow* win)=0;
			virtual void releaseWindow(IWindow* win)=0;

			virtual ISurface* createSurface()=0;
			virtual ISurface* createSurface(int w, int h)=0;
			virtual ISurface* createSurfaceFrom(
					void* underlyingSurface)=0;
			virtual bool hasSurface(ISurface* sur)=0;
			virtual bool releaseSurface(ISurface* sur)=0;


			/* interfacing content */

			virtual IContinuousMediaProvider* createContinuousMediaProvider(
					const char* mrl, bool isRemote)=0;

			virtual void releaseContinuousMediaProvider(
					IContinuousMediaProvider* provider)=0;

			virtual IFontProvider* createFontProvider(
					const char* mrl, int fontSize)=0;

			virtual void releaseFontProvider(IFontProvider* provider)=0;

			virtual IImageProvider* createImageProvider(const char* mrl)=0;
			virtual void releaseImageProvider(IImageProvider* provider)=0;

			virtual ISurface* createRenderedSurfaceFromImageFile(
					const char* mrl)=0;


			/* interfacing input */

			virtual IInputManager* getInputManager()=0;
			virtual IEventBuffer* createEventBuffer()=0;

			virtual IInputEvent* createInputEvent(
					void* event, const int symbol)=0;

			virtual IInputEvent* createApplicationEvent(int type, void* data)=0;
			virtual int fromMBToGinga(int keyCode)=0;
			virtual int fromGingaToMB(int keyCode)=0;


			/* interfacing underlying multimedia system */

			virtual void* getGfxRoot()=0;
	};
}
}
}
}
}
}

#endif /*IDEVICESCREEN_H_*/
