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

/*
 * CAUTION: Graphics may be included only in cpp files and header files that
 * are not provided to others libs.
 */

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if HAVE_DIRECTFB
#include "directfb.h"
#endif

#if HAVE_FUSIONSOUND
#include "fusionsound/fusionsound.h"
#endif

#ifdef __cplusplus
}
#endif

#if HAVE_DIRECTFB
/* macro for a safe call to DirectFB functions */
#ifndef DFBCHECK
#define DFBCHECK(x...)                                            \
{                                                                 \
	DFBResult err = x;                                            \
	if (err != DFB_OK) {                                          \
		fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ );    \
		DirectFBErrorFatal( #x, err );                            \
	}                                                             \
}
#endif /*DFBCHECK*/

#include <directfb_strings.h>
static const DirectFBScreenCapabilitiesNames(screen_caps);
static const DirectFBDisplayLayerCapabilitiesNames(layer_caps);
static const DirectFBDisplayLayerTypeFlagsNames(layer_types);
#endif /*HAVE_DIRECTFB*/

#include <set>
#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace io {
	class Graphics {
#if HAVE_DIRECTFB
		private:
			static set<IDirectFBWindow*>* windowPool;
			static set<IDirectFBSurface*>* surfacePool;

			static IDirectFB* dfb;
			static IDirectFBDisplayLayer* gfxLayer;
#endif /* HAVE_DIRECTFB */
#if HAVE_FUSIONSOUND
			static IFusionSound* sound;
#endif
			static int screenWidth;
			static int screenHeight;
			static const string version;

		public:
			static bool isInitialized();
			static void releasePoolObjects();
			static void initialize();
			static void release();
			static int getDeviceWidth();
			static int getDeviceHeight();
			static void setLayerColorKey(int r, int g, int b);
			static void* createWindow(void* desc);
			static void releaseWindow(void* win);
			static void* createSurface(void* desc);
			static void releaseSurface(void* sur);
			static void* getRoot();
			static void* getFSRoot();
	};
}
}
}
}
}
}

#endif /*GRAPHICS_H_*/
