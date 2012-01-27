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

#ifndef ILocalScreenManager_H_
#define ILocalScreenManager_H_

#include "IMBDefs.h"
#include "interface/ISurface.h"

#include <vector>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class ILocalScreenManager {
		public:
			virtual ~ILocalScreenManager(){};

			virtual void releaseHandler()=0;

			virtual void setParentScreen(
					GingaScreenID screenId, GingaWindowID parentId)=0;

			virtual void setBackgroundImage(
					GingaScreenID screenId, string uri)=0;

			virtual int getDeviceWidth(GingaScreenID screenId)=0;

			virtual int getDeviceHeight(GingaScreenID screenId)=0;

			virtual void* getGfxRoot(GingaScreenID screenId)=0;

			virtual void clearWidgetPools(GingaScreenID screenId)=0;

			virtual unsigned long createScreen(int numArgs, char** args)=0;

			virtual void mergeIds(
					GingaScreenID screenId,
					GingaWindowID destId,
					vector<GingaWindowID>* srcIds)=0;

			virtual void* getWindow(
					GingaScreenID screenId, GingaWindowID windowId)=0;

			virtual void* createWindow(
					GingaScreenID screenId, void* windowDesc)=0;

			virtual void releaseWindow(GingaScreenID screenId, void* window)=0;

			virtual void* createSurface(
					GingaScreenID screenId, void* surfaceDesc)=0;

			virtual void releaseSurface(GingaScreenID screenId, void* sur)=0;
	};
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::mb::ILocalScreenManager*
		LocalScreenManagerCreator();

typedef void LocalScreenManagerDestroyer(
		::br::pucrio::telemidia::ginga::core::mb::
				ILocalScreenManager* dm);

#endif /*ILocalScreenManager_H_*/
