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

#include "../../include/services/PassiveDeviceDomain.h"

#include "../../include/services/device/PassiveDeviceService.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
	PassiveDeviceDomain::PassiveDeviceDomain() : DeviceDomain() {
		deviceClass      = CT_PASSIVE;
		deviceService    = new PassiveDeviceService();

		//passiveMulticast = new BroadcastSocketService();

		passiveMulticast = new MulticastSocketService(
				(char*)(PASSIVE_MCAST_ADDR.c_str()),
				BROADCAST_PORT + CT_PASSIVE);
	}

	PassiveDeviceDomain::~PassiveDeviceDomain() {

	}

	bool PassiveDeviceDomain::taskRequest(
			int destDevClass,
			char* data,
			int taskSize) {

		return passiveTaskRequest(data, taskSize);
	}

	bool PassiveDeviceDomain::passiveTaskRequest(char* data, int taskSize) {
		//cout << "PassiveDeviceDomain::passiveTaskRequest" << endl;
		passiveMulticast->dataRequest(data, taskSize);
		return true;
	}

	void PassiveDeviceDomain::postConnectionRequestTask(int w, int h) {
		char* task;
		int connReqPayloadSize = 5;
		int taskSize;

		cout << "PassiveDeviceDomain::postConnectionRequestTask";
		cout << endl;

		//prepare frame
		task = mountFrame(
				myIP, CT_BASE, FT_CONNECTIONREQUEST, connReqPayloadSize);

		task[HEADER_SIZE] = deviceClass;

		task[HEADER_SIZE + 1] = w & 0xFF;
		task[HEADER_SIZE + 2] = (w & 0xFF00) >> 8;

		task[HEADER_SIZE + 3] = h & 0xFF;
		task[HEADER_SIZE + 4] = (h & 0xFF00) >> 8;

		taskSize = HEADER_SIZE + connReqPayloadSize;
		broadcastTaskRequest(task, taskSize);
	}

	void PassiveDeviceDomain::receiveAnswerTask(char* task) {
		unsigned int taskIP;

		taskIP = getUIntFromStream(task);
		if (taskIP != myIP) {
			return; //this is'nt a warning
		}

		deviceService->connectedToBaseDevice(sourceIp);

		if (connected) {
			cout << "PassiveDeviceDomain::receiveAnswerTask Warning! ";
			cout << "received an answer task in connected state" << endl;
		}

		//TODO: check if central domain IP + port received in task is correct
		//cout << "PassiveDeviceDomain::receiveAnswerTask Connected with ";
		//cout << "base multi-device domain" << endl;
		connected = true;
	}

	bool PassiveDeviceDomain::receiveMediaContentTask(char* task) {
		/*cout << "PassiveDeviceDomain::receiveMediaContentTask ";
		cout << "destcass = '" << destClass << "'" << endl;*/
		return deviceService->receiveMediaContent(
				sourceIp, task, this->frameSize);
	}

	void PassiveDeviceDomain::setDeviceInfo(int width, int height) {
		DeviceDomain::setDeviceInfo(width, height);
		connected = false;
	}

	bool PassiveDeviceDomain::runControlTask() {
		char* task;

		if (taskIndicationFlag) {
			task = taskReceive();
			if (task == NULL) {
				taskIndicationFlag = false;
				cout << "PassiveDeviceDomain::runControlTask Warning! ";
				cout << "received a NULL task" << endl;
				return false;
			}

			if (myIP == sourceIp) {
				cout << "PassiveDeviceDomain::runControlTask got my own task ";
				cout << "(size = '" << frameSize << "')" << endl;

				delete[] task;
				taskIndicationFlag = false;
				return false;
			}

			if (destClass != deviceClass) {
				cout << "PassiveDeviceDomain::runControlTask Task isn't for me!";
				cout << endl;

				delete[] task;
				taskIndicationFlag = false;
				return false;
			}

			if (frameSize + HEADER_SIZE != bytesRecv) {
				delete[] task;
				taskIndicationFlag = false;
				cout << "PassiveDeviceDomain::runControlTask Warning! ";
				cout << "received a wrong size frame '" << frameSize;
				cout << "' bytes received '" << bytesRecv << "'" << endl;
				return false;
			}
			//cout << "PassiveDeviceDomain::runControlTask frame type '";
			//cout << frameType << "'" << endl;

			switch (frameType) {
				case FT_ANSWERTOREQUEST:
					if (frameSize != 11) {
						cout << "PassiveDeviceDomain::runControlTask Warning!";
						cout << "received an answer to connection request with";
						cout << " wrong size: '" << frameSize << "'" << endl;
						delete[] task;
						taskIndicationFlag = false;
						return false;

					} else {
						receiveAnswerTask(task);
					}
					break;

				case FT_KEEPALIVE:
					cout << "PassiveDeviceDomain::runControlTask KEEPALIVE";
					cout << endl;
					break;

				default:
					cout << "PassiveDeviceDomain::runControlTask WHAT? FT '";
					cout << frameType << "'" << endl;
					delete[] task;
					taskIndicationFlag = false;
					return false;
			}

			delete[] task;
		}

		taskIndicationFlag = false;
		return true;
	}

	bool PassiveDeviceDomain::runDataTask() {
		char* task;

		task = taskReceive();
		if (task == NULL) {
			return false;
		}

		if (myIP == sourceIp) {
			cout << "PassiveDeviceDomain::runDataTask receiving my own task";
			cout << endl;

			delete[] task;
			return false;
		}

		if (destClass != deviceClass) {
			cout << "PassiveDeviceDomain::runDataTask";
			cout << " should never reaches here (receiving wrong destination";
			cout << " class '" << destClass << "')";
			cout << endl;

			delete[] task;
			taskIndicationFlag = false;
			return false;
		}

		if (frameSize + HEADER_SIZE != bytesRecv) {
			delete[] task;
			cout << "PassiveDeviceDomain::runDataTask Warning! wrong ";
			cout << "frameSize '" << bytesRecv << "'" << endl;
			return false;
		}

		//cout << "PassiveDeviceDomain::runDataTask frame type '";
		//cout << frameType << "'" << endl;

		switch (frameType) {
			case FT_MEDIACONTENT:
				/*cout << "PassiveDeviceDomain::runDataTask call ";
				cout << "receiveMediaContentTask " << endl;*/
				receiveMediaContentTask(task);
				break;

			default:
				cout << "PassiveDeviceDomain::runDataTask WHAT? frame type '";
				cout << frameType << "'" << endl;
				delete[] task;
				taskIndicationFlag = false;
				return false;
		}

		delete[] task;
		return true;
	}

	void PassiveDeviceDomain::checkDomainTasks() {
		DeviceDomain::checkDomainTasks();

		if (passiveMulticast->checkInputBuffer(mdFrame, &bytesRecv)) {
 			runDataTask();
		}

		passiveMulticast->checkOutputBuffer();
	}
}
}
}
}
}
}
