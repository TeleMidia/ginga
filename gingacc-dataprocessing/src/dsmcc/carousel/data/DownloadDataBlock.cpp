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

#include "dataprocessing/dsmcc/carousel/data/DownloadDataBlock.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace carousel {
	DownloadDataBlock::DownloadDataBlock(DsmccMessageHeader* message) {
		header        = message;
		moduleId      = 0;
		moduleVersion = 0;
	}

	DownloadDataBlock::~DownloadDataBlock() {
		if (header != NULL) {
			delete header;
			header = NULL;
		}
	}

	int DownloadDataBlock::processDataBlock(
			map<unsigned int, Module*>* ocModules) {

		FILE* fd;
		int rval, trval;
		unsigned int i, blockNumber;
		unsigned int messageId, adaptationLength, messageLength;
		char headerBytes[12];
		char* bytes;
		Module* mod;

		map<unsigned int, Module*> mods;

		mods.insert(ocModules->begin(), ocModules->end());

		trval = 0;
		rval  = 1;
		fd    = fopen(header->getFileName().c_str(), "rb");

		while (rval > 0 && fd != NULL) {
			rval = fread((void*)&(headerBytes[0]), 1, 12, fd);
			trval = trval + rval;

			if (rval == 0) {
				break;
			}

			messageId = ((headerBytes[2] & 0xFF) << 8) |
					(headerBytes[3] & 0xFF);

			adaptationLength = (headerBytes[9] & 0xFF);
			messageLength = ((headerBytes[10] & 0xFF) << 8) |
					(headerBytes[11] & 0xFF);

			if (messageId == 0x1003) {
				i = adaptationLength;

				bytes = new char[messageLength];
				memset(bytes, 0, messageLength);
				rval = fread((void*)&(bytes[0]), 1, messageLength, fd);
				trval = trval + rval;
				if (rval == (int)messageLength) {
					moduleId = ((bytes[i] & 0xFF) << 8) |
						    (bytes[i + 1] & 0xFF);

					if (mods.count(moduleId) != 0) {
						mod = mods[moduleId];
						if (!(mod->isConsolidated())) {
							//TODO: offer update version support
							moduleVersion = (bytes[i + 2] & 0xFF);

							//checking reserved
							if ((bytes[i + 3] & 0xFF) != 0xFF) {
								clog << "DownloadDataBlock::processDataBlock ";
								clog << "Warning! Reserved DDB shall be set";
								clog << "to 0xFF" << endl;
							}

							blockNumber = ((bytes[i + 4] & 0xFF) << 8) |
								    (bytes[i + 5] & 0xFF);

							mod->pushDownloadData(
									blockNumber,
									(void*)&(bytes[i + 6]),
									(messageLength - (i + 6)));
						}

					} else {
						clog << "DownloadDataBlock::processDataBlock ";
						clog << "DDB Warning! modId '" << moduleId << "' not";
						clog << " found!" << endl;
						return -1;
					}

				} else {
					clog << "DownloadDataBlock::processDataBlock Warning!!" << endl;
					return -2;
				}

				delete[] bytes;
				bytes = NULL;

			} else {
				clog << "Warning! Unknown DDB MessageId: ";
				clog << hex << messageId << endl;
				return -3;
			}
		}
		if (fd != NULL) {
			fclose(fd);
		}

		remove(header->getFileName().c_str());
		return 0;
	}

	unsigned int DownloadDataBlock::getModuleId() {
		return moduleId;
	}

	unsigned int DownloadDataBlock::getModuleVersion() {
		return moduleVersion;
	}

	void DownloadDataBlock::print() {
		clog << "messageLength " << header->getMessageLength() << endl;
		clog << "datablock moduleId " << moduleId << endl;
		clog << "datablock moduleVersion " << moduleVersion << endl;
	}
}
}
}
}
}
}
}
