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

#ifndef _IInteractiveChannelManager_H_
#define _IInteractiveChannelManager_H_

#include "IInteractiveChannel.h"

#include <set>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace ic {
  class IInteractiveChannelManager {
	public:
		virtual ~IInteractiveChannelManager(){};
		virtual bool hasInteractiveChannel()=0;
		virtual set<IInteractiveChannel*>* getInteractiveChannels()=0;
		virtual IInteractiveChannel* createInteractiveChannel(
				string remoteUri)=0;

		virtual void clearInteractiveChannelManager()=0;
		virtual void releaseInteractiveChannel(IInteractiveChannel* ic)=0;
		virtual IInteractiveChannel* getInteractiveChannel(string remoteUri)=0;
		virtual void performPendingUrls()=0;
  };
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::ic::IInteractiveChannelManager*
		ICMCreator();

typedef void ICMDestroyer(
		::br::pucrio::telemidia::ginga::core::ic::
		IInteractiveChannelManager* icm);

#endif /*_IInteractiveChannelManager_H_*/
