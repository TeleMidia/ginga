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

#ifndef SDLIMAGEPROVIDER_H_
#define SDLIMAGEPROVIDER_H_

#include "IImageProvider.h"


#include "SDL_image.h"


GINGA_MB_BEGIN

	class SDLImageProvider : public IImageProvider {
		private:
			static bool mutexInit;
			static bool initialized;
			static short imageRefs;

			static pthread_mutex_t pMutex;

			GingaScreenID myScreen;
			string imgUri;

		public:
			SDLImageProvider(GingaScreenID screenId, const char* mrl);
			virtual ~SDLImageProvider();

			string getLoadSymbol() {return "SDLImageProvider";};

			void playOver(GingaSurfaceID surface);

			bool releaseAll();
	};

GINGA_MB_END
#endif /*SDLIMAGEPROVIDER_H_*/
