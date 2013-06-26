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

#ifndef DEMUXER_H_
#define DEMUXER_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "Pmt.h"
#include "Pat.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser::si;

#include "FrontendFilter.h"
#include "TSPacket.h"
#include "ITSFilter.h"
#include "IDemuxer.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include <map>
#include <vector>
#include <set>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
	class Demuxer : public IDemuxer {
		private:
			Pat* pat;
			Pmt* newPmt;
			map<unsigned int, Pmt*> pmts;
			map<unsigned int, ITSFilter*> pidFilters;
			map<short, ITSFilter*> stFilters;
			map<unsigned int, ITSFilter*> pesFilters;
			IFrontendFilter* audioFilter;
			IFrontendFilter* videoFilter;
			set<IFrontendFilter*> feFilters;
			set<IFrontendFilter*> feFiltersToSetup;
			static vector<Pat*> pats;
			static unsigned int sectionPid; //debug only
			static set<unsigned int> knownSectionPids;
			ITuner* tuner;

			short debugDest;
			unsigned int debugPacketCounter;
			bool isWaitingPI;

			pthread_mutex_t flagLockUntilSignal;
			pthread_cond_t flagCondSignal;

			static pthread_mutex_t stlMutex;

			unsigned char packetSize;
			bool nptPrinter;
			int nptPid;

		public:
			//defs
			static const short NB_PID_MAX = 0x1FFF; //8191
			static const short ERR_CONDITION_SATISFIED = 5;

		public:
			Demuxer(ITuner* tuner);
			virtual ~Demuxer();

			bool hasStreamType(short streamType);
			void printPat();
			void setNptPrinter(bool nptPrinter);

		private:
			void createPSI();
			void clearPSI();
			void initMaps();
			void clearMaps();
			void resetDemuxer();
			void setDestination(short streamType); //debug purpose only
			void removeFilter(IFrontendFilter* filter);
			void setupUnsolvedFilters();
			bool setupFilter(IFrontendFilter* filter);
			void demux(ITSPacket* packet);

		public:
			map<unsigned int, Pmt*>* getProgramsInfo();
			unsigned int getTSId();
			Pat* getPat();
			int getDefaultMainVideoPid();
			int getDefaultMainAudioPid();
			void removeFilter(ITSFilter* tsFilter);
			void addFilter(ITSFilter* tsFilter, int pid, int tid);
			void addFilter(IFrontendFilter* filter);

		private:
			void attachFilter(IFrontendFilter* filter);
			void createPatFilter(INetworkInterface* ni);
			void createPmtFilter(INetworkInterface* ni);

		public:
			void receiveSection(
					char* section, int secLen, IFrontendFilter* filter);

			void addPidFilter(unsigned int pid, ITSFilter* filter);
			void addSectionFilter(unsigned int tid, ITSFilter* filter);
			void addStreamTypeFilter(short streamType, ITSFilter* filter);
			void addPesFilter(short type, ITSFilter* filter);
			void addVideoFilter(unsigned int pid, ITSFilter* f);

		private:
			void receiveData(char* buff, unsigned int size);
			void updateChannelStatus(short newStatus, IChannel* channel);

		public:
			static void addPat(Pat* pat);
			static bool isSectionStream(unsigned int pid);
			static void setSectionPid(unsigned int pid); //debug only

		private:
			unsigned int hunt(char* buff, unsigned int size);

		public:
			short getCaps();

		private:
			void checkProgramInformation();

		public:
			bool waitProgramInformation();
			bool waitBuffers();
	 };
}
}
}
}
}
}

#endif /*DEMUXER_H_*/
