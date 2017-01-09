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

#ifndef TERMIMAGEPROVIDER_H_
#define TERMIMAGEPROVIDER_H_

#include "mb/interface/IImageProvider.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class TermImageProvider : public IImageProvider {
		private:
			GingaScreenID myScreen;

		public:
			TermImageProvider(GingaScreenID screenId, const char* mrl);
			virtual ~TermImageProvider();

			string getLoadSymbol() {return "TermImageProvider";};
			void playOver(ISurface* surface);
			bool releaseAll();
	};
}
}
}
}
}
}

#endif /*TERMIMAGEPROVIDER_H_*/
