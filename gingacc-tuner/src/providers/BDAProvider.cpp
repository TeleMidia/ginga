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

#include "tuner/providers/BDAProvider.h"
#include "tuner/providers/IProviderListener.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
	BDAProvider::BDAProvider(long freq) {
		this->capabilities   = DPC_CAN_FETCHDATA;
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
		if (bda->execute(frequency) == 0) {
			frequency = bda->getTunedFreq();
			return true;
		}
		if ((frequency == -1) && (bda->searchChannels() == 0)) {
			frequency = bda->getTunedFreq();
			return true;
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

	int BDAProvider::receiveData(char* buff,  int skipSize,
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
	}
}
}
}
}
}
}
