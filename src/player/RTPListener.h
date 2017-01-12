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

#ifndef RTPLISTENER_H_
#define RTPLISTENER_H_

#include "PlayersComponentSupport.h"

#include "ic/IInteractiveChannelManager.h"
#include "ic/IInteractiveChannelListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::ic;

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
  class RTPListener : public IInteractiveChannelListener, public Thread {
	private:
		FILE* fd;
		string url;

		IInteractiveChannelManager* icm;
		IInteractiveChannel* ic;

		static const short CMD_CREATEP = 0;
		static const short CMD_PERFORM = 1;
		static const short CMD_RELEASE = 2;

	public:
		RTPListener(string url);
		virtual ~RTPListener();

	private:
		void closeFile();

	public:
		string getUrl();
		void performIC();
		void releaseIC();
		void receiveCode(long respCode);
		void receiveDataStream(char* buffer, int size);
		void receiveDataPipe(FILE* fd, int size);
		void downloadCompleted(const char* localUri);
		void run();
  };
}
}
}
}
}
}

#endif /* RTPLISTENER_H_ */
