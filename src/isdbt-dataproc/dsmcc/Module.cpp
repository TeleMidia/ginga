/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

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

#include "config.h"
#include "Module.h"

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
		return (SystemCompat::getTemporaryDir() +
				"ginga" + SystemCompat::getIUriD() +
				"carousel" + SystemCompat::getIUriD() +
			    "modules" + SystemCompat::getIUriD() +
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

		if (isConsolidated() && moduleFd > 0) {
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
