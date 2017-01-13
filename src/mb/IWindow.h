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

#ifndef IWINDOW_H_
#define IWINDOW_H_

#include "util/Color.h"
using namespace ::br::pucrio::telemidia::util;

#include "ISurface.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class IWindow {
		public:
			virtual ~IWindow(){};

			virtual void addMirror(IWindow* window)=0;
			virtual bool removeMirror(IWindow* window)=0;
			virtual void setMirrorSrc(IWindow* mirrorSrc)=0;
			virtual IWindow* getMirrorSrc()=0;

			virtual void setBgColor(int r, int g, int b, int alpha)=0;
			virtual Color* getBgColor()=0;
			virtual void setColorKey(int r, int g, int b)=0;
			virtual Color* getColorKey()=0;
			virtual void setWindowColor(int r, int g, int b, int alpha)=0;
			virtual Color* getWindowColor()=0;
			virtual void setBorder(
					int r, int g, int b, int alpha=255, int bWidth=0)=0;

			virtual void getBorder(
					int* r, int* g, int* b, int* alpha, int* bWidth)=0;

			virtual GingaScreenID getScreen()=0;
			virtual void revertContent()=0;
			virtual void setChildSurface(ISurface* listener)=0;
			virtual int getCap(string capName)=0;
			virtual void setCaps(int caps)=0;
			virtual void addCaps(int capability)=0;
			virtual int getCaps()=0;
			virtual void draw()=0;
			virtual void setBounds(int x, int y, int width, int height)=0;
			virtual void moveTo(int x, int y)=0;
			virtual void resize(int width, int height)=0;
			virtual void raiseToTop()=0;
			virtual void lowerToBottom()=0;
			virtual void setCurrentTransparency(int alpha)=0;
			virtual int getTransparencyValue()=0;
			virtual GingaWindowID getId()=0;
			virtual void show()=0;
			virtual void hide()=0;

			virtual int getX()=0;
			virtual int getY()=0;
			virtual int getW()=0;
			virtual int getH()=0;
			virtual float getZ()=0;

			virtual void setX(int x)=0;
			virtual void setY(int y)=0;
			virtual void setW(int w)=0;
			virtual void setH(int h)=0;
			virtual void setZ(float h)=0;

			virtual void* getContent()=0;

			virtual bool isGhostWindow()=0;
			virtual void setGhostWindow(bool ghost)=0;
			virtual bool isVisible()=0;
			virtual void validate()=0;
			virtual void setStretch(bool stretchTo)=0;
			virtual bool getStretch()=0;
			virtual void setFit(bool fitTo)=0;
			virtual bool getFit()=0;
			virtual void clearContent()=0;
			virtual void renderImgFile(string serializedImageUrl)=0;
			virtual void renderFrom(ISurface* s)=0;
			virtual void blit(IWindow* src)=0;
			virtual void stretchBlit(IWindow* src)=0;
			virtual void lock()=0;
			virtual void unlock()=0;
			virtual void lockChilds()=0;
			virtual void unlockChilds()=0;
			virtual string getDumpFileUri(int quality, int dumpW, int dumpH)=0;
	};
}
}
}
}
}
}

#endif /*IWINDOW_H_*/
