/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

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
#include "mb/IWindow.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "system/fs/IGingaLocatorFactory.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::fs;

#include "util/Color.h"
#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "Player.h"

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
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
}
}
}
}
}
}

#endif /*IMAGEPLAYER_H_*/
