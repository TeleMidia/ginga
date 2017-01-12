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

#ifndef _IPRESENTATIONCONTEXT_H_
#define _IPRESENTATIONCONTEXT_H_

#include "ctxmgmt/IContextListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::contextmanager;

#if HAVE_MULTIDEVICE
#include "multidevice/IDeviceDomain.h"
using namespace ::br::pucrio::telemidia::ginga::core::multidevice;
#endif

#include <set>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adaptation {
namespace context {
  class IPresentationContext {
	public:
		virtual ~IPresentationContext(){};

		virtual void setPropertyValue(string propertyName, string value)=0;
		virtual void incPropertyValue(string propertyName)=0;
		virtual void decPropertyValue(string propertyName)=0;
		virtual set<string>* getPropertyNames()=0;
		virtual string getPropertyValue(string attributeId)=0;
		virtual void save()=0;
		virtual void setGlobalVarListener(IContextListener* listener)=0;
		virtual void receiveGlobalAttribution(
				string propertyName, string value)=0;

#if HAVE_MULTIDEVICE
		virtual void setRemoteDeviceListener(IRemoteDeviceListener* rdl)=0;
#endif
  };
}
}
}
}
}
}
}

#endif //_IPRESENTATIONCONTEXT_H_
