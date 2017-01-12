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

#ifndef IIMAGEPROVIDER_H_
#define IIMAGEPROVIDER_H_

#include "IDiscreteMediaProvider.h"

#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class IImageProvider : public IDiscreteMediaProvider {
		public:
			virtual ~IImageProvider(){};

			virtual string getLoadSymbol()=0;
			virtual void playOver(GingaSurfaceID surface)=0;
			virtual bool releaseAll()=0;
	};
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::mb::IImageProvider*
		ImageProviderCreator(GingaScreenID screenId, const char* mrl);

typedef void ImageProviderDestroyer(
		::br::pucrio::telemidia::ginga::core::mb::IImageProvider* ip);

#endif /*IIMAGEPROVIDER_H_*/
