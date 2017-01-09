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

#ifndef ICONTINUOUSMEDIAPROVIDER_H_
#define ICONTINUOUSMEDIAPROVIDER_H_

#include "ISurface.h"

#include "IProviderListener.h"
#include "IMediaProvider.h"

extern "C" {
#include <stdint.h>
}

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class IContinuousMediaProvider : public IMediaProvider {
		public:
			virtual ~IContinuousMediaProvider(){};
			virtual bool getHasVisual()=0;
			virtual void setLoadSymbol(string symbol)=0;
			virtual string getLoadSymbol()=0;
			virtual void setAVPid(int aPid, int vPid)=0;
			virtual void feedBuffers()=0;
			virtual bool checkVideoResizeEvent(GingaSurfaceID frame)=0;
			virtual double getTotalMediaTime()=0;
			virtual int64_t getVPts()=0;
			virtual double getMediaTime()=0;
			virtual void setMediaTime(double pos)=0;
			virtual void playOver(GingaSurfaceID surface)=0;
			virtual void resume(GingaSurfaceID surface)=0;
			virtual void pause()=0;
			virtual void stop()=0;
			virtual void setSoundLevel(float level)=0;
			virtual float getSoundLevel()=0;
			virtual void setProviderContent(void* content)=0;
			virtual void* getProviderContent()=0;
			virtual void getOriginalResolution(int* width, int* height)=0;
			virtual bool releaseAll()=0;
			virtual void refreshDR(void* data)=0;
	};
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::mb::IContinuousMediaProvider*
		CMPCreator(GingaScreenID screenId, const char* mrl);

typedef void CMPDestroyer(::br::pucrio::telemidia::ginga::core::mb::
		IContinuousMediaProvider* cmp);

#endif /*ICONTINUOUSMEDIAPROVIDER_H_*/
