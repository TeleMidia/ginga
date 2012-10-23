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

#include "interface/IContinuousMediaProvider.h"
#include "interface/IFontProvider.h"
#include "interface/IImageProvider.h"

#include "IInputManager.h"
#include "interface/IInputEvent.h"
#include "interface/IEventBuffer.h"

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

			virtual void setBackgroundImage(
					GingaScreenID screenId, string uri)=0;

			virtual int getDeviceWidth(GingaScreenID screenId)=0;

			virtual int getDeviceHeight(GingaScreenID screenId)=0;

			virtual void* getGfxRoot(GingaScreenID screenId)=0;

			virtual void releaseScreen(GingaScreenID screenId)=0;
			virtual void releaseMB(GingaScreenID screenId)=0;
			virtual void clearWidgetPools(GingaScreenID screenId)=0;

			virtual GingaScreenID createScreen(int numArgs, char** args)=0;
			virtual string getScreenName(GingaScreenID screenId)=0;

			virtual GingaWindowID getScreenUnderlyingWindow(
					GingaScreenID screenId)=0;

			virtual IWindow* getIWindowFromId(
					GingaScreenID screenId, GingaWindowID winId)=0;

			virtual bool mergeIds(
					GingaScreenID screenId,
					GingaWindowID destId,
					vector<GingaWindowID>* srcIds)=0;

			virtual void blitScreen(
					GingaScreenID screenId, ISurface* destination)=0;

			virtual void blitScreen(GingaScreenID screenId, string fileUri)=0;
			virtual void refreshScreen(GingaScreenID screenId)=0;


			/* Interfacing output */

			virtual IWindow* createWindow(
					GingaScreenID screenId,
					int x, int y,
					int w, int h,
					float z)=0;

			virtual GingaWindowID createUnderlyingSubWindow(
					GingaScreenID screenId,
					int x, int y,
					int w, int h,
					float z)=0;

			virtual IWindow* createWindowFrom(
					GingaScreenID screenId, GingaWindowID underlyingWindow)=0;

			virtual bool hasWindow(
					GingaScreenID screenId, IWindow* window)=0;

			virtual void releaseWindow(
					GingaScreenID screenId, IWindow* window)=0;

			virtual ISurface* createSurface(GingaScreenID screenId)=0;

			virtual ISurface* createSurface(
					GingaScreenID screenId, int w, int h)=0;

			virtual ISurface* createSurfaceFrom(
					GingaScreenID screenId, void* underlyingSurface)=0;

			virtual bool hasSurface(
					GingaScreenID screenId, ISurface* sur)=0;

			virtual bool releaseSurface(
					GingaScreenID screenId, ISurface* sur)=0;


			/* Interfacing content */

			virtual IContinuousMediaProvider* createContinuousMediaProvider(
					GingaScreenID screenId,
					const char* mrl,
					bool* hasVisual,
					bool isRemote)=0;

			virtual void releaseContinuousMediaProvider(
					GingaScreenID screenId,
					IContinuousMediaProvider* provider)=0;

			virtual IFontProvider* createFontProvider(
					GingaScreenID screenId,
					const char* mrl,
					int fontSize)=0;

			virtual void releaseFontProvider(
					GingaScreenID screenId, IFontProvider* provider)=0;

			virtual IImageProvider* createImageProvider(
					GingaScreenID screenId, const char* mrl)=0;

			virtual void releaseImageProvider(
					GingaScreenID screenId, IImageProvider* provider)=0;

			virtual ISurface* createRenderedSurfaceFromImageFile(
					GingaScreenID screenId, const char* mrl)=0;


			/* Interfacing input */
			virtual IInputManager* getInputManager(GingaScreenID screenId)=0;
			virtual IEventBuffer* createEventBuffer(GingaScreenID screenId)=0;

			virtual IInputEvent* createInputEvent(
					GingaScreenID screenId, void* event, const int symbol)=0;

			virtual IInputEvent* createApplicationEvent(
					GingaScreenID screenId, int type, void* data)=0;

			virtual int fromMBToGinga(GingaScreenID screenId, int keyCode)=0;
			virtual int fromGingaToMB(GingaScreenID screenId, int keyCode)=0;
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
