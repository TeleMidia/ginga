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

#ifndef IGINGALOCATORFACTORY_H_
#define IGINGALOCATORFACTORY_H_

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace fs {
  class IGingaLocatorFactory {
	public:
		virtual ~IGingaLocatorFactory(){};
		virtual void release()=0;
		virtual void createLocator(string providerUri, string ior)=0;
		virtual string getLocation(string ior)=0;
		virtual string getName(string ior)=0;
		virtual void addClientLocation(
				string ior, string clientUri, string name)=0;

		virtual void waitNewLocatorCondition()=0;
		virtual bool newLocatorConditionSatisfied()=0;
  };
}
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::system::fs::IGingaLocatorFactory*
		GingaLocatorFactoryCreator();

typedef void GingaLocatorFactoryDestroyer(
		::br::pucrio::telemidia::ginga::core::system::fs::
				IGingaLocatorFactory* glf);

#endif /*IGINGALOCATORFACTORY_H_*/
