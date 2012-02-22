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

#ifndef IWINDOW_H_
#define IWINDOW_H_

#include "util/IColor.h"
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

			virtual void setBgColor(int r, int g, int b, int alpha)=0;
			virtual IColor* getBgColor()=0;
			virtual void setColorKey(int r, int g, int b)=0;
			virtual IColor* getColorKey()=0;
			virtual void setWindowColor(int r, int g, int b, int alpha)=0;
			virtual IColor* getWindowColor()=0;
			virtual void setBorder(
					int r, int g, int b, int alpha=255, int bWidth=1)=0;

			virtual void getBorder(
					int* r, int* g, int* b, int* alpha, int* bWidth)=0;

			virtual GingaScreenID getScreen()=0;
			virtual void revertContent()=0;
			virtual void setReleaseListener(ISurface* listener)=0;
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
			virtual void setX(int x)=0;
			virtual void setY(int y)=0;
			virtual void setW(int w)=0;
			virtual void setH(int h)=0;
			virtual void* getContent()=0;

			virtual void setGhostWindow(bool ghost)=0;
			virtual bool isVisible()=0;
			virtual void validate()=0;
			virtual void addChildSurface(ISurface* s)=0;
			virtual bool removeChildSurface(ISurface* s)=0;
			virtual void setStretch(bool stretchTo)=0;
			virtual bool getStretch()=0;
			virtual void setFit(bool fitTo)=0;
			virtual bool getFit()=0;
			virtual void clearContent()=0;
			virtual void renderFrom(string serializedImageUrl)=0;
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
