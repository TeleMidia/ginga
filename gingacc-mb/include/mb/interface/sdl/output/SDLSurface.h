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

#ifndef SDLSURFACE_H_
#define SDLSURFACE_H_

#include "SDL.h"

#include <pthread.h>

#include "mb/interface/IWindow.h"
#include "mb/interface/sdl/output/SDLWindow.h"
#include "mb/interface/IFontProvider.h"

#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class SDLSurface : public ISurface {
		private:
			GingaScreenID myScreen;
			SDL_Surface* sur;
			SDL_Surface* pending;
			IWindow* parent;
			bool hasExtHandler;
			IColor* chromaColor;
			IColor* borderColor;
			IColor* bgColor;
			IColor* surfaceColor;
			IFontProvider* iFont;
			int caps;
			bool isDeleting;

			vector<DrawData*> drawData;
			pthread_mutex_t ddMutex;

			pthread_mutex_t sMutex;
			pthread_mutex_t pMutex;

		public:
			SDLSurface(GingaScreenID screenId);
			SDLSurface(GingaScreenID screenId, void* underlyingSurface);

			virtual ~SDLSurface();

		private:
			void releasePendingSurface();
			bool createPendingSurface();
			void checkPendingSurface();
			void fill();
			void releaseChromaColor();
			void releaseBgColor();
			void releaseBorderColor();
			void releaseSurfaceColor();

			void releaseFont();
			void releaseDrawData();

			void initialize(GingaScreenID screenId);

		public:
			void takeOwnership();

			SDL_Surface* getPendingSurface();

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

			vector<DrawData*>* createDrawDataList();

		private:
			void pushDrawData(int c1, int c2, int c3, int c4, short type);

		public:
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

		private:
			void initContentSurface();
			SDL_Surface* createSurface();

		public:
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

#endif /*SDLSURFACE_H_*/
