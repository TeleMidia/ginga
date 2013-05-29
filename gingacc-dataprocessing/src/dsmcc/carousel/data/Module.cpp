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

#include "dataprocessing/dsmcc/carousel/data/Module.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace carousel {
	Module::Module(unsigned int moduleId) {
		this->pid = 0;
		this->id = moduleId;
		currentDownloadSize = 0;
		overflowNotification = true;
	}

	void Module::setESId(unsigned int pid) {
		this->pid = pid;
	}

	unsigned int Module::getESId() {
		return this->pid;
	}

	void Module::openFile() {
		moduleFd = fopen(getModuleFileName().c_str(), "wb");
	}

	void Module::setCarouselId(unsigned int id) {
		carouselId = id;
	}

	void Module::setSize(unsigned int size) {
		this->size = size;
	}

	void Module::setVersion(unsigned int version) {
		this->version = version;
	}

	void Module::setInfoLength(unsigned int length) {
		this->infoLength = length;
	}

	bool Module::isConsolidated() {
		if (currentDownloadSize > size) {
			if (overflowNotification) {
				clog << "Module::isConsolidated Warning! ";
				clog << "MODULE '" << getModuleFileName() << "' ";
				clog << "OVERFLOW!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
				clog << " SIZE='" << size << "' RCVD='";
				clog << currentDownloadSize << "'" << endl;
				overflowNotification = false;
			}
		}

		return (currentDownloadSize == size);
	}

	unsigned int Module::getId() {
		return id;
	}

	unsigned int Module::getCarouselId() {
		return carouselId;
	}

	unsigned int Module::getSize() {
		return size;
	}

	unsigned int Module::getVersion() {
		return version;
	}

	unsigned int Module::getInfoLength() {
		return infoLength;
	}

	string Module::getModuleFileName() {
		return ("carousel/modules/" +
				itos(pid) + itos(id) + itos(version) + ".mod");
	}

	void Module::pushDownloadData(
			unsigned int blockNumber, void* data, unsigned int dataSize) {

		unsigned int bytesSaved;

		if (!isConsolidated()) {
			if (blocks.find(blockNumber) != blocks.end()) {
				return;
			}

			blocks.insert(blockNumber);
			if (moduleFd != NULL) {
				bytesSaved = fwrite(data, 1, dataSize, moduleFd);
				if (bytesSaved != dataSize) {
					clog << "Module::pushDownloadData Warning!";
					clog << " size of data is '" << dataSize;
					clog << "' saved only '" << bytesSaved << "'";
					clog << endl;
				}

				currentDownloadSize = currentDownloadSize + bytesSaved;

			} else {
				clog << "Module Warning! File not open." << endl;
			}
		}

		if (isConsolidated()) {
			fclose(moduleFd);
		}
	}

	void Module::print() {
		clog << endl << endl;
		clog << "id: " << id << endl;
		clog << "currentDownloadSize: " << currentDownloadSize << endl;
		clog << "size: " << size << endl;
		clog << "version: " << version << endl;
		clog << "infolengh: " << infoLength << endl;
		clog << endl << endl;
	}
}
}
}
}
}
}
}
