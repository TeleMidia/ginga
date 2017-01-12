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

#ifndef _ITUNER_H_
#define _ITUNER_H_

#include "mb/IMBDefs.h"

#ifndef BUFFSIZE
#define BUFFSIZE 9588
#endif //BUFFSIZE

#include "ITunerListener.h"
#include "INetworkInterface.h"

#include <string>
using namespace std;

typedef struct {
	char* buff;
	unsigned int size;
} Buffer;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
	class ITuner {
		public:
			virtual ~ITuner(){};
			virtual void setLoopListener(ITunerListener* loopListener)=0;
			virtual INetworkInterface* getCurrentInterface()=0;
			virtual void channelUp()=0;
			virtual void channelDown()=0;
			virtual void changeChannel(int factor)=0;
			virtual void setSpec(string ni, string ch)=0;
			virtual void tune()=0;
			virtual void setTunerListener(ITunerListener* listener)=0;
			virtual bool hasSignal()=0;
	};
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::tuning::ITuner* TunerCreator(
		GingaScreenID screenId);

typedef void TunerDestroyer(
		::br::pucrio::telemidia::ginga::core::tuning::ITuner*);

#endif //_ITUNER_H_
