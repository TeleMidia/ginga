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

#ifndef IPROGRAMAV_H_
#define IPROGRAMAV_H_

#include "mb/IMBDefs.h"

#include "IPlayer.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	class IProgramAV {
		public:
			virtual ~IProgramAV(){};
			virtual void release()=0;
			virtual void setAVPid(int programPid, int aPid, int vPid)=0;
			virtual IPlayer* getPlayer(int pid)=0;
			virtual void setPlayer(int pid, IPlayer*)=0;
			virtual void setPropertyValue(string pName, string pValue)=0;
	};
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::player::IPlayer*
		ProgramHandlerCreator(GingaScreenID screenId);

typedef void ProgramHandlerDestroyer(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* ipav);

#endif /*IPROGRAMAV_H_*/
