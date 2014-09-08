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

#include "tuner/ITuner.h"
#include "tuner/providers/FileSystemProvider.h"
#include "tuner/providers/IProviderListener.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
	FileSystemProvider::FileSystemProvider(string fileName) {
		this->fileName       = fileName;
		this->fileDescriptor = NULL;
		this->capabilities   = DPC_CAN_FETCHDATA;
		this->listener       = NULL;
	}

	FileSystemProvider::~FileSystemProvider() {

	}

	void FileSystemProvider::setListener(ITProviderListener* listener) {
		this->listener = listener;
	}

	short FileSystemProvider::getCaps() {
		return capabilities;
	}

	bool FileSystemProvider::tune() {
		fileDescriptor = fopen(fileName.c_str(), "rb");
		if (fileDescriptor == NULL) {
			perror("openFile");
			return false;
		}

		return true;
	}

	IChannel* FileSystemProvider::getCurrentChannel() {
		return NULL;
	}

	bool FileSystemProvider::getSTCValue(uint64_t* stc, int* valueType) {
		return false;
	}

	bool FileSystemProvider::changeChannel(int factor) {
		return false;
	}

	bool FileSystemProvider::setChannel(string channelValue) {
		fileName = channelValue;
		return true;
	}

	int FileSystemProvider::createPesFilter(
			int pid, int pesType, bool compositeFiler) {

		return -1;
	}

	string FileSystemProvider::getPesFilterOutput() {
		return "";
	}

	void FileSystemProvider::close() {
		fclose(fileDescriptor);
	}

	bool FileSystemProvider::checkPossiblePacket(char *buff, const int &pos) {
		if ((buff[pos] == 0x47 &&
				(buff[pos+188] == 0x47 || buff[pos+204] == 0x47))) {
			return true;
		} else {
			return false;
		}
	}

	int FileSystemProvider::nextPacket(char *buff) {
		if (!(checkPossiblePacket(buff, 0))) {
			for (int i = 1; i < BUFFSIZE - 204; i++) {
				if (checkPossiblePacket(buff, i)) {
					return i;
				}
			}
		}
		return 0;
	}

	int FileSystemProvider::synchBuffer(char *buff, int diff) {
		memcpy(buff, buff + diff, BUFFSIZE - diff);
		return BUFFSIZE - diff;
	}

	char* FileSystemProvider::receiveData(int* len) {
		bool loop = false;
		int diff, pos;
		char* buff = NULL;
		*len = 0;

		if (fileDescriptor > 0) {
			buff = new char[BUFFSIZE];
			*len = fread((void*)buff, 1, BUFFSIZE, fileDescriptor);
			if (*len < BUFFSIZE) loop = true;

			if (!loop) {
				diff = nextPacket(buff);
				if (diff > 0) {
					pos = synchBuffer(buff, diff);
					int r = fread(buff + pos, 1, diff, fileDescriptor);
					if (r < diff) loop = true;
				}
			}

			if (loop) {
				clog << "FileSystemProvider::receiveData" << endl;
				clog << "File is over, set file to begin again!" << endl;
				fseek(fileDescriptor, 0L, SEEK_SET);
				if (listener != NULL) {
					listener->receiveSignal(PST_LOOP);
				}
			}
		}

		return buff;
	}
}
}
}
}
}
}
