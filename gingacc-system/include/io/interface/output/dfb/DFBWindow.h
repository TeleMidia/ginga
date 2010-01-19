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

#ifndef DFBWINDOW_H_
#define DFBWINDOW_H_

#include "util/IColor.h"
using namespace ::br::pucrio::telemidia::util;

#include "../IWindow.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <directfb.h>
#ifdef __cplusplus
}
#endif

#include <iostream>
#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	class DFBWindow : public IWindow {
		private:
			IDirectFBWindow* win;
			IDirectFBSurface* winSur;

			int x;
			int y;
			int width;
			int height;
			int r;
			int g;
			int b;
			int alpha;
			static int maxTransparencyValue;
			int transparencyValue;
			bool visible;
			bool ghost;

		    vector<ISurface*>* childSurfaces;
		    ISurface* releaseListener;
		    bool fit;
		    bool stretch;
		    int caps;

		    pthread_mutex_t mutex;
		    pthread_mutex_t mutexC;

		public:
			DFBWindow(int x, int y, int width, int height);
			virtual ~DFBWindow();

			void setMaxTransparencyValue(int maxValue);
			void setReleaseListener(ISurface* listener);
			int getCap(string cap);
			void setCaps(int caps);
			void addCaps(int capability);
			int getCaps();

			void draw();
			void setBounds(int x, int y, int width, int height);
			void setBackgroundColor(int r, int g, int b, int alpha);
			IColor* getBgColor();
			void setColorKey(int r, int g, int b);
			void moveTo(int x, int y);
			void resize(int width, int height);
			void raise();
			void lower();
			void raiseToTop();
			void lowerToBottom();
			void setCurrentTransparency(int alpha);
			void setOpaqueRegion(int x1, int y1, int x2, int y2);
			void setTransparencyValue(int alpha);
			int getTransparencyValue();
			void show();
			void hide();
			int getX();
			int getY();
			int getW();
			int getH();
			void* getContent();
			void setColor(int r, int g, int b, int alpha=255);
			void setBorder(int r, int g, int b, int alpha=255, int bWidth=1);
			void setBorder(IColor* color, int bWidth=1);
			void setGhostWindow(bool ghost);
			bool isVisible();
			void validate();
			void addChildSurface(ISurface* s);
			bool removeChildSurface(ISurface* s);
			void setStretch(bool stretchTo);
			bool getStretch();
			void setFit(bool fitTo);
			bool getFit();
			void clearContent();

		private:
			bool isMine(ISurface* surface);

		public:
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
}

#endif /*WINDOW_H_*/
