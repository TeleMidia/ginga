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
	};
}
}
}
}
}
}

#endif /*SURFACE_H_*/
