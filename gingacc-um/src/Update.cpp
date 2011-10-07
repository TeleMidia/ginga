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

#include "um/Update.h"

#include "config.h"

#if HAVE_COMPSUPPORT
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#else
#include "ic/InteractiveChannelManager.h"
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace um {
#if HAVE_COMPSUPPORT
	static IComponentManager* cm = IComponentManager::getCMInstance();
#endif
	Update::Update(IComponent* component) {
		this->listeners = NULL;
		this->component = component;
		this->location = "";
	}

	Update::~Update() {
		if (listeners != NULL) {
			delete listeners;
			listeners = NULL;
		}

		this->component = NULL;
	}

	void Update::addListener(IUpdateListener* listener) {
		if (listeners != NULL) {
			listeners = new set<IUpdateListener*>;
		}

		listeners->insert(listener);
	}

	void Update::removeListener(IUpdateListener* listener) {
		set<IUpdateListener*>::iterator i;

		i = listeners->find(listener);
		if (i != listeners->end()) {
			listeners->erase(i);
		}
	}

	void Update::setLocation(string location) {
		this->location = location;
	}

	void Update::start() {
		IInteractiveChannel* ic;
		IInteractiveChannelManager* icm;
		string remoteFileName, localFileName;
		int fd;

#if HAVE_COMPSUPPORT
		icm = ((ICMCreator*)(cm->getObject(
				"InteractiveChannelManager")))();
#else
		icm = InteractiveChannelManager::getInstance();
#endif

		remoteFileName = component->getRepository(0) + component->getName();
		localFileName = location + component->getName();

		clog << "Update::start" << endl;
		mkdir(location.c_str(), 0666);
		fd = open(
				localFileName.c_str(),
				O_CREAT | O_WRONLY | O_LARGEFILE,
				0644);

		if (fd > 0) {
			ic = icm->createInteractiveChannel(remoteFileName);
			if (ic != NULL) {
				ic->setTarget(fd);
				ic->reserveUrl(remoteFileName, NULL);

			} else {
				clog << "Update::start warning: can't create Interactive ";
				clog << "channel for '" << remoteFileName << "'" << endl;
			}

		} else {
			clog << "Update::start warning: can't create local file '";
			clog << localFileName << "'" << endl;
		}
	}

	void Update::pause() {

	}

	void Update::resume() {

	}

	void Update::abort() {

	}

	void Update::notifyListeners() {
		set<IUpdateListener*>::iterator i;

		if (listeners == NULL || listeners->empty()) {
			return;
		}

		i = listeners->begin();
		while (i != listeners->end()) {
			(*i)->downloadComplete(location, component);
			++i;
		}
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::um::IUpdate* createUpdate(
		::br::pucrio::telemidia::ginga::core::cm::IComponent* c) {

	return new ::br::pucrio::telemidia::ginga::core::um::Update(c);
}

extern "C" void destroyUpdate(
		::br::pucrio::telemidia::ginga::core::um::IUpdate* u) {

	delete u;
}
