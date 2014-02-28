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

#include "config.h"
#if HAVE_TUNER && HAVE_TSPARSER && HAVE_DSMCC
#include "gingalssm/DataWrapperListener.h"

#if HAVE_COMPONENTS
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#else
#include "system/fs/GingaLocatorFactory.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::fs;
#endif

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace lssm {
#if HAVE_COMPONENTS
	static IComponentManager* cm = IComponentManager::getCMInstance();
#endif
	DataWrapperListener::DataWrapperListener(
			IPresentationEngineManager* pem) {

		this->ait         = NULL;
		this->pem         = pem;
		this->documentUri = "";
		this->autoMount   = false;
		this->docToStart  = "";
	}

	DataWrapperListener::~DataWrapperListener() {
		this->pem = NULL;
		this->ait = NULL;
	}

	void DataWrapperListener::autoMountOC(bool autoMountIt) {
		this->autoMount = autoMountIt;
	}

	bool DataWrapperListener::processAIT() {
		vector<IApplication*>* apps;
		vector<IApplication*>::iterator i;
		map<string, string>::iterator j;
		string nclName, baseDir;

		if (ait != NULL) {
			apps = ait->copyApplications();
			clog << "DataWrapperListener::processAIT with '";
			clog << apps->size() << "' application(s)" << endl;
			if (!apps->empty()) {
				i = apps->begin();
				while (i != apps->end()) {
					nclName = (*i)->getInitialClass();

					clog << "DataWrapperListener::processAIT checking '";
					clog << nclName << "'" << endl;
					if ((*i)->getControlCode() == IApplication::CC_AUTOSTART) {
						j = ncls.find(nclName);

						//TODO: we should be able to avoid names conflict (two or more OC)
						if (j != ncls.end()) {
							delete apps;

							nclName = j->second;

							if (fileExists(nclName)) {
								clog << "DataWrapperListener::processAIT starting '";
								clog << nclName << "'" << endl;
								docToStart = nclName;
								Thread::startThread();

							} else {
								clog << "DataWrapperListener::processAIT can't start '";
								clog << nclName << "'" << endl;
							}

							return true;

						} else {
							clog << "DataWrapperListener::processAIT '";
							clog << nclName << "' still not available." << endl;
						}

					} else {
						clog << "DataWrapperListener::processAIT control code is '";
						clog << (*i)->getControlCode() << "'" << endl;
					}
					++i;
				}
			}

			delete apps;
		}

		return false;
	}

	bool DataWrapperListener::applicationInfoMounted(IAIT* ait) {
		if (ait != this->ait) {
			this->ait = ait;
		}

		return processAIT();
	}

	void DataWrapperListener::objectMounted(
			string ior, string clientUri, string name) {

		IGingaLocatorFactory* glf = NULL;

#if HAVE_COMPONENTS
		glf = ((GingaLocatorFactoryCreator*)(cm->getObject(
				"GingaLocatorFactory")))();
#else
		glf = GingaLocatorFactory::getInstance();
#endif

		clog << "DataWrapperListener::objectMounted ior '" << ior;
		clog << "', uri '" << clientUri << "' and name '" << name;
		clog << "'" << endl;
		if (glf != NULL) {
			glf->addClientLocation(ior, clientUri, name);
		}

		addNCLInfo(name, clientUri);

		if (name.find(".ncl") != std::string::npos && documentUri == "") {
			documentUri = SystemCompat::updatePath(clientUri + SystemCompat::getIUriD() + name);
		}
	}

	void DataWrapperListener::receiveStreamEvent(IStreamEvent* event) {
		string eventType;
		string eventData;

		eventType = event->getEventName();
		eventData.assign(event->getData(), event->getDescriptorLength());
/*
		clog << "DsmccWrapper::receiveStreamEvent ";
		clog << "eventId = '" << (eventData[0] & 0xFF) << (eventData[1] & 0xFF);
		clog << "' commandTag = '" << (eventData[11] & 0xFF);
		clog << "' eventType = '" << eventType;
		clog << "' and payload = '" << eventData;
		clog << "'" << endl;
*/
		if (eventType == "gingaEditingCommands") {
			if (pem != NULL) {
				pem->editingCommand(eventData);
			}

		} else {
			//TODO: fix eventName stuffs
			if (pem != NULL) {
				pem->editingCommand(eventData);
			}
		}
	}

	void DataWrapperListener::addNCLInfo(
			string name, string path) {

		if (name.find(".ncl") != std::string::npos) {
			ncls[name] = SystemCompat::updatePath(path + "/" + name);
		}
	}

	void DataWrapperListener::serviceDomainMounted(
			string mountPoint,
			map<string, string>* names,
			map<string, string>* paths) {

		map<string, string>::iterator i;
		string nclName, baseDir;

		if (processAIT()) {
			return;
		}

		if (!autoMount) {
			clog << "DataWrapperListener::serviceDomainMounted waiting ";
			clog << "ncl editing commands";
			clog << endl;
			return;
		}

		i = names->begin();
		while (i != names->end()) {
			if (i->second.find("main.ncl") != std::string::npos) {
				nclName = i->second;
				i = paths->find(i->first);
				if (i != paths->end()) {
					clog << "DataWrapperListener::serviceDomainMounted call ";
					clog << "start presentation '";
					clog << i->second + nclName;
					clog << "'" << endl;
					pem->startPresentation(i->second + nclName, "");
					return;
				}
			}
			++i;
		}

		clog << "DataWrapperListener::serviceDomainMounted Warning! Can't ";
		clog << "find ncl document inside service domain" << endl;
	}

	void DataWrapperListener::run() {
		assert(docToStart != "");

		clog << "DataWrapperListener::run starting presentation ";
		clog << "for '" << docToStart << "'" << endl;
		pem->startPresentation(docToStart, "");
	}
}
}
}
}
}
#endif //HAVE_TUNER && HAVE_TSPARSER && HAVE_DSMCC
