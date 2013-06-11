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

#include "dataprocessing/dsmcc/carousel/data/DownloadInfoIndication.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace carousel {
	DownloadInfoIndication::DownloadInfoIndication() {

	}

	DownloadInfoIndication::~DownloadInfoIndication() {
		map<unsigned int, Module*>::iterator i;

		i = modules.begin();
		while (i != modules.end()) {
			delete i->second;

			++i;
		}
		
		if (header != NULL) {
			delete header;
			header = NULL;
		}

		modules.clear();
	}

	int DownloadInfoIndication::processMessage(DsmccMessageHeader* message) {
		FILE* fd;
		int rval;
		unsigned int i, moduleId, moduleSize, moduleVersion, moduleInfoLength;
		Module* module;

		header = message;

		// dsmccmessageheader = 12
		i = header->getAdaptationLength() + 12;

		char* bytes = new char[(header->getMessageLength() + i)];

		fd = fopen(header->getFileName().c_str(), "rb");
		if (fd != NULL) {
			rval = fread(
					(void*)bytes, 1, header->getMessageLength() + i, fd);

			this->downloadId = ((bytes[i] & 0xFF) << 24) |
				    ((bytes[i + 1] & 0xFF) << 16) |
				    ((bytes[i + 2] & 0xFF) << 8) |
				    (bytes[i + 3] & 0xFF);

			this->blockSize = ((bytes[i + 4] & 0xFF) << 8) |
					(bytes[i + 5] & 0xFF);

			//jump 10 bytes of unused fields
			i = i + 10;

			// checking compatibilityDescriptor()
			unsigned int compatDesc;
			compatDesc = ((bytes[i + 6] & 0xFF) << 8) | (bytes[i + 7] & 0xFF);
			if ((compatDesc) != 0) {
				i = i + compatDesc;
			}

			this->numberOfModules = ((bytes[i + 8] & 0xFF) << 8) |
					(bytes[i + 9] & 0xFF);

			i = i + 10;

			unsigned int j;
			for (j=0; j < numberOfModules; j++) {
				moduleId = ((bytes[i] & 0xFF) << 8) | (bytes[i + 1] & 0xFF);
				moduleSize = ((bytes[i + 2] & 0xFF) << 24) |
				    ((bytes[i + 3] & 0xFF) << 16) |
				    ((bytes[i + 4] & 0xFF) << 8) |
				    (bytes[i + 5] & 0xFF);

				moduleVersion = (bytes[i + 6] & 0xFF);
				moduleInfoLength = (bytes[i + 7] & 0xFF);

				module = new Module(moduleId);
				module->setESId(header->getESId());
				module->setSize(moduleSize);
				module->setVersion(moduleVersion);
				module->setInfoLength(moduleInfoLength);
				module->setCarouselId(downloadId);
				module->openFile();
				modules[moduleId] = module;

				i = i + 8;
				i = i + moduleInfoLength;
				module = NULL;
			}
			fclose(fd);

			remove(header->getFileName().c_str());

		} else {
			clog << "DownloadInfoIndication::processMessage - Message header error: could not open file ";
			clog << header->getFileName().c_str() << endl;
			return -1;
		}

		delete bytes;

		return 0;
	}

	unsigned int DownloadInfoIndication::getDonwloadId() {
		return downloadId;
	}

	unsigned int DownloadInfoIndication::getBlockSize() {
		return blockSize;
	}

	unsigned int DownloadInfoIndication::getNumberOfModules() {
		return numberOfModules;
	}

	void DownloadInfoIndication::getInfo(map<unsigned int, Module*>* ocInfo) {
		ocInfo->insert(modules.begin(), modules.end());
	}

	void DownloadInfoIndication::print() {
		clog << "downloadId = " << this->downloadId << endl;
		clog << "blockSize = " << this->blockSize << endl;
		clog << "numberOfModules = " << this->numberOfModules << endl;
	}
}
}
}
}
}
}
}
