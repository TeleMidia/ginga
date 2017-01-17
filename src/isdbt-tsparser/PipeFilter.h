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

#ifndef PIPEFILTER_H_
#define PIPEFILTER_H_

#include "util/functions.h"
using namespace ::ginga::util;

#include "system/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "ITSFilter.h"

#include "system/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include <sys/stat.h>
#include <stdio.h>
#include <map>
#include <string>
using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_CORE_TSPARSER_BEGIN

	class PipeFilter : public ITSFilter, Thread {
		private:
			unsigned int pid;
			bool dataReceived;
			bool running;
			unsigned int packetsReceived;
			unsigned long debugBytesWritten;
			map<int, int> pids;

			bool srcIsAPipe;
			bool dstPipeCreated;
			string srcUri;
			string dstUri;

			PipeDescriptor srcPd;
			PipeDescriptor dstPd;

		public:
			PipeFilter(unsigned int pid);
			virtual ~PipeFilter();

			void addPid(int pid);
			bool hasData();

			void receiveTSPacket(ITSPacket* pack);
			void receiveSection(char* buf, int len, IFrontendFilter* filter);
			void receivePes(char* buf, int len, IFrontendFilter* filter);

			void setSourceUri(string srcUri, bool isPipe);
			string setDestinationUri(string dstUri);

		private:
			void run();
	};

BR_PUCRIO_TELEMIDIA_GINGA_CORE_TSPARSER_END
#endif /*PIPEFILTER_H_*/
