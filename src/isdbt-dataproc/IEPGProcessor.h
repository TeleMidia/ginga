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

#ifndef IEPGPROCESSOR_H_
#define IEPGPROCESSOR_H_

#include "isdbt-tsparser/ITransportSection.h"
using namespace br::pucrio::telemidia::ginga::core::tsparser;

#include "IEPGListener.h"


namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace epg {
	class IEPGProcessor {
		public:
			virtual ~IEPGProcessor(){};
			virtual void decodeSdtSection(ITransportSection* section)=0;
			virtual void decodeEitSection(ITransportSection* section)=0;
			//virtual void addEPGListener(IEPGListener* listener,
				//	struct tRequest* request)=0;
			virtual void decodeTot(ITransportSection* section)=0;
			virtual void addEPGListener(IEPGListener* listener,
					string request, unsigned char type)=0;


			//virtual void removeEPGListener(IEPGListener * listener)=0;;
			//virtual static IEPGProcessor* getInstance()=0;

	};
}
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::dataprocessing::epg::IEPGProcessor*
		epgpCreator();

typedef void epgpDestroyer(
		::br::pucrio::telemidia::ginga::core::dataprocessing::epg::IEPGProcessor* epgProcessor);



#endif /* IEPGPROCESSOR_H_ */
