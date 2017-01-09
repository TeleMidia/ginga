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

#ifndef IDATAPROCESSOR_H_
#define IDATAPROCESSOR_H_

#include "system/time/ITimeBaseProvider.h"
using namespace br::pucrio::telemidia::ginga::core::system::time;

#include "tuner/providers/ISTCProvider.h"
using namespace br::pucrio::telemidia::ginga::core::tuning;

#include "tsparser/IDemuxer.h"
#include "tsparser/IFilterListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;

#include "dsmcc/IStreamEventListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing;

#include "dsmcc/carousel/object/IObjectListener.h"
#include "dsmcc/carousel/IServiceDomainListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing::carousel;

#include "IEPGListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing::epg;

#include <map>
#include <set>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
  class IDataProcessor : public IFilterListener, public ITunerListener {
	public:
		virtual ~IDataProcessor(){};

		virtual void setNptPrinter(bool nptPrinter)=0;
		virtual void setDemuxer(IDemuxer* demux)=0;
		virtual void removeOCFilterAfterMount(bool removeIt)=0;

		virtual void setSTCProvider(ISTCProvider* stcProvider)=0;
		virtual ITimeBaseProvider* getNPTProvider()=0;
		virtual void createStreamTypeSectionFilter(short streamType)=0;

		virtual void createPidSectionFilter(int pid)=0;

		virtual void addSEListener(
				string eventType, IStreamEventListener* listener)=0;

		virtual void removeSEListener(
				string eventType, IStreamEventListener* listener)=0;

		virtual void addObjectListener(IObjectListener* listener)=0;

		virtual void setServiceDomainListener(
				IServiceDomainListener* listener)=0;

		virtual void removeObjectListener(IObjectListener* listener)=0;
		virtual void receiveSection(ITransportSection* section)=0;

		virtual void receiveData(char* buff, unsigned int size){};
		virtual void updateChannelStatus(
					short newStatus, IChannel* channel)=0;
		virtual bool isReady()=0;
  };
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::dataprocessing::IDataProcessor*
		dpCreator();

typedef void dpDestroyer(
		::br::pucrio::telemidia::ginga::core::dataprocessing::IDataProcessor*);

#endif /*IDataProcessor_H_*/
