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

#ifndef TERMDEVICESCREEN_H_
#define TERMDEVICESCREEN_H_

#include "mb/interface/IDeviceScreen.h"

#include <iostream>
#include <string>
#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class TermDeviceScreen : public IDeviceScreen {
		private:
			set<string> windowPool;
			set<string> surfacePool;

		public:
			TermDeviceScreen(
					int numArgs, char** args,
					GingaWindowID embedId,
					bool externalRenderer);

			virtual ~TermDeviceScreen();

			void releaseScreen();
			void releaseMB();

			void clearWidgetPools();

			string getScreenName();
			GingaWindowID getScreenUnderlyingWindow(){return NULL;};
			void setBackgroundImage(string uri);

			unsigned int getWidthResolution();
			void setWidthResolution(unsigned int wRes);
			unsigned int getHeightResolution();
			void setHeightResolution(unsigned int hRes);

			void setColorKey(int r, int g, int b);

			IWindow* getIWindowFromId(GingaWindowID winId) {return NULL;};
			bool mergeIds(GingaWindowID destId, vector<GingaWindowID>* srcIds);
			void blitScreen(ISurface* destination);
			void blitScreen(string fileUri);
			void refreshScreen(){};

			/* interfacing output */

			IWindow* createWindow(int x, int y, int w, int h, float z);

			GingaWindowID createUnderlyingSubWindow(
					int x, int y, int w, int h, float z) {return NULL;};

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
					const char* mrl, bool* hasVisual, bool isRemote);

			void releaseContinuousMediaProvider(
					IContinuousMediaProvider* provider);

			IFontProvider* createFontProvider(
					const char* mrl, int fontSize);

			void releaseFontProvider(IFontProvider* provider);

			IImageProvider* createImageProvider(const char* mrl);
			void releaseImageProvider(IImageProvider* provider);

			ISurface* createRenderedSurfaceFromImageFile(const char* mrl);


			/* interfacing input */

			IInputManager* getInputManager();
			IEventBuffer* createEventBuffer();
			IInputEvent* createInputEvent(void* event, const int symbol);
			IInputEvent* createApplicationEvent(int type, void* data);
			int fromMBToGinga(int keyCode);
			int fromGingaToMB(int keyCode);

			/* interfacing underlying multimedia system */

			void* getGfxRoot();
	};
}
}
}
}
}
}

#endif /*TERMDEVICESCREEN_H_*/
