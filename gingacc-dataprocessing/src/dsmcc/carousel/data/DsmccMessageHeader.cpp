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

#include "dataprocessing/dsmcc/carousel/data/DsmccMessageHeader.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace carousel {
	DsmccMessageHeader::DsmccMessageHeader(string fileName, unsigned int pid) {
		int fd, rval;
		char bytes[12];

		this->pid = pid;
		memset(bytes, 0, sizeof(bytes));
		fd = open(fileName.c_str(), O_RDONLY|O_LARGEFILE);
		if (fd >= 0) {
			this->fileName = fileName;

			rval = read(fd, (void*)&(bytes[0]), 12);
			if (rval == 12) {
				this->protocolDiscriminator = (bytes[0] & 0xFF);
				this->dsmccType = (bytes[1] & 0xFF);
				this->messageId = ((bytes[2] & 0xFF) << 8) | (bytes[3] & 0xFF);
				this->transactionId = ((bytes[4] & 0xFF) << 24) |
					    ((bytes[5] & 0xFF) << 16) |
					    ((bytes[6] & 0xFF) << 8) |
					    (bytes[7] & 0xFF);

				//reserved
				if ((bytes[8] & 0xFF) != 0xFF) {
					cout << "Warning: reserved field shall be equal to 0xFF";
					cout << endl;
				}

				this->adaptationLength = (bytes[9] & 0xFF);
				this->messageLength = ((bytes[10] & 0xFF) << 8) |
						(bytes[11] & 0xFF);

			} else {
				cout << "Warning: cannot read 12 bytes, " << rval;
				cout << " readed." << endl;
			}
		} else {
			cout << "Message header error: could not open file ";
			cout << fileName.c_str() << endl;
		}
		close(fd);
	}

	unsigned int DsmccMessageHeader::getESId() {
		return this->pid;
	}

	string DsmccMessageHeader::getFileName() {
		return fileName;
	}

	unsigned int DsmccMessageHeader::getDsmccType() {
		return dsmccType;
	}

	unsigned int DsmccMessageHeader::getMessageId() {
		return messageId;
	}

	unsigned int DsmccMessageHeader::getTrasnsactionId() {
		return transactionId;
	}

	unsigned int DsmccMessageHeader::getAdaptationLength() {
		return adaptationLength;
	}

	unsigned int DsmccMessageHeader::getMessageLength() {
		return messageLength;
	}

	void DsmccMessageHeader::print() {
		cout << "fileName = " << fileName.c_str() << endl;
		cout << "protocolDiscriminator = " << protocolDiscriminator << endl;
		cout << "dsmccType = " << dsmccType << endl;
		cout << "messageId = " << messageId << endl;
		cout << "transactionId = " << transactionId << endl;
		cout << "adaptationLength = " << adaptationLength << endl;
		cout << "messageLength = " << messageLength << endl;
	}
}
}
}
}
}
}
}
