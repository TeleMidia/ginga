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

#ifndef IChannel_H_
#define IChannel_H_

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
	class IChannel {
		public:
			virtual ~IChannel(){};
			virtual short getId()=0;
			virtual void setId(short id)=0;
			virtual string getName()=0;
			virtual void setName(string name)=0;
			virtual unsigned int getFrequency()=0;
			virtual void setFrequency(unsigned int freq)=0;
			virtual bool isFullSeg()=0;
			virtual void setSegment(bool isFullSeg)=0;
	};
}
}
}
}
}
}

#endif /*IChannel_H_*/
