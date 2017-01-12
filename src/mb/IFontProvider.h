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

#ifndef IFONTPROVIDER_H_
#define IFONTPROVIDER_H_

#include "IMBDefs.h"
#include "IDiscreteMediaProvider.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class IFontProvider : public IDiscreteMediaProvider {
		public:
			static const short FP_AUTO_WORDWRAP = -10;

			static const short FP_TA_LEFT   = 0x01;
			static const short FP_TA_CENTER = 0x02;
			static const short FP_TA_RIGHT  = 0x04;

			virtual ~IFontProvider(){};

			virtual string getLoadSymbol()=0;

			virtual void* getFontProviderContent()=0;

			virtual void getStringExtents(const char* text, int* w, int* h)=0;
			virtual int getStringWidth(const char* text, int textLength=0)=0;
			virtual int getHeight()=0;

			virtual void playOver(GingaSurfaceID surface)=0;

			virtual void playOver(
					GingaSurfaceID surface,
					const char* text,
					int x, int y, short align)=0;
	};
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::mb::IFontProvider*
		FontProviderCreator(
				GingaScreenID screenId,
				const char* fontUri,
				int heightInPixel);

typedef void FontProviderDestroyer(
		::br::pucrio::telemidia::ginga::core::mb::IFontProvider* fp);

#endif /*IFONTPROVIDER_H_*/
