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

#include "../../include/component/Component.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace cm {
	Component::Component(string name, string version, string type) {
		this->name = name;
		this->version = version;
		this->type = type;
		this->location = "";
		this->locationType = LT_UNKNOWN;
		this->dlComponent = NULL;

		uris = new set<string>;
		dependencies = new set<IComponent*>;
		creatorSymbols = new map<string, string>;
		destroyerSymbols = new map<string, string>;
	}

	Component::~Component() {
		delete uris;
		delete dependencies;
		delete creatorSymbols;
		delete destroyerSymbols;
	}

	void Component::setLocation(string location, string type) {
		this->location = location;
		if (type == "" || type == "local") {
			this->locationType = LT_LOCAL;

		} else if (type == "remote") {
			this->locationType = LT_REMOTE;

		} else {
			this->locationType = LT_UNKNOWN;
		}
	}

	string Component::getName() {
		return name;
	}

	string Component::getVersion() {
		return version;
	}

	string Component::getType() {
		return type;
	}

	string Component::getLocation() {
		return location;
	}

	string Component::getStrLocationType() {
		string strLocationType = "";

		if (locationType == LT_LOCAL) {
			strLocationType = "local";

		} else if (locationType == LT_REMOTE) {
			strLocationType = "remote";

		} else {
			strLocationType = "unknown";
		}

		return strLocationType;
	}

	short Component::getLocationType() {
		return locationType;
	}

	void Component::addCreatorSymbol(string objectName, string symbol) {
		(*creatorSymbols)[objectName] = symbol;
	}

	void Component::addDestroyerSymbol(string objectName, string symbol) {
		(*destroyerSymbols)[objectName] = symbol;
	}

	void Component::addDependency(IComponent* dependency) {
		dependencies->insert(dependency);
	}

	void Component::addUri(string uri) {
		uris->insert(uri);
	}

	map<string, string>* Component::getCreatorSymbols() {
		return creatorSymbols;
	}

	string Component::getCreatorSymbol(string objectName) {
		map<string, string>::iterator i;

		i = creatorSymbols->find(objectName);
		if (i != creatorSymbols->end()) {
			return i->second;
		}

		return "";
	}

	map<string, string>* Component::getDestroyerSymbols() {
		return destroyerSymbols;
	}

	string Component::getDestroyerSymbol(string objectName) {
		map<string, string>::iterator i;

		i = destroyerSymbols->find(objectName);
		if (i != destroyerSymbols->end()) {
			return i->second;
		}

		return "";
	}

	set<IComponent*>* Component::getDependencies() {
		return dependencies;
	}

	IComponent* Component::getDependency(unsigned int ix) {
		set<IComponent*>::iterator i;

		if (dependencies == NULL || ix >= dependencies->size()) {
			return NULL;
		}

		i = dependencies->begin();
		for (unsigned int j=0; j < ix; j++) {
			++i;
		}

		return *i;
	}

	set<string>* Component::getRepositories() {
		return uris;
	}

	string Component::getRepository(unsigned int ix) {
		set<string>::iterator i;

		if (uris == NULL || ix >= uris->size()) {
			return "";
		}

		i = uris->begin();
		for (unsigned int j=0; j < ix; j++) {
			++i;
		}

		return *i;
	}

	void Component::setComponent(void* dlComponent) {
		this->dlComponent = dlComponent;
	}

	void* Component::getComponent() {
		return dlComponent;
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::cm::IComponent*
		createComponent(string name, string version, string type) {

	return new ::br::pucrio::telemidia::ginga::core::cm::Component(
			name, version, type);
}

extern "C" void destroyComponent(
		::br::pucrio::telemidia::ginga::core::cm::IComponent* comp) {

	delete comp;
}
