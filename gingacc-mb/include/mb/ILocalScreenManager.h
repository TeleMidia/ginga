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

#include "interface/IWindow.h"
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
			/* Ginga defining its Multimedia Backend System Types (GMBST)     */
			                                     /* System Description  String*/
			static const short GMBST_DFLT   = 0; /* Default system:      dflt */
			static const short GMBST_DFB    = 1; /* DirectFB:            dfb  */
			static const short GMBST_DX     = 2; /* DirectX:             dx   */
			static const short GMBST_TERM   = 3; /* Teminal:             term */
			static const short GMBST_SDL    = 4; /* SDL:                 sdl  */

			/* Ginga defining its Multimedia Backend SubSystem Types (GMBSST) */
			                                     /* System Description  String*/
			static const short GMBSST_DFLT  = 0; /* Default subsystem:  dflt  */
			static const short GMBSST_FBDEV = 1; /* Frame buffer:       fbdev */
			static const short GMBSST_X11   = 2; /* X11:                x11   */
			static const short GMBSST_HWND  = 3; /* MS-W Window Handle: hwnd  */
			static const short GMBSST_SDL   = 4; /* SDL:                sdl   */

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

			virtual GingaScreenID createScreen(int numArgs, char** args)=0;

			virtual void mergeIds(
					GingaScreenID screenId,
					GingaWindowID destId,
					vector<GingaWindowID>* srcIds)=0;

			virtual IWindow* createWindow(
					GingaScreenID screenId, int x, int y, int w, int h)=0;

			virtual IWindow* createWindowFrom(
					GingaScreenID screenId, GingaWindowID underlyingWindow)=0;

			virtual void releaseWindow(
					GingaScreenID screenId, IWindow* window)=0;

			virtual ISurface* createSurface(GingaScreenID screenId)=0;

			virtual ISurface* createSurface(
					GingaScreenID screenId, int w, int h)=0;

			virtual ISurface* createSurfaceFrom(
					GingaScreenID screenId, void* underlyingSurface)=0;

			virtual void releaseSurface(
					GingaScreenID screenId, ISurface* sur)=0;
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
