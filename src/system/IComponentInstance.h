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

#ifndef _IComponentInstance_H_
#define _IComponentInstance_H_

#include <string>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
	class IComponentInstance {
		protected:
			static const unsigned short SC_BESTCASE  = 0;
			static const unsigned short SC_AVGCASE   = 1;
			static const unsigned short SC_WORSTCASE = 2;

		public:
			virtual ~IComponentInstance(){};

			/* each interface can use its own initilization method */
			/* these are for profilers and testers implementation  */
			virtual void initializeInstance(std::string& data, short scenario)=0;
			virtual void testInstance(std::string& data, short scenario)=0;
	};

typedef IComponentInstance* CICreator();

}
}
}
}
}
}

#endif //_IComponentInstance_H_
