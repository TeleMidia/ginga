/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#ifndef DATAPROCESSOR_H_
#define DATAPROCESSOR_H_

extern "C" {
	#include <sys/stat.h>
	#include <stdio.h>
	#include <stdio.h>
}

#include "tsparser/IMpegDescriptor.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;

#include "tsparser/IAIT.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser::si;

#include "dsmcc/carousel/ServiceDomain.h"
#include "dsmcc/carousel/IServiceDomainListener.h"
#include "dsmcc/carousel/object/IObjectListener.h"
#include "dsmcc/carousel/data/MessageProcessor.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing::carousel;

#include "EPGProcessor.h"
#include "IEPGListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing::epg;

#include "FilterManager.h"
#include "IDataProcessor.h"
#include "dsmcc/IStreamEventListener.h"

#include "dsmcc/npt/NPTProcessor.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing::dsmcc::npt;

#include <sys/stat.h>
#include <map>
#include <set>
#include <string>
using namespace std;

struct notifyData {
	IStreamEventListener* listener;
	IStreamEvent* se;
	pthread_mutex_t* mutex;
};

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
	class DataProcessor : public IDataProcessor,
				public IServiceDomainListener, public Thread {

		private:
			EPGProcessor* epgProcessor;
			FilterManager* filterManager;
			map<unsigned int, MessageProcessor*> processors;
			map<string, set<IStreamEventListener*>*> eventListeners;
			set<IObjectListener*> objectListeners;
			IServiceDomainListener* sdl;
			set<unsigned int> processedIds;
			pthread_mutex_t mutex;
			NPTProcessor* nptProcessor;
			vector<ITransportSection*> sections;
			IDemuxer* demux;
			IAIT* ait;
			bool running;
			bool removeOCFilter;

		public:
			DataProcessor();
			virtual ~DataProcessor();

			void applicationInfoMounted(IAIT* ait);
			void serviceDomainMounted(
					string mountPoint,
					map<string, string>* names,
					map<string, string>* paths);

			void setDemuxer(IDemuxer* demux);
			void removeOCFilterAfterMount(bool removeIt);

			void setSTCProvider(ISTCProvider* stcProvider);
			ITimeBaseProvider* getNPTProvider();

			void createStreamTypeSectionFilter(short streamType);
			void createPidSectionFilter(int pid);

			void addSEListener(
					string eventType, IStreamEventListener* listener);

			void removeSEListener(
					string eventType, IStreamEventListener* listener);

			void setServiceDomainListener(IServiceDomainListener* listener);
			void addObjectListener(IObjectListener* listener);
			void removeObjectListener(IObjectListener* listener);

		private:
			void notifySEListeners(IStreamEvent* se);
			static void* notifySEListener(void* data);
			void notifyEitListeners(set<IEventInfo*>* events);

		public:
			void receiveSection(ITransportSection* section);

		private:
			void run();
	};
}
}
}
}
}
}

#endif /*DataProcessor_H_*/
