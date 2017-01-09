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

#ifndef TERMFONTPROVIDER_H_
#define TERMFONTPROVIDER_H_

#include "mb/interface/IFontProvider.h"
#include "mb/interface/ISurface.h"

#include <string.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class TermFontProvider : public IFontProvider {
		private:
			GingaScreenID myScreen;

		public:
			TermFontProvider(
					GingaScreenID screenId,
					const char* fontUri,
					int heightInPixel);

			virtual ~TermFontProvider();

			string getLoadSymbol() {return "TermFontProvider";};
			void* getFontProviderContent();
			void getStringExtents(const char* text, int* w, int* h){};
			int getStringWidth(const char* text, int textLength=0);
			int getHeight();
			void playOver(
					ISurface* surface,
					const char* text,
					int x=0, int y=0, short align=0);

			void playOver(ISurface* surface);
	};
}
}
}
}
}
}

#endif /*TERMFONTPROVIDER_H_*/
