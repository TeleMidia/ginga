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
		    DownloadInfoIndication* dii) : Thread() {

		this->serviceGatewayIor = dsi->getServiceGatewayIor();
		this->info              = dii->getInfo();
		this->carouselId        = dii->getDonwloadId();

		mounted                 = false;
		hasServiceGateway       = false;
		mountingServiceDomain   = true;

		mountPoint              = "carousel/" +
				itos(carouselId) + SystemCompat::getIUriD();

		remove(mountPoint.c_str());

		SystemCompat::makeDir(mountPoint.c_str(), 0777);

		processor = new ObjectProcessor();
		sdl       = NULL;

		startThread();
	}

	void ServiceDomain::setServiceDomainListener(IServiceDomainListener* sdl) {
		this->sdl = sdl;
	}

	void ServiceDomain::setObjectsListeners(set<IObjectListener*>* l) {
		processor->setObjectsListeners(l);
	}

	void ServiceDomain::receiveDDB(DownloadDataBlock* ddb) {
		ddb->processDataBlock(info);
		clog << "ddb done!" << endl;
	}

	Module* ServiceDomain::getModuleById(unsigned int id) {
		if (info->count(id) != 0) {
			return (*info)[id];
		}

		return NULL;
	}

	bool ServiceDomain::isMounted() {
		return mounted;
	}

	bool ServiceDomain::hasModules() {
		if (info->empty()) {
			if (!processor->hasObjects()) {
				mountingServiceDomain = false;
			}

			return false;
		}

		map<unsigned int, Module*>::iterator i;
		for (i=info->begin(); i!=info->end(); ++i) {
			if ((i->second)->isConsolidated()) {
				return true;
			}
		}
		return false;
	}

	void ServiceDomain::run() {
		Module* module;
		Biop* biop;
		map<unsigned int, Module*>::iterator i;
		unsigned int modId;

		i = info->begin();
		while (mountingServiceDomain) {
			if (hasModules()) {
				if (!hasServiceGateway) {
					modId = serviceGatewayIor->getModuleId();
					if (info->count(modId) == 0) {
						break;

					} else {
						i = info->find(modId);
						module = (*info)[modId];
					}

					clog << "ServiceDomain::run waiting srg module" << endl;
					while (!module->isConsolidated()) {
						SystemCompat::uSleep(1000);
					}
					clog << "ServiceDomain::run srg module is consolidated, ";
					clog << "creating biop";
					clog << endl;

					biop = new Biop(module, processor);
					clog << "ServiceDomain::run BIOP processing SRG" << endl;
					biop->processServiceGateway(
							serviceGatewayIor->getObjectKey());

					hasServiceGateway = true;
					clog << "ServiceDomain::run SRG PROCESSED!" << endl;

					delete biop;
					biop = NULL;

					clog << "ServiceDomain::run PROCESSING SRG MODULE" << endl;
					biop = new Biop(module, processor);
					biop->process();

					info->erase(i);
					i = info->begin();

					clog << "ServiceDomain::run SRG MODULE PROCESSED!" << endl;

				} else {
					module = i->second;
					if (module->isConsolidated()) {
						biop = new Biop(module, processor);
						clog << "ServiceDomain::run BIOP->process" << endl;
						biop->process();

						info->erase(i);
						i = info->begin();

						clog << "ServiceDomain::run BIOP->process DONE!";
						clog << endl;
//						delete biop;
//						biop = NULL;

//						delete module;
//						module = NULL;

					} else {
						SystemCompat::uSleep(1000);
						++i;

						if (i == info->end())
							i = info->begin();
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
