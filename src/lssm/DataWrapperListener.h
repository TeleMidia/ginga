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

#ifndef DataWrapperListener_H_
#define DataWrapperListener_H_

#include "system/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "system/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "system/GingaLocatorFactory.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::fs;

#include "player/INCLPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "isdbt-tuner/Tuner.h"
using namespace ::br::pucrio::telemidia::ginga::core::tuning;

#include "isdbt-tsparser/IEventInfo.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;

#include "isdbt-tsparser/IShortEventDescriptor.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser::si::descriptors;

#include "isdbt-dataproc/dsmcc/IStreamEventListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing;

#include "system/ITimeBaseProvider.h"
using namespace br::pucrio::telemidia::ginga::core::system::time;

#include "isdbt-dataproc/dsmcc/IServiceDomainListener.h"
#include "isdbt-dataproc/dsmcc/IObjectListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing::carousel;

#include "PresentationEngineManager.h"

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace lssm {
  class DataWrapperListener :
				public IStreamEventListener,
				public IObjectListener,
				public IServiceDomainListener,
				public Thread {

	private:
		PresentationEngineManager* pem;
		string documentUri;
		bool autoMount;
		map<string, string> ncls;
		set<string> present;
		string docToStart;

		IAIT* ait;

	public:
		DataWrapperListener(PresentationEngineManager* pem);
		virtual ~DataWrapperListener();
		void autoMountOC(bool autoMountIt);

	private:
		static void writeAITCommand(const string &appName, const string &appUri, IApplication* app);
		bool startApp(const string &appName, IApplication* app);
		bool appIsPresent(const string &appName, IApplication* app);
		bool processAIT();

	public:
		bool applicationInfoMounted(IAIT* ait);
		void objectMounted(string ior, string clientUri, string name);
		void receiveStreamEvent(StreamEvent* event);
		
	private:
		void addNCLInfo(string name, string path);

	public:
		void serviceDomainMounted(
				string mountPoint,
				map<string, string>* names,
				map<string, string>* paths);

	private:
		void run();
  };
}
}
}
}
}

#endif /*DataWrapperListener_H_*/
