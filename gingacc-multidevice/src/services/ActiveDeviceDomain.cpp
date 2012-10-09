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

#include "multidevice/services/ActiveDeviceDomain.h"

#include "multidevice/services/device/ActiveDeviceService.h"
#include "multidevice/services/network/MulticastSocketService.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
	ActiveDeviceDomain::ActiveDeviceDomain(bool deviceSearch, int srvPort) : DeviceDomain(deviceSearch, srvPort) {
		clog << "ActiveDeviceDomain::ActiveDeviceDomain()" <<endl;
		deviceClass   = CT_ACTIVE;
		deviceService = NULL;
	}

	ActiveDeviceDomain::~ActiveDeviceDomain() {

	}

	void ActiveDeviceDomain::postConnectionRequestTask(int w, int h) {

		char* task;
		int connReqPayloadSize = 7;
		int taskSize;


		if (!deviceSearch)
			return;

		//TODO: offer configuration parameters during requests connection to CT_ACTIVE devs

		//clog << "ActiveDeviceDomain::postConnectionRequestTask";
		//clog << " myIP = " << myIP << endl;
		//clog << endl;

		//prepare frame
		task = mountFrame(
				myIP, CT_BASE, FT_CONNECTIONREQUEST, connReqPayloadSize);

		task[HEADER_SIZE] = deviceClass;

		task[HEADER_SIZE + 1] = w & 0xFF;
		task[HEADER_SIZE + 2] = (w & 0xFF00) >> 8;

		task[HEADER_SIZE + 3] = h & 0xFF;
		task[HEADER_SIZE + 4] = (h & 0xFF00) >> 8;

		task[HEADER_SIZE + 5] = servicePort & 0xFF;
		task[HEADER_SIZE + 6] = (servicePort & 0xFF00) >> 8;

		taskSize = HEADER_SIZE + connReqPayloadSize;
		broadcastTaskRequest(task, taskSize);
	}

	void ActiveDeviceDomain::receiveAnswerTask(char* task) {
		/*unsigned int taskIP;

		taskIP = getUIntFromStream(task);
		if (taskIP != myIP) {
			return; //this is'nt a warning
		}

		deviceService->connectedToBaseDevice(sourceIp);*/

		if (connected) {
			clog << "ActiveDeviceDomain::receiveAnswerTask Warning! ";
			clog << "received an answer task in connected state" << endl;
		}

		//TODO: check if central domain IP + port received in task is correct
		clog << "ActiveDeviceDomain::receiveAnswerTask Connected with ";
		clog << "base multi-device domain" << endl;
		connected = true;
	}

	bool ActiveDeviceDomain::receiveMediaContentTask(char* task) {
		clog << "ActiveDeviceDomain::receiveMediaContentTask" << endl;

		/*return deviceService->receiveMediaContent(
				sourceIp, task, this->frameSize);*/

		return false;
	}

	bool ActiveDeviceDomain::runControlTask() {
		char* task;

		if (taskIndicationFlag) {
			task = taskReceive();
			if (task == NULL) {
				taskIndicationFlag = false;
				clog << "ActiveDeviceDomain::runControlTask Warning! ";
				clog << "received a NULL task" << endl;
				return false;
			}

			if (myIP == sourceIp) {
				/*
				clog << "ActiveDeviceDomain::runControlTask got my own task ";
				clog << "(size = '" << frameSize << "')" << endl;*/

				delete[] task;
				taskIndicationFlag = false;
				return false;
			}

			if (destClass != deviceClass) {
				clog << "ActiveDeviceDomain::runControlTask Task isn't for me!";
				clog << endl;

				delete[] task;
				taskIndicationFlag = false;
				return false;
			}

			if (frameSize + HEADER_SIZE != bytesRecv) {
				delete[] task;
				taskIndicationFlag = false;
				clog << "ActiveDeviceDomain::runControlTask Warning! ";
				clog << "received a wrong size frame '" << frameSize;
				clog << "' bytes received '" << bytesRecv << "'" << endl;
				return false;
			}
			//clog << "ActiveDeviceDomain::runControlTask frame type '";
			//clog << frameType << "'" << endl;

			switch (frameType) {
				case FT_ANSWERTOREQUEST:
					if (frameSize != 11) {
						clog << "ActiveDeviceDomain::runControlTask Warning!";
						clog << "received an answer to connection request with";
						clog << " wrong size: '" << frameSize << "'" << endl;
						delete[] task;
						taskIndicationFlag = false;
						return false;

					} else {
						receiveAnswerTask(task);
					}
					break;

				case FT_KEEPALIVE:
					clog << "ActiveDeviceDomain::runControlTask KEEPALIVE";
					clog << endl;
					break;

				default:
					clog << "ActiveDeviceDomain::runControlTask WHAT? FT '";
					clog << frameType << "'" << endl;
					delete[] task;
					taskIndicationFlag = false;
					return false;
			}

			delete[] task;

		} else {
			clog << "ActiveDeviceDomain::runControlTask Warning! ";
			clog << "task indication flag is false" << endl;
		}

		taskIndicationFlag = false;
		return true;
	}

	void ActiveDeviceDomain::checkDomainTasks() {
		DeviceDomain::checkDomainTasks();
	}
}
}
}
}
}
}
