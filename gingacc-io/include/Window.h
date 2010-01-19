/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware 
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob 
os termos da Licen�a Publica Geral GNU versao 2 conforme publicada pela Free 
Software Foundation.

Este programa eh distribu�do na expectativa de que seja util, porem, SEM 
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

#ifndef WINDOW_H_
#define WINDOW_H_

#include "util/Color.h"
using namespace ::br::pucrio::telemidia::util;

#include "Surface.h"

#include <iostream>
#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace io {
	class Window : IOContainer {
		private:
			void* win; //WindowGraphicsRoot

			int x;
			int y;
			int width;
			int height;
			int r;
			int g;
			int b;
			int transparencyValue;
			bool visible;

		    vector<Surface*>* childSurfaces;
		    bool fit;
		    bool stretch;
		    int caps;

		    pthread_mutex_t mutex;
		    pthread_mutex_t mutexC;

		public:
			static const int CAPS_NONE;
			static const int CAPS_NOSTRUCTURE;
			static const int CAPS_ALPHACHANNEL;
			static const int CAPS_ALL;

			Window(int x, int y, int width, int height);
			~Window();

			void setCaps(int caps);
			void addCaps(int capability);
			int getCaps();

			void draw();
			void setBounds(int x, int y, int width, int height);
			void setBackgroundColor(int r, int g, int b);
			Color* getBgColor();
			void setColorKey(int r, int g, int b);
			void setOpacity(int alpha);

			void moveTo(int x, int y);
			void resize(int width, int height);
			void raise();
			void lower();
			void raiseToTop();
			void lowerToBottom();
			void setCurrentTransparency(int alpha);
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
			void setBorder(Color* color, int bWidth=1);
			bool isVisible();
			void validate();
			void addChildSurface(Surface* s);
			bool removeChildSurface(Surface* s);
			void setStretch(bool stretchTo);
			bool getStretch();
			void setFit(bool fitTo);
			bool getFit();
			void clear();
			void renderFrom(Surface* s);
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

#endif /*WINDOW_H_*/
