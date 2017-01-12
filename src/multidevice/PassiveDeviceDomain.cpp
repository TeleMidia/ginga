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

#include "PassiveDeviceDomain.h"
#include "PassiveDeviceService.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
	PassiveDeviceDomain::PassiveDeviceDomain(bool useMulticast, int srvPort) : DeviceDomain(useMulticast, srvPort) {
		deviceClass      = CT_PASSIVE;
		deviceService    = new PassiveDeviceService();
/*
		passiveMulticast = new MulticastSocketService(
				(char*)(PASSIVE_MCAST_ADDR.c_str()),
				BROADCAST_PORT + CT_PASSIVE);
				*/

		if (useMulticast) {
			passiveSocket = new MulticastSocketService(
					(char*)(PASSIVE_MCAST_ADDR.c_str()),
					BROADCAST_PORT + CT_PASSIVE);

		}
		else {
			passiveSocket = new BroadcastDualSocketService(
			SECO_WRITE_BCAST_PORT,BASE_WRITE_BCAST_PORT);
		}

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
		clog << "PassiveDeviceDomain::passiveTaskRequest" << endl;
		passiveSocket->dataRequest(data, taskSize);
		return true;
	}

	void PassiveDeviceDomain::postConnectionRequestTask(int w, int h) {
		char* task;
		int connReqPayloadSize = 5;
		int taskSize;

		if (connected)
			return;

		clog << "PassiveDeviceDomain::postConnectionRequestTask";
		clog << endl;

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
			clog << "PassiveDeviceDomain::receiveAnswerTask (taskIP != myIP) "<<endl;
			return; //this is'nt a warning
		}

		if (connected) {
			clog << "PassiveDeviceDomain::receiveAnswerTask Warning! ";
			clog << "received an answer task in connected state" << endl;
		}
		else {
			deviceService->connectedToBaseDevice(sourceIp);
		}

		//TODO: check if central domain IP + port received in task is correct
		//clog << "PassiveDeviceDomain::receiveAnswerTask Connected with ";
		//clog << "base multi-device domain" << endl;
		connected = true;
	}

	bool PassiveDeviceDomain::receiveMediaContentTask(char* task) {
		clog << "PassiveDeviceDomain::receiveMediaContentTask ";
		clog << "destcass = '" << destClass << "'" << endl;
		return deviceService->receiveMediaContent(
				sourceIp, task, this->frameSize);
	}

	void PassiveDeviceDomain::setDeviceInfo(int width, int height, string base_device_ncl_path) {
		DeviceDomain::setDeviceInfo(width, height, "");
		connected = false;
	}

	bool PassiveDeviceDomain::runControlTask() {
		char* task;

		if (taskIndicationFlag) {
			task = taskReceive();
			if (task == NULL) {
				taskIndicationFlag = false;
				clog << "PassiveDeviceDomain::runControlTask Warning! ";
				clog << "received a NULL task" << endl;
				return false;
			}
			/*
			if (myIP == sourceIp) {
				clog << "PassiveDeviceDomain::runControlTask got my own task ";
				clog << "(size = '" << frameSize << "')" << endl;

				delete[] task;
				taskIndicationFlag = false;
				return false;
			}
			*/

			if (destClass != deviceClass) {
				clog << "PassiveDeviceDomain::runControlTask Task isn't for me!";
				clog << endl;

				delete[] task;
				taskIndicationFlag = false;
				return false;
			}

			if (frameSize + HEADER_SIZE != bytesRecv) {
				delete[] task;
				taskIndicationFlag = false;
				clog << "PassiveDeviceDomain::runControlTask Warning! ";
				clog << "received a wrong size frame '" << frameSize;
				clog << "' bytes received '" << bytesRecv << "'" << endl;
				return false;
			}
			//clog << "PassiveDeviceDomain::runControlTask frame type '";
			//clog << frameType << "'" << endl;

			switch (frameType) {
				case FT_ANSWERTOREQUEST:
					if (frameSize != 11) {
						clog << "PassiveDeviceDomain::runControlTask Warning!";
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
					clog << "PassiveDeviceDomain::runControlTask KEEPALIVE";
					clog << endl;
					break;

				default:
					clog << "PassiveDeviceDomain::runControlTask WHAT? FT '";
					clog << frameType << "'" << endl;
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
		/*
		if (myIP == sourceIp) {
			clog << "PassiveDeviceDomain::runDataTask receiving my own task";
			clog << endl;

			delete[] task;
			return false;
		}
		*/

		if (destClass != deviceClass) {
			/*
			clog << "PassiveDeviceDomain::runDataTask";
			clog << " should never reach here (receiving wrong destination";
			clog << " class '" << destClass << "')";
			clog << endl;
			*/
			delete[] task;
			taskIndicationFlag = false;
			return false;
		}

		if (frameSize + HEADER_SIZE != bytesRecv) {
			delete[] task;
			clog << "PassiveDeviceDomain::runDataTask Warning! wrong ";
			clog << "frameSize '" << bytesRecv << "'" << endl;
			return false;
		}

		//clog << "PassiveDeviceDomain::runDataTask frame type '";
		//clog << frameType << "'" << endl;

		switch (frameType) {
			case FT_MEDIACONTENT:
				/*clog << "PassiveDeviceDomain::runDataTask call ";
				clog << "receiveMediaContentTask " << endl;*/
				receiveMediaContentTask(task);
				break;

			default:
				clog << "PassiveDeviceDomain::runDataTask WHAT? frame type '";
				clog << frameType << "'" << endl;
				delete[] task;
				taskIndicationFlag = false;
				return false;
		}

		delete[] task;
		return true;
	}

	void PassiveDeviceDomain::checkDomainTasks() {
		DeviceDomain::checkDomainTasks();

		if (passiveSocket->checkInputBuffer(mdFrame, &bytesRecv)) {
 			runDataTask();
		}

		passiveSocket->checkOutputBuffer();
	}
}
}
}
}
}
}
