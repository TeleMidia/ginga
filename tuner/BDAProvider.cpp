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

#include "tuner/providers/BDAProvider.h"
#include "tuner/providers/IProviderListener.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
	BDAProvider::BDAProvider(long freq) {
		this->capabilities   = DPC_CAN_FETCHDATA | DPC_CAN_CTLSTREAM;
		this->listener       = NULL;
		bda = new BDAGraph(SystemCompat::getTemporaryDir() +
							"ginga" + SystemCompat::getIUriD() +
							"channels.txt", &channels);
		frequency = freq;
	}

	BDAProvider::~BDAProvider() {
		delete bda;
	}

	void BDAProvider::setListener(ITProviderListener* listener) {
		this->listener = listener;
	}

	short BDAProvider::getCaps() {
		return capabilities;
	}

	bool BDAProvider::tune() {
		if (bda->initDevice()) {
			if (frequency > 0 && bda->execute(frequency) == 0) {
				frequency = bda->getTunedFreq();
				return true;

			} else if (frequency == -1 && bda->searchChannels() == 0) {
				frequency = bda->getTunedFreq();
				return true;
			}
		}

		return false;
	}

	IChannel* BDAProvider::getCurrentChannel() {
		return NULL;
	}

	bool BDAProvider::getSTCValue(uint64_t* stc, int* valueType) {
		return false;
	}

	bool BDAProvider::changeChannel(int factor) {
		if (bda->changeChannelTo(factor, true) == 0) {
			frequency = factor;
			return true;
		}
		frequency = -1;
		return false;
	}

	Channels* BDAProvider::getChannels() {
		return &channels;
	}

	bool BDAProvider::setChannel(string channelValue) {
		long freq = stoi(channelValue);
		if (bda->changeChannelTo(freq, true) == 0) {
			frequency = freq;
			return true;
		}
		frequency = -1;
		return false;
	}

	int BDAProvider::createPesFilter(
			int pid, int pesType, bool compositeFiler) {

		return -1;
	}

	string BDAProvider::getPesFilterOutput() {
		return "";
	}

	void BDAProvider::close() {
		delete bda;
		bda = NULL;
	}

	char* BDAProvider::receiveData(int* len) {
		Buffer* buf;
		char* buffer;

		if (bda->getBuffer(&buf)) {
			*len = buf->len;
			buffer = buf->buffer;

			delete buf;
			return buffer;
		}

		return 0;
	}

	/*int BDAProvider::receiveData(char* buff,  int skipSize,
									    unsigned char packetSize) {
		Buffer* buf;
		int bufSize;
		if (bda->getBuffer(&buf)) {
			memcpy(buff, buf->buffer, buf->len);
			bufSize = buf->len;
			delete (buf->buffer);
			delete buf;
			return bufSize;
		}

		return 0;
	}*/
}
}
}
}
}
}
