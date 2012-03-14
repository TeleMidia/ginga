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

#ifndef SDLFONTPROVIDER_H_
#define SDLFONTPROVIDER_H_

#include "util/functions.h"

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "mb/interface/IFontProvider.h"
#include "mb/interface/ISurface.h"

#include <pthread.h>

#include "SDL_ttf.h"

#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class SDLFontProvider : public IFontProvider {
		private:
			GingaScreenID myScreen;
			string dfltFont;
			string fontUri;
			int height;
			TTF_Font* font;
			ISurface* content;
			string plainText;
			int coordX;
			int coordY;
			short align;
			bool fontInit;

			static pthread_mutex_t ntsMutex;
			pthread_mutex_t pMutex;

			bool isWaiting;
			pthread_mutex_t cMutex;
			pthread_cond_t cond;

			static bool initialized;
			static short fontRefs;

		public:
			static const short A_LEFT;
			static const short A_CENTER;
			static const short A_RIGHT;

			static const short A_TOP;
			static const short A_TOP_CENTER;
			static const short A_TOP_LEFT;
			static const short A_TOP_RIGHT;

			static const short A_BOTTOM;
			static const short A_BOTTOM_CENTER;
			static const short A_BOTTOM_LEFT;
			static const short A_BOTTOM_RIGHT;

			SDLFontProvider(
					GingaScreenID screenId,
					const char* fontUri,
					int heightInPixel);

			virtual ~SDLFontProvider();

		private:
			bool initializeFont();

		public:
			string getLoadSymbol() {return "SDLFontProvider";};
			void* getFontProviderContent();

			void getStringExtents(const char* text, int* w, int* h);
			int getStringWidth(const char* text, int textLength=0);
			int getHeight();
			void playOver(
					ISurface* surface,
					const char* text,
					int x=0, int y=0, short align=A_TOP_LEFT);

		private:
			void playOver(ISurface* surface);

		public:
			void ntsPlayOver();

		private:
			void waitNTSRenderer();
			bool ntsRenderer();
	};
}
}
}
}
}
}

#endif /*SDLFONTPROVIDER_H_*/
