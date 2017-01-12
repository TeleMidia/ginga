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

#ifndef _ISystemInfo_H_
#define _ISystemInfo_H_

#include "mb/IMBDefs.h"

#include <map>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace contextmanager {
  class ISystemInfo {
	public:
		virtual ~ISystemInfo(){};
		virtual void setSystemTable(map<string, string>* sysTable)=0;
		virtual string getSystemLanguage()=0;
		virtual string getCaptionLanguage()=0;
		virtual string getSubtitleLanguage()=0;
		virtual float getReturnBitRate()=0;
		virtual void getScreenSize(
				GingaScreenID screenId, int* width, int* height)=0;

		virtual void getScreenGraphicSize(
				GingaScreenID screenId, int* width, int* height)=0;

		virtual string getAudioType()=0;
		virtual float getCPUClock()=0;
		virtual float getMemorySize()=0;
		virtual string getOperatingSystem()=0;
		virtual string getJavaConfiguration()=0;
		virtual string getJavaProfile()=0;
		virtual string getLuaVersion()=0;
  };
}
}
}
}
}
}

#endif /*_ISystemInfo_H_*/
