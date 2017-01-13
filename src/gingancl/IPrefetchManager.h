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

#ifndef IPrefecthManager_H_
#define IPrefecthManager_H_

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace prefetch {
	class IPrefetchManager {
		public:
			virtual ~IPrefetchManager(){};
			virtual void release()=0;
			virtual void releaseContents()=0;
			virtual string createDocumentPrefetcher(string remoteDocUri)=0;
			virtual string createSourcePrefetcher(
					string localDocUri, string srcUri)=0;

			virtual bool hasIChannel()=0;
			virtual bool hasRemoteLocation(string localUri)=0;
			virtual string getRemoteLocation(string localUri)=0;
			virtual string getLocalRoot(string remoteUri)=0;
			virtual void setSynchPrefetch(bool synch)=0;
			virtual void getScheduledContent(string clientUri)=0;
			virtual void getScheduledContents()=0;
	};
}
}
}
}
}
}

#endif /*IPrefetchManager_H_*/
