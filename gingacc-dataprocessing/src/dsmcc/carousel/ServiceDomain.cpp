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

#include "dataprocessing/dsmcc/carousel/ServiceDomain.h"

#include <stdio.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace carousel {
	ServiceDomain::ServiceDomain(
		    DownloadServerInitiate* dsi,
		    DownloadInfoIndication* dii, unsigned short pid) : Thread() {

		Thread::mutexInit(&stlMutex, true);

		this->serviceGatewayIor = dsi->getServiceGatewayIor();
		this->carouselId        = dii->getDonwloadId();

		dii->getInfo(&this->info);

		mounted                 = false;
		hasServiceGateway       = false;
		mountingServiceDomain   = true;

		mountPoint              = "carousel/" + itos(pid) + "." +
				itos(carouselId) + SystemCompat::getIUriD();

		remove(mountPoint.c_str());

		SystemCompat::makeDir(mountPoint.c_str(), 0777);

		processor = new ObjectProcessor(pid);
		sdl       = NULL;

		startThread();
	}

	ServiceDomain::~ServiceDomain() {
		map<unsigned int, Module*>::iterator i;
		set<Module*>::iterator j;

		Thread::mutexLock(&stlMutex);

		if (processor != NULL) {
			delete processor;
			processor = NULL;
		}

		//modules are deleted in DII destructor
		info.clear();

		Thread::mutexUnlock(&stlMutex);
		Thread::mutexDestroy(&stlMutex);
	}

	void ServiceDomain::setServiceDomainListener(IServiceDomainListener* sdl) {
		this->sdl = sdl;
	}

	void ServiceDomain::setObjectsListeners(set<IObjectListener*>* l) {
		processor->setObjectsListeners(l);
	}

	int ServiceDomain::receiveDDB(DownloadDataBlock* ddb) {
		if (ddb->processDataBlock(&info) < 0) {
			clog << "ServiceDomain::receiveDDB - error." << endl;
			return -1;
		}
		clog << "ddb done!" << endl;
		return 0;
	}

	Module* ServiceDomain::getModuleById(unsigned int id) {
		Module* mod = NULL;

		Thread::mutexLock(&stlMutex);
		if (info.count(id) != 0) {
			mod = info[id];
		}
		Thread::mutexUnlock(&stlMutex);

		return mod;
	}

	bool ServiceDomain::isMounted() {
		return mounted;
	}

	Module* ServiceDomain::getModule(int pos) {
		Module* module = NULL;
		int i;
		map<unsigned int, Module*>::iterator j;

		Thread::mutexLock(&stlMutex);

		j = info.begin();
		for (i = 0; i < pos; i++) {
			j++;

			if (j == info.end()) {
				Thread::mutexUnlock(&stlMutex);
				return NULL;
			}
		}

		module = j->second;

		Thread::mutexUnlock(&stlMutex);

		return module;
	}

	void ServiceDomain::eraseModule(Module* module) {
		map<unsigned int, Module*>::iterator i;

		Thread::mutexLock(&stlMutex);

		i = info.begin();
		while (i != info.end()) {
			if (i->second == module) {
				info.erase(i);
				if (remove(module->getModuleFileName().c_str()) == -1) {
					clog << errno << endl;
				}
				Thread::mutexUnlock(&stlMutex);
				return;
			}
			++i;
		}

		Thread::mutexUnlock(&stlMutex);
	}

	bool ServiceDomain::hasModules() {

		Thread::mutexLock(&stlMutex);

		if (info.empty()) {
			if (!processor->hasObjects()) {
				mountingServiceDomain = false;
			}

			Thread::mutexUnlock(&stlMutex);
			return false;
		}

		map<unsigned int, Module*>::iterator i;
		for (i=info.begin(); i!=info.end(); ++i) {
			if ((i->second)->isConsolidated()) {
				Thread::mutexUnlock(&stlMutex);
				return true;
			}
		}

		Thread::mutexUnlock(&stlMutex);

		return false;
	}

	void ServiceDomain::run() {
		Module* module = NULL;
		Biop* biop;
		map<unsigned int, Module*>::iterator i;
		unsigned int modId;
		int j = 0;

		while (mountingServiceDomain) {
			if (hasModules()) {
				if (!hasServiceGateway) {
					modId = serviceGatewayIor->getModuleId();

					Thread::mutexLock(&stlMutex);
					if (info.count(modId) == 0) {
						Thread::mutexUnlock(&stlMutex);
						break;

					} else {
						module = info[modId];
					}

					clog << "ServiceDomain::run waiting srg module" << endl;
					while (!module->isConsolidated()) {
						SystemCompat::uSleep(1000);
					}
					clog << "ServiceDomain::run srg module is consolidated, ";
					clog << "creating biop";
					clog << endl;

					try {
						biop = new Biop(module, processor);
					} catch (...) {
						clog << "ServiceDomain::run - error: BIOP - SRG not processed." << endl;
						return;
					}

					clog << "ServiceDomain::run BIOP processing SRG" << endl;

					biop->processServiceGateway(
							serviceGatewayIor->getObjectKey());

					hasServiceGateway = true;
					clog << "ServiceDomain::run SRG PROCESSED!" << endl;

					clog << "ServiceDomain::run PROCESSING SRG MODULE" << endl;

					biop->process();
					delete biop;
					biop = NULL;

					eraseModule(module);
					Thread::mutexUnlock(&stlMutex);

					j = 0;
					clog << "ServiceDomain::run SRG MODULE PROCESSED!" << endl;

				} else {
					module = getModule(j);
					if (module == NULL) {
						j = 0;
						continue;
					}

					if (module->isConsolidated()) {
						try {
							biop = new Biop(module, processor);
						} catch (...) {
							clog << "ServiceDomain::run BIOP->process (init error)" << endl;
							return;
						}

						clog << "ServiceDomain::run BIOP->process" << endl;
						biop->process();

						eraseModule(module);

						clog << "ServiceDomain::run BIOP->process DONE!";
						clog << endl;
						delete biop;
						biop = NULL;

					} else {
						SystemCompat::uSleep(1000);
						j++;
					}
				}

			} else {
				SystemCompat::uSleep(1000);
			}
		}
		mounted = true;
		clog << "ServiceDomain::run ";
		clog << "CAROUSEL " << carouselId << " MOUNTED!" << endl;
		if (sdl != NULL) {
			sdl->serviceDomainMounted(
					mountPoint,
					processor->getSDNames(),
					processor->getSDPaths());
		}
	}
}
}
}
}
}
}
}
