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

#include "../../include/ccrtpic/CCRTPInteractiveChannel.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace ic {
	CCRTPInteractiveChannel::CCRTPInteractiveChannel() {
		this->fd         = -1;
		this->listener   = NULL;
		this->uri        = "";
		this->userAgent  = "";
		this->hasSession = false;
	}

	CCRTPInteractiveChannel::~CCRTPInteractiveChannel() {

	}

	CCRTPClient* CCRTPInteractiveChannel::createClient() {
		string::size_type pos;
		string address;
		string strPort;
		InetMcastAddress ima;

		cout << "CCRTPInteractiveChannel::createClient with uri '";
		cout << uri << "'";
		cout << endl;

		if (uri.length() > 6 && uri.substr(0, 6) == "rtp://") {
			uri = uri.substr(6, uri.length() - 6);
		}

		cout << "CCRTPInteractiveChannel::createClient url: '";
		cout << uri << "'" << endl;

		if (uri.find(":") == std::string::npos) {
			return NULL;
		}

		pos     = uri.find_first_of(":");
		address = uri.substr(0, pos);
		strPort = uri.substr(pos + 1, uri.length() - pos + 1);

		ima = InetMcastAddress(address.c_str());

		cout << "CCRTPInteractiveChannel::createClient Listening ";

		tpport_t port = atoi(strPort.c_str());
		if (ima.isInetAddress()) {
			cout << "on multicast address " << ima << ":" << port << endl;
			return new CCRTPClient(ima, port);

		} else {
			InetHostAddress ia(address.c_str());
			cout << "on unicast address " << ia << ":" << port << endl;
			return new CCRTPClient(ia, port);
		}
	}

	void CCRTPInteractiveChannel::setSourceTarget(string url) {

	}

	bool CCRTPInteractiveChannel::hasConnection() {
		//TODO: check connection
		return true;
	}

	void CCRTPInteractiveChannel::setTarget(int fd) {
		this->fd = fd;
	}

	void CCRTPInteractiveChannel::setTarget(char* buffer) {

	}

	void CCRTPInteractiveChannel::setListener(
			IInteractiveChannelListener* listener) {

		this->listener = listener;
	}

	bool CCRTPInteractiveChannel::performUrl() {
		CCRTPClient* rtpClient;
		char* buffer;
		int size;

		cout << "CCRTPInteractiveChannel::performUrl" << endl;

		if (listener == NULL) {
			return false;
		}

		rtpClient = createClient();
		if (rtpClient == NULL) {
			return false;
		}

		rtpClient->openSession();
		rtpClient->printSSRC();
		hasSession = true;
		while (hasSession) {
			buffer = rtpClient->getPacketData(&size);
			if (listener != NULL && size > 0) {
				if (fd < 0) {
					listener->receiveDataStream(buffer, size);

				} else {
					write(fd, buffer, size);
					listener->receiveDataPipe(fd, size);
				}

				delete buffer;
			}
		}

		delete rtpClient;
		return true;
	}

	bool CCRTPInteractiveChannel::reserveUrl(
			string uri,
			IInteractiveChannelListener* listener,
			string userAgent) {

		cout << "CCRTPInteractiveChannel::reserveUrl '" << uri << "'" << endl;
		this->uri       = uri;
		this->listener  = listener;
		this->userAgent = userAgent;

		return true;
	}

	bool CCRTPInteractiveChannel::releaseUrl() {
		hasSession = false;

		if (fd > 0) {
			close(fd);
			fd = -1;
		}

		return true;
	}

	short CCRTPInteractiveChannel::getType() {
		//TODO: type
		return 0;
	}

	float CCRTPInteractiveChannel::getRate() {
		//TODO: rate
		return 0.0;
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::ic::IInteractiveChannel*
		createCCRTPIC() {

	return new ::br::pucrio::telemidia::ginga::core::ic::
			CCRTPInteractiveChannel();
}

extern "C" void destroyCCRTPIC(
		::br::pucrio::telemidia::ginga::core::ic::IInteractiveChannel* ic) {

	delete ic;
}
