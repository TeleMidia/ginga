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

#ifndef DFBDEVICESCREEN_H_
#define DFBDEVICESCREEN_H_

#include "config.h"

#if HAVE_COMPSUPPORT
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#endif

#include "mb/interface/IDeviceScreen.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <directfb.h>
#ifdef __cplusplus
}
#endif

/* macro for a safe call to DirectFB functions */
#ifndef DFBCHECK
#define DFBCHECK(x...)                                            \
{                                                                 \
	DFBResult err = x;                                            \
	if (err != DFB_OK) {                                          \
		fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ );    \
	}                                                             \
}
#endif /*DFBCHECK*/

#include <pthread.h>

#include <set>
#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class DFBDeviceScreen : public IDeviceScreen {
		public:
			static const unsigned int DSA_UNKNOWN;
			static const unsigned int DSA_4x3;
			static const unsigned int DSA_16x9;

		protected:
			unsigned int aspect;
			unsigned int hSize;
			unsigned int vSize;
			unsigned int hRes;
			unsigned int wRes;

		private:
#if HAVE_COMPSUPPORT
			static IComponentManager* cm;
#endif
			set<IWindow*>* windowPool;
			set<ISurface*>* surfacePool;
			GingaScreenID id;
			IInputManager* im;

			pthread_mutex_t winMutex;
			pthread_mutex_t surMutex;

			static unsigned int numOfDFBScreens;
			static IDirectFB* dfb;
			static IDirectFBDisplayLayer* gfxLayer;

			static pthread_mutex_t ieMutex;
			static map<int, int>* gingaToDFBCodeMap;
			static map<int, int>* dfbToGingaCodeMap;

		public:
			DFBDeviceScreen(
					int numArgs, char** args,
					GingaScreenID myId, GingaWindowID parentId);

			virtual ~DFBDeviceScreen();

			void releaseScreen();
			void releaseMB();

			void clearWidgetPools();

			void setParentScreen(GingaWindowID parentId);

			void setBackgroundImage(string uri);

			unsigned int getWidthResolution();
			void setWidthResolution(unsigned int wRes);
			unsigned int getHeightResolution();
			void setHeightResolution(unsigned int hRes);

			void setColorKey(int r, int g, int b);

			void mergeIds(GingaWindowID destId, vector<GingaWindowID>* srcIds);


			/* interfacing output */

			IWindow* createWindow(int x, int y, int w, int h);
			IWindow* createWindowFrom(GingaWindowID underlyingWindow);
			void releaseWindow(IWindow* win);

			ISurface* createSurface();
			ISurface* createSurface(int w, int h);
			ISurface* createSurfaceFrom(void* underlyingSurface);
			void releaseSurface(ISurface* sur);


			/* interfacing content */

			IContinuousMediaProvider* createContinuousMediaProvider(
					const char* mrl, bool hasVisual, bool isRemote);

			void releaseContinuousMediaProvider(
					IContinuousMediaProvider* provider);

			IFontProvider* createFontProvider(
					const char* mrl, int fontSize);

			void releaseFontProvider(IFontProvider* provider);

			IImageProvider* createImageProvider(const char* mrl);
			void releaseImageProvider(IImageProvider* provider);

			ISurface* createRenderedSurfaceFromImageFile(const char* mrl);


			/* interfacing input */

			IInputManager* getInputManager();
			void setInputManager(IInputManager* im);

			IEventBuffer* createEventBuffer();
			IInputEvent* createInputEvent(void* event, const int symbol);
			IInputEvent* createUserEvent(int type, void* data);
			int fromMBToGinga(int keyCode);
			int fromGingaToMB(int keyCode);

			/* interfacing underlying multimedia system */

			void* getGfxRoot();


			/* libgingaccmbdfb internal use*/

			/* input */
			static void initCodeMaps();

			/* output */
			static IDirectFBWindow* getUnderlyingWindow(GingaWindowID winId);

			static IDirectFBWindow* createUnderlyingWindow(
					DFBWindowDescription* desc);

			static void releaseUnderlyingWindow(IDirectFBWindow* uWin);

			static IDirectFBSurface* createUnderlyingSurface(
					DFBSurfaceDescription* desc);

			static void releaseUnderlyingSurface(IDirectFBSurface* uSur);
	};
}
}
}
}
}
}

#endif /*DFBDEVICESCREEN_H_*/
