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

#ifndef IOHANDLER_H_
#define IOHANDLER_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "interface/output/Window.h"
#include "interface/output/Surface.h"
#include "interface/content/FontProvider.h"
#include "interface/content/VideoProvider.h"
#include "interface/content/ImageProvider.h"
#include "interface/input/InputEvent.h"
#include "interface/input/InputEventBuffer.h"

#include <iostream>
#include <limits>
#include <string>
#include <set>
using namespace std;

#include <pthread.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	class IOHandler {
		private:
			static bool busy;
			static pthread_cond_t* busyCond;
			static pthread_mutex_t* busyCondLocker;
			static void check(string functionSource="");

		public:
			/*static void clearWidgetsPools();
			static void initializeGraphics(int numArgs, char* args[]);
			static void releaseGraphics();
			static void setLayerColorKey(int r, int g, int b);

			static void* createWindow(void* windowDescription);
			static void releaseWindow(Window* win);
			static void* createSurface(void* surfaceDescription);
			static void releaseSurface(Surface* sur);*/

			static void* createInputEventBuffer();
			static void releaseInputBuffer(InputEventBuffer* buffer);

			static void* createVideoProvider(char* mrl);
			static void releaseVideoProvider(ContinuousMediaProvider* provider);
			static void* createImageProvider(char* mrl);
			static void releaseImageProvider(ImageProvider* provider);
			static void* createFont(char* fontUri, int heightInPixel);
			static void releaseFont(FontProvider* font);
			//static void* getGraphicsRoot();
	};
}
}
}
}
}
}
}

#endif /*IOHandler_H_*/
