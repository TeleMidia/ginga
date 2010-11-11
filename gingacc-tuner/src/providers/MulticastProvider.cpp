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

#include "tuner/providers/MulticastProvider.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
	MulticastProvider::MulticastProvider(string groupAddr, int port) {
		this->addr         = groupAddr;
		this->portNumber   = port;
		this->capabilities = DPC_CAN_FETCHDATA;
	}

	MulticastProvider::~MulticastProvider() {

	}

	int MulticastProvider::callServer() {
		if (createSocket()) {
			if (tryToBind()) {
				if (setSocketOptions()) {
					if (addToGroup()) {
						return socketDescriptor;
					}
				}
			}
		}
		return -1;
	}

	bool MulticastProvider::createSocket() {
		socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
		if (socketDescriptor < 0){
			perror("MulticastProvider::createSocket");
			return false;
		}
		return true;
	}

	bool MulticastProvider::tryToBind() {
		int ret;

		struct sockaddr_in stSourceAddr;

		stSourceAddr.sin_family = AF_INET;
		stSourceAddr.sin_port = htons(this->portNumber);
		stSourceAddr.sin_addr.s_addr = INADDR_ANY;

		/*
		* Calling bind() is not required, but some implementations need it
		* before you can reference any multicast socket options
		*/
		ret = bind(
				socketDescriptor,
				(struct sockaddr*)&stSourceAddr,
				sizeof(struct sockaddr));

		if (ret < 0) {
			perror ("MulticastProvider::tryToBind bind");
			return false;
		}

		if (setsockopt(
				socketDescriptor,
				IPPROTO_IP,
				IP_MULTICAST_IF,
				(char*)&stSourceAddr, sizeof(struct in_addr)) < 0) {

			perror ("Multicast::tryToBind IP_MULTICAST_IF");
	    }

		return true;
	}

	bool MulticastProvider::setSocketOptions() {
		int opt = 0;
		int ret = setsockopt(
				socketDescriptor,
				IPPROTO_IP,
				IP_MULTICAST_LOOP,
				(char*)opt, sizeof(int));

		if (ret < 0) {
			perror("MulticastProvider::setSocketOptions loop");
		}

		opt = 15;
		ret = setsockopt(
				socketDescriptor,
				IPPROTO_IP,
				IP_MULTICAST_TTL,
				(char*)opt, sizeof(int));

		if (ret < 0) {
			perror("MulticastProvider::setSocketOptions TTL");
		}

		return true;
	}

	bool MulticastProvider::addToGroup() {
		int ret;
		struct ip_mreq stIpMreq;

		stIpMreq.imr_multiaddr.s_addr = inet_addr(addr.c_str());
		stIpMreq.imr_interface.s_addr = INADDR_ANY;
		ret = setsockopt(
				socketDescriptor,
				IPPROTO_IP,
				IP_ADD_MEMBERSHIP,
				(char*)&stIpMreq, sizeof(struct ip_mreq));

		if (ret < 0) {
			perror("MulticastProvider::addClientToGroup IP_ADD_MEMBERSHIP");
			return false;
		}

		return true;
	}

	int MulticastProvider::receiveData(char* buff) {
		return recvfrom(
				socketDescriptor,
				buff, BUFFSIZE, 0, (struct sockaddr*)NULL, (socklen_t*)NULL);
	}
}
}
}
}
}
}
