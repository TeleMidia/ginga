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

#ifndef SECTIONFILTER_H_
#define SECTIONFILTER_H_

extern "C" {
	#include <sys/stat.h>
	#include <stdio.h>
	#include <stdio.h>
}

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "system/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "tsparser/IDemuxer.h"
#include "tsparser/ITSPacket.h"
#include "tsparser/ITransportSection.h"
#include "tsparser/IFilterListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;

#include <string>
#include <map>
#include <set>
#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
	struct SectionHandler {
		// Last section's packet continuityCounter.
		int lastContinuityCounter;
		char sectionHeader[8];
		unsigned int headerSize;
		int recvPack;
		ITransportSection* section;
	};

	class SectionFilter : public ITSFilter {
		private:
			map<unsigned int, SectionHandler*> sectionPidSelector;

			IFilterListener* listener;
			set<string> processedSections;

			int lastPid;
			map<int, ITransportSection*> hFilteredSections;
			pthread_mutex_t stlMutex;

		public:
			SectionFilter();
			virtual ~SectionFilter();

			void addPid(int pid){};

		private:
			string setDestinationUri(string dstUri){return dstUri;};
			void setSourceUri(string srcUri, bool isPipe){};

			void resetHandler(SectionHandler* handler);
			void ignore(unsigned int pid);
			SectionHandler* getSectionHandler(unsigned int pid);
			void process(ITransportSection* section, unsigned int pid);
			void verifyAndAddData(ITSPacket* pack, bool lastPacket);
			bool verifyAndCreateSection(ITSPacket* pack);
			bool setSectionParameters(ITSPacket* pack);

		public:
			void setListener(IFilterListener* listener);
			bool checkProcessedSections(string sectionName);
			void addProcessedSection(string sectionName);
			void removeProcessedSection(string sectionName);
			void clearProcessedSections();
			bool checkSectionVersion(ITransportSection* section);
			void receiveTSPacket(ITSPacket* pack);
			void receiveSection(char* buf, int len, IFrontendFilter* filter);
			void receivePes(char* buf, int len, IFrontendFilter* filter);
	};
}
}
}
}
}
}

#endif /*SECTIONFILTER_H_*/
