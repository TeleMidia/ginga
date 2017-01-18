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

#ifndef IMAGEPLAYER_H_
#define IMAGEPLAYER_H_

#include "mb/IImageProvider.h"
#include "mb/SDLWindow.h"
using namespace ::ginga::mb;

#include "system/GingaLocatorFactory.h"
using namespace ::ginga::system;

#include "util/Color.h"
#include "util/functions.h"
using namespace ::ginga::util;

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "Player.h"


GINGA_PLAYER_BEGIN

	class ImagePlayer : public Player {
		private:
			GingaProviderID provider;

		public:
			ImagePlayer(GingaScreenID screenId, string mrl);
			virtual ~ImagePlayer();
			bool play();
			void stop();
			void resume();

			virtual void setPropertyValue(string name, string value);

		private:
			GingaSurfaceID prepareSurface(GingaProviderID provider, string mrl);
	};

GINGA_PLAYER_END
#endif /*IMAGEPLAYER_H_*/
