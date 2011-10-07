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

#include "system/io/GingaLocatorFactory.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	GingaLocatorFactory* GingaLocatorFactory::_instance = 0;

	GingaLocatorFactory::GingaLocatorFactory() {
		iorClients = new map<string, string>;
		iorProviders = new map<string, string>;
		iorNames = new map<string, string>;

		isWaiting = false;
		pthread_cond_init(&flagCondSignal, NULL);
		pthread_mutex_init(&flagMutexSignal, NULL);
	}

	GingaLocatorFactory::~GingaLocatorFactory() {
		if (iorClients != NULL) {
			delete iorClients;
			iorClients = NULL;
		}

		if (iorProviders != NULL) {
			delete iorProviders;
			iorProviders = NULL;
		}

		if (iorNames != NULL) {
			delete iorNames;
			iorNames = NULL;
		}

		pthread_cond_destroy(&flagCondSignal);
		pthread_mutex_destroy(&flagMutexSignal);
	}

	void GingaLocatorFactory::release() {
		if (_instance != NULL) {
			delete _instance;
			_instance = NULL;
		}
	}

	GingaLocatorFactory* GingaLocatorFactory::getInstance() {
		if (GingaLocatorFactory::_instance == NULL) {
			GingaLocatorFactory::_instance = new GingaLocatorFactory();
		}

		return GingaLocatorFactory::_instance;
	}

	void GingaLocatorFactory::createLocator(string providerUri, string ior) {
		vector<string>* args;
		vector<string>::iterator i;
		string arg, newIor;

		if (ior.find(".") == std::string::npos) {
			newIor = ior;

		} else {
			newIor = "";
			args = split(ior, ".");
			i = args->begin();
			while(i != args->end()) {
				arg = *i;
				newIor = newIor + itos(strHexaToInt(arg));
				++i;
			}

			delete args;
			args = NULL;
		}

		while (providerUri.find("//") != std::string::npos) {
			providerUri = providerUri.substr(0,
					providerUri.find_first_of("//") - 1) + "/" +
					providerUri.substr(
							providerUri.find_first_of("//") + 2,
							providerUri.length());
		}

		while (providerUri.find("\\\\") != std::string::npos) {
			providerUri = providerUri.substr(0,
					providerUri.find_first_of("\\\\") - 1) + "\\" +
					providerUri.substr(
							providerUri.find_first_of("\\\\") + 2,
							providerUri.length());
		}

		while (providerUri.substr(providerUri.length() - 1, 1) == "/") {
			providerUri = providerUri.substr(0, providerUri.length() - 1);
		}

		while (providerUri.substr(providerUri.length() - 1, 1) == "\\") {
			providerUri = providerUri.substr(0, providerUri.length() - 1);
		}

		(*iorProviders)[newIor] = providerUri;
	}

	string GingaLocatorFactory::getLocation(string providerUri) {
		map<string, string>::iterator i;
		string clientUri = "";
		string ior = "-1";

		while (providerUri.find("//") != std::string::npos) {
			providerUri = providerUri.substr(0,
					providerUri.find_first_of("//") - 1) + "/" +
					providerUri.substr(
							providerUri.find_first_of("//") + 2,
							providerUri.length());
		}

		while (providerUri.find("\\\\") != std::string::npos) {
			providerUri = providerUri.substr(0,
					providerUri.find_first_of("\\\\") - 1) + "\\" +
					providerUri.substr(
							providerUri.find_first_of("\\\\") + 2,
							providerUri.length());
		}

		while (providerUri.substr(providerUri.length() - 1, 1) == "/") {
			providerUri = providerUri.substr(0, providerUri.length() - 1);
		}

		while (providerUri.substr(providerUri.length() - 1, 1) == "\\") {
			providerUri = providerUri.substr(0, providerUri.length() - 1);
		}

		i = iorProviders->begin();
		while (i != iorProviders->end()) {
			if (i->second == providerUri) {
				ior = i->first;
			}
			++i;
		}

		if (iorClients->count(ior) != 0) {
			clientUri = (*iorClients)[ior];
		}

		if (clientUri == "") {
			waitNewLocatorCondition();
			return getLocation(providerUri);
		}

		return clientUri;
	}

	string GingaLocatorFactory::getName(string ior) {
		vector<string>* args;
		vector<string>::iterator i;
		string arg, newIor;

		if (ior.find(".") == std::string::npos) {
			newIor = ior;

		} else {
			newIor = "";
			args = split(ior, ".");
			i = args->begin();
			while(i != args->end()) {
				arg = *i;
				newIor = newIor + itos(strHexaToInt(arg));
				++i;
			}

			delete args;
			args = NULL;
		}

		if (iorNames->count(newIor) != 0) {
			return (*iorNames)[newIor];
		}

		waitNewLocatorCondition();
		return getName(newIor);
	}

	void GingaLocatorFactory::addClientLocation(
			string ior, string clientUri, string name) {

		vector<string>* args;
		vector<string>::iterator i;
		string arg, newIor;

		if (ior.find(".") == std::string::npos) {
			newIor = ior;

		} else {
			newIor = "";
			args = split(ior, ".");
			i = args->begin();
			while(i != args->end()) {
				arg = *i;
				newIor = newIor + itos(strHexaToInt(arg));
				++i;
			}

			delete args;
			args = NULL;
		}

		clog << "GingaLocatorFactory::addClientLocation ";
		clog << "clientUri '" << clientUri << "' ior '";
		clog << newIor << "'" << endl;
		(*iorClients)[newIor] = clientUri;
		(*iorNames)[newIor] = name;
		newLocatorConditionSatisfied();
	}

	void GingaLocatorFactory::waitNewLocatorCondition() {
		isWaiting = true;
		pthread_mutex_lock(&flagMutexSignal);
		pthread_cond_wait(&flagCondSignal, &flagMutexSignal);
		isWaiting = false;
		pthread_mutex_unlock(&flagMutexSignal);
	}

	bool GingaLocatorFactory::newLocatorConditionSatisfied() {
		if (isWaiting) {
			pthread_cond_signal(&flagCondSignal);
			return true;
		}
		return false;
	}
}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::system::io::
		IGingaLocatorFactory* createGingaLocatorFactory() {

	return (::br::pucrio::telemidia::ginga::core::system::io::
			GingaLocatorFactory::getInstance());
}

extern "C" void destroyGingaLocatorFactory(
		::br::pucrio::telemidia::ginga::core::system::io::
				IGingaLocatorFactory* glf) {

	glf->release();
}
