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

#ifndef _IInteractiveChannel_H_
#define _IInteractiveChannel_H_

#include <stdio.h>

#include "IInteractiveChannelListener.h"

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace ic {
  class IInteractiveChannel {
	public:
		virtual ~IInteractiveChannel(){};
		virtual bool hasConnection()=0;
		virtual void setSourceTarget(string url)=0;
		virtual void setTarget(FILE* fd)=0;
		virtual void setTarget(char* buffer)=0;
		virtual short getType()=0;
		virtual float getRate()=0;
		virtual void setListener(IInteractiveChannelListener* listener)=0;
		virtual bool reserveUrl(
				string uri,
				IInteractiveChannelListener* listener=NULL,
				string userAgent="")=0;

		virtual bool performUrl()=0;
		virtual bool releaseUrl()=0;
  };
}
}
}
}
}
}

#endif /*_IInteractiveChannel_H_*/
