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

#include "multidevice/services/network/BroadcastDualSocketService.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
BroadcastDualSocketService::BroadcastDualSocketService(
		unsigned int readPort, unsigned int writePort) {

		outputBuffer = new vector<struct frame*>;

		interfaceIP = 0;

		broadcastReadPort = readPort;
		broadcastWritePort = writePort;

		interfaceIP = 0;

		Thread::mutexInit(&mutexBuffer, NULL);
		createSocket();
}

BroadcastDualSocketService::~BroadcastDualSocketService() {
		if (readSocket != NULL) {
			try {
				readSocket->disconnect();
				delete readSocket;
			}
			catch (SocketException &e) {
				clog << e.what() << endl;
			}

		}
		if (writeSocket != NULL) {
			try {
				writeSocket->disconnect();
				delete writeSocket;
			}
			catch (SocketException &e) {
				clog << e.what() << endl;
			}

		}
		if (outputBuffer != NULL) {
			delete outputBuffer;
			outputBuffer = NULL;
		}

		Thread::mutexDestroy(&mutexBuffer);
	}


	bool BroadcastDualSocketService::createSocket() {
		unsigned char trueVar = 1;

		try {
			writeSocket = new UDPSocket();
			readSocket = new UDPSocket(broadcastReadPort);
			readSocket->setNonBlocking(true);

			writeSocket->setReuseAddr(true);
			readSocket->setReuseAddr(true);

			broadcastIPAddr = readSocket->getBroadcastAddress();

			clog << "Network info: "<<endl;
			clog << "broadcastIPAddr: " << broadcastIPAddr << endl;
			clog << "broadcast read port: " << broadcastReadPort;

			interfaceIP = readSocket->getLocalIPAddress();

			clog << "interfaceIP: " << interfaceIP << endl;

			readSocket->setReuseAddr(true);

		}

		catch (SocketException &e) {
			clog << "BroadcastDualSocketService::createSocket()" << endl;
			clog << e.what() << endl;
			return false;
		}

		return true;
	}

	int BroadcastDualSocketService::getServicePort() {
		return 0;
	}

	void BroadcastDualSocketService::dataRequest(
			char* data, int taskSize, bool repeat) {

		struct frame* f;

		Thread::mutexLock(&mutexBuffer);
		f = new struct frame;
		f->data      = data;
		f->size      = taskSize;
		f->repeat    = repeat;
		outputBuffer->push_back(f);
		Thread::mutexUnlock(&mutexBuffer);
	}

	bool BroadcastDualSocketService::sendData(struct frame* f) {
		char* data;
		int taskSize, result, i;

		data     = f->data;
		taskSize = f->size;

		clog << "BroadcastDualSocketService::sendData Sending";
		clog << " taskSize = '" << taskSize  << "'" << endl;

		for (i = 0; i < NUM_OF_COPIES; i++) {
			try {
				writeSocket->sendTo(data,taskSize,broadcastIPAddr,broadcastWritePort);
			}
			catch (SocketException &e) {
				clog << "BroadcastDualSocketService::sendData writeSocket sendTo";
				clog << " TASKSIZE = '" << taskSize << "'" << endl;
				clog << e.what() << endl;
				return false;
			}
		}

		return true;
	}

	bool BroadcastDualSocketService::checkOutputBuffer() {
		vector<struct frame*>::iterator i;
		bool sent = false;
		struct frame* f;

		Thread::mutexLock(&mutexBuffer);
		i = outputBuffer->begin();
		if (i != outputBuffer->end()) {
			f = *i;
			sent = sendData(f);
			if (sent) {
				outputBuffer->erase(i);
				delete[] f->data;
				delete f;
			}

		} else {
			//clog << "BroadcastDualSocketService::checkOutputBuffer ";
			//clog << "empty buffer" << endl;
		}
		Thread::mutexUnlock(&mutexBuffer);

		return sent;
	}

	bool BroadcastDualSocketService::checkInputBuffer(char* data, int* size) {

		int res, recvFrom;
		string null_string;
		unsigned short null_short;

		if (readSocket == NULL) {
			clog << "BroadcastDualSocketService::checkInputBuffer readSocket == NULL" << endl;
			return false;
		}

		res = readSocket->select_t(0,0);
		//clog << "BroadcastDualSocketService::checkInputBuffer readSocket->select_t(0,0)" << endl;

		switch (res) {
			case -1:
				clog << "BroadcastDualSocketService::checkInputBuffer ";
				clog << "Warning! select ERRNO = " << errno << endl;
				memset(data, 0, MAX_FRAME_SIZE);
				return false;
			case 1:{
				memset(data, 0, MAX_FRAME_SIZE);
				try {
					*size = readSocket->recvFrom((void *)data,
							MAX_FRAME_SIZE,null_string,null_short);
				}
				catch (SocketException &e) {
					memset(data, 0, MAX_FRAME_SIZE);
					clog << e.what() << endl;
					return false;
				}


				if (*size <= HEADER_SIZE) {
					clog << "BroadcastDualSocketService::checkInputBuffer ";
					clog << "Warning! Received invalid frame: ";
					clog << "bytes received = '" << *size << "' ";
					clog << "HEADER_SIZE = '" << HEADER_SIZE << "' ";
					clog << endl;

					memset(data, 0, MAX_FRAME_SIZE);
					return false;
				}

				recvFrom = getUIntFromStream(data + 1);

//				if (!isValidRecvFrame(recvFrom, getUIntFromStream(groupAddress), data)) {
				if (!isValidRecvFrame(recvFrom, interfaceIP, data)) {
					clog << "BroadcastDualSocketService::checkInputBuffer() !isValidRecvFrame"<<endl;
					memset(data, 0, MAX_FRAME_SIZE);
					return false;
				}
				break;

			}

			default:
				memset(data, 0, MAX_FRAME_SIZE);
				return false;
		}
		return true;
	}
}
}
}
}
}
}
