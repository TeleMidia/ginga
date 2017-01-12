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

#ifndef PrefecthManager_H_
#define PrefecthManager_H_

#include "config.h"

#include "IPrefetchManager.h"

#include "system/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "ic/IInteractiveChannelManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::ic;

#include <sys/stat.h>
#include <map>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace prefetch {
	class PrefetchManager : public IPrefetchManager,
			public IInteractiveChannelListener {

		private:
			map<string, string>* localToRemoteUris;
			map<string, string>* urisToLocalRoots;
			set<string>* scheduledRemoteUris;
			set<string>* scheduledLocalUris;
			IInteractiveChannelManager* icm;
			string prefetchRoot;
			bool synch;

			int kbytes;
			int filesDown;
			int filesSched;

			PrefetchManager();
			~PrefetchManager();
			static IPrefetchManager* _instance;

		public:
			static IPrefetchManager* getInstance();

		private:
			void createDirectory(string newDir);

		public:
			void release();
			void releaseContents();
			string createDocumentPrefetcher(string remoteDocUri);
			string createSourcePrefetcher(string localDocUri, string srcUri);
			bool hasIChannel();
			bool hasRemoteLocation(string localUri);
			string getRemoteLocation(string localUri);
			string getLocalRoot(string remoteUri);

		private:
			void getContent(string remoteUri, string localUri);

		public:
			void setSynchPrefetch(bool synch);

		private:
			void scheduleContent(string remoteUri, string localUri);
			void receiveCode(long respCode){};
			void receiveDataStream(char* buffer, int size){};
			void receiveDataPipe(FILE* fd, int size);
			void downloadCompleted(const char* localUri){};

		public:
			void getScheduledContent(string clientUri);
			void getScheduledContents();
	};
}
}
}
}
}
}

#endif /*PrefetchManager_H_*/
