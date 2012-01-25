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

#ifndef IDEVICESCREEN_H_
#define IDEVICESCREEN_H_

#include <vector>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class IDeviceScreen {
		public:
			virtual ~IDeviceScreen(){};
			virtual void setParentDevice(void* devId)=0;
			virtual void setBackgroundImage(string uri)=0;
			virtual unsigned int getWidthResolution()=0;
			virtual void setWidthResolution(unsigned int wRes)=0;
			virtual unsigned int getHeightResolution()=0;
			virtual void setHeightResolution(unsigned int hRes)=0;
			virtual void setColorKey(int r, int g, int b)=0;
			virtual void mergeIds(int destId, vector<int>* srcIds)=0;
			virtual void* getWindow(int winId)=0;
			virtual void* createWindow(void* desc)=0;
			virtual void releaseWindow(void* win)=0;
			virtual void* createSurface(void* desc)=0;
			virtual void releaseSurface(void* sur)=0;
			virtual void* getGfxRoot()=0;
	};
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::mb::IDeviceScreen*
		ScreenCreator(int numArgs, char** args, void* parentId);

typedef void ScreenDestroyer(
		::br::pucrio::telemidia::ginga::core::mb::IDeviceScreen* ds);

#endif /*IDEVICESCREEN_H_*/
