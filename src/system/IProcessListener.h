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

#ifndef __IProcessListener_h__
#define __IProcessListener_h__

#include <errno.h>
#include <sys/time.h>
#include <pthread.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace process {
  class IProcessListener {
	public:
		static const short PST_EXIT_OK     = 0;
		static const short PST_EXIT_ERROR  = 1;
		static const short PST_EXEC_SIGNAL = 2;

		virtual ~IProcessListener(){};
		virtual void receiveProcessSignal(int sigType, int pSig, int ppid)=0;
  };
}
}
}
}
}
}
}

#endif //__IProcessListener_h__
