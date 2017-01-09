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

#ifndef DFBSURFACE_H_
#define DFBSURFACE_H_

#include "mb/interface/IWindow.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class TermSurface : public ISurface {
		private:
			void* sur;
			GingaScreenID myScreen;
			IWindow* parent;
			bool hasExtHandler;
			IColor* chromaColor;
			int caps;

		public:
			TermSurface(GingaScreenID screenId);
			TermSurface(GingaScreenID screenId, int w, int h);
			TermSurface(GingaScreenID screenId, void* underlyingSurface);

			virtual ~TermSurface();

		private:
			void initialize(GingaScreenID screenId);

		public:
			void setExternalHandler(bool extHandler);
			bool hasExternalHandler();

			void addCaps(int caps);
			void setCaps(int caps);
			int getCap(string cap);
			int getCaps();
			bool setParentWindow(void* parentWindow); //IWindow
			void* getParentWindow();                  //IWindow
			void* getSurfaceContent();
			void setSurfaceContent(void* surface);
			void clearContent();
			void clearSurface();
			void drawLine(int x1, int y1, int x2, int y2);
			void drawRectangle(int x, int y, int w, int h);
			void fillRectangle(int x, int y, int w, int h);
			void drawString(int x, int y, const char* txt);
			void setChromaColor(int r, int g, int b, int alpha);
			IColor* getChromaColor();
			void setBorderColor(int r, int g, int b, int alpha);
			IColor* getBorderColor();
			void setBgColor(int r, int g, int b, int alpha);
			IColor* getBgColor();
			void setColor(int r, int g, int b, int alpha);
			IColor* getColor();
			void setSurfaceFont(void* font);
			void getStringExtents(const char* text, int* w, int* h);
			void flip();
			void scale(double x, double y);
			void blit(
					int x,
					int y,
					ISurface* src=NULL,
					int srcX=-1, int srcY=-1, int srcW=-1, int srcH=-1);

			void setClip(int x, int y, int w, int h);
			void getSize(int* width, int* height);
			string getDumpFileUri();
			void setMatrix(void* matrix);
	};
}
}
}
}
}
}

#endif /*DFBSURFACE_H_*/
