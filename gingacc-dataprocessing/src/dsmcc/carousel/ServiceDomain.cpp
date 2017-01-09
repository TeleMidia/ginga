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
		blockSize = dii->getBlockSize();

		dii->getInfo(&this->info);

		mounted                 = false;
		hasServiceGateway       = false;
		mountingServiceDomain   = true;

		mountPoint = SystemCompat::getTemporaryDir() +
					 "ginga" + SystemCompat::getIUriD() +
					 "carousel" + SystemCompat::getIUriD() +
					 itos(pid) + "." + itos(carouselId) +
					 SystemCompat::getIUriD();

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

	map<unsigned int, Module*>* ServiceDomain::getInfo() {
		return &info;
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

	unsigned short ServiceDomain::getBlockSize() {
		return blockSize;
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

					while (!module->isConsolidated()) {
						SystemCompat::uSleep(1000);
					}
					clog << endl;

					try {
						biop = new Biop(module, processor);

					} catch (...) {
						clog << "ServiceDomain::run - error: BIOP - SRG not processed." << endl;
						return;
					}

					biop->processServiceGateway(
							serviceGatewayIor->getObjectKey());

					hasServiceGateway = true;

					biop->process();
					delete biop;
					biop = NULL;

					Thread::mutexUnlock(&stlMutex);

					j = 0;
					continue;

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

						biop->process();

						eraseModule(module);

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
