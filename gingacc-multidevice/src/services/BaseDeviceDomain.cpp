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

#include "multidevice/services/BaseDeviceDomain.h"
#include "multidevice/services/device/ActiveDeviceService.h"
#include "multidevice/services/device/PassiveDeviceService.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include <stdio.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
	BaseDeviceDomain::BaseDeviceDomain(bool useMulticast, int srvPort) : DeviceDomain(useMulticast, srvPort) {
		timerCount    = 0;

		Thread::mutexInit(&pMutex, false);
		hasNewPassiveTask = false;
		passiveTimestamp  = 0;
		lastMediaContentTask.size = 0;

		deviceClass   = CT_BASE;
		deviceService = new BaseDeviceService();

		if (useMulticast) {
			passiveSocket = new MulticastSocketService(
							(char*)(PASSIVE_MCAST_ADDR.c_str()),
							BROADCAST_PORT + CT_PASSIVE);
		}
		else {
			passiveSocket = new BroadcastDualSocketService(
							BASE_WRITE_BCAST_PORT, SECO_WRITE_BCAST_PORT);
		}
			/*
		passiveMulticast  = new MulticastSocketService(
				(char*)(PASSIVE_MCAST_ADDR.c_str()),
				BROADCAST_PORT + CT_PASSIVE);
				*/
	}

	BaseDeviceDomain::~BaseDeviceDomain() {
		Thread::mutexDestroy(&pMutex);
	}

	bool BaseDeviceDomain::taskRequest(
			int destDevClass,
			char* data,
			int taskSize) {

		RemoteTask* task;

		switch (destDevClass) {
			case CT_PASSIVE:
				task = new RemoteTask;
				task->data = data;
				task->size = taskSize;
				task->timestamp = getCurrentTimeMillis();
				//hasNewPassiveTask = true;
				Thread::mutexLock(&pMutex);
				passiveTasks.push_back(task);
				//hasNewPassiveTask = false;
				Thread::mutexUnlock(&pMutex);
				return true;

			case CT_ACTIVE:
				return activeTaskRequest(data, taskSize);

			default:
				return false;
		}
	}

	bool BaseDeviceDomain::passiveTaskRequest(char* data, int taskSize) {
		bool repeat = true;

		if (!passiveTasks.empty() || hasNewPassiveTask) {
			//repeat = false;
		}

		passiveSocket->dataRequest(data, taskSize, repeat);
		return true;
	}

	bool BaseDeviceDomain::activeTaskRequest(char* data, int taskSize) {
		/*clog << "BaseDeviceDomain::activeTaskRequest ";
		clog << endl;*/

		return true;
	}

	void BaseDeviceDomain::receiveConnectionRequest(char* task) {
		int reqDevClass, width, height, srvPort;
		clog << "BaseDeviceDomain::receiveConnectionRequest " << endl;

		reqDevClass = (int)(unsigned char)task[0];
		width = ((((unsigned char)task[1]) & 0xFF) |
				((((unsigned char)task[2]) << 8) & 0xFF00));

		height = ((((unsigned char)task[3]) & 0xFF) |
				((((unsigned char)task[4]) << 8) & 0xFF00));

		srvPort = ((((unsigned char)task[5]) & 0xFF) |
				((((unsigned char)task[6]) << 8) & 0xFF00));

		if (addDevice(reqDevClass, width, height, srvPort)) {
			schedDevClass = reqDevClass;
			schedulePost  = FT_ANSWERTOREQUEST;

		} else {
			clog << "BaseDeviceDomain::receiveConnectionRequest can't add ";
			clog << "device" << endl;
		}
	}

	void BaseDeviceDomain::postAnswerTask(
			int reqDeviceClass, int answer) {

		int taskSize, pos, broadcastPort;
		int answerPayloadSize = 11;
		char* task;
		char* streamSourceIP;

		clog << "BaseDeviceDomain::postAnswerTask answer '";
		clog << answer << "' for device '";
		clog << sourceIp << "', which means '" << getStrIP(sourceIp);
		clog << "', of class '" << reqDeviceClass << endl;

		//prepare frame
		task = mountFrame(
				myIP, reqDeviceClass, FT_ANSWERTOREQUEST, answerPayloadSize);

		//fill prepared frame with answer payload
		pos = HEADER_SIZE;

		streamSourceIP = getStreamFromUInt(sourceIp);
		memcpy(task + pos, streamSourceIP, 4);
		delete[] streamSourceIP;

		pos = pos + 4;
		task[pos] = answer;

		pos++;
		memcpy(task + pos, task, 4);

		broadcastPort = broadcastService->getServicePort();
		pos = pos + 4;
		task[pos] = broadcastPort & 0xFF;
		task[pos+1] = (broadcastPort & 0xFF00) >> 8;

		taskSize = HEADER_SIZE + answerPayloadSize;
		clog << "BaseDeviceDomain::answer with taskSize="<<taskSize<<endl;
		broadcastTaskRequest(task, taskSize);
		clog << "BaseDeviceDomain::called broadcastTaskRequest()"<<endl;

	}
