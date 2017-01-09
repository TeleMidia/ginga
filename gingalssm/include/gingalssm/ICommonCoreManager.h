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

#ifndef _ICOMMONCOREMANAGER_H_
#define _ICOMMONCOREMANAGER_H_

#include "system/IComponentInstance.h"
using namespace ::br::pucrio::telemidia::ginga::core::system;

#include "IPresentationEngineManager.h"

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace lssm {
  class ICommonCoreManager : public IComponentInstance {
	public:
		virtual ~ICommonCoreManager(){};

		virtual void addPEM(
				IPresentationEngineManager* pem, GingaScreenID screenId)=0;

		virtual void initializeInstance(std::string& data, short scenario)=0;
		virtual void testInstance(std::string& data, short scenario)=0;

		virtual void enableNPTPrinter(bool enableNPTPrinter)=0;
		virtual void setOCDelay(double ocDelay)=0;
		virtual void setInteractivityInfo(bool hasOCSupport)=0;
		virtual void removeOCFilterAfterMount(bool removeIt)=0;
		virtual void setTunerSpec(string tunerSpec)=0;
		virtual void tune()=0;
		virtual void startPresentation()=0;
  };
}
}
}
}
}

#endif /*_ICOMMONCOREMANAGER_H_*/
