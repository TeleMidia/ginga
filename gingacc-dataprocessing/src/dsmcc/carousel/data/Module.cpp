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
	}

	void Module::setESId(unsigned int pid) {
		this->pid = pid;
	}

	unsigned int Module::getESId() {
		return this->pid;
	}

	void Module::openFile() {
		moduleFd = open(
				getModuleFileName().c_str(),
				O_CREAT|O_WRONLY|O_LARGEFILE|O_APPEND|O_SYNC, 0777);
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
			cout << "MODULE OVERFLOW!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
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

	void Module::pushDownloadData(void* data, unsigned int dataSize) {
		unsigned int bytesSaved;

		if (!isConsolidated()) {
			//cout << "Module opening '" << getModuleFileName() << "'" << endl;
			if (moduleFd != -1) {
				bytesSaved = write(moduleFd, data, dataSize);
				if (bytesSaved != dataSize) {
					cout << "Module Warning! size of data is '" << dataSize;
					cout << "' saved only '" << bytesSaved << "'" << endl;
				}

				currentDownloadSize = currentDownloadSize + bytesSaved;

			} else {
				cout << "Module Warning! File not open." << endl;
			}
		}

		if (isConsolidated()) {
			close(moduleFd);
		}
	}

	void Module::print() {
		cout << endl << endl;
		cout << "id: " << id << endl;
		cout << "currentDownloadSize: " << currentDownloadSize << endl;
		cout << "size: " << size << endl;
		cout << "version: " << version << endl;
		cout << "infolengh: " << infoLength << endl;
		cout << endl << endl;
	}
}
}
}
}
}
}
}
