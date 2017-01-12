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

#ifndef Channel_H_
#define Channel_H_

#include "IChannel.h"

#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
	class Channel : public IChannel {
		private:
			short id;
			string name;
			unsigned int frequency;
			bool isFullSegment;

		public:
			Channel();
			~Channel();
			short getId();
			void setId(short id);
			string getName();
			void setName(string name);
			unsigned int getFrequency();
			void setFrequency(unsigned int freq);
			bool isFullSeg();
			void setSegment(bool isFullSeg);
	};
}
}
}
}
}
}

#endif /*Channel_H_*/
