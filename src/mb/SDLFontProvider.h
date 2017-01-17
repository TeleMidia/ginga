/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef SDLFONTPROVIDER_H_
#define SDLFONTPROVIDER_H_

#include "util/functions.h"

#include "system/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "IFontProvider.h"
#include "SDLSurface.h"

#include <pthread.h>

#include "SDL_ttf.h"

#include <set>
#include <map>
using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_CORE_MB_BEGIN

	class SDLFontProvider : public IFontProvider {
		private:
			GingaScreenID myScreen;
			string dfltFont;
			string fontUri;
			int height;
			TTF_Font* font;
			SDLSurface* content;
			string plainText;
			int coordX;
			int coordY;
			short align;
			bool fontInit;

			static map<string, TTF_Font*> fonts;
			static pthread_mutex_t ntsMutex;
			static bool initNTSMutex;

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
			void releaseFonts();
			bool initializeFont();
			bool createFont();

		public:
			string getLoadSymbol() {return "SDLFontProvider";};
			void* getFontProviderContent();

			void getStringExtents(const char* text, int* w, int* h);
			int getStringWidth(const char* text, int textLength=0);
			int getHeight();
			void playOver(
					GingaSurfaceID surface,
					const char* text,
					int x, int y, short align);

		private:
			void playOver(GingaSurfaceID surface);
	};

BR_PUCRIO_TELEMIDIA_GINGA_CORE_MB_END
#endif /*SDLFONTPROVIDER_H_*/
