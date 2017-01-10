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

#ifndef ISHOWBUTTON_H_
#define ISHOWBUTTON_H_

#include "mb/IMBDefs.h"
#include <string>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	class IShowButton {
		public:
			virtual ~IShowButton(){};
			virtual void initializeWindow()=0;
			virtual void stop()=0;
			virtual void pause()=0;
			virtual void resume()=0;

		private:
			virtual void render(::std::string mrl)=0;
	};
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::player::IShowButton*
         WidgetCreator(GingaScreenID screenId);

typedef void WidgetDestroyer(
		::br::pucrio::telemidia::ginga::core::player::IShowButton*
		deadInterface);

#endif /*IShowButton_H_*/
