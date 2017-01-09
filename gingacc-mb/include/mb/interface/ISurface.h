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

#ifndef ISURFACE_H_
#define ISURFACE_H_

#include "mb/IMBDefs.h"

#include "util/IColor.h"
using namespace ::br::pucrio::telemidia::util;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class ISurface {
		public:
			virtual ~ISurface(){};

			virtual void setExternalHandler(bool extHandler)=0;
			virtual bool hasExternalHandler()=0;
			virtual void addCaps(int caps)=0;
			virtual void setCaps(int caps)=0;
			virtual int getCap(string cap)=0;
			virtual int getCaps()=0;
			virtual bool setParentWindow(void* parentWindow)=0;
			virtual void* getParentWindow()=0;
			virtual void* getSurfaceContent()=0;
			virtual void setSurfaceContent(void* surface)=0;
			virtual void clearContent()=0;
			virtual void clearSurface()=0;
			virtual void drawLine(int x1, int y1, int x2, int y2)=0;
			virtual void drawRectangle(int x, int y, int w, int h)=0;
			virtual void fillRectangle(int x, int y, int w, int h)=0;
			virtual void drawString(int x, int y, const char* txt)=0;
			virtual void setBorderColor(int r, int g, int b, int alpha)=0;
			virtual IColor* getBorderColor()=0;
			virtual void setBgColor(int r, int g, int b, int alpha)=0;
			virtual IColor* getBgColor()=0;
			virtual void setColor(int r, int g, int b, int alpha)=0;
			virtual IColor* getColor()=0;
			virtual void setChromaColor(int r, int g, int b, int alpha)=0;
			virtual IColor* getChromaColor()=0;
			virtual void setSurfaceFont(void* font)=0;
			virtual void flip()=0;
			virtual void scale(double x, double y)=0;
			virtual void blit(
					int x,
					int y,
					ISurface* src=NULL,
					int srcX=-1, int srcY=-1, int srcW=-1, int srcH=-1)=0;

			virtual void getStringExtents(const char* text, int* w, int* h)=0;
			virtual void setClip(int x, int y, int w, int h)=0;
			virtual void getSize(int* width, int* height)=0;
			virtual string getDumpFileUri()=0;
			virtual void setMatrix(void* matrix)=0;

			virtual GingaSurfaceID getId () const = 0;
			//virtual void setId (const GingaSurfaceID &surId) = 0;
	};
}
}
}
}
}
}

#endif /*SURFACE_H_*/
