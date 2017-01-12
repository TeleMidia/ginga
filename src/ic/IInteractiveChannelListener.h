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

#ifndef _IInteractiveChannelListener_H_
#define _IInteractiveChannelListener_H_

#include <stdio.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace ic {
  class IInteractiveChannelListener {
	public:
		virtual ~IInteractiveChannelListener(){};
		virtual void receiveCode(long respCode)=0;
		virtual void receiveDataStream(char* buffer, int size)=0;
		virtual void receiveDataPipe(FILE* fd, int size)=0;
		virtual void downloadCompleted(const char* localUri)=0;
  };
}
}
}
}
}
}

#endif /*_IInteractiveChannelListener_H_*/
