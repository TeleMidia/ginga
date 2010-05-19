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

#include "../config.h"
#include "../include/NetworkInterface.h"

#include "../include/providers/FileSystemProvider.h"
#include "../include/providers/MulticastProvider.h"
#include "../include/providers/UnicastProvider.h"

#if HAVE_FEV4L
#include "../include/providers/frontends/isdbt/ISDBTProvider.h"
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
	NetworkInterface::NetworkInterface(
			int id, string name, string protocol, string addr) {

		this->provider = NULL;
		this->id       = id;
		this->name     = name;
		this->protocol = protocol;
		this->address  = addr;
		this->tuned    = false;
	}

	NetworkInterface::~NetworkInterface() {
		close();
	}

	short NetworkInterface::getCaps() {
		if (provider != NULL) {
			return provider->getCaps();

		} else {
			cout << "NetworkInterface::getCaps return 0 (NULL provider)";
			cout << endl;
		}

		return 0;
	}

	int NetworkInterface::getId() {
		return id;
	}

	string NetworkInterface::getName() {
		return name;
	}

	string NetworkInterface::getProtocol() {
		return protocol;
	}

	string NetworkInterface::getAddress() {
		return address;
	}

	void NetworkInterface::attachFilter(IFrontendFilter* filter) {
		if (provider == NULL) {
			return;
		}

		provider->attachFilter(filter);
	}

	void NetworkInterface::removeFilter(IFrontendFilter* filter) {
		if (provider == NULL) {
			return;
		}

		provider->removeFilter(filter);
	}

	void NetworkInterface::setDataProvider(IDataProvider* provider) {
		this->provider = provider;
	}

	bool NetworkInterface::createProvider() {
		string ip, portNumber;

		close();

		if (name == "eth") {
			ip = address.substr(0, address.find(":"));
			portNumber = address.substr(
					address.find(":") + 1, address.length());

			if (protocol == "udp_unicast") {
				provider = new UnicastProvider(ip, (int)stof(portNumber));
				return true;

			} else if (protocol == "udp_multicast") {
				provider = new MulticastProvider(ip, (int)stof(portNumber));
				return true;
			}

		} else if (name == "fs") {
			provider = new FileSystemProvider(address);
			return true;

#if HAVE_FEV4L
		} else if (name == "sbtvd" && protocol == "terrestrial") {
			provider = new ISDBTProvider("");
			return true;
#endif
		}

		return false;
	}

	bool NetworkInterface::hasSignal() {
		return tuned;
	}

	IDataProvider* NetworkInterface::tune() {
		if (provider == NULL) {
			if (createProvider()) {
				tuned = provider->tune();
			}

		} else {
			tuned = provider->tune();
		}

		if (tuned) {
			return provider;
		}

		return NULL;
	}

	bool NetworkInterface::changeChannel(int factor) {
		if (provider != NULL) {
			return provider->changeChannel(factor);
		}

		return false;
	}

	bool NetworkInterface::setChannel(string channelValue) {
		if (provider != NULL) {
			return provider->setChannel(channelValue);
		}

		return false;
	}

	bool NetworkInterface::getSTCValue(uint64_t* stc, int* valueType) {
		if (provider != NULL) {
			return provider->getSTCValue(stc, valueType);
		}

		return false;
	}

	int NetworkInterface::createPesFilter(
			int pid, int pesType, bool compositeFiler) {

		if (provider != NULL) {
			return provider->createPesFilter(pid, pesType, compositeFiler);
		}

		return -1;
	}

	string NetworkInterface::getPesFilterOutput() {
		if (provider == NULL) {
			return "";
		}

		return provider->getPesFilterOutput();
	}

	IChannel* NetworkInterface::getCurrentChannel() {
		return provider->getCurrentChannel();
	}

	int NetworkInterface::receiveData(char* buff) {
		if (provider != NULL) {
			return provider->receiveData(buff);
		}

		return -1;
	}

	void NetworkInterface::close() {
		if (provider != NULL) {
			provider->close();
			delete provider;
			provider = NULL;
		}
	}
}
}
}
}
}
}