/*
	void BaseDeviceDomain::postNclMetadata(
			int devClass, vector<StreamData*>* streams) {

		char* task;
		StreamData* streamData;
		vector<StreamData*>::iterator i;

		if (devClass != IDeviceDomain::CT_BASE &&
				devClass != IDeviceDomain::CT_ACTIVE) {

			return;
		}

		i = streams->begin();
		while (i != streams->end()) {
			streamData = *i;

			//prepare frame
			task = mountFrame(
					myIP, devClass, FT_MEDIACONTENT, streamData->size);

			memcpy(task + HEADER_SIZE, streamData->stream, streamData->size);
			taskRequest(devClass, task, streamData->size + HEADER_SIZE);

			delete streamData;
			streams->erase(i);
			i = streams->begin();
		}
	}*/

	bool BaseDeviceDomain::postMediaContentTask(
			int destDevClass,
			string url) {

		FILE* fd;
		int fileSize, bytesRead, tSize;
		char* task;

		clog << "BaseDeviceDomain::postMediaContentTask file '";
		clog << url << "' to devices of class '" << destDevClass << "'";
		clog << endl;

		if (destDevClass == 0) {
			return false;
		}

		if (!deviceService->hasDevices()) {
			clog << "BaseDeviceDomain::postMediaContentTask no devs found!";
			clog << endl;
			return false;
		}

		fd = fopen(url.c_str(), "rb");
		if (fd != NULL) {
			fseek(fd, 0L, SEEK_END);
			fileSize = ftell(fd);
			if (fileSize > 0) {
				fclose(fd);

				if (fileSize > MAX_FRAME_SIZE) {
					//TODO: frame segmentation support
					clog << "BaseDeviceDomain::postMediaContentTask ";
					clog << "Warning! Can't post a frame that the ";
					clog << "network doesn't support (" << fileSize;
					clog << ")" << endl;
					return false;
				}

				fd = fopen(url.c_str(), "rb");
				if (fd < 0) {
					clog << "BaseDeviceDomain::postMediaContentTask ";
					clog << "Warning! Can't re-open file '" << url;
					clog << "'" << endl;
					return false;
				}

				if (destDevClass == IDeviceDomain::CT_PASSIVE) {
					//prepare frame
					task = mountFrame(
							myIP, destDevClass, FT_MEDIACONTENT, fileSize);

					bytesRead = fread(task + HEADER_SIZE, 1, fileSize, fd);
					if (bytesRead == fileSize) {
						tSize = fileSize + HEADER_SIZE;
						if (lastMediaContentTask.size != 0) {
							delete[] lastMediaContentTask.data;
						}
						lastMediaContentTask.data = new char[tSize];
						lastMediaContentTask.size = tSize;
						memcpy(lastMediaContentTask.data, task, tSize);

						taskRequest(destDevClass, task, tSize);

						} else {
						clog << "BaseDeviceDomain::";
						clog << "postMediaContentTask ";
						clog << "Warning! Can't read '" << fileSize;
						clog << "' bytes from file '" << url << "' (";
						clog << bytesRead << " bytes read)" << endl;
						delete[] task;
					}
				}

			} else {
				clog << "BaseDeviceDomain::postMediaContentTask ";
				clog << "Warning! Can't seek file '" << url << "'" << endl;
			}

			fclose(fd);

		} else {
			clog << "BaseDeviceDomain::postMediaContentTask ";
			clog << "Warning! Can't open file '" << url << "'" << endl;
		}

		return true;
	}

	bool BaseDeviceDomain::receiveEventTask(char* task) {
		clog << "BaseDeviceDomain::receiveEventTask destClass '";
		clog << destClass << "'" << endl;
		return deviceService->receiveEvent(
				sourceIp, frameType, task, this->frameSize);
	}

	void BaseDeviceDomain::setDeviceInfo(int width, int height) {
		DeviceDomain::setDeviceInfo(width, height);
		connected = true;
	}

	bool BaseDeviceDomain::runControlTask() {
		char* task = NULL;

		//clog << "BaseDeviceDomain::runControlTask :: " << endl;

		if (taskIndicationFlag) {
			task = taskReceive();
			if (task == NULL) {
				taskIndicationFlag = false;
				return false;
			}
/*
			if (myIP == sourceIp) {
				clog << "DeviceDomain::runControlTask got my own task ";
				clog << "(size = '" << frameSize << "')" << endl;

				delete[] task;
				task = NULL;
				taskIndicationFlag = false;
				return false;
			}
*/
			if (destClass != deviceClass) {
				clog << "DeviceDomain::runControlTask Task isn't for me!";
				clog << endl;

				delete[] task;
				task = NULL;
				taskIndicationFlag = false;
				return false;
			}

			if (frameSize + HEADER_SIZE != bytesRecv) {
				delete[] task;
				task = NULL;
				taskIndicationFlag = false;
				clog << "DeviceDomain::runControlTask Warning! Invalid task ";
				clog << "size '" << frameSize + HEADER_SIZE << "' (received '";
				clog << bytesRecv << "'" << endl;
				return false;
			}

			clog << "BaseDeviceDomain::runControlTask frame type '";
			clog << frameType << "'" << endl;

			switch (frameType) {
				case FT_CONNECTIONREQUEST:
					//TODO: fix passive connection request - use 7 bytes as well (add port field)
					if ((frameSize != 5)&&(frameSize != 7)) {
						clog << "25BaseDeviceDomain::runControlTask Warning!";
						clog << "received a connection request frame with";
						clog << " wrong size: '" << frameSize << "'" << endl;

					} else {
						clog << "calling receiveConnectionRequest" << endl;
						receiveConnectionRequest(task);
					}
					break;

				case FT_KEEPALIVE:
					clog << "BaseDeviceDomain::runControlTask KEEPALIVE";
					clog << endl;
					break;

				//what?
				default:
					clog << "DeviceDomain::runControlTask frame type ";
					clog << "WHAT?" << endl;
					delete[] task;
					task = NULL;
					taskIndicationFlag = false;
					return false;
			}

			delete[] task;
			task = NULL;

		} else {
			clog << "DeviceDomain::runControlTask Warning! Trying to control";
			clog << "a non indicated task." << endl;
		}

		taskIndicationFlag = false;
		return true;
	}

	bool BaseDeviceDomain::runDataTask() {
		char* task;

		task = taskReceive();
		if (task == NULL) {
			return false;
		}
/*
		if (myIP == sourceIp) {
			clog << "BaseDeviceDomain::runDataTask receiving my own task";
			clog << endl;

			delete[] task;
			return false;
		}
*/
		if (destClass != deviceClass) {
			clog << "BaseDeviceDomain::runDataTask";
			clog << " should never reach here (receiving wrong destination";
			clog << " class '" << destClass << "')";
			clog << endl;

			delete[] task;
			return false;
		}

		if (frameSize + HEADER_SIZE != bytesRecv) {
			delete[] task;
			clog << "BaseDeviceDomain::runDataTask Warning! wrong ";
			clog << "frameSize '" << bytesRecv << "'" << endl;
			return false;
		}

		switch (frameType) {
			case FT_SELECTIONEVENT:
			case FT_ATTRIBUTIONEVENT:
			case FT_PRESENTATIONEVENT:
				receiveEventTask(task);
				break;

			//what?
			default:
				clog << "BaseDeviceDomain::runDataTask frame type ";
				clog << "WHAT?" << endl;
				delete[] task;
				return false;
		}

		delete[] task;
		return true;
	}

	void BaseDeviceDomain::checkPassiveTasks() {
		vector<RemoteTask*>::iterator i;
		RemoteTask* remoteTask;
		char* data;
		int taskSize;

		Thread::mutexLock(&pMutex);
		i = passiveTasks.begin();
		if (i != passiveTasks.end()) {
			remoteTask = *i;
			data = remoteTask->data;
			taskSize = remoteTask->size;
			passiveTasks.erase(i);

			if (((remoteTask->timestamp - passiveTimestamp) >
					(1000 / PASSIVE_FPS)) || passiveTimestamp == 0) {

				passiveTimestamp = getCurrentTimeMillis();
				passiveTaskRequest(data, taskSize);

			} else {
				//clog << "CONTIGUOUS FRAME" << endl;
				/*if (passiveTasks.size() > 1) {
					clog << "DISCARDING FRAME" << endl;
					delete[] data;

				} else {*/
					passiveTimestamp = getCurrentTimeMillis();
					passiveTaskRequest(data, taskSize);
				//}
			}

			delete remoteTask;
		}
		Thread::mutexUnlock(&pMutex);
	}

	void BaseDeviceDomain::checkDomainTasks() {
		char* data;

		DeviceDomain::checkDomainTasks();
		if (newAnswerPosted) {
			newAnswerPosted = false;
			deviceService->newDeviceConnected(sourceIp);
		}

		if (passiveSocket->checkInputBuffer(mdFrame, &bytesRecv)) {
 			runDataTask();
		}

		checkPassiveTasks();
		if (!passiveSocket->checkOutputBuffer()) {
			if (lastMediaContentTask.size != 0) {
				timerCount++;
				if (timerCount > 5) {
					data = new char[lastMediaContentTask.size];
					memcpy(
							data,
							lastMediaContentTask.data,
							lastMediaContentTask.size);

					taskRequest(
							IDeviceDomain::CT_PASSIVE,
							data,
							lastMediaContentTask.size);

					timerCount = 0;
				}
			}
		}
	}
}
}
}
}
}
}
