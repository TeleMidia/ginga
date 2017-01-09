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

#ifndef TERMWINDOW_H_
#define TERMWINDOW_H_

#include "util/IColor.h"
using namespace ::br::pucrio::telemidia::util;

#include "mb/interface/IWindow.h"

#include <iostream>
#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class TermWindow : public IWindow {
		private:
			void* win;

			GingaScreenID myScreen;
			GingaWindowID windowId;
			GingaWindowID parentId;
			int x;
			int y;
			int width;
			int height;
			int r;
			int g;
			int b;
			int alpha;
			int transparencyValue;
			bool visible;
			bool ghost;

		    ISurface* childSurface;
		    bool fit;
		    bool stretch;
		    int caps;

		    pthread_mutex_t mutex;
		    pthread_mutex_t mutexC;

		public:
			TermWindow(
					GingaWindowID underlyingWindowID,
					GingaWindowID parentWindowID,
					GingaScreenID screenId,
					int x, int y, int width, int height);

			virtual ~TermWindow();

		private:
			void initialize(
					GingaWindowID underlyingWindowID,
					GingaWindowID parentWindowID,
					GingaScreenID screenId,
					int x, int y, int width, int height);

		public:
			void setBgColor(int r, int g, int b, int alpha){};
			IColor* getBgColor(){};
			void setColorKey(int r, int g, int b){};
			IColor* getColorKey(){};
			void setWindowColor(int r, int g, int b, int alpha){};
			IColor* getWindowColor(){};
			void setBorder(int r, int g, int b, int alpha=255, int bWidth=0){};
			void getBorder(int* r, int* g, int* b, int* alpha, int* bWidth){};

			GingaScreenID getScreen();
			void revertContent();
			void setChildSurface(ISurface* listener);
			int getCap(string cap);
			void setCaps(int caps);
			void addCaps(int capability);
			int getCaps();

			void draw();
			void setBounds(int x, int y, int width, int height);
			void moveTo(int x, int y);
			void resize(int width, int height);
			void raiseToTop();
			void lowerToBottom();
			void setCurrentTransparency(int alpha);
			int getTransparencyValue();
			GingaWindowID getId();
			void show();
			void hide();

			int getX();
			int getY();
			int getW();
			int getH();
			float getZ();

			void setX(int x);
			void setY(int y);
			void setW(int w);
			void setH(int h);
			void setZ(float z);

			void* getContent();
			bool isGhostWndow() {return true;};
			void setGhostWindow(bool ghost);
			bool isVisible();
			void validate();

		private:
			void unprotectedValidate();

		public:
			void setStretch(bool stretchTo);
			bool getStretch();
			void setFit(bool fitTo);
			bool getFit();
			void clearContent();

		private:
			bool isMine(ISurface* surface);

		public:
			void renderImgFile(string serializedImageUrl);
			void renderFrom(ISurface* s);
			void blit(IWindow* src);
			void stretchBlit(IWindow* src);
			string getDumpFileUri(int quality, int dumpW, int dumpH);
			void lock();
			void unlock();
			void lockChilds();
			void unlockChilds();
	};
}
}
}
}
}
}

#endif /*TERMWINDOW_H_*/
