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

#include "player/RTPListener.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	RTPListener::RTPListener(string url) {
		this->fd  = -1;
		this->url = "";

		this->icm = NULL;
		this->ic  = NULL;

		if (url.find("rtp://") != std::string::npos) {
			this->url = url.substr(6, url.length() - 6);
		}

		if (this->url != "") {
			icm = InteractiveChannelManager::getInstance();

			ic = icm->createInteractiveChannel(url);
			if (ic != NULL) {
				ic->reserveUrl(url, this);
				Thread::startThread();

			} else {
				clog << "RTPListener::RTPListener Warning! Can't create ";
				clog << "IC for '" << url << "'" << endl;
			}
		}
	}

	RTPListener::~RTPListener() {
		closeFile();
		this->url = "";

		if (ic != NULL) {
			ic->releaseUrl();
			delete ic;
			ic = NULL;
		}
	}

	void RTPListener::closeFile() {
		if (fd > -1) {
			fclose(fd);
			fd = -1;
		}
	}

	string RTPListener::getUrl() {
		return url;
	}

	void RTPListener::performIC() {
		ic->performUrl();
	}

	void RTPListener::releaseIC() {
		ic->releaseUrl();
		closeFile();
	}

	void RTPListener::receiveCode(long respCode) {

	}

	void RTPListener::receiveDataStream(char* buffer, int size) {
		if ((fd != NULL) && (size > 0)) {
			try {
				fwrite(buffer, 1, size, fd);

			} catch (const char *except) {
				clog << "RTPListener::receiveDataStream catch: ";
				clog << except << endl;
			}

		} else {
			clog << "RTPListener::receiveDataStream can't write fd = '";
			clog << fd << "' size = '" << size << "'" << endl;
		}
	}

	void RTPListener::receiveDataPipe(FILE* fd, int size) {

	}

	void RTPListener::downloadCompleted(const char* localUri) {

	}

	void RTPListener::run() {
		if (fd < 0) {
			mkfifo(this->url.c_str(), S_IFIFO);
			clog << "RTPListener::RTPListener '" << url << "' CREATEP" << endl;
			fd = fopen(this->url.c_str(), "wb");
			clog << "RTPListener::RTPListener CREATEP OPENED" << endl;
		}
	}
}
}
}
}
}
}
